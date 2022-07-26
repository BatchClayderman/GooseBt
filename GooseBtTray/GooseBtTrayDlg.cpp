
// GooseBtTrayDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "GooseBtTray.h"
#include "GooseBtTrayDlg.h"
#include "GooseBtAlertBRDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#ifndef LAUNCH_TIMER
#define LAUNCH_TIMER 3000
#endif

#include <iostream>
using namespace std;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CGooseBtTrayDlg 对话框



CGooseBtTrayDlg::CGooseBtTrayDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_GOOSEBTTRAY_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGooseBtTrayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGooseBtTrayDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_NC, &CGooseBtTrayDlg::OnNotifyIcon)
	ON_UPDATE_COMMAND_UI(ID_TRAY_MAIN, &CGooseBtTrayDlg::OnUpdateTrayMain)
	ON_UPDATE_COMMAND_UI(ID_TRAY_WMIP, &CGooseBtTrayDlg::OnUpdateTrayWmip)
	ON_UPDATE_COMMAND_UI(ID_TRAY_SHOW, &CGooseBtTrayDlg::OnUpdateTrayShow)
	ON_UPDATE_COMMAND_UI(ID_TRAY_QUIT, &CGooseBtTrayDlg::OnUpdateTrayQuit)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_ALERT, &CGooseBtTrayDlg::OnBnClickedButtonAlert)
	ON_STN_CLICKED(IDC_STATIC_RULE_1, &CGooseBtTrayDlg::OnStnClickedStaticRule1)
	ON_STN_CLICKED(IDC_STATIC_RULE_2, &CGooseBtTrayDlg::OnStnClickedStaticRule2)
	ON_STN_CLICKED(IDC_STATIC_RULE_3, &CGooseBtTrayDlg::OnStnClickedStaticRule3)
	ON_EN_CHANGE(IDC_EDIT_HIPS_STATIC, &CGooseBtTrayDlg::OnEnChangeEditHipsStatic)
	ON_EN_CHANGE(IDC_EDIT_HIPS_DYNAMIC, &CGooseBtTrayDlg::OnEnChangeEditHipsDynamic)
	ON_EN_CHANGE(IDC_EDIT_ARTEXT, &CGooseBtTrayDlg::OnEnChangeEditArtext)
	ON_BN_CLICKED(IDC_BUTTON_APPLY_STATIC, &CGooseBtTrayDlg::OnBnClickedButtonApplyStatic)
	ON_BN_CLICKED(IDC_BUTTON_APPLY_DYNAMIC, &CGooseBtTrayDlg::OnBnClickedButtonApplyDynamic)
END_MESSAGE_MAP()


// CGooseBtTrayDlg 消息处理程序

BOOL CGooseBtTrayDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	InitTray();//初始化托盘参数
	SetTimer(LAUNCH_TIMER, LAUNCH_TIMER, NULL);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CGooseBtTrayDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CGooseBtTrayDlg::OnPaint()
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
		int x = (rect.Width() - cxIcon + 1) >> 1;
		int y = (rect.Height() - cyIcon + 1) >> 1;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}


HCURSOR CGooseBtTrayDlg::OnQueryDragIcon()//当用户拖动最小化窗口时系统调用此函数取得光标显示
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGooseBtTrayDlg::OnClose()
{
	ShowWindow(SW_HIDE);
	return;
}

void CGooseBtTrayDlg::OnUpdateTrayQuit(CCmdUI* pCmdUI)
{
	if (MessageBox(L"确实要退出吗？\n退出后反病毒监控将继续运行，但您可能无法及时接收监控信息。", L"提示", MB_OKCANCEL | MB_ICONWARNING) == IDOK)
	{
		Shell_NotifyIcon(NIM_DELETE, &m_stNotifyIcon);//删除托盘图标
		CDialogEx::OnClose();
		exit(0);
	}
	return;
}


