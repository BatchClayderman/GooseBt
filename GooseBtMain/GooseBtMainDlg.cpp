
// GooseBtMainDlg.cpp: 实现文件
//

#include "pch.h"
#include "GooseBtMain.h"
#include "GooseBtMainDlg.h"
#include "afxdialogex.h"

using namespace std;
short int MonLevel = IDLE;
short int RemoteState = IDLE;
short int LoseFocus = NULL;//失去焦点
NOTIFYICONDATA nid;//托盘消息

_TCHAR Desc[6][120] = {
	L"      反病毒服务已关闭，如果计算机上没有其它第三代或以上反病毒产品，请尽快开启。",//关闭
	L"      发生文件读写删除执行时，相似度 60% 及以上 的文件将会被播报为病毒或木马。请注意，在该模式下，我们将难以防范未知病毒。",//低
	L"      发生文件读写删除执行时，相似度 80% 及以上 的文件将会被播报为病毒或木马。",//中
	L"      发生文件读写删除执行时，相似度 95% 及以上 的文件将会被播报为病毒或木马。请注意，在该模式下，误报会比较严重。",//高
	L"      对一切含有文件读写删除执行的文件进行播报，由用户选择是否处理。",//严格
	L"      您正在使用核心功能模式，如要切换到其它监控模式，请先在主功能区选择关闭。",//实时
};

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


WCHAR* CCharToLpcwstr(const char* strings)//转换宽字符
{
	int ASize = MultiByteToWideChar(CP_ACP, 0, strings, -1, NULL, 0);
	WCHAR* PwszUnicode = (wchar_t*)malloc(ASize * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, strings, -1, PwszUnicode, ASize);
	return PwszUnicode;
}

BOOL FindFirstFileExists(LPCTSTR lpPath, BOOL dwFilter)//检查文件是否存在
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(lpPath, &fd);
	BOOL bFilter = (FALSE == dwFilter) ? TRUE : fd.dwFileAttributes & dwFilter;
	BOOL RetValue = ((hFind != INVALID_HANDLE_VALUE) && bFilter) ? TRUE : FALSE;
	if (RetValue == FALSE)
	{
		TCHAR SystemPath[MAX_PATH];
		GetSystemDirectory(SystemPath, MAX_PATH);
		TCHAR* _str = new TCHAR[(size_t)lstrlen(SystemPath) + (size_t)lstrlen(L"\\") + (size_t)lstrlen(lpPath) + 1];
		if (_str)
		{
			_str[0] = _T('\0');
			lstrcat(_str, SystemPath);
			lstrcat(_str, L"\\");
			lstrcat(_str, lpPath);
			WIN32_FIND_DATA _fd;
			HANDLE _hFind = FindFirstFile(_str, &_fd);
			BOOL _bFilter = (FALSE == dwFilter) ? TRUE : _fd.dwFileAttributes & dwFilter;
			BOOL _RetValue = ((_hFind != INVALID_HANDLE_VALUE) && _bFilter) ? TRUE : FALSE;
			RetValue = _RetValue;
			delete[]_str;
			FindClose(_hFind);
		}
	}
	FindClose(hFind);
	return RetValue;
}

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
public:
	afx_msg void OnBnClickedWmip();
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_BN_CLICKED(ID_WMIP, &CAboutDlg::OnBnClickedWmip)
	ON_MESSAGE(UM_TRAYNOTIFY, &CGooseBtMainDlg::OnTrayNotify)
END_MESSAGE_MAP()


// CGooseBtMainDlg 对话框



CGooseBtMainDlg::CGooseBtMainDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_GOOSEBTMAIN_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGooseBtMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL CGooseBtMainDlg::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CDialogEx::PreCreateWindow(cs))
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying…
	//禁止调整窗口大小
	cs.style &= ~WS_THICKFRAME;
	return TRUE;
}

int CGooseBtMainDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == EOF)
		return EOF;

	// TODO:  在此添加您专用的创建代码
	//注册热键 ALT + U(u)
	RegisterHotKey(this->GetSafeHwnd(), 1001, MOD_ALT, 'U');
	RegisterHotKey(this->GetSafeHwnd(), 1002, MOD_ALT, 'u');
	return 0;
}


