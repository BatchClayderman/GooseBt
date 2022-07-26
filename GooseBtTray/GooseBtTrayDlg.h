
// GooseBtTrayDlg.h: 头文件
//

#pragma once

#define WM_NC (WM_USER + 1) //托盘消息
const CString TRAY_TIP("GooseBt 反病毒软件");

// CGooseBtTrayDlg 对话框
class CGooseBtTrayDlg : public CDialogEx
{
// 构造
public:
	CGooseBtTrayDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GOOSEBTTRAY_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnClose();//WM_CLOSE消息响应
	afx_msg LRESULT OnNotifyIcon(WPARAM wParam, LPARAM lParam);//托盘消息相应
	afx_msg void OnUpdateTrayMain(CCmdUI* pCmdUI);//对托盘鼠标右键消息ID_TRAY_MAIN的响应
	afx_msg void OnUpdateTrayWmip(CCmdUI* pCmdUI);//对托盘鼠标右键消息ID_TRAY_WMIP的响应
	afx_msg void OnUpdateTrayShow(CCmdUI* pCmdUI);//对托盘鼠标右键消息ID_TRAY_SHOW的响应
	afx_msg void OnUpdateTrayQuit(CCmdUI* pCmdUI);//对托盘鼠标右键消息ID_TRAY_QUIT的响应

private:
	NOTIFYICONDATA m_stNotifyIcon = { NULL };// 托盘图标
	void InitTray();//初始化托盘图标
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonAlert();
	afx_msg void OnStnClickedStaticRule1();
	afx_msg void OnStnClickedStaticRule2();
	afx_msg void OnStnClickedStaticRule3();
	afx_msg void OnEnChangeEditHipsStatic();
	afx_msg void OnEnChangeEditHipsDynamic();
	afx_msg void OnEnChangeEditArtext();
	afx_msg void OnBnClickedButtonApplyStatic();
	afx_msg void OnBnClickedButtonApplyDynamic();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