void CGooseBtTrayDlg::InitTray()//设定托盘参数
{
	m_stNotifyIcon.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	m_stNotifyIcon.hWnd = this->m_hWnd;
	m_stNotifyIcon.hIcon = m_hIcon/*AfxGetApp()->LoadIcon(IDR_MAINFRAME)*/;//加载托盘图标资源
	m_stNotifyIcon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	m_stNotifyIcon.uCallbackMessage = WM_NC;
	wcscpy_s(m_stNotifyIcon.szTip, TRAY_TIP);//信息提示

	ShowWindow(SW_HIDE);
	Shell_NotifyIcon(NIM_ADD, &m_stNotifyIcon);//在托盘区添加图标
}

LRESULT CGooseBtTrayDlg::OnNotifyIcon(WPARAM wParam, LPARAM lParam)//托盘消息相应
{
	if ((lParam == WM_LBUTTONDOWN) ||
		(lParam == WM_LBUTTONDBLCLK))
	{
		ModifyStyleEx(0, WS_EX_TOPMOST);
		ShowWindow(SW_SHOWNORMAL);
		SetForegroundWindow();//在最前端显示
	}
	else if ((lParam == WM_RBUTTONDOWN))
	{
		CMenu menuPopup;
		menuPopup.LoadMenu(IDR_MENU_TRAY);
		CMenu* subMenu = menuPopup.GetSubMenu(0);
		if (nullptr == subMenu)
			return 0;
		CPoint pos;
		GetCursorPos(&pos);
		SetForegroundWindow();//失去焦点时菜单自动消失
		subMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
			pos.x, pos.y, FromHandle(m_stNotifyIcon.hWnd), NULL);
	}
	return 0;
}

/* 文件相关操作 */
#ifdef WIN32
string GF_GetEXEPath()
{
	char FilePath[MAX_PATH];
	GetModuleFileNameA(NULL, FilePath, MAX_PATH);
	(strrchr(FilePath, '\\'))[1] = 0;
	return string(FilePath);
}
#else
string GF_GetEXEPath()
{
	int rval;
	char link_target[4096];
	char* last_slash;
	size_t result_Length;
	char* result;
	string strExeDir;
	rval = readlink("/proc/self/exe", link_target, 4096);
	if (rval < 0 || rval >= 1024)
		return "";
	link_target[rval] = 0;
	last_slash = strrchr(link_target, '/');
	if (last_slash == 0 || last_slash == link_target)
		return "";
	result_Length = last_slash - link_target;
	result = (char*)malloc(result_Length + 1);
	strncpy(result, link_target, result_Length);
	result[result_Length] = 0;
	strExeDir.append(result);
	strExeDir.append("/");
	free(result);
	return strExeDir;
}
#endif

void CGooseBtTrayDlg::OnUpdateTrayMain(CCmdUI* pCmdUI)
{
	string dirpath = GF_GetEXEPath();
	ShellExecuteA(NULL, NULL, (dirpath + "GooseBtMain.exe").c_str(), "", dirpath.substr(0, dirpath.length() - 1).c_str(), SW_SHOWNORMAL);
	return;
}

void CGooseBtTrayDlg::OnUpdateTrayWmip(CCmdUI* pCmdUI)
{
	string dirpath = GF_GetEXEPath();
	ShellExecuteA(NULL, "open", (dirpath + "wmip.exe").c_str(), "", dirpath.substr(0, dirpath.length() - 1).c_str(), SW_SHOWNORMAL);
	return;
}

void CGooseBtTrayDlg::OnUpdateTrayShow(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	ModifyStyleEx(0, WS_EX_TOPMOST);
	ShowWindow(SW_SHOWNORMAL);
}



void CGooseBtTrayDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == LAUNCH_TIMER)
	{
		ShowWindow(SW_HIDE);
		KillTimer(LAUNCH_TIMER);
	}
	CDialogEx::OnTimer(nIDEvent);
	return;
}


void CGooseBtTrayDlg::OnStnClickedStaticRule1()
{
	AfxMessageBox(L"输入文本运行测试。\n该功能用于测试反病毒播报管道是否正常。");
	return;
}


void CGooseBtTrayDlg::OnStnClickedStaticRule2()
{
	AfxMessageBox(L"静态规则。\n静态规则基于 json，代号如下：\n  0 允许   1 组织   2 拦截   3 挂起\n  4 混淆   5 允许并促进   Q 询问\n\n更多信息请查看帮助文件。");
	return;
}


