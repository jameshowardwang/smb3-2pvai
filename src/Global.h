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

#ifndef GLOBAL_H
#define GLOBAL_H

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <math.h>
#include <stdint.h>
#include <time.h>

#include <pwd.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#include <array>
#include <atomic>
#include <deque>
#include <limits>
#include <memory>
#include <mutex>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

// generic macro to test version numbers
#define TEST_MAJOR_MINOR(major, minor, required_major, required_minor) (major > required_major || (major == required_major && minor >= required_minor))

// test GCC version
#define TEST_GCC_VERSION(major, minor) TEST_MAJOR_MINOR(__GNUC__, __GNUC_MINOR__, major, minor)

// 'override' was added in GCC 4.7
#if !TEST_GCC_VERSION(4, 7)
#define override
#endif

#ifdef __MACH__
#include <sys/time.h>
#define CLOCK_MONOTONIC 0
//clock_gettime is not implemented on OSX
inline int clock_gettime(int /*clk_id*/, struct timespec* t) {
    struct timeval now;
    int rv = gettimeofday(&now, NULL);
    if (rv) return rv;
    t->tv_sec  = now.tv_sec;
    t->tv_nsec = now.tv_usec * 1000;
    return 0;
}
#endif

class GLInjectException : public std::exception {
public:
	inline virtual const char* what() const throw() override {
		return "GLInjectException";
	}
};
class SSRStreamException : public std::exception {
public:
	inline virtual const char* what() const throw() override {
		return "SSRStreamException";
	}
};

// simple function to do 16-byte alignment
inline size_t grow_align16(size_t size) {
	return (size_t) (size + 15) & ~((size_t) 15);
}

// high resolution timer
inline int64_t hrt_time_micro() {
	timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (uint64_t) ts.tv_sec * (uint64_t) 1000000 + (uint64_t) (ts.tv_nsec / 1000);
}

// Returns the name of the user.
inline std::string GetUserName() {
	std::vector<char> buf(std::max((long) 16384, sysconf(_SC_GETPW_R_SIZE_MAX)));
	passwd pwd, *result = NULL;
	if(getpwuid_r(geteuid(), &pwd, buf.data(), buf.size(), &result) == 0 && result != NULL)
		return std::string(result->pw_name);
	return "unknownuser";
}

template<typename T>
inline T positive_mod(T x, T y) {
	T z = x % y;
	return (z < 0)? z + y : z;
}

template<typename T>
inline T clamp(T v, T lo, T hi) {
	assert(lo <= hi);
	if(v < lo)
		return lo;
	if(v > hi)
		return hi;
	return v;
}
template<> inline float clamp<float>(float v, float lo, float hi) {
	assert(lo <= hi);
	return fmin(fmax(v, lo), hi);
}
template<> inline double clamp<double>(double v, double lo, double hi) {
	assert(lo <= hi);
	return fmin(fmax(v, lo), hi);
}

// Generic number-to-string conversion and vice versa
// Unlike the standard functions, these are locale-independent, and the functions never throw exceptions.
template<typename T>
inline std::string NumToString(T number) {
	std::ostringstream ss;
	ss << number;
	return ss.str();
}
template<typename T>
inline bool StringToNum(const std::string& str, T* number) {
	std::istringstream ss(str);
	ss >> *number;
	return !ss.fail();
}

#endif // GLOBAL_H
