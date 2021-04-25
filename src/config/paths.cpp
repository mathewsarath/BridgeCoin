#include "paths.hpp"

#include <boost/predef.h>

#include <cassert>

#ifdef Live
constexpr bool isLive = true;
constexpr bool isTest = false;
constexpr bool isDev = false;
#elif Test
constexpr bool isLive = false;
constexpr bool isTest = true;
constexpr bool isDev = false;
#else
constexpr bool isLive = false;
constexpr bool isTest = false;
[[maybe_unused]] constexpr bool isDev = true;
#endif

namespace {
std::filesystem::path workingPath;
bool pathInitialized = false;
}	 // namespace

void InitWorkingPath() {
	auto path = AppPath() / PROJECT_NAME;
	if constexpr (isLive) {
		workingPath = path / "Live";
	} else if (isTest) {
		workingPath = path / "Test";
	} else {
		assert(isDev);
		workingPath = path / "Dev";
	}
	pathInitialized = true;
}

const std::filesystem::path& WorkingPath() {
	assert(pathInitialized);
	return workingPath;
}

#if BOOST_OS_WINDOWS
	#include <shlobj.h>

std::filesystem::path AppPath() {
	std::filesystem::path result;
	WCHAR path[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path))) {
		result = std::filesystem::path(path);
	} else {
		assert(false);
	}
	return result;
}

#elif BOOST_OS_MACOS
	#include <Foundation/Foundation.h>

std::filesystem::path AppPath() {
	NSString* dir_string = [NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES) lastObject];
	char const* dir_chars = [dir_string UTF8String];
	std::filesystem::path result(dir_chars);
	[dir_string release];
	return result;
}
#else
	#include <pwd.h>
	#include <sys/types.h>
	#include <unistd.h>

std::filesystem::path AppPath() {
	auto entry(getpwuid(getuid()));
	assert(entry != nullptr);
	return std::filesystem::path(entry->pw_dir);
}
#endif
