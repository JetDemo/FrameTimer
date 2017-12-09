
// FrameTimerDlg.cpp : 实现文件
//

#include "stdafx.h"

#include "FrameTimer.h"
#include "jzl_app_event.h"
#include "FrameTimerDlg.h"
#include "afxdialogex.h"

#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef USE_SDL
#define SDL_E_RENDER (SDL_USEREVENT)

// CFrameTimerDlg 对话框
uint32_t sdl_render(uint32_t interval, void *ctx)
{
    UNREFERENCED_PARAMETER(interval);

    SDL_Event event;

    event.type = SDL_E_RENDER;
    event.user.data1 = ctx;
    SDL_PushEvent(&event);

    return 0;
}
#endif

CFrameTimerDlg::CFrameTimerDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(CFrameTimerDlg::IDD, pParent),
      m_pBoldFont(NULL),
      m_hTimer(NULL),
	  m_hThread(NULL),
      m_bStop(false),
      can_move(false),
	  m_nFrameCnt(0), 
	  m_cmbFpsIdx(0),
	  event_timer_thread()
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    m_pBoldFont = new CFont; //delete it in destructor
    m_pBoldFont->CreateFont(24,0,0,0,900,0,0,0,0,0,0,
                            ANTIALIASED_QUALITY,0,_T("Arial"));

    m_hTimer = CreateWaitableTimer(NULL,0,0);

	init_fps_vec();
}

CFrameTimerDlg::~CFrameTimerDlg()
{
    if(m_pBoldFont)
    {
        delete m_pBoldFont;
        m_pBoldFont = NULL;
    }
}

void CFrameTimerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ST_FRAME_TIMER, m_stFrameTimer);
	DDX_Control(pDX, IDC_CMB_FPS, m_cmbFps);
	DDX_CBIndex(pDX, IDC_CMB_FPS, m_cmbFpsIdx);
}

