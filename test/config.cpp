#include <gtest/gtest.h>

#include <config/paths.hpp>

TEST(Config, DefaultPaths) {
	InitWorkingPath();
	ASSERT_FALSE(WorkingPath().empty());
	ASSERT_FALSE(AppPath().empty());
}