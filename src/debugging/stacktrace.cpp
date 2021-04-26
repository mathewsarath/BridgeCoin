#include "stacktrace.hpp"

#include <sstream>

// Some builds (mac) fail due to "Boost.Stacktrace requires `_Unwind_Backtrace` function".
#ifndef _WIN32
	#ifndef _GNU_SOURCE
		#define BEFORE_GNU_SOURCE 0
		#define _GNU_SOURCE
	#else
		#define BEFORE_GNU_SOURCE 1
	#endif
#endif
// On Windows this include defines min/max macros, so keep below other includes
// to reduce conflicts with other std functions
#include <boost/stacktrace.hpp>
#ifndef _WIN32
	#if !BEFORE_GNU_SOURCE
		#undef _GNU_SOURCE
	#endif
#endif

void DumpCrashStacktrace() {
	boost::stacktrace::safe_dump_to(PROJECT_NAME "_backtrace.dump");
}

std::string GenerateStacktrace() {
	auto stacktrace = boost::stacktrace::stacktrace();
	std::ostringstream ss;
	ss << stacktrace;
	return ss.str();
}