BEGIN_MESSAGE_MAP(CGooseBtMainDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_WMIP, &CGooseBtMainDlg::OnBnClickedWmip)
	ON_BN_CLICKED(IDC_SCAN, &CGooseBtMainDlg::OnBnClickedScan)
	ON_BN_CLICKED(IDC_FIX, &CGooseBtMainDlg::OnBnClickedFix)
	ON_BN_CLICKED(IDC_START, &CGooseBtMainDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_SETTINGS, &CGooseBtMainDlg::OnBnClickedSettings)
	ON_BN_CLICKED(IDC_REALTIME, &CGooseBtMainDlg::OnBnClickedRealtime)
	ON_BN_CLICKED(IDC_SEVERE, &CGooseBtMainDlg::OnBnClickedSevere)
	ON_BN_CLICKED(IDC_HIGH, &CGooseBtMainDlg::OnBnClickedHigh)
	ON_BN_CLICKED(IDC_MIDDLE, &CGooseBtMainDlg::OnBnClickedMiddle)
	ON_BN_CLICKED(IDC_LOW, &CGooseBtMainDlg::OnBnClickedLow)
	ON_BN_CLICKED(IDC_IDLE, &CGooseBtMainDlg::OnBnClickedIdle)
	ON_STN_CLICKED(IDC_PIC, &CGooseBtMainDlg::OnStnClickedPic)
	ON_WM_CTLCOLOR()
	ON_WM_HOTKEY()
	ON_BN_CLICKED(IDC_COREMODE, &CGooseBtMainDlg::OnBnClickedCoremode)
	ON_STN_CLICKED(IDC_DESC, &CGooseBtMainDlg::OnStnClickedDesc)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_KILLFOCUS()
	ON_BN_CLICKED(IDC_CHANGEBMP, &CGooseBtMainDlg::OnBnClickedChangebmp)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_REFRESH, &CGooseBtMainDlg::OnBnClickedRefresh)
	ON_BN_CLICKED(IDC_Win_DOS, &CGooseBtMainDlg::OnBnClickedWinDos)
END_MESSAGE_MAP()


// CGooseBtMainDlg 消息处理程序

BOOL CGooseBtMainDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	//ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	//ASSERT(IDM_ABOUTBOX < 0xF000);

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

	/*
	nid.hWnd = GetSafeHwnd();
	nid.uCallbackMessage = UM_TRAYNOTIFY;
	// Set tray icon and tooltip
	nid.hIcon = m_hIcon;
	// Set tray notification tip information
	CString strToolTip = _T("GooseBt 第四代反病毒软件");
	_tcsncpy_s(nid.szTip, strToolTip, strToolTip.GetLength());
	Shell_NotifyIcon(NIM_ADD, &nid);
	*/
	if (!FindFirstFileExists(CCharToLpcwstr((GF_GetEXEPath() + "\\Win DOS\\Welcome.bat\"").c_str()), FALSE))
		GetDlgItem(IDC_Win_DOS)->EnableWindow(FALSE);
	InitFaces();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CGooseBtMainDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CGooseBtMainDlg::OnPaint()
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
		return;
		/* 背景生成 */
		CPaintDC dc(this);
		CRect rect;
		GetClientRect(&rect);//获取对话框长宽      
		CDC dcBmp;//定义并创建一个内存设备环境
		dcBmp.CreateCompatibleDC(&dc);//创建兼容性 DC
		CBitmap bmpBackground;
		bmpBackground.LoadBitmap(IDB_BACKGROUND1);//载入资源中图片
		BITMAP m_bitmap;//图片变量               
		bmpBackground.GetBitmap(&m_bitmap);//将图片载入位图中
		CBitmap* pbmpOld = dcBmp.SelectObject(&bmpBackground);//将位图选入临时内存设备环境
		dc.StretchBlt(0, 0, rect.Width(), rect.Height(), &dcBmp, 0, 0, m_bitmap.bmWidth, m_bitmap.bmHeight, SRCCOPY);//调用函数显示图片 StretchBlt 显示形状可变
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CGooseBtMainDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL IsAboveVistaVersion(DWORD Win)//判断系统版本是否为 Win 或以上
{
	if (Win <= _WIN32_WINNT)//返回的是 bool
		return TRUE;
	else
		return FALSE;
}

WCHAR* CharToLpcwstr(char* strings)//转换宽字符
{
	int ASize = MultiByteToWideChar(CP_ACP, 0, strings, -1, NULL, 0);
	WCHAR* PwszUnicode = (wchar_t*)malloc(ASize * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, strings, -1, PwszUnicode, ASize);
	return PwszUnicode;
}

BOOL IsAdmin()//是否具备管理员权限
{
	BOOL b;
	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	PSID AdministratorsGroup;
	b = AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &AdministratorsGroup);
	if (b)
	{
		if (!CheckTokenMembership(NULL, AdministratorsGroup, &b))
			b = FALSE;
		FreeSid(AdministratorsGroup);
	}
	return b;
}

