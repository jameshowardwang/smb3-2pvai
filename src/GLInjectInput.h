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

#pragma once
#include "Global.h"

#include "MutexDataPair.h"

struct SSRVideoStream;
class SSRVideoStreamWatcher;
class SSRVideoStreamReader;

class GLInjectInput {

private:
	struct SharedData {
		bool m_capturing;
		std::unique_ptr<SSRVideoStreamWatcher> m_stream_watcher;
		std::unique_ptr<SSRVideoStreamReader> m_stream_reader;
	};
	typedef MutexDataPair<SharedData>::Lock SharedLock;

private:
	static const int64_t MAX_COMMUNICATION_LATENCY;

private:
	std::string m_channel;
	bool m_relax_permissions;
	unsigned int m_flags;
	unsigned int m_target_fps;

	std::thread m_thread;
	MutexDataPair<SharedData> m_shared_data;
	std::atomic<bool> m_should_stop, m_error_occurred;

public:
	GLInjectInput(const std::string& channel, bool relax_permissions, bool record_cursor, bool limit_fps, unsigned int target_fps);
	~GLInjectInput();

	// Reads the current size of the stream. If the stream hasn't been started yet, this will be 0x0.
	// This function is thread-safe.
	void GetCurrentSize(unsigned int* width, unsigned int* height);

	// Returns the total number of captured frames.
	// This function is thread-safe.
	double GetFPS();

	// Start/stop capturing.
	// This function is thread-safe.
	void SetCapturing(bool capturing);

	// Returns whether an error has occurred in the input thread.
	// This function is thread-safe.
	inline bool HasErrorOccurred() { return m_error_occurred; }

	static bool LaunchApplication(const std::string& channel, bool relax_permissions, const std::string& command, const std::string& working_directory);

private:
	void Init();
	void Free();

	bool SwitchStream(SharedData* lock, const SSRVideoStream& stream);

	static void StreamAddCallback(const SSRVideoStream& stream, void* userdata);
	static void StreamRemoveCallback(const SSRVideoStream& stream, size_t pos, void* userdata);

	void InputThread();

};
