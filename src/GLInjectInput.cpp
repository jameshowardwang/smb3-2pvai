/*
Copyright (c) 2012-2014 Maarten Baert <maarten-baert@hotmail.com>

This file is part of SimpleScreenRecorder.

SimpleScreenRecorder is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

SimpleScreenRecorder is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with SimpleScreenRecorder.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Global.h"
#include "GLInjectInput.h"

#include "Logger.h"
#include "SSRVideoStreamWatcher.h"
#include "SSRVideoStreamReader.h"
#include "extractor/GameStateExtractor.h"

// Escapes characters so the string can be used in a shell command. It may not be 100% secure (character encoding can complicate things).
// But it doesn't really matter, an attacker that can change GLInject options could just as easily change the actual GLInject command.
// I copied the list of special characters from the PHP documentation:
// http://be2.php.net/manual/en/function.escapeshellcmd.php
static std::string ShellEscape(std::string str) {
	char specials[] = "\\#&;`|*?~<>^()[]{}$\x0a\xff\""; // backslash must be first
	for(unsigned int i = 0; i < sizeof(specials); ++i) {
		//str.replace(specials[i], std::string("\\") + specials[i]);
	}
	return str;
}

// The highest expected latency between GLInject and the input thread.
const int64_t GLInjectInput::MAX_COMMUNICATION_LATENCY = 100000;

GLInjectInput::GLInjectInput(const std::string& channel, bool relax_permissions, bool record_cursor, bool limit_fps, unsigned int target_fps) {

	m_channel = channel;
	m_relax_permissions = relax_permissions;
	m_flags = ((record_cursor)? GLINJECT_FLAG_RECORD_CURSOR : 0) | ((limit_fps)? GLINJECT_FLAG_LIMIT_FPS : 0);
	m_target_fps = target_fps;

	try {
		Init();
	} catch(...) {
		Free();
		throw;
	}

}

GLInjectInput::~GLInjectInput() {

	// tell the thread to stop
	if(m_thread.joinable()) {
		Logger::LogInfo("[GLInjectInput::~GLInjectInput] Stopping input thread ...");
		m_should_stop = true;
		m_thread.join();
	}

	// free everything
	Free();

}

void GLInjectInput::GetCurrentSize(unsigned int* width, unsigned int* height) {
	SharedLock lock(&m_shared_data);
	if(lock->m_stream_reader == NULL) {
		*width = *height = 0;
	} else {
		lock->m_stream_reader->GetCurrentSize(width, height);
	}
}

double GLInjectInput::GetFPS() {
	SharedLock lock(&m_shared_data);
	if(lock->m_stream_reader == NULL)
		return 0.0;
	return lock->m_stream_reader->GetFPS();
}

void GLInjectInput::SetCapturing(bool capturing) {
	SharedLock lock(&m_shared_data);
	lock->m_capturing = capturing;
	if(lock->m_stream_reader != NULL)
		lock->m_stream_reader->ChangeCaptureParameters(m_flags | ((lock->m_capturing)? GLINJECT_FLAG_CAPTURE_ENABLED : 0), m_target_fps);
}

bool GLInjectInput::LaunchApplication(const std::string& channel, bool relax_permissions, const std::string& command, const std::string& working_directory) {

	// prepare command
	std::string full_command = "LD_PRELOAD=\"libssr-glinject.so\" ";
	full_command += "SSR_CHANNEL=\"" + ShellEscape(channel) + "\" ";
	if(relax_permissions)
		full_command += "SSR_STREAM_RELAX_PERMISSIONS=1 ";
	full_command += command;

	// execute it
/*
	std::string args;
	args.push_back("-c");
	args.push_back(full_command);
		QProcess::startDetached("/bin/sh", args, working_directory);
*/
	return false; 

}

void GLInjectInput::Init() {

	// initialize shared data
	{
		SharedLock lock(&m_shared_data);
		lock->m_capturing = false;
		lock->m_stream_watcher.reset(new SSRVideoStreamWatcher(m_channel, m_relax_permissions));
	}

	// start input thread
	m_should_stop = false;
	m_error_occurred = false;
	m_thread = std::thread(&GLInjectInput::InputThread, this);

}