BOOL GetAdminPermission(char* exe, char* param, char* path, bool bShow)//以管理员权限启动
{
	SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
	sei.lpVerb = TEXT("runas");
	sei.lpFile = CharToLpcwstr(exe);
	sei.lpParameters = CharToLpcwstr(param);
	if (bShow)
		sei.nShow = SW_SHOWNORMAL;
	else
		sei.nShow = SW_HIDE;
	if (ShellExecuteEx(&sei))
		return TRUE;
	else
		return FALSE;
}

BOOL OpenExeA(char* exe, char* param, char* path, bool bShow, bool openfirst)
{
	if (IsAboveVistaVersion(_WIN32_WINNT_WIN7) && !IsAdmin())
		return GetAdminPermission(exe, param, path, bShow);
	HINSTANCE result = 0;
	int ishow = SW_SHOW;

	if (bShow)
		ishow = SW_SHOW;
	else
		ishow = SW_HIDE;

	if (openfirst)
		result = ::ShellExecuteA(NULL, "open", param, NULL, NULL, ishow);
	else
		result = ::ShellExecuteA(NULL, NULL, exe, param, path, ishow);
	return (long long int)result <= HINSTANCE_ERROR ? FALSE : TRUE;
}

void CAboutDlg::OnBnClickedWmip()
{
	return;
}

void CGooseBtMainDlg::OnBnClickedWmip()
{
	char RunPath[MAX_PATH] = { 0 };
	strcpy_s(RunPath, GF_GetEXEPath().c_str());
	if (!OpenExeA("wmip.exe", NULL, RunPath, true, false))
		MessageBoxA(NULL, "启动 wmip 失败！", "GooseBtMain", MB_OK | MB_ICONERROR | MB_TOPMOST);
	return;
}

void CGooseBtMainDlg::OnBnClickedScan()
{
	char RunPath[MAX_PATH] = { 0 };
	strcpy_s(RunPath, GF_GetEXEPath().c_str());
	if (!OpenExeA("Scanner.exe", NULL, RunPath, true, false))
		MessageBoxA(NULL, "启动 Scanner 失败！", "GooseBtMain", MB_OK | MB_ICONERROR | MB_TOPMOST);
	return;
}

void CGooseBtMainDlg::OnBnClickedStart()
{
	char RunPath[MAX_PATH] = { 0 };
	strcpy_s(RunPath, GF_GetEXEPath().c_str());
	if (!OpenExeA("SVCLauncher.exe", "/Start", RunPath, false, false))
		MessageBoxA(NULL, "启动服务启动程序失败！", "GooseBtMain", MB_OK | MB_ICONERROR | MB_TOPMOST);
	return;
}

void CGooseBtMainDlg::OnBnClickedFix()
{
	char RunPath[MAX_PATH] = { 0 };
	strcpy_s(RunPath, GF_GetEXEPath().c_str());
	if (!OpenExeA("SVCLauncher.exe", "/Fix", RunPath, false, false))
		MessageBoxA(NULL, "启动服务修复程序失败！\n如有授权提示，请确保您已授权运行。", "GooseBtMain", MB_OK | MB_ICONERROR | MB_TOPMOST);
	return;
}

void CGooseBtMainDlg::OnBnClickedSettings()
{
	if (ApplySettings(MonLevel))
	{
		GetDlgItem(IDC_SETTINGS)->EnableWindow(FALSE);
		Refresh();
	}
	else
		MessageBoxA(NULL, "试图写入的管道不存在！", "应用设置失败", MB_OK | MB_ICONERROR | MB_TOPMOST);
	return;
}

void CGooseBtMainDlg::OnBnClickedRealtime()
{
	MonLevel = REALTIME;
	GetDlgItem(IDC_DESC)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_DESC)->SetWindowTextW(Desc[MonLevel]);
	GetDlgItem(IDC_DESC)->ShowWindow(SW_NORMAL);
	UpdateData(false);
	GetDlgItem(IDC_SETTINGS)->EnableWindow(RemoteState == REALTIME ? FALSE : TRUE);
	return;
}

void CGooseBtMainDlg::OnBnClickedSevere()
{
	MonLevel = SEVERE;
	GetDlgItem(IDC_DESC)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_DESC)->SetWindowTextW(Desc[MonLevel]);
	GetDlgItem(IDC_DESC)->ShowWindow(SW_NORMAL);
	UpdateData(false);
	GetDlgItem(IDC_SETTINGS)->EnableWindow(RemoteState == SEVERE ? FALSE : TRUE);
	return;
}

void CGooseBtMainDlg::OnBnClickedHigh()
{
	MonLevel = HIGH;
	GetDlgItem(IDC_DESC)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_DESC)->SetWindowTextW(Desc[MonLevel]);
	GetDlgItem(IDC_DESC)->ShowWindow(SW_NORMAL);
	UpdateData(false);
	GetDlgItem(IDC_SETTINGS)->EnableWindow(RemoteState == HIGH ? FALSE : TRUE);
	return;
}

void CGooseBtMainDlg::OnBnClickedMiddle()
{
	MonLevel = MIDDLE;
	GetDlgItem(IDC_DESC)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_DESC)->SetWindowTextW(Desc[MonLevel]);
	GetDlgItem(IDC_DESC)->ShowWindow(SW_NORMAL);
	UpdateData(false);
	GetDlgItem(IDC_SETTINGS)->EnableWindow(RemoteState == MIDDLE ? FALSE : TRUE);
	return;
}

void CGooseBtMainDlg::OnBnClickedLow()
{
	MonLevel = LOW;
	GetDlgItem(IDC_DESC)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_DESC)->SetWindowTextW(Desc[MonLevel]);
	GetDlgItem(IDC_DESC)->ShowWindow(SW_NORMAL);
	UpdateData(false);
	GetDlgItem(IDC_SETTINGS)->EnableWindow(RemoteState == LOW ? FALSE : TRUE);
	return;
}

void CGooseBtMainDlg::OnBnClickedIdle()
{
	MonLevel = IDLE;
	GetDlgItem(IDC_DESC)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_DESC)->SetWindowTextW(Desc[MonLevel]);
	GetDlgItem(IDC_DESC)->ShowWindow(SW_NORMAL);
	UpdateData(false);
	GetDlgItem(IDC_SETTINGS)->EnableWindow(RemoteState == IDLE ? FALSE : TRUE);
	return;
}

void CGooseBtMainDlg::OnBnClickedCoremode()
{
	GetDlgItem(IDC_COREMODE)->EnableWindow(FALSE);
	if (RemoteState == REALTIME)
	{
		MonLevel = SEVERE;
		if (ApplySettings(MonLevel))
		{
			MessageBoxA(NULL, "核心模式已关闭，已将模式过度到严格。", "GooseBt 第四代反病毒软件", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
			LoseFocus--;
			GetDlgItem(IDC_COREMODE)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_COREMODE)->SetWindowText(L"开启核心模式");
			GetDlgItem(IDC_COREMODE)->ShowWindow(SW_NORMAL);
			CButton* radio;
			const int allButton[5] = { IDC_SEVERE,IDC_HIGH,IDC_MIDDLE,IDC_LOW,IDC_IDLE };
			for (int i = 0; i < 5; ++i)
			{
				radio = (CButton*)GetDlgItem(allButton[i]);
				radio->EnableWindow(TRUE);
			}
		}
		else
		{
			MessageBoxA(NULL, "核心模式关闭失败，请重启计算机以解决此问题。", "GooseBt 第四代反病毒软件", MB_OK | MB_ICONERROR | MB_TOPMOST);
			LoseFocus--;
		}
		GetDlgItem(IDC_COREMODE)->EnableWindow(TRUE);
		Refresh();
	}
	else
	{
		if (MessageBoxA(NULL, "该功能一旦开启，未保存的数据将会丢失，请保存好所有数据，并点击“确定”继续。请注意，这是本程序的核心驱动功能，该模式一旦开启，计算机将无法创建任何进程或读写任意文件。", "注意", MB_OKCANCEL | MB_ICONWARNING | MB_TOPMOST) == IDOK)
		{
			MonLevel = REALTIME;
			if (ApplySettings(MonLevel))
			{
				MessageBoxA(NULL, "核心模式已开启，请保持主界面打开。", "GooseBt 第四代反病毒软件", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
				LoseFocus--;
				CButton* radio;
				const int allButton[6] = { IDC_REALTIME,IDC_SEVERE,IDC_HIGH,IDC_MIDDLE,IDC_LOW,IDC_IDLE };
				for (int i = 0; i < 6; ++i)
				{
					radio = (CButton*)GetDlgItem(allButton[i]);
					radio->EnableWindow(FALSE);
				}
				Refresh();
				GetDlgItem(IDC_COREMODE)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_COREMODE)->SetWindowText(L"关闭核心模式");
				GetDlgItem(IDC_COREMODE)->ShowWindow(SW_NORMAL);
			}
			else
			{
				MessageBoxA(NULL, "核心模式开启失败，如果您的计算机上拥有其它第三代或以上反病毒软件，该功能将可能开启失败。", "GooseBt 第四代反病毒软件", MB_OK | MB_ICONERROR | MB_TOPMOST);
				LoseFocus--;
			}
			GetDlgItem(IDC_COREMODE)->EnableWindow(TRUE);
		}
		else
			GetDlgItem(IDC_COREMODE)->EnableWindow(TRUE);
	}
	return;
}


