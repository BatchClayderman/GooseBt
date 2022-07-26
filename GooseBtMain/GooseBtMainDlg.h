
// GooseBtMainDlg.h: 头文件
//

#pragma once

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#ifndef FocusTime
#define FocusTime 5
#endif
#ifndef BmpLog
#define BmpLog "BmpPath.log"
#endif
#ifdef WM_SHOWTASK
#undef WM_SHOWTASK
#endif
#define WM_SHOWTASK (WM_USER + 10)
#ifdef UM_TRAYNOTIFY
#undef UM_TRAYNOTIFY
#endif
#ifdef WM_SHOWTASK
#define UM_TRAYNOTIFY WM_SHOWTASK
#else
#define UM_TRAYNOTIFY (WM_USER + 11)
#endif

#ifndef ERROR_OPEN_CONSOLE
#define ERROR_OPEN_CONSOLE 2
#endif
#ifndef ERROR_OPEN_SERVICE
#define ERROR_OPEN_SERVICE 3
#endif
#ifndef ERROR_QUERY_VALUE
#define ERROR_QUERY_VALUE 4
#endif
#ifndef ERROR_START_SERVICE
#define ERROR_START_SERVICE 11
#endif
#ifndef ERROR_STOP_SERVICE
#define ERROR_STOP_SERVICE 21
#endif
#ifndef ERROR_INSTALL_SERVICE
#define ERROR_INSTALL_SERVICE 31
#endif
#ifndef ERROR_DELETE_SERVICE
#define ERROR_DELETE_SERVICE 41
#endif
#ifndef EXIT_OUT_OF_SCHEDULE
#define EXIT_OUT_OF_SCHEDULE 9009
#endif


// CGooseBtMainDlg 对话框
class CGooseBtMainDlg : public CDialogEx
{
// 构造
public:
	CGooseBtMainDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GOOSEBTMAIN_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	int CGooseBtMainDlg::OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedWmip();
	afx_msg void OnBnClickedScan();
	afx_msg void OnBnClickedFix();
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedSettings();
	afx_msg void OnBnClickedRealtime();
	afx_msg void OnBnClickedSevere();
	afx_msg void OnBnClickedHigh();
	afx_msg void OnBnClickedMiddle();
	afx_msg void OnBnClickedLow();
	afx_msg void OnBnClickedIdle();
	afx_msg void InitFaces();
	afx_msg void OnStnClickedPic();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
	afx_msg void OnBnClickedCoremode();
	afx_msg void OnStnClickedDesc();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnBnClickedChangebmp();
	afx_msg LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam);
	afx_msg void CGooseBtMainDlg::Refresh();
	afx_msg void OnBnClickedRefresh();
	afx_msg void OnBnClickedWinDos();
};
