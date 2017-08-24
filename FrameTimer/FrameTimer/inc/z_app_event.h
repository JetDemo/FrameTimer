#pragma once

class ScopedEvent
{
public:
    explicit ScopedEvent(CEvent* pEvent) :
        m_pEvent(pEvent)
    {
        m_pEvent->ResetEvent();
    }

    ~ScopedEvent()
    {
        m_pEvent->SetEvent();
    }

private:
    CEvent* m_pEvent;
private:
    DISALLOW_COPY_AND_ASSIGN(ScopedEvent);
};
