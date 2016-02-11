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
#include "Logger.h"

Logger *Logger::s_instance = NULL;

Logger::Logger() {
	assert(s_instance == NULL);
	s_instance = this;
}

Logger::~Logger() {
	assert(s_instance == this);
	s_instance = NULL;
}

void Logger::LogInfo(const std::string& str) {
	assert(s_instance != NULL);
	std::lock_guard<std::mutex> lock(s_instance->m_mutex);
	fprintf(stderr, "%s\n", str.c_str());
}

void Logger::LogWarning(const std::string& str) {
	assert(s_instance != NULL);
	std::lock_guard<std::mutex> lock(s_instance->m_mutex);
	fprintf(stderr, "\033[1;33m%s\033[0m\n", str.c_str());
}

void Logger::LogError(const std::string& str) {
	assert(s_instance != NULL);
	std::lock_guard<std::mutex> lock(s_instance->m_mutex);
	fprintf(stderr, "\033[1;31m%s\033[0m\n", str.c_str());
}
