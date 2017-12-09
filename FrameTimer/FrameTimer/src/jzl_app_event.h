#pragma once

#include "jzl_non_copyable.h"
#include "jzl_win32_event.h"

class JzlScopedEvent
{
public:
    explicit JzlScopedEvent(JzlWin32Event* pEvent) :
        m_pEvent(pEvent)
    {
        m_pEvent->reset_event();
    }

    ~JzlScopedEvent()
    {
        m_pEvent->set_event();
    }

private:
    JzlWin32Event* m_pEvent;

private:
    DISALLOW_COPY_AND_ASSIGN(JzlScopedEvent);
};
