// CGooseBtAlertBRDlg.cpp: 实现文件
//

#include "pch.h"
#include "GooseBtTray.h"
#include "GooseBtAlertBRDlg.h"
#include "afxdialogex.h"


// CGooseBtAlertBRDlg 对话框

IMPLEMENT_DYNAMIC(CGooseBtAlertBRDlg, CDialogEx)
int pauseTime = 30;
CGooseBtAlertBRDlg::CGooseBtAlertBRDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ALERT_BR, pParent)
{
}

CGooseBtAlertBRDlg::~CGooseBtAlertBRDlg()
{
}

BOOL CGooseBtAlertBRDlg::OnInitDialog()
{
	ModifyStyle(WS_SYSMENU, 0);
	SetMenu(NULL);
	GetDlgItem(IDC_STATIC_TITLE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_TITLE)->SetWindowText(this->title);
	GetDlgItem(IDC_STATIC_TITLE)->ShowWindow(SW_SHOWNORMAL);
	CDialogEx::OnInitDialog();
	((CButton*)GetDlgItem(IDC_RADIO_1))->SetCheck(TRUE);
	SetTimer(pauseTime * 1000, pauseTime * 1000, NULL);
	this->SetFocus();
	return FALSE;  // return TRUE unless you set the focus to a control
}

void CGooseBtAlertBRDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGooseBtAlertBRDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_ENTER, &CGooseBtAlertBRDlg::OnBnClickedButtonEnter)
	ON_BN_CLICKED(IDC_CHECK_2, &CGooseBtAlertBRDlg::OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_CHECK_1, &CGooseBtAlertBRDlg::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK_0, &CGooseBtAlertBRDlg::OnBnClickedCheck0)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RADIO_0, &CGooseBtAlertBRDlg::OnBnClickedRadio0)
	ON_BN_CLICKED(IDC_RADIO_1, &CGooseBtAlertBRDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO_2, &CGooseBtAlertBRDlg::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO_3, &CGooseBtAlertBRDlg::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO_4, &CGooseBtAlertBRDlg::OnBnClickedRadio4)
	ON_BN_CLICKED(IDC_RADIO_5, &CGooseBtAlertBRDlg::OnBnClickedRadio5)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CGooseBtAlertBRDlg 消息处理程序


void CGooseBtAlertBRDlg::OnBnClickedButtonEnter()
{
	CGooseBtAlertBRDlg::~CGooseBtAlertBRDlg();
}

void CGooseBtAlertBRDlg::resetTimer()
{
	KillTimer(pauseTime * 1000);
	SetTimer(pauseTime * 1000, pauseTime * 1000, NULL);
}

void CGooseBtAlertBRDlg::OnBnClickedCheck2()
{
	CGooseBtAlertBRDlg::resetTimer();
	if (((CButton*)GetDlgItem(IDC_CHECK_2))->GetCheck())
	{
		((CButton*)GetDlgItem(IDC_CHECK_1))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_CHECK_0))->SetCheck(TRUE);
	}
	this->mode = ((CButton*)GetDlgItem(IDC_CHECK_2))->GetCheck() << 2
		| ((CButton*)GetDlgItem(IDC_CHECK_1))->GetCheck() << 1
		| ((CButton*)GetDlgItem(IDC_CHECK_1))->GetCheck();
	return;
}


void CGooseBtAlertBRDlg::OnBnClickedCheck1()
{
	CGooseBtAlertBRDlg::resetTimer();
	if (((CButton*)GetDlgItem(IDC_CHECK_1))->GetCheck())
		((CButton*)GetDlgItem(IDC_CHECK_0))->SetCheck(TRUE);
	else
		((CButton*)GetDlgItem(IDC_CHECK_2))->SetCheck(FALSE);
	this->mode = ((CButton*)GetDlgItem(IDC_CHECK_2))->GetCheck() << 2
		| ((CButton*)GetDlgItem(IDC_CHECK_1))->GetCheck() << 1
		| ((CButton*)GetDlgItem(IDC_CHECK_1))->GetCheck();
	return;
}


void CGooseBtAlertBRDlg::OnBnClickedCheck0()
{
	CGooseBtAlertBRDlg::resetTimer();
	if (!((CButton*)GetDlgItem(IDC_CHECK_0))->GetCheck())
	{
		((CButton*)GetDlgItem(IDC_CHECK_1))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_CHECK_2))->SetCheck(FALSE);
	}
	this->mode = ((CButton*)GetDlgItem(IDC_CHECK_2))->GetCheck() << 2
		| ((CButton*)GetDlgItem(IDC_CHECK_1))->GetCheck() << 1
		| ((CButton*)GetDlgItem(IDC_CHECK_1))->GetCheck();
	return;
}


void CGooseBtAlertBRDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == pauseTime * 1000)
	{
		ShowWindow(SW_HIDE);
		KillTimer(pauseTime * 1000);
	}
	CDialogEx::OnTimer(nIDEvent);
	return;
}


void CGooseBtAlertBRDlg::OnBnClickedRadio0()
{
	CGooseBtAlertBRDlg::resetTimer();
	this->method = 0;
	return;
}


void CGooseBtAlertBRDlg::OnBnClickedRadio1()
{
	CGooseBtAlertBRDlg::resetTimer();
	this->method = 1;
	return;
}


void CGooseBtAlertBRDlg::OnBnClickedRadio2()
{
	CGooseBtAlertBRDlg::resetTimer();
	this->method = 2;
	return;
}


void CGooseBtAlertBRDlg::OnBnClickedRadio3()
{
	CGooseBtAlertBRDlg::resetTimer();
	this->method = 3;
	return;
}


void CGooseBtAlertBRDlg::OnBnClickedRadio4()
{
	CGooseBtAlertBRDlg::resetTimer();
	this->method = 4;
	return;
}


void CGooseBtAlertBRDlg::OnBnClickedRadio5()
{
	CGooseBtAlertBRDlg::resetTimer();
	this->method = 5;
	return;
}


void CGooseBtAlertBRDlg::OnClose()
{
	return;
}


BOOL CGooseBtAlertBRDlg::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST)//键盘事件
	{
		int IDC[6] = { IDC_RADIO_0, IDC_RADIO_1, IDC_RADIO_2, IDC_RADIO_3, IDC_RADIO_4, IDC_RADIO_5 };
		switch (pMsg->wParam)
		{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
			((CButton*)GetDlgItem(IDC[pMsg->wParam - '0']))->SetCheck(TRUE);
			CGooseBtAlertBRDlg::resetTimer();
			return true;
		case VK_RETURN:
			CGooseBtAlertBRDlg::OnBnClickedButtonEnter();
			return true;
		default:
			break;
		}
	}
	// TODO: 在此添加专用代码和/或调用基类
	return CDialogEx::PreTranslateMessage(pMsg);
}
