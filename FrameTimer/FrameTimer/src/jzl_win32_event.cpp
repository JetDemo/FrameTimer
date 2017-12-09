#include "stdafx.h"
#include "jzl_win32_event.h"

JzlWin32Event::JzlWin32Event()
{
    hEvent = CreateEvent(
                 NULL,               // default security attributes
                 TRUE,               // manual-reset event
                 FALSE,              // initial state is nonsignaled
                 NULL// object name
             );
}

JzlWin32Event::~JzlWin32Event()
{
    CloseHandle(hEvent);
    hEvent = NULL;
}

void JzlWin32Event::set_event()
{
    SetEvent(hEvent);
}

void JzlWin32Event::reset_event()
{
    ResetEvent(hEvent);
}

void JzlWin32Event::pulse_event()
{
    PulseEvent(hEvent);
}
