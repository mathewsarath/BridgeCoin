#pragma once

#include <array>
#include <chrono>
#include <cstdint>

using Amount = std::uint64_t;
using Account = std::uint64_t;	// For now an account is a primitive
using Hash = std::array<uint8_t, 32>;
using Root = Hash;	// Could also be an account...
using Delegator = Account;
using ValidatorID = Account;
using Height = std::uint64_t;
using SystemClock = std::chrono::system_clock;	// TODO: Change this to utc_clock when it is supported
using Clock = std::chrono::steady_clock;