void CGooseBtMainDlg::InitFaces()//初始化
{
	setlocale(LC_CTYPE, "");
	/*string PicPath = GF_GetEXEPath();
	PicPath += bgName;
	CString strImgPath = PicPath.c_str();
	CImage image;
	image.Load(strImgPath);
	CRect rtPic;
	GetDlgItem(IDC_PIC)->GetClientRect(rtPic);
	CDC* pDc = GetDlgItem(IDC_PIC)->GetDC();
	pDc->SetStretchBltMode(STRETCH_HALFTONE);
	//int ModeOld = SetStretchBltMode(pDc->m_hDC, STRETCH_HALFTONE);
	image.Draw(pDc->m_hDC, rtPic);
	//SetStretchBltMode(pDc->m_hDC, ModeOld);
	ReleaseDC(pDc);*/
	RemoteState = GetRemoteState();
	if (RemoteState < IDLE || RemoteState > REALTIME)
	{
		if (MessageBoxA(NULL, "管道不安全，点击“确定”修复服务。", "严重错误", MB_OKCANCEL | MB_ICONWARNING | MB_TOPMOST) == IDOK)
			OnBnClickedFix();
		else
			CDialog::OnCancel();
	}
	else
	{
		GetDlgItem(IDC_DESC)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DESC)->SetWindowTextW(Desc[RemoteState]);
		GetDlgItem(IDC_DESC)->ShowWindow(SW_NORMAL);
	}
	Refresh();
	if (RemoteState == REALTIME)
		GetDlgItem(IDC_COREMODE)->SetWindowTextW(L"关闭核心模式");
	else
	{
		GetDlgItem(IDC_REALTIME)->EnableWindow(FALSE);
		GetDlgItem(IDC_SEVERE)->EnableWindow(TRUE);
		GetDlgItem(IDC_HIGH)->EnableWindow(TRUE);
		GetDlgItem(IDC_MIDDLE)->EnableWindow(TRUE);
		GetDlgItem(IDC_LOW)->EnableWindow(TRUE);
		GetDlgItem(IDC_IDLE)->EnableWindow(TRUE);
	}
	return;
}

void CGooseBtMainDlg::OnStnClickedPic()
{
	// TODO: 在此添加控件通知处理程序代码
	return;
}