void CGooseBtTrayDlg::OnStnClickedStaticRule3()
{
	AfxMessageBox(L"动态规则。\n动态规则常用的接口如下：\n  OnCreate OnRead OnWrite\n  OnDelete OnRun OnExit\n\n更多信息请查看帮助文件。");
	return;
}


void CGooseBtTrayDlg::OnEnChangeEditArtext()
{
	CString chketxt;
	GetDlgItem(IDC_EDIT_ARTEXT)->GetWindowTextW(chketxt);
	GetDlgItem(IDC_BUTTON_ALERT)->EnableWindow(chketxt.GetLength() > 0);
	return;
}


void CGooseBtTrayDlg::OnEnChangeEditHipsStatic()
{
	GetDlgItem(IDC_BUTTON_APPLY_STATIC)->EnableWindow(TRUE);
	return;
}


void CGooseBtTrayDlg::OnEnChangeEditHipsDynamic()
{
	GetDlgItem(IDC_BUTTON_APPLY_DYNAMIC)->EnableWindow(TRUE);
	return;
}


void CGooseBtTrayDlg::OnBnClickedButtonAlert()
{
	TCHAR ch[MAX_PATH] = { 0 };
	GetDlgItem(IDC_EDIT_ARTEXT)->GetWindowTextW(ch, MAX_PATH);
	CGooseBtAlertBRDlg* g_pCboutDlg = new CGooseBtAlertBRDlg;
	g_pCboutDlg->title = ch;
	g_pCboutDlg->Create(IDD_ALERT_BR);
	g_pCboutDlg->ShowWindow(SW_HIDE);
	CRect rc, rc_pop;
	SystemParametersInfo(SPI_GETWORKAREA, NULL, (PVOID)&rc, NULL);
	g_pCboutDlg->GetWindowRect(&rc_pop);
	rc.left = rc.right - rc_pop.Width();
	rc.top = rc.bottom - rc_pop.Height();
	//设置弹出窗口在顶层显示
	LONG IStyle = ::GetWindowLong(g_pCboutDlg->m_hWnd, GWL_STYLE);
	::SetWindowLong(g_pCboutDlg->m_hWnd, GWL_STYLE, IStyle & ~WS_CAPTION);
	::SetWindowPos(g_pCboutDlg->GetSafeHwnd(), HWND_TOPMOST, rc.left, rc.top, rc_pop.Width(), rc_pop.Height(), NULL);
	//dlg->MoveWindow(&rc);
	short int mode = 1;
	switch (mode)
	{
	case 1:
		::AnimateWindow(g_pCboutDlg->GetSafeHwnd(), 800, AW_SLIDE | AW_VER_NEGATIVE);
		break;
	case 2:
		::AnimateWindow(g_pCboutDlg->GetSafeHwnd(), 6000, AW_SLIDE | AW_BLEND | AW_ACTIVATE);
		break;
	default:
		break;
	}
	MSG msg;                             //如果是非模态对话框一定要加入这个消息循环
	while (GetMessage(&msg, NULL, 0, 0)) //模态对话就不要了
	{                                    //因为 DoModal 内部执行了这个工作
		TranslateMessage(&msg);          //只要是子线程中要创建窗口都要有一个消息循环
		DispatchMessage(&msg);
	}
	delete g_pCboutDlg;
	return;
}


void CGooseBtTrayDlg::OnBnClickedButtonApplyStatic()
{
	GetDlgItem(IDC_BUTTON_APPLY_STATIC)->EnableWindow(FALSE);
	return;
}


void CGooseBtTrayDlg::OnBnClickedButtonApplyDynamic()
{
	GetDlgItem(IDC_BUTTON_APPLY_DYNAMIC)->EnableWindow(FALSE);
	return;
}


BOOL CGooseBtTrayDlg::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST)
	{
		switch (pMsg->wParam)
		{
		case VK_RETURN:
			if (GetDlgItem(IDC_EDIT_HIPS_STATIC)->GetFocus() || GetDlgItem(IDC_EDIT_HIPS_DYNAMIC)->GetFocus())
				break;
			else
				return TRUE;//没有声音
		case VK_ESCAPE:
			ShowWindow(SW_HIDE);
			return TRUE;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