BEGIN_MESSAGE_MAP(CFrameTimerDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_TIMER()
    ON_WM_DESTROY()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
	ON_CBN_SELCHANGE(IDC_CMB_FPS, &CFrameTimerDlg::OnCbnSelchangeCmbFps)
	ON_MESSAGE(WM_APP_UPDATE_TIMER, OnTimerUpdate)
END_MESSAGE_MAP()


// CFrameTimerDlg 消息处理程序

void CFrameTimerDlg::init_fps_vec()
{
	v_fps.reserve(4);

	v_fps.push_back(25);
	v_fps.push_back(30);
	v_fps.push_back(50);
	v_fps.push_back(60);
}

void CFrameTimerDlg::init_fps_cmb()
{
	CString str;
	
	const int length = static_cast<int>(v_fps.size());
	for(int i = 0 ; i < length; ++i)
	{
		str.Format(_T("%d"),v_fps[i]);

		m_cmbFps.AddString(str);
	}

	m_cmbFps.SetCurSel(m_cmbFpsIdx);
}

BOOL CFrameTimerDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
    //  执行此操作
    SetIcon(m_hIcon, TRUE);			// 设置大图标
    SetIcon(m_hIcon, FALSE);		// 设置小图标

    // TODO: 在此添加额外的初始化代码
    InitUI();

    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFrameTimerDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // 用于绘制的设备上下文

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // 使图标在工作区矩形中居中
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // 绘制图标
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CFrameTimerDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CFrameTimerDlg::InitUI()
{
    m_stFrameTimer.SetColor(RGB(255,0,0));
    m_stFrameTimer.SetGradientColor(RGB(0,0,0));
    m_stFrameTimer.SetTextColor(RGB(0,255,0));

    m_stFrameTimer.SetFont(m_pBoldFont,FALSE);
    m_stFrameTimer.SetTextAlign(1);

    CRect rW;
    GetWindowRect(&rW);

    const int nW = rW.Width();
    const int nH = rW.Height();

    int nDesktopW,nDesktopH;
    GetDesktopResolution(nDesktopW,nDesktopH);

    ::SetWindowPos(m_hWnd,        // handle to window
                   HWND_TOPMOST,  // placement-order handle
                   (nDesktopW-nW)/2,     // horizontal position
                   0,      // vertical position
                   nW,  // width
                   nH, // height
                   SWP_SHOWWINDOW );// window-positioning options);

	init_fps_cmb();

	start_timer();
}

const int CFrameTimerDlg::get_cur_fps() const
{
	return v_fps[m_cmbFpsIdx];
}

void CFrameTimerDlg::start_timer()
{
	stop_timer();

	m_bStop = false;

	LARGE_INTEGER li;
	li.QuadPart = 10*1000;
		
	const int cur_fps = get_cur_fps();	

	SetWaitableTimer(
		m_hTimer,
		&li,
		static_cast<int>(1000.0f/cur_fps),
		0,
		0,
		false);

	m_hThread = (HANDLE)_beginthreadex(0,0,TH_Worker,(void*) this,0,0);
}

void CFrameTimerDlg::stop_timer()
{
	if(m_hThread)
	{
		m_bStop = true;

		WaitForSingleObject(event_timer_thread, INFINITE);
		event_timer_thread.reset_event();
		key_reset_timer();

		m_hThread = NULL;
	}

	if (m_hTimer)
	{
		CancelWaitableTimer(m_hTimer);
	}
}

unsigned int _stdcall CFrameTimerDlg::TH_Worker(PVOID pM)
{
    CFrameTimerDlg* pObj = (CFrameTimerDlg*)pM;
    if (pObj)
    {
        pObj->DoWorker();
    }

    return TRUE;
}

void CFrameTimerDlg::key_reset_timer()
{
	m_nFrameCnt = 0;
}

void CFrameTimerDlg::ui_refresh_timer()
{
    int nCnt = m_nFrameCnt;
	++m_nFrameCnt;
	const int fps = get_cur_fps();
    const int nHour = nCnt /(3600*fps);
    nCnt -= nHour*3600*fps;
    const int nMinuter = (nCnt) /(60*fps);
    nCnt -= nMinuter*60*fps;
    const int nSecond = nCnt/fps;
    nCnt -= nSecond*fps;
    const int nFrame = nCnt;
    strFrameTimer.Format(_T("%02d-%02d-%02d:%02d"),nHour,nMinuter,nSecond,nFrame);

	PostMessage(WM_APP_UPDATE_TIMER, (WPARAM)&strFrameTimer, NULL);
}

void CFrameTimerDlg::DoWorker()
{
	JzlScopedEvent scope(&event_timer_thread);

    while(!m_bStop)
    {
        WaitForSingleObject(m_hTimer,INFINITE);

        /*
        ::SetWindowPos(m_hWnd ,       // handle to window
        	HWND_TOPMOST,  // placement-order handle
        	0,     // horizontal position
        	0,      // vertical position
        	0,  // width
        	0, // height
        	SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOMOVE );// window-positioning options);
        */
        ui_refresh_timer();
    }
}

void CFrameTimerDlg::OnTimer(UINT_PTR nIDEvent)
{
    switch(nIDEvent)
    {
    case TIMER_LATENCY_EXE:
        // KillTimer(TIMER_LATENCY_EXE);
        break;
    }

    CDialogEx::OnTimer(nIDEvent);
}

void CFrameTimerDlg::OnDestroy()
{
    CDialogEx::OnDestroy();

    m_bStop = true;
    CancelWaitableTimer(m_hTimer);    // Stop timer
    CloseHandle(m_hTimer);            // Delete handle
    CloseHandle(m_hThread);
}

BOOL CFrameTimerDlg::PreTranslateMessage(MSG* pMsg)
{
    if( pMsg->message == WM_KEYDOWN )
    {
        if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
        {
            return TRUE;                // Do not process further
        }

		if (pMsg->wParam == 'R')
		{
			key_reset_timer();
		}
    }

    return CWnd::PreTranslateMessage(pMsg);
}

void CFrameTimerDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    CPoint p;
    GetCursorPos(&p);
    CRect r;
    GetWindowRect(&r);
    if (r.PtInRect(p))
    {
        ReleaseCapture();
        SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, 0);
        SendMessage(WM_NCLBUTTONUP, HTCAPTION, 0);
    }

    CDialogEx::OnMouseMove(nFlags, point);
}


void CFrameTimerDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    can_move = true;
    CDialogEx::OnLButtonDown(nFlags, point);
}


void CFrameTimerDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    can_move = false;
    CDialogEx::OnLButtonUp(nFlags, point);
}


void CFrameTimerDlg::OnCbnSelchangeCmbFps()
{
	m_cmbFpsIdx = m_cmbFps.GetCurSel();

	start_timer();
}

LRESULT CFrameTimerDlg::OnTimerUpdate(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	CString* cur_frame_timer = (CString*)wParam;

	m_stFrameTimer.SetWindowText(*cur_frame_timer);

	return 0;
}
