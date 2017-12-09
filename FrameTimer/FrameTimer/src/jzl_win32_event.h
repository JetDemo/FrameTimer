#pragma once

#include "stdafx.h"

class JzlWin32Event
{
private:
    HANDLE hEvent;

public:
    JzlWin32Event();
    ~JzlWin32Event();

    void set_event();
    void reset_event();
    void pulse_event();
    operator HANDLE() const
    {
        return hEvent;
    }
};
