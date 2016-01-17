#pragma once

#include <stdint.h>
#include "core.h"

enum class Buttons : uint8_t
{
	None = 0,
	Left = LEFT_BUTTON,
	Right = RIGHT_BUTTON,
	Up =  UP_BUTTON,
	Down = DOWN_BUTTON,
	A = A_BUTTON,
	B = B_BUTTON,
};

inline Buttons operator ~ (const Buttons buttons)
{
	return Buttons(~static_cast<uint8_t>(buttons));
}

inline Buttons operator & (const Buttons left, const Buttons right)
{
	return Buttons(static_cast<uint8_t>(left) & static_cast<uint8_t>(right));
}

inline Buttons & operator &= (Buttons & left, const Buttons right)
{
	return (left = left & right);
}

inline Buttons operator | (const Buttons left, const Buttons right)
{
	return Buttons(static_cast<uint8_t>(left) | static_cast<uint8_t>(right));
}

inline Buttons & operator |= (Buttons & left, const Buttons right)
{
	return (left = left | right);
}

inline Buttons operator ^ (const Buttons left, const Buttons right)
{
	return Buttons(static_cast<uint8_t>(left) ^ static_cast<uint8_t>(right));
}

inline Buttons & operator ^= (Buttons & left, const Buttons right)
{
	return (left = left ^ right);
}

inline bool AllFlagsSet(const Buttons buttons, const Buttons flags)
{
	return ((buttons & flags) == flags);
}

inline bool AnyFlagsSet(const Buttons buttons, const Buttons flags)
{
	return ((buttons & flags) != Buttons::None);
}

inline bool AllFlagsClear(const Buttons buttons, const Buttons flags)
{
	return ((buttons & flags) == Buttons::None);
}

inline bool AnyFlagsClear(const Buttons buttons, const Buttons flags)
{
	return ((buttons & flags) != flags);
}