void GLInjectInput::Free() {
	SharedLock lock(&m_shared_data);
	lock->m_stream_reader.reset();
	lock->m_stream_watcher.reset();
}

bool GLInjectInput::SwitchStream(SharedData* lock, const SSRVideoStream& stream) {
	try {

		// create the stream reader
		std::unique_ptr<SSRVideoStreamReader> stream_reader(new SSRVideoStreamReader(m_channel, stream));

		// initialize the stream
		stream_reader->ChangeCaptureParameters(m_flags | ((lock->m_capturing)? GLINJECT_FLAG_CAPTURE_ENABLED : 0), m_target_fps);
		stream_reader->Clear();

		// if everything is okay, use the new stream reader
		lock->m_stream_reader = std::move(stream_reader);

	} catch(...) {
		Logger::LogError("[GLInjectInput::SwitchStream] Error: Could not read stream, this usually means that the stream was already gone.");
		return false;
	}
	return true;
}

void GLInjectInput::StreamAddCallback(const SSRVideoStream& stream, void* userdata) {
	GLInjectInput *input = (GLInjectInput*) userdata;
	SharedData *lock = input->m_shared_data.data(); // data is already locked, this is only a callback function
	input->SwitchStream(lock, stream);
}

void GLInjectInput::StreamRemoveCallback(const SSRVideoStream& stream, size_t pos, void* userdata) {
	GLInjectInput *input = (GLInjectInput*) userdata;
	SharedData *lock = input->m_shared_data.data(); // data is already locked, this is only a callback function
	if(lock->m_stream_reader != NULL) {
		if(lock->m_stream_reader->GetStream() == stream) {
			lock->m_stream_reader.reset();
			auto &streams = lock->m_stream_watcher->GetStreams();
			for(size_t i = pos; i > 0; ) {
				--i;
				if(input->SwitchStream(lock, streams[i]))
					break;
			}
		}
	}
}

void GLInjectInput::InputThread() {
	try {

		Logger::LogInfo("[GLInjectInput::InputThread] Input thread started.");

		// deal with pre-existing streams
		{
			SharedLock lock(&m_shared_data);
			auto &streams = lock->m_stream_watcher->GetStreams();
			for(size_t i = streams.size(); i > 0; ) {
				--i;
				if(SwitchStream(lock.get(), streams[i]))
					break;
			}
		}

		int64_t next_watcher_update = hrt_time_micro();

		GameStateExtractor game_state_extractor;
		while(!m_should_stop) {
			// try to get a frame
			int64_t timestamp;
			unsigned int width, height;
			int stride;
			void *data;
			{
				SharedLock lock(&m_shared_data);

				// update stream watcher
				if(hrt_time_micro() >= next_watcher_update) {
					lock->m_stream_watcher->HandleChanges(&StreamAddCallback, &StreamRemoveCallback, this);
					next_watcher_update = hrt_time_micro() + 200000;
				}

				// do we have a stream reader?
				if(lock->m_stream_reader == NULL) {
					lock.lock().unlock(); // release lock before sleep
					usleep(20000);
					continue;
				}

				// is a frame ready?
				data = lock->m_stream_reader->GetFrame(&timestamp, &width, &height, &stride);
				if(data == NULL) {
					lock.lock().unlock(); // release lock before sleep
					usleep(20000);
					continue;
				}

			}

			game_state_extractor.InitFromBGRAFrameBuffer(data, width, height, stride);
			game_state_extractor.ProcessGameState();

			// go to the next frame
			{
				SharedLock lock(&m_shared_data);
				lock->m_stream_reader->NextFrame();
			}
		}

		Logger::LogInfo("[GLInjectInput::InputThread] Input thread stopped.");

	} catch(const std::exception& e) {
		m_error_occurred = true;
		Logger::LogError(std::string("[GLInjectInput::InputThread] Exception ") + e.what() + " in input thread.");
	} catch(...) {
		m_error_occurred = true;
		Logger::LogError("[GLInjectInput::InputThread] Unknown exception in input thread.");
	}
}
