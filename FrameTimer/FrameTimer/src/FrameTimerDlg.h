
// FrameTimerDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "Utility.h"
#include "GradientStatic.h"

const int TIMER_LATENCY_EXE = 3927;

// CFrameTimerDlg 对话框
class CFrameTimerDlg : public CDialogEx
{
// 构造
public:
    CFrameTimerDlg(CWnd* pParent = NULL);	// 标准构造函数
    ~CFrameTimerDlg();

// 对话框数据
    enum { IDD = IDD_FRAMETIMER_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

private:
    DISALLOW_COPY_AND_ASSIGN(CFrameTimerDlg);
    void InitUI();
    void UnInit();

    static unsigned int _stdcall TH_Worker(PVOID pM);
    void DoWorker();

    void ui_refresh_timer();

    static unsigned int _stdcall sdl_event(PVOID pM);
    void do_sdl_event();
    void schedule_render(const int delay);

// data
private:
    CFont* m_pBoldFont;
    CString strFrameTimer;
    HANDLE m_hTimer;
    HANDLE m_hThread;

    bool m_bStop;
    bool can_move;
    int fps;

// 实现
protected:
    HICON m_hIcon;

    // 生成的消息映射函数
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
public:
    CGradientStatic m_stFrameTimer;
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnDestroy();
    BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