HBRUSH CGooseBtMainDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	// TODO:  在此更改 DC 的任何特性
	CFont m_font_title, m_font_checking;//设置字体大小和字体
	m_font_title.CreatePointFont(200, L"Times New Roman");
	m_font_checking.CreatePointFont(100, L"宋体");
	if (pWnd->GetDlgCtrlID() == IDC_MainText)//标题
	{
		pDC->SetBkMode(TRANSPARENT);
		hbr = (HBRUSH)GetStockObject(NULL_BRUSH);
		pDC->SelectObject(&m_font_title);//设置字体
		pDC->SetTextColor(RGB(128, 64, 0));//设置字体颜色
	}
	if (pWnd->GetDlgCtrlID() == IDC_CHECKING)//状态
	{
		pDC->SetBkMode(TRANSPARENT);
		hbr = (HBRUSH)GetStockObject(NULL_BRUSH);
		pDC->SelectObject(&m_font_checking);
		switch (RemoteState)
		{
		case REALTIME:
		case SEVERE:
		case HIGH:
		case MIDDLE:
		case LOW:
			pDC->SetTextColor(RGB(0, 255, 0));
			break;
		case IDLE:
			pDC->SetTextColor(RGB(255, 255, 0));
			break;
		default:
			pDC->SetTextColor(RGB(255, 0, 0));
			break;
		}
	}
	if (pWnd->GetDlgCtrlID() == IDC_FUNCTION)//主功能区
	{
		pDC->SetBkMode(TRANSPARENT);
		hbr = (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	if (pWnd->GetDlgCtrlID() == IDC_MODE)//模式
	{
		pDC->SetBkMode(TRANSPARENT);
		hbr = (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	if (pWnd->GetDlgCtrlID() == IDC_DESC)//文字描述
	{
		pDC->SetBkMode(TRANSPARENT);
		hbr = (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	if (pWnd->GetDlgCtrlID() == IDC_REALTIME || 
		pWnd->GetDlgCtrlID() == IDC_SEVERE ||
		pWnd->GetDlgCtrlID() == IDC_HIGH ||
		pWnd->GetDlgCtrlID() == IDC_MIDDLE ||
		pWnd->GetDlgCtrlID() == IDC_LOW ||
		pWnd->GetDlgCtrlID() == IDC_IDLE
		)//调控范围
	{
		pDC->SetBkMode(TRANSPARENT);
		if (pWnd->GetDlgCtrlID() == IDC_IDLE)
			pDC->SetTextColor(RGB(255, 255, 0));
		else
			pDC->SetTextColor(RGB(0, 255, 0));
		CRect rc;
		pWnd->GetWindowRect(&rc);
		ScreenToClient(&rc);
		CDC* dc = GetDC();
		pDC->BitBlt(0, 0, rc.Width(), rc.Height(), dc, rc.left, rc.top, SRCCOPY);//把父窗口背景先画到按钮上
		ReleaseDC(dc);
		hbr = (HBRUSH) ::GetStockObject(NULL_BRUSH);
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

void CGooseBtMainDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nHotKeyId == 1001 || nHotKeyId == 1002)
	{
		nid.hIcon = NULL;
		Shell_NotifyIcon(NIM_DELETE, &nid);//清除托盘图标
		SetForegroundWindow();
		ShowWindow(SW_SHOWNORMAL);
		SendMessage(WM_SYSCOMMAND, SC_RESTORE);
		return;
	}
	CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
	return;
}

void CGooseBtMainDlg::OnStnClickedDesc()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CGooseBtMainDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (MessageBox(_T("您点击了关闭按钮，是否只是隐藏主界面？\n请注意，退出并不代表反病毒服务停止了运行。\n事实上，反病毒服务仍在后台运行。"), _T("GooseBt 第四代反病毒软件"), MB_YESNOCANCEL | MB_ICONQUESTION | MB_TOPMOST))
	{
	case IDYES:
		SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_HIDEWINDOW);
		ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);//移除任务栏图标显示
		nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
		nid.hWnd = this->m_hWnd;
		nid.uID = IDR_MAINFRAME;
		nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		nid.uCallbackMessage = WM_SHOWTASK;//自定义的消息名称 
		nid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
		lstrcpy(nid.szTip, L"GooseBt 第四代反病毒软件");//信息提示条
		Shell_NotifyIcon(NIM_ADD, &nid);//在托盘区添加图标
		ShowWindow(SW_HIDE);//隐藏主窗口
		break;
	case IDNO:
		CDialogEx::OnClose();
		break;
	default:
		break;
	}
	return;
}

LRESULT CGooseBtMainDlg::OnTrayNotify(WPARAM wParam, LPARAM lParam)
{
	UINT uMsg = (UINT)lParam;
	switch (uMsg)
	{
	/*
	case WM_RBUTTONUP://右键单击
	{
		CMenu menuTray;
		CPoint point;
		int id;
		GetCursorPos(&point);

		menuTray.LoadMenu(IDR_MENU_TRAY);
		id = menuTray.GetSubMenu(0)->TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
#if 0
		CString strInfo;
		strInfo.Format(L"menuid %d", id);
		LPCTSTR strtmp;
		strtmp = strInfo.GetBuffer(0);
		MessageBox(strtmp, L"test");
#endif
		switch (id) {
		case IDR_TRAY_EXIT:
			OnOK();
			break;
		case IDR_TRAY_RESTORE:
			SetForegroundWindow();//窗口前端显示
			ShowWindow(SW_SHOWNORMAL);
			break;
		default:
			break;
		}
		break;
	}
	*/
	case WM_LBUTTONUP://左键单击
	case WM_LBUTTONDBLCLK://左键双击
		nid.hIcon = NULL;
		Shell_NotifyIcon(NIM_DELETE, &nid);//清除托盘图标
		SetForegroundWindow();
		ShowWindow(SW_SHOWNORMAL);
		SendMessage(WM_SYSCOMMAND, SC_RESTORE);
		break;
	default:
		break;
	}
	return 0;
}

void CGooseBtMainDlg::OnDestroy()
{

	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	return;
}


void CGooseBtMainDlg::OnKillFocus(CWnd* pNewWnd)
{
	CDialogEx::OnKillFocus(pNewWnd);

	// TODO: 在此处添加消息处理程序代码
	if ((++LoseFocus) >= FocusTime)
	{
		MessageBox(_T("当前窗口频繁失去焦点，建议运行弱人工智能防御！"), _T("GooseBt 第四代反病毒软件"), MB_OK | MB_ICONWARNING | MB_TOPMOST);
		LoseFocus = NULL;
	}
	return;
}


void CGooseBtMainDlg::OnBnClickedChangebmp()
{
	// TODO: 在此添加控件通知处理程序代码
	/*
	TCHAR szFolderPath[MAX_PATH] = { 0 };
	CString strFolderPath = L"";
	BROWSEINFO sInfo;
	ZeroMemory(&sInfo, sizeof(BROWSEINFO));

	sInfo.pidlRoot = 0;
	sInfo.lpszTitle = L"请选择一个文件夹：";
	sInfo.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_EDITBOX;
	sInfo.lpfn = NULL;

	// 显示文件夹选择对话框
	LPITEMIDLIST lpidlBrowse = SHBrowseForFolder(&sInfo);
	if (lpidlBrowse != NULL)
	{
		// 取得文件夹名
		if (SHGetPathFromIDList(lpidlBrowse, szFolderPath))
		{
			strFolderPath = szFolderPath;
		}
	}
	if (lpidlBrowse != NULL)
	{
		AfxMessageBox(L"获取目录路径成功！");
		CoTaskMemFree(lpidlBrowse);
	}
	*/
#ifdef UNICODE
	static _TCHAR BASED_CODE szFilter[] = L"BMP格式|*.bmp|jpg格式|*.jpg|jpeg格式|*.jpeg||";//以||结尾否则会产生乱码
	_TCHAR defaultExName[] = L"bmp";
	_TCHAR defaultFileName[MAX_PATH] = L"";
#else
	static char BASED_CODE szFilter[] = "BMP格式|*.bmp|jpg格式|*.jpg|jpeg格式|*.jpeg||";
	char defaultExName[] = "bmp";
	char defaultFileName[MAX_PATH] = "";
#endif
	FILE* fp;
	fopen_s(&fp, (GF_GetEXEPath() + BmpLog).c_str(), "r");
	if (fp)
	{
#ifdef UNICODE
		fgetws(defaultFileName, MAX_PATH, fp);
#else
		fgets(defaultFileName, MAX_PATH, fp);
#endif
		rewind(fp);
		fclose(fp);
	}
	else
	{
#ifdef WIN32
		GetModuleFileName(NULL, defaultFileName, MAX_PATH);
#endif
	}
	bool isIncluded = false;
#ifdef UNICODE
	for (unsigned int i = lstrlen(defaultFileName); i > 0; --i)
		if (defaultFileName[i] == '\\')
		{
			(wcsrchr(defaultFileName, '\\'))[1] = 0;
			isIncluded = true;
			break;
		}
	if (!isIncluded)//数据非法
#ifdef WIN32
		GetModuleFileName(NULL, defaultFileName, MAX_PATH);
#else
		lstrcpy(defaultFileName, L"");
#endif
#else
	for (unsigned int i = strlen(defaultFileName); i > 0; --i)
		if (defaultFileName[i] == '\\')
		{
			(strrchr(defaultFileName, '\\'))[1] = 0;
			isIncluded = true;
			break;
		}
	if (!isIncluded)//数据非法
#ifdef WIN32
		GetModuleFileName(NULL, defaultFileName, sizeof(defaultFileName));
#else
		strcpy_s(defaultFileName, "");
#endif
#endif
	LoseFocus--;//防止产生误报
	CFileDialog dlg(TRUE, defaultExName, defaultFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	if (dlg.DoModal() != IDOK)
		return;
	fopen_s(&fp, (GF_GetEXEPath() + BmpLog).c_str(), "w");
	CString CS_name = dlg.GetPathName();
	LPTSTR p_name = new _TCHAR[(size_t)CS_name.GetLength() + 1];
	lstrcpy(p_name, CS_name);
	if (fp)//尝试记录下最后一次使用的路径
	{
#ifdef UNICODE
		fputws(p_name, fp);
#else
		fputs(p_name, fp);
#endif
		fclose(fp);
	}
	CImage img;
	img.Load(p_name);
	CBitmap bitmap;
	bitmap.Attach(img.Detach());

	/*
	CDC memDC;
	CBitmap bmp;
	memDC.CreateCompatibleDC(pDC);
	bmp.CreateCompatibleBitmap(pDC, 32, 32);
	memDC.SelectObject(&bmp);
	pDC->BitBlt(0, 0, 32, 32, &memDC, 0, 0, SRCCOPY);
	*/

	/*
	CPaintDC dc(this);
	CRect rect;
	GetClientRect(&rect);//获取对话框长宽      
	CDC dcBmp;//定义并创建一个内存设备环境
	dcBmp.CreateCompatibleDC(&dc);//创建兼容性 DC
	CBitmap bmpBackground;
	bmpBackground.LoadBitmap(IDB_BACKGROUND1);//载入资源中图片
	BITMAP m_bitmap;//图片变量               
	bmpBackground.GetBitmap(&bitmap);//将图片载入位图中
	CBitmap* pbmpOld = dcBmp.SelectObject(&bmpBackground);//将位图选入临时内存设备环境
	dc.StretchBlt(0, 0, rect.Width(), rect.Height(), &dcBmp, 0, 0, m_bitmap.bmWidth, m_bitmap.bmHeight, SRCCOPY);//调用函数显示图片 StretchBlt 显示形状可变
	*/
	return;
}

void CGooseBtMainDlg::Refresh()
{
	RemoteState = GetRemoteState();
	CButton* radio;
	const int allButton[6] = { IDC_REALTIME,IDC_SEVERE,IDC_HIGH,IDC_MIDDLE,IDC_LOW,IDC_IDLE };
	for (int i = 0; i < 6; ++i)
	{
		radio = (CButton*)GetDlgItem(allButton[i]);
		radio->SetCheck(FALSE);
	}
	switch (RemoteState)
	{
	case REALTIME:
		radio = (CButton*)GetDlgItem(IDC_REALTIME);
		GetDlgItem(IDC_CHECKING)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECKING)->SetWindowText(L"当前监控模式：实时");
		GetDlgItem(IDC_CHECKING)->ShowWindow(SW_NORMAL);
		break;
	case SEVERE:
		radio = (CButton*)GetDlgItem(IDC_SEVERE);
		GetDlgItem(IDC_CHECKING)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECKING)->SetWindowText(L"当前监控模式：严格");
		GetDlgItem(IDC_CHECKING)->ShowWindow(SW_NORMAL);
		break;
	case HIGH:
		radio = (CButton*)GetDlgItem(IDC_HIGH);
		GetDlgItem(IDC_CHECKING)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECKING)->SetWindowText(L"当前监控模式：高");
		GetDlgItem(IDC_CHECKING)->ShowWindow(SW_NORMAL);
		break;
	case MIDDLE:
		radio = (CButton*)GetDlgItem(IDC_MIDDLE);
		GetDlgItem(IDC_CHECKING)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECKING)->SetWindowText(L"当前监控模式：中");
		GetDlgItem(IDC_CHECKING)->ShowWindow(SW_NORMAL);
		break;
	case LOW:
		radio = (CButton*)GetDlgItem(IDC_LOW);
		GetDlgItem(IDC_CHECKING)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECKING)->SetWindowText(L"当前监控模式：低");
		GetDlgItem(IDC_CHECKING)->ShowWindow(SW_NORMAL);
		break;
	case IDLE:
		radio = (CButton*)GetDlgItem(IDC_IDLE);
		GetDlgItem(IDC_CHECKING)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECKING)->SetWindowText(L"当前监控模式：关闭");
		GetDlgItem(IDC_CHECKING)->ShowWindow(SW_NORMAL);
		break;
	case EOF:
		radio = (CButton*)GetDlgItem(IDC_IDLE);
		GetDlgItem(IDC_CHECKING)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECKING)->SetWindowText(L"获取相关信息失败！");
		GetDlgItem(IDC_CHECKING)->ShowWindow(SW_NORMAL);
		break;
	default:
		radio = (CButton*)GetDlgItem(IDC_IDLE);
		GetDlgItem(IDC_CHECKING)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECKING)->SetWindowText(L"当前监控模式：数据异常");
		GetDlgItem(IDC_CHECKING)->ShowWindow(SW_NORMAL);
		break;
	}
	radio->SetCheck(TRUE);
	if (RemoteState >= IDLE && RemoteState <= REALTIME)
	{
		GetDlgItem(IDC_DESC)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DESC)->SetWindowTextW(Desc[RemoteState]);
		GetDlgItem(IDC_DESC)->ShowWindow(SW_NORMAL);
	}
	return;
}

void CGooseBtMainDlg::OnBnClickedRefresh()
{
	Refresh();
	return;
}

void CGooseBtMainDlg::OnBnClickedWinDos()
{
	if (!FindFirstFileExists(CCharToLpcwstr((GF_GetEXEPath() + "\\Win DOS\\Welcome.bat\"").c_str()), FALSE))
	{
		GetDlgItem(IDC_Win_DOS)->EnableWindow(FALSE);
		return;
	}
	string cmd = "start /realtime \"\" \"" + GF_GetEXEPath() + "\\Win DOS\\Welcome.bat\"";
	system(cmd.c_str());
	return;
}
