
// FrameTimer.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"


// CFrameTimerApp:
// FrameTimer.cpp
//

class CFrameTimerApp : public CWinApp
{
private:
    DISALLOW_COPY_AND_ASSIGN(CFrameTimerApp);

public:
    CFrameTimerApp();

public:
    virtual BOOL InitInstance();

    DECLARE_MESSAGE_MAP()
};

extern CFrameTimerApp theApp;
