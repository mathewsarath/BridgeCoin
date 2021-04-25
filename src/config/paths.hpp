#pragma once

#include <filesystem>

std::filesystem::path AppPath();
void InitWorkingPath();
const std::filesystem::path& WorkingPath();