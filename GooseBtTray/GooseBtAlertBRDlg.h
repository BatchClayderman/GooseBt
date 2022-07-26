#pragma once


// CGooseBtAlertBRDlg 对话框

class CGooseBtAlertBRDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CGooseBtAlertBRDlg)

public:
	CGooseBtAlertBRDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CGooseBtAlertBRDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ALERT_BR };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	LPCTSTR title = L"可疑程序正在尝试打鹅！";
	LPCTSTR description = L"";
	short int method = 1;
	short int mode = 0;

	afx_msg void OnBnClickedButtonEnter();
	afx_msg void OnBnClickedCheck2();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedCheck0();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	void resetTimer();
	afx_msg void OnBnClickedRadio0();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio4();
	afx_msg void OnBnClickedRadio5();
	afx_msg void OnClose();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
