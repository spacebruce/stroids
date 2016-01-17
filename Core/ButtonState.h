#pragma once

#include "Buttons.h"

class ButtonState
{
private:
    const Buttons previous;
    const Buttons current;
  
public:
    inline ButtonState(Buttons current) :
		previous(), current(current)
	{
	}
	
    inline ButtonState(Buttons previous, Buttons current) :
		previous(previous), current(current)
	{
	}

    inline Buttons GetPrevious(void) const
    {
        return this->previous;
    }

    inline Buttons GetCurrent(void) const
    {
        return this->current;
    }

    inline bool IsHeld(const Buttons buttons) const
	{
		return AllFlagsSet(this->current, buttons);
	}
	
    inline bool IsPressed(const Buttons buttons) const
	{
		return AnyFlagsClear(this->previous, buttons)
			&& AllFlagsSet(this->current, buttons);
	}
	
    inline bool IsReleased(const Buttons buttons) const
	{
		return AllFlagsSet(this->previous, buttons)
			&& AnyFlagsClear(this->current, buttons);
	}
};
