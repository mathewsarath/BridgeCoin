#pragma once
#include "core/units.hpp"

class ReceiveBlock {
public:
	Hash source;
	Amount amount;
	SystemClock::time_point timestamp;	// From send block, useful for providing an ordering
};

class SendBlock {
public:
	Height blockHeight;
	Root previous;	// Previous could be block or account
	Account destination;

	// This will have to be sizeof (Height) * 8 << sizeof (Amount) * 8
	Amount totalMinimumReceiveAmount;	 // This is the amount you have at least received.
	Amount totalSendAmount;
	SystemClock::rep timestamp;	 // This is only an approximation but gives

	constexpr bool operator==(const SendBlock& block) const = default;

	Hash GenerateHash() const;
};