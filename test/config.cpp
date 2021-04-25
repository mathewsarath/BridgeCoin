#include <gtest/gtest.h>

#include <config/paths.hpp>

TEST(Config, default_paths) {
	InitWorkingPath();
	ASSERT_FALSE(WorkingPath().empty());
	ASSERT_FALSE(AppPath().empty());
}