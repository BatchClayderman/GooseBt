// ClickProgrameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ClickPrograme.h"
#include "ClickProgrameDlg.h"


#include <math.h>
#include <stdio.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CClickProgrameDlg dialog

CClickProgrameDlg::CClickProgrameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClickProgrameDlg::IDD, pParent)
	, m_iLoopCount(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pWnd = NULL;
	//m_hWnd = GetSafeHwnd();
	m_nHotKeyID = 0;
	m_iRecordState = 0;
	m_pdlgInput = NULL;
	m_pdlgInput = new CDlgInput( );
	//m_pdlgInput = new CDlgInput(m_hWnd);
	m_tagEventRecord.InitData();
//	m_nIndex = -1;
}

void CClickProgrameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ACTION, m_wndListAction);
	DDX_Text(pDX, IDC_EDIT_INPUT, m_iLoopCount);
	DDV_MinMaxInt(pDX, m_iLoopCount, 1, 1000000000);
}

BEGIN_MESSAGE_MAP(CClickProgrameDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_SETTEXT , OnSetText )
	ON_MESSAGE( WM_HOTKEY , OnHotKey )//消息映射 
	ON_MESSAGE( WM_USER_EDIT_END ,OnRefreshData )
	ON_MESSAGE( WM_USER_MOUSEMOTION , OnMouseMotion )
	ON_MESSAGE( WM_USER_MOUSECONTROL_LEFTDOWN , OnMouseControlLeftDown )
	ON_MESSAGE( WM_USER_MOUSECONTROL_LEFTUP ,OnMouseControlLeftUp )
	ON_MESSAGE( WM_USER_MOUSECONTROL_MOVE ,OnMouseControlMove )
	ON_MESSAGE( WM_USER_MOUSECONTROL_RIGHTDOWN ,OnMouseControlRightDown )
	ON_MESSAGE( WM_USER_MOUSECONTROL_RIGHTUP ,OnMouseControlRightUp )
	ON_MESSAGE( WM_USER_MOUSECONTROL_MIDDOWN , OnMouseControlMidDown )
	ON_MESSAGE( WM_USER_MOUSECONTROL_MIDDOWN , OnMouseControlMidUp )
	
	ON_WM_DESTROY()
ON_NOTIFY(NM_CLICK, IDC_LIST_ACTION, &CClickProgrameDlg::OnNMClickListAction)
ON_WM_SETCURSOR()
ON_BN_CLICKED(IDC_BUT_RUN, &CClickProgrameDlg::OnBnClickedButRun)
ON_BN_CLICKED(IDC_BUT_STOP, &CClickProgrameDlg::OnBnClickedButStop)
ON_BN_CLICKED(IDC_BUT_DELETE, &CClickProgrameDlg::OnBnClickedButDelete)
END_MESSAGE_MAP()


// CClickProgrameDlg message handlers

LRESULT CClickProgrameDlg::OnSetText( WPARAM wp , LPARAM lp )
{
	AfxMessageBox( _T("recive message") );
	return 1;
}
BOOL CClickProgrameDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

//	GetDlgItem( IDC_INFO )->SetWindowText( _T("1.移动鼠标至拖拽的起点。\n2.Shift+F1:开始/停止左键随机测试.\n3.Shift+F2:开始/停止右键随机测试。"));
	SetHotKey();

	m_wndListAction.SetExtendedStyle( m_wndListAction.GetExtendedStyle()|
										LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT  );
	m_wndListAction.InsertColumn( 0  , _T("序号") , LVCFMT_LEFT , 50 );
	m_wndListAction.InsertColumn( 1  , _T("事件") , LVCFMT_LEFT , 60 );
	m_wndListAction.InsertColumn( 2  , _T("动作类型") , LVCFMT_LEFT , 60 );
	m_wndListAction.InsertColumn( 3  , _T("延时") , LVCFMT_LEFT , 50 );
	m_wndListAction.InsertColumn( 4  , _T("循环次数") , LVCFMT_LEFT , 80 );
	m_wndListAction.InsertColumn( 5  , _T("起始坐标") , LVCFMT_LEFT , 80 );
	m_wndListAction.InsertColumn( 6  , _T("终止坐标") , LVCFMT_LEFT , 80 );

//	g_MainHwnd = m_hWnd;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CClickProgrameDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CClickProgrameDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

long RandCursor( long CurMax , long CurMin ) 
{ 
	long  nRand = rand()%(CurMax) + CurMin;
	int   nFlag   =   rand()   %   2;   
	nFlag   =   (nFlag   ==   1)   ?   1   :   -1;   
	nRand   *=   nFlag;   
	return nRand;
} 

// 在对象框初始化的时候调用它 
void CClickProgrameDlg::SetHotKey(void)
{
	::RegisterHotKey(m_hWnd , CG_ACTION_RUN ,	MOD_SHIFT , KeyInputStart ); 
	::RegisterHotKey(m_hWnd , CG_ACTION_PAUSE ,  MOD_SHIFT , KeyInputPause ); 
	::RegisterHotKey(m_hWnd , CG_ACTION_STOP ,  MOD_SHIFT , KeyInputStop ); 
	::RegisterHotKey(m_hWnd , CG_ACTION_RECORDMOUSE_CLICK ,  MOD_SHIFT , KeyInputRecordClick );
	::RegisterHotKey(m_hWnd , CG_ACTION_RECORDMOUSE_MOVE ,  MOD_SHIFT , KeyInputRecordMove );
}

LRESULT CClickProgrameDlg::OnHotKey(WPARAM wp,LPARAM lp)
{
	switch ( wp )
	{
	case CG_ACTION_RUN:
		{
			//AfxMessageBox(_T("运行"));
			m_eState = Run;
			RunAllAction();
			break;
		}
	case CG_ACTION_PAUSE:
		{
			m_eState = Pause;
			AfxMessageBox(_T("暂停"));
			break;
		}
	case CG_ACTION_STOP:
		{
			m_eState = Stop;
			//AfxMessageBox(_T("停止"));
			break;
		}
	case CG_ACTION_RECORDMOUSE_MOVE:
		{
			//AfxMessageBox(_T("录制"));
			RecordAction();
			break;
		}
	case CG_ACTION_RECORDMOUSE_CLICK:
		{
			//AfxMessageBox(_T("录制"));
			RecordActionB();
			break;
		}
	default:
		{
			break;
		}
	}

	return 0;
}

void CClickProgrameDlg::OnDestroy()
{
	CDialog::OnDestroy();

	UnregisterHotKey(m_hWnd,CG_ACTION_RUN); 
	UnregisterHotKey(m_hWnd,CG_ACTION_PAUSE ); 
	UnregisterHotKey(m_hWnd,CG_ACTION_STOP ); 
	UnregisterHotKey(m_hWnd,CG_ACTION_RECORDMOUSE_MOVE ); 
	UnregisterHotKey(m_hWnd,CG_ACTION_RECORDMOUSE_CLICK ); 


	for ( int nIndex = 0 ; nIndex < m_wndListAction.GetItemCount(); nIndex++)
	{
		tagActionEvent * pNewEvent;
		pNewEvent = (tagActionEvent *)m_wndListAction.GetItemData(nIndex);//m_listActionEvent.RemoveHead( );
		if ( pNewEvent != NULL )
		{
			delete pNewEvent;
			pNewEvent = NULL;

		}
	}
	if ( m_pdlgInput )
	{
		delete m_pdlgInput;
		m_pdlgInput = NULL;
	}
	// TODO: Add your message handler code here
}

void CClickProgrameDlg::SetStopFlag(void)
{
	//m_bRuned = FALSE;

	//m_iRunType = -1;
	m_eState = Stop;
//	mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
}


int CClickProgrameDlg::StartRandom( tagActionEvent * pNewEvent )
{
	// TODO: Add your control notification handler code here
	CRect rect;
	this->GetWindowRect(&rect);
	int   cx   =   GetSystemMetrics(   SM_CXSCREEN   );   
	int   cy   =   GetSystemMetrics(   SM_CYSCREEN   );

	srand( (unsigned)time( NULL ) ); 
	if ( pNewEvent == NULL )
	{
		return 0;
	}
	int iRandomTime = pNewEvent->m_iLoopTime;
	if ( pNewEvent->m_tagMouseEvent.m_iActionType & MOUSE_MASK_LEFT )
	{//左键随机
		SetCursorPos( pNewEvent->m_tagMouseEvent.m_StartPoint.x , pNewEvent->m_tagMouseEvent.m_StartPoint.y );
		mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
	} 
	else
	{//右键随机
		SetCursorPos( pNewEvent->m_tagMouseEvent.m_StartPoint.x , pNewEvent->m_tagMouseEvent.m_StartPoint.y );
		mouse_event(MOUSEEVENTF_RIGHTDOWN,0,0,0,0);
	}
	while( ( iRandomTime > 0) && (m_eState == Run) )
	{
		PeepMessage();
		long Cusor_X = RandCursor( CG_CURSOR_RANDOM_X , 0 );
		long Cusor_Y = RandCursor( CG_CURSOR_RANDOM_Y , 0 );
		TRACE(" Curor x : %d  , Cursor y: %d \n" ,Cusor_X , Cusor_Y );

		mouse_event(  MOUSEEVENTF_MOVE , Cusor_X ,Cusor_Y ,0 ,0 );

		Sleep(pNewEvent->m_iSleepTime);
		iRandomTime--;
	}
	SetCursorPos( pNewEvent->m_tagMouseEvent.m_EndPoint.x , pNewEvent->m_tagMouseEvent.m_EndPoint.y );
	return 1;
}

int CClickProgrameDlg::SingleClick( tagActionEvent * pNewEvent )
{
	if ( pNewEvent == NULL )
	{
		return 0;
	}

	for ( int i = 0 ; i < pNewEvent->m_iLoopTime ; i++ )
	{
		SetCursorPos( pNewEvent->m_tagMouseEvent.m_StartPoint.x , pNewEvent->m_tagMouseEvent.m_StartPoint.y );
		if ( pNewEvent->m_tagMouseEvent.m_iActionType & MOUSE_MASK_LEFT )
		{//左键
			mouse_event( MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP , 0 , 0 , 0 , 0 );
		}
		else
		{
			mouse_event( MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP , 0 , 0 , 0 , 0 );
		}	
		Sleep( pNewEvent->m_iSleepTime );
		PeepMessage();
		if ( m_eState != Run )
		{
			break;
		}
	}

	return 1;
}

int CClickProgrameDlg::DoubleClick( tagActionEvent * pNewEvent )
{
	if ( pNewEvent == NULL )
	{
		return 0;
	}
	for ( int i = 0 ; i < pNewEvent->m_iLoopTime ; i++ )
	{
		SetCursorPos( pNewEvent->m_tagMouseEvent.m_StartPoint.x , pNewEvent->m_tagMouseEvent.m_StartPoint.y );
		if ( pNewEvent->m_tagMouseEvent.m_iActionType & MOUSE_MASK_LEFT )
		{//左键
			mouse_event( MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP , 0 , 0 , 0 , 0 );
			mouse_event( MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP , 0 , 0 , 0 , 0 );
		}
		else
		{
			mouse_event( MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP , 0 , 0 , 0 , 0 );
			mouse_event( MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP , 0 , 0 , 0 , 0 );
		}	
		Sleep( pNewEvent->m_iSleepTime );
		PeepMessage();
		if ( m_eState != Run )
		{
			break;
		}
	}

	return 1;
}


int CClickProgrameDlg::MouseMove( tagActionEvent * pNewEvent )
{
	if ( pNewEvent == NULL )
	{
		return 0;
	}
	for ( int i = 0 ; i < pNewEvent->m_iLoopTime ; i++ )
	{
		CPoint offPoint;
		offPoint = pNewEvent->m_tagMouseEvent.m_EndPoint - pNewEvent->m_tagMouseEvent.m_StartPoint;
		SetCursorPos( pNewEvent->m_tagMouseEvent.m_StartPoint.x , pNewEvent->m_tagMouseEvent.m_StartPoint.y );
		if ( pNewEvent->m_tagMouseEvent.m_iActionType & MOUSE_MASK_LEFT )
		{//左键
			mouse_event( MOUSEEVENTF_LEFTDOWN , 0 , 0 , 0 , 0 );

		}
		else
		{
			mouse_event( MOUSEEVENTF_RIGHTDOWN , 0 , 0 , 0 , 0 );
			//mouse_event( MOUSEEVENTF_MOVE  , offPoint.x , offPoint.y , 0 , 0 );
		}	
		Sleep( Delay );
		mouse_event(  MOUSEEVENTF_MOVE  , offPoint.x , offPoint.y , 0 , 0 );
		//mouse_event(  MOUSEEVENTF_RIGHTDOWN|MOUSEEVENTF_MOVE  , offPoint.x , offPoint.y , 0 , 0 );
		if ( pNewEvent->m_tagMouseEvent.m_iActionType & MOUSE_MASK_LEFT )
		{//左键
			mouse_event( MOUSEEVENTF_LEFTUP , 0 , 0 , 0 , 0 );

		}
		else
		{
			mouse_event( MOUSEEVENTF_RIGHTUP , 0 , 0 , 0 , 0 );
		}	
		Sleep( pNewEvent->m_iSleepTime );
		PeepMessage();

		if ( m_eState != Run )
		{
			break;
		}
	}
	return 1;
}

int CClickProgrameDlg::RecordAction()
{
	// TODO: Add your control notification handler code here
	//HINSTANCE hinst;            // handle to current instance 
	
	if (  m_iRecordState == 0)
	{//起始位置
		m_iRecordState = 1;

		CPoint CurPoint;
		GetCursorPos( &CurPoint );
		//ScreenToClient( &CurPoint );

		m_tagEventRecord.m_tagMouseEvent.m_StartPoint = CurPoint;

		//m_hCurs = LoadCursor(NULL, IDC_CROSS ); // Create a standard hourglass cursor. 

		//SetCursor(m_hCurs); 

	}
	else
	{//终止位置
		m_iRecordState = 0;

		CPoint CurPoint;
		GetCursorPos( &CurPoint );
		//ScreenToClient( &CurPoint );

		m_tagEventRecord.m_tagMouseEvent.m_EndPoint = CurPoint;

		tagActionEvent * pNewEvent;
		pNewEvent = new tagActionEvent;
		pNewEvent->m_iEventType = ACTIONTYPE_MOUSE;//_T("鼠标动作");
		pNewEvent->m_tagMouseEvent.m_StartPoint = m_tagEventRecord.m_tagMouseEvent.m_StartPoint;
		pNewEvent->m_tagMouseEvent.m_EndPoint = m_tagEventRecord.m_tagMouseEvent.m_EndPoint;
		pNewEvent->m_tagMouseEvent.m_iActionType = MOUSE_RIGHT_MOVE;
		pNewEvent->m_iSleepTime = DefaultSleepTime;
		pNewEvent->m_iLoopTime = DefaultLoopTime;
		InsertAction( pNewEvent );
		
		//DestroyCursor( m_hCurs );
	}

	return 1;
}

int CClickProgrameDlg::RecordActionB()
{
	// TODO: Add your control notification handler code here
	//HINSTANCE hinst;            // handle to current instance 

	CPoint CurPoint;
	GetCursorPos( &CurPoint );

	m_tagEventRecord.m_tagMouseEvent.m_StartPoint = CurPoint;
	m_tagEventRecord.m_tagMouseEvent.m_EndPoint = CurPoint;

	tagActionEvent * pNewEvent;
	pNewEvent = new tagActionEvent;
	pNewEvent->m_iEventType = ACTIONTYPE_MOUSE;//_T("鼠标动作");
	pNewEvent->m_tagMouseEvent.m_StartPoint = m_tagEventRecord.m_tagMouseEvent.m_StartPoint;
	pNewEvent->m_tagMouseEvent.m_EndPoint = m_tagEventRecord.m_tagMouseEvent.m_EndPoint;
	pNewEvent->m_tagMouseEvent.m_iActionType = MOUSE_LEFT_CLICK;
	pNewEvent->m_iSleepTime = DefaultSleepTime;
	pNewEvent->m_iLoopTime = DefaultLoopTime;
	InsertAction( pNewEvent );

	return 1;
}

int CClickProgrameDlg::InsertAction( tagActionEvent * pNewEvent )
{
	CString strTemp;
	switch ( pNewEvent->m_iEventType )
	{
	case ACTIONTYPE_MOUSE:
		{
			int nIndex = m_wndListAction.GetItemCount();
			TRACE("nIndex : %d \n" , nIndex );
			strTemp.Format(_T("%d") , nIndex );
			LV_ITEM lvi;
			lvi.mask=LVIF_TEXT|LVIF_PARAM;
			lvi.iSubItem=0;
			lvi.pszText =  strTemp.GetBuffer();//_T("鼠标事件");//pNewEvent->m_strActionName.GetBuffer(256);
			lvi.iItem = nIndex;
			lvi.lParam = (LPARAM)pNewEvent;
			m_wndListAction.InsertItem( &lvi );
			ReFreshList();
			break;
		}
	case ACTIONTYPE_KEYBOARD:
		{
			break;
		}
	}
	return 1;
}

int CClickProgrameDlg::RunAllAction()
{
	UpdateData(TRUE);
	for ( int n = 0 ; n < m_iLoopCount; n++ )
	{
		for ( int i = 0 ; i < m_wndListAction.GetItemCount(); i++ )
		{
			if ( m_eState == Stop )
			{
				break;
			}
			tagActionEvent * pNewEvent;
			pNewEvent = (tagActionEvent *) m_wndListAction.GetItemData(i);//m_listActionEvent.GetNext( pos );

			if ( pNewEvent )
			{
				if ( pNewEvent->m_tagMouseEvent.m_iActionType & MOUSE_MASK_RANDOM )
				{//随机动作
					StartRandom( pNewEvent );
				}
				else if ( pNewEvent->m_tagMouseEvent.m_iActionType & MOUSE_MASK_DBCLICK )
				{//双击动作
					DoubleClick( pNewEvent );
				}
				else if ( pNewEvent->m_tagMouseEvent.m_iActionType & MOUSE_MASK_CLICK )
				{//单击动作
					SingleClick( pNewEvent );
				}
				else if ( pNewEvent->m_tagMouseEvent.m_iActionType & MOUSE_MASK_MOVE )
				{
					MouseMove( pNewEvent );
					//MOVE
				}
			}
		}
	}



	return 1;
}

LRESULT CClickProgrameDlg::OnRefreshData(WPARAM wp,LPARAM lp)
{
	int nIndex = (int)wp;
	TRACE( _T("recevie item: %d \n") , nIndex );
	if ( nIndex >= 0 )
	{
		tagActionEvent * pNewEvent;
		pNewEvent = ( tagActionEvent* )lp;//(tagActionEvent*) m_wndListAction.GetItemData( m_nIndex );
		if ( pNewEvent )
		{
			//m_wndListAction.SetItemData( nIndex ,(DWORD) pNewEvent );
			ReFreshList();
		}
		pNewEvent = NULL;
	}
	return 1;
}

void CClickProgrameDlg::ReFreshList()
{
	CString strTemp;
	tagActionEvent * pNewEvent;
	for ( int nIndex = 0 ; nIndex < m_wndListAction.GetItemCount(); nIndex++ )
	{
		pNewEvent = (tagActionEvent *)m_wndListAction.GetItemData(nIndex);
		if ( pNewEvent )
		{
			if ( pNewEvent->m_iEventType == ACTIONTYPE_MOUSE )
			{
				m_wndListAction.SetItemText( nIndex , 1 , _T("鼠标动作") );
				if ( pNewEvent->m_tagMouseEvent.m_iActionType & MOUSE_MASK_RANDOM )
				{//随机动作
					if ( pNewEvent->m_tagMouseEvent.m_iActionType & MOUSE_MASK_LEFT )
					{
						m_wndListAction.SetItemText( nIndex , 2 , _T("左键随机") );
					} 
					else
					{
						m_wndListAction.SetItemText( nIndex , 2 , _T("右键随机") );
					}
				}
				else if ( pNewEvent->m_tagMouseEvent.m_iActionType & MOUSE_MASK_DBCLICK )
				{//双击动作
					if ( pNewEvent->m_tagMouseEvent.m_iActionType & MOUSE_MASK_LEFT )
					{
						m_wndListAction.SetItemText( nIndex , 2 , _T("左键双击") );
					} 
					else
					{
						m_wndListAction.SetItemText( nIndex , 2 , _T("右键双击") );
					}
				}
				else if ( pNewEvent->m_tagMouseEvent.m_iActionType & MOUSE_MASK_CLICK )
				{//单击动作
					if (pNewEvent->m_tagMouseEvent.m_iActionType & MOUSE_MASK_LEFT )
					{
						m_wndListAction.SetItemText( nIndex , 2 , _T("左键单击") );
					} 
					else
					{
						m_wndListAction.SetItemText( nIndex , 2 , _T("右键单击") );
					}
				}
				else if ( pNewEvent->m_tagMouseEvent.m_iActionType & MOUSE_MASK_MOVE )
				{
					if (pNewEvent->m_tagMouseEvent.m_iActionType & MOUSE_MASK_LEFT )
					{
						m_wndListAction.SetItemText( nIndex , 2 , _T("左键移动") );
					} 
					else
					{
						m_wndListAction.SetItemText( nIndex , 2 , _T("右键移动") );
					}
				}
				strTemp.Format( _T("%d") , pNewEvent->m_iSleepTime );
				m_wndListAction.SetItemText( nIndex , 3 , strTemp );
				strTemp.Format( _T("%d") , pNewEvent->m_iLoopTime );
				m_wndListAction.SetItemText( nIndex , 4 , strTemp );
				strTemp.Format( _T("%d,%d") , pNewEvent->m_tagMouseEvent.m_StartPoint.x , pNewEvent->m_tagMouseEvent.m_StartPoint.y);
				m_wndListAction.SetItemText( nIndex , 5 , strTemp );
				strTemp.Format( _T("%d,%d"),  pNewEvent->m_tagMouseEvent.m_EndPoint.x , pNewEvent->m_tagMouseEvent.m_EndPoint.y);
				m_wndListAction.SetItemText( nIndex , 6 , strTemp );
			}
			else
			{

			}
		}
	}
}

void CClickProgrameDlg::PeepMessage()
{
	ASSERT( m_hWnd != NULL );
	MSG msg;
	while(PeekMessage( &msg , NULL , 0 , 0 , PM_REMOVE))
	{
		if( !IsDialogMessage(&msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

void CClickProgrameDlg::OnNMClickListAction(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	
	int nIndex = m_wndListAction.GetNextItem( -1 , LVNI_SELECTED );

	TRACE(_T("select item: %d\n") , nIndex );
	if (m_wndListAction.GetItemCount() <= 0 )
	{
		return;
	}

	if ( nIndex >= 0 )
	{
		tagActionEvent * pNewEvent;
		pNewEvent = (tagActionEvent *)m_wndListAction.GetItemData( nIndex );
		if ( pNewEvent )
		{
			if ( m_pdlgInput == NULL )
			{
				return;
			}
			m_pdlgInput->InitEventData( nIndex , pNewEvent );
			m_pdlgInput->ShowWindow(SW_SHOW);
		}	
		pNewEvent = NULL;
	}
	*pResult = 0;
}

BOOL CClickProgrameDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default

	//SetCursor( m_hCurs );

	//return TRUE;

	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}
void CClickProgrameDlg::OnBnClickedButRun()
{
//	if ( m_listActionEvent.GetCount() > 0)
	if ( m_wndListAction.GetItemCount() > 0 )
	{
		m_eState = Run;
		RunAllAction();
	}
	else
	{
		AfxMessageBox(_T("动作列表为空，录制动作先."));
	}

	// TODO: Add your control notification handler code here
}

void CClickProgrameDlg::OnBnClickedButStop()
{
	m_eState = Stop;
	SetStopFlag();
	// TODO: Add your control notification handler code here
}

void CClickProgrameDlg::OnBnClickedButDelete()
{
	// TODO: Add your control notification handler code here
}

int OnMouseAction( CPoint startpoint ,  CPoint offmove )
{

	if ( offmove.x == 0 && offmove.y == 0 )
	{
		return 0;
	}

	int iCount = 20;
	int posx = offmove.x / iCount;
	int posy = offmove.y / iCount;

	CPoint CurPoint;
	for ( int i = 1 ; i <= iCount ; i++ )
	{
		Sleep(10);
		GetCursorPos( &CurPoint );
		SetCursorPos( (CurPoint.x + posx) , (CurPoint.y +  posy) );
	}


	return 1;
}

/*
#define  WM_USER_MOUSECONTROL				WM_USER+100		//鼠标操作
#define  WM_USER_MOUSECONTROL_LEFTDOWN		WM_USER+101
#define  WM_USER_MOUSECONTROL_LEFTUP		WM_USER+102
#define  WM_USER_MOUSECONTROL_LEFTMOVE		WM_USER+103
#define  WM_USER_MOUSECONTROL_RIGHTDOWN		WM_USER+104
#define  WM_USER_MOUSECONTROL_RIGHTUP		WM_USER+105
#define  WM_USER_MOUSECONTROL_RIGHTMOVE		WM_USER+106
*/

//LRESULT CClickProgrameDlg::OnMouseControl( WPARAM wp , LPARAM lp )
//{
//	return 1;
//}

LRESULT CClickProgrameDlg::OnMouseControlLeftDown( WPARAM wp , LPARAM lp )
{
	HWND hwnd;
	CRect rect1;		//窗口的屏幕坐标
	CRect rect2;		//窗口的大小
	hwnd = (HWND)lp;
	//char strTemp[128];
	//strcpy( strTemp , (char*) lp );
	

	CPoint startpoint;
	startpoint.y = HIWORD(wp);//(LONG) wp / 65536 ;
	startpoint.x = LOWORD(wp);//(LONG) wp % 65536 ;
	//if ( IsWindow(hwnd) != NULL )
	//{
	//		BringWindowToTop( hwnd );
	::GetWindowRect( hwnd , rect1 );
	::GetClientRect( hwnd , rect2 );
	//}
	//else
	//{
	//	return 0;
	//}
	//SCROLLINFO si;
	//si.cbSize = sizeof(si);
	//si.fMask = SIF_ALL;

	//::GetScrollInfo( hwnd , SB_VERT , &si );
	
	startpoint.x = rect1.left - rect2.left + startpoint.x ;
	startpoint.y = rect1.top - rect2.top + startpoint.y;


	SetCursorPos( startpoint.x , startpoint.y );
	mouse_event( MOUSEEVENTF_LEFTDOWN , 0 , 0 , 0 , 0 );//左键按下

	//INPUT input;
	//memset( &input , 0 ,sizeof(input));
	//input.type = INPUT_MOUSE;
	//input.mi.dx = startpoint.x ;
	//input.mi.dy = startpoint.y;
	//input.mi.dwFlags =  MOUSEEVENTF_LEFTDOWN;

	//SendInput( 1 , &input , sizeof(input) );

	return 1;
}
LRESULT CClickProgrameDlg::OnMouseControlLeftUp( WPARAM wp , LPARAM lp )
{
	HWND hwnd;
	CRect rect1;		//窗口的屏幕坐标
	CRect rect2;		//窗口的大小
	hwnd = (HWND)lp;

	CPoint startpoint;
	startpoint.y = HIWORD(wp);//(LONG) wp /  65536 ;
	startpoint.x = LOWORD(wp);//(LONG) wp % 65536 ;
	//if ( IsWindow(hwnd) != NULL )
	//{
	//		BringWindowToTop( hwnd );
	::GetWindowRect( hwnd , rect1 );
	::GetClientRect( hwnd , rect2 );
	//}
	//else
	//{
	//	return 0;
	//}


	startpoint.x = rect1.left - rect2.left + startpoint.x ;
	startpoint.y = rect1.top - rect2.top + startpoint.y;

	//SetCursorPos( startpoint.x , startpoint.y );
	mouse_event( MOUSEEVENTF_LEFTUP , 0 , 0 , 0 , 0 );//左键按下

	//INPUT input;
	//memset( &input , 0 ,sizeof(input));
	//input.type = INPUT_MOUSE;
	//input.mi.dx = 0;
	//input.mi.dy = 0;
	//input.mi.dwFlags =  MOUSEEVENTF_LEFTUP;

	//SendInput( 1 , &input , sizeof(input) );

	return 1;
}
LRESULT CClickProgrameDlg::OnMouseControlMove( WPARAM wp , LPARAM lp )
{
	HWND hwnd;
	CRect rect1;		//窗口的屏幕坐标
	CRect rect2;		//窗口的大小
	hwnd = (HWND)lp;

	CPoint startpoint;
	startpoint.y = HIWORD(wp);//(LONG) wp /  65536 ;
	startpoint.x = LOWORD(wp);//(LONG) wp % 65536 ;
	//if ( IsWindow(hwnd) != NULL )
	//{
	//		BringWindowToTop( hwnd );
	//::GetWindowRect( hwnd , rect1 );
	//::GetClientRect( hwnd , rect2 );
	//}
	//else
	//{
	//	return 0;
	//}

	//startpoint.x = rect1.left - rect2.left + startpoint.x ;
	//startpoint.y = rect1.top - rect2.top + startpoint.y;

	//SetCursorPos( startpoint.x , startpoint.y );
	//INPUT input[2];
	//memset( input , 0 ,sizeof(input));
	//input[0].type = INPUT_MOUSE;
	//input[0].mi.dx = 20;
	//input[0].mi.dy = 20;
	//input[0].mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_LEFTDOWN;
	//

	//input[1].type = INPUT_MOUSE;
	//input[1].mi.dx = 20;
	//input[1].mi.dy = 20;
	//input[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

	//SendInput( 2 , input , sizeof(input) );
	mouse_event( MOUSEEVENTF_MOVE | MOUSEEVENTF_LEFTDOWN  , 20 , 20 , 0 , 0 );//左键按下
	return 1;
}
LRESULT CClickProgrameDlg::OnMouseControlRightDown( WPARAM wp , LPARAM lp )
{
	HWND hwnd;
	CRect rect1;		//窗口的屏幕坐标
	CRect rect2;		//窗口的大小
	hwnd = (HWND)lp;

	CPoint startpoint;
	startpoint.y = HIWORD(wp);//(LONG) wp /  65536 ;
	startpoint.x = LOWORD(wp);//(LONG) wp % 65536 ;
	//if ( IsWindow(hwnd) != NULL )
	//{
	//		BringWindowToTop( hwnd );
	::GetWindowRect( hwnd , rect1 );
	::GetClientRect( hwnd , rect2 );
	//}
	//else
	//{
	//	return 0;
	//}

	startpoint.x = rect1.left - rect2.left + startpoint.x ;
	startpoint.y = rect1.top - rect2.top + startpoint.y;

	SetCursorPos( startpoint.x , startpoint.y );
	mouse_event( MOUSEEVENTF_RIGHTDOWN , 0 , 0 , 0 , 0 );//左键按下
	return 1;
}
LRESULT CClickProgrameDlg::OnMouseControlRightUp( WPARAM wp , LPARAM lp )
{
	HWND hwnd;
	CRect rect1;		//窗口的屏幕坐标
	CRect rect2;		//窗口的大小
	hwnd = (HWND)lp;

	CPoint startpoint;
	startpoint.y = HIWORD(wp);//(LONG) wp /  65536 ;
	startpoint.x = LOWORD(wp);//(LONG) wp % 65536 ;
	//if ( IsWindow(hwnd) != NULL )
	//{
	//		BringWindowToTop( hwnd );
	::GetWindowRect( hwnd , rect1 );
	::GetClientRect( hwnd , rect2 );
	//}
	//else
	//{
	//	return 0;
	//}

	startpoint.x = rect1.left - rect2.left + startpoint.x ;
	startpoint.y = rect1.top - rect2.top + startpoint.y;

	SetCursorPos( startpoint.x , startpoint.y );
	mouse_event( MOUSEEVENTF_RIGHTUP , 0 , 0 , 0 , 0 );//左键按下
	return 1;
}

LRESULT CClickProgrameDlg::OnMouseControlMidDown( WPARAM wp , LPARAM lp )
{
	HWND hwnd;
	CRect rect1;		//窗口的屏幕坐标
	CRect rect2;		//窗口的大小
	hwnd = (HWND)lp;

	CPoint startpoint;
	startpoint.y = HIWORD(wp);//(LONG) wp /  65536 ;
	startpoint.x = LOWORD(wp);//(LONG) wp % 65536 ;
	//if ( IsWindow(hwnd) != NULL )
	//{
	//		BringWindowToTop( hwnd );
	::GetWindowRect( hwnd , rect1 );
	::GetClientRect( hwnd , rect2 );
	//}
	//else
	//{
	//	return 0;
	//}

	startpoint.x = rect1.left - rect2.left + startpoint.x ;
	startpoint.y = rect1.top - rect2.top + startpoint.y;

	SetCursorPos( startpoint.x , startpoint.y );
	mouse_event( MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP , 0 , 0 , 0 , 0 );//左键按下
	return 1;
}

LRESULT CClickProgrameDlg::OnMouseControlMidUp( WPARAM wp , LPARAM lp )
{
	HWND hwnd;
	CRect rect1;		//窗口的屏幕坐标
	CRect rect2;		//窗口的大小
	hwnd = (HWND)lp;

	CPoint startpoint;
	startpoint.y = HIWORD(wp);// /  65536 ;
	startpoint.x = LOWORD(wp);//(LONG) wp % 65536 ;
	//if ( IsWindow(hwnd) != NULL )
	//{
	//		BringWindowToTop( hwnd );
	::GetWindowRect( hwnd , rect1 );
	::GetClientRect( hwnd , rect2 );
	//}
	//else
	//{
	//	return 0;
	//}

	startpoint.x = rect1.left - rect2.left + startpoint.x ;
	startpoint.y = rect1.top - rect2.top + startpoint.y;

	SetCursorPos( startpoint.x , startpoint.y );
	mouse_event( MOUSEEVENTF_MIDDLEUP , 0 , 0 , 0 , 0 );//左键按下
	return 1;
}


/*
#define  WM_USER_MOUSEMOTION				WM_USER+0x1
#define  WM_USER_MOUSEMOTION_UP			WM_USER+0x10		//鼠标手势：向上
#define  WM_USER_MOUSEMOTION_DOWN			WM_USER+0x11		//鼠标手势：向下
#define  WM_USER_MOUSEMOTION_LEFT			WM_USER+0x12		//鼠标手势：向左
#define  WM_USER_MOUSEMOTION_RIGHT			WM_USER+0x13		//鼠标手势：向右
#define  WM_USER_MOUSEMOTION_DOWNRIGHT		WM_USER+0x14		//鼠标手势：下右
#define  WM_USER_MOUSEMOTION_DOWNLEFT		WM_USER+0x15		//鼠标手势：下左
#define  WM_USER_MOUSEMOTION_DOWNUP		WM_USER+0x16		//鼠标手势：下上
#define  WM_USER_MOUSEMOTION_LEFTDOWN		WM_USER+0x17		//鼠标手势：左下
#define  WM_USER_MOUSEMOTION_RIGHTDOWN		WM_USER+0x18		//鼠标手势：右下
#define  WM_USER_MOUSEMOTION_LEFTRIGHT		WM_USER+0x19		//鼠标手势：左右
#define  WM_USER_MOUSEMOTION_LEFTUP		WM_USER+0X20		//鼠标手势：左上
#define  WM_USER_MOUSEMOTION_RIGHTUP		WM_USER+0X21		//鼠标手势：右上
#define  WM_USER_MOUSEMOTION_RIGHTLEFT		WM_USER+0X22		//鼠标手势：右左
*/
LRESULT CClickProgrameDlg::OnMouseMotion( WPARAM wp , LPARAM lp )
{
	int iAction = (int) wp;

	HWND hwnd;
	CRect rect1;		//窗口的屏幕坐标
	CRect rect2;		//窗口的大小
	hwnd = (HWND)lp;

	//if ( IsWindow(hwnd) != NULL )
	//{
	//		BringWindowToTop( hwnd );
	::GetWindowRect( hwnd , rect1 );
	::GetClientRect( hwnd , rect2 );
	//}
	//else
	//{
	//	return 0;
	//}

	CPoint startpoint;
	startpoint.x = rect1.left + rect2.Width() / 2;
	startpoint.y = rect1.top + rect2.Height() / 2;
	//startpoint.x = GetSystemMetrics( SM_CXFULLSCREEN ) / 2;
	//startpoint.y = GetSystemMetrics( SM_CYFULLSCREEN ) / 2 ;

	Sleep( Delay);
	CPoint offPoint;
	switch ( iAction )
	{
	case WM_USER_MOUSEMOTION_UP:
		{

			offPoint.x = 0;
			offPoint.y = -StepLenth;

			SetCursorPos( startpoint.x , startpoint.y );

			mouse_event( MOUSEEVENTF_RIGHTDOWN , 0 , 0 , 0 , 0 );//右键键按下
			Sleep(Delay);

			OnMouseAction( startpoint , offPoint );

			mouse_event( MOUSEEVENTF_RIGHTUP , 0 , 0 , 0 , 0 );//右键键按下

			break;

		}
	case WM_USER_MOUSEMOTION_DOWN:
		{

			offPoint.x = 0;
			offPoint.y = StepLenth;

			SetCursorPos( startpoint.x , startpoint.y );

			mouse_event( MOUSEEVENTF_RIGHTDOWN , 0 , 0 , 0 , 0 );//右键键按下
			Sleep( Delay );

			OnMouseAction( startpoint , offPoint );

			mouse_event( MOUSEEVENTF_RIGHTUP  , 0 , 0 , 0 , 0 );//右键抬起
			break;
		}
	case WM_USER_MOUSEMOTION_LEFT:
		{
			offPoint.x = -StepLenth;
			offPoint.y = 0;

			SetCursorPos( startpoint.x , startpoint.y );
			mouse_event( MOUSEEVENTF_RIGHTDOWN , 0 , 0 , 0 , 0 );//右键键按下
			Sleep( Delay );

			OnMouseAction( startpoint , offPoint );

			mouse_event( MOUSEEVENTF_RIGHTUP  , 0 , 0 , 0 , 0 );//右键抬起
			break;
		}
	case WM_USER_MOUSEMOTION_RIGHT:
		{

			offPoint.x = StepLenth;
			offPoint.y = 0;

			SetCursorPos( startpoint.x , startpoint.y );
			mouse_event( MOUSEEVENTF_RIGHTDOWN , 0 , 0 , 0 , 0 );//右键键按下
			Sleep( Delay );

			OnMouseAction( startpoint , offPoint );

			mouse_event( MOUSEEVENTF_RIGHTUP  , 0 , 0 , 0 , 0 );//右键抬起
			break;
		}
	case WM_USER_MOUSEMOTION_DOWNRIGHT:
		{

			offPoint.x = 0;
			offPoint.y = StepLenth;

			SetCursorPos( startpoint.x , startpoint.y );

//			SendMessage( hwnd , WM_RBUTTONDOWN , NULL , NULL );
			mouse_event( MOUSEEVENTF_RIGHTDOWN , 0 , 0 , 0 , 0 );//右键键按下
			Sleep( Delay );

			OnMouseAction( startpoint , offPoint );

			offPoint.x = StepLenth;
			offPoint.y = 0;

			OnMouseAction( startpoint , offPoint );

			mouse_event( MOUSEEVENTF_RIGHTUP  , 0 , 0 , 0 , 0 );//右键抬起
			break;
		}
	case WM_USER_MOUSEMOTION_DOWNLEFT:
		{

			offPoint.x = 0;
			offPoint.y = StepLenth;

			SetCursorPos( startpoint.x , startpoint.y );
			mouse_event( MOUSEEVENTF_RIGHTDOWN , 0 , 0 , 0 , 0 );//右键键按下
			Sleep( Delay );

			OnMouseAction( startpoint , offPoint );

			offPoint.x = -StepLenth;
			offPoint.y = 0;
			OnMouseAction( startpoint, offPoint );

			mouse_event( MOUSEEVENTF_RIGHTUP  , 0 , 0 , 0 , 0 );//右键抬起
			break;
		}
	case WM_USER_MOUSEMOTION_DOWNUP:
		{

			offPoint.x = 0;
			offPoint.y = StepLenth;

			SetCursorPos( startpoint.x , startpoint.y );
			mouse_event( MOUSEEVENTF_RIGHTDOWN , 0 , 0 , 0 , 0 );//右键键按下
			Sleep( Delay );

			OnMouseAction( startpoint , offPoint );

			offPoint.x = 0;
			offPoint.y = -StepLenth;
			OnMouseAction( startpoint, offPoint );

			mouse_event( MOUSEEVENTF_RIGHTUP  , 0 , 0 , 0 , 0 );//右键抬起
			break;
		}
	case WM_USER_MOUSEMOTION_LEFTDOWN:
		{

			offPoint.x = -StepLenth;
			offPoint.y = 0;

			SetCursorPos( startpoint.x , startpoint.y );
			mouse_event( MOUSEEVENTF_RIGHTDOWN , 0 , 0 , 0 , 0 );//右键键按下
			Sleep( Delay );

			OnMouseAction( startpoint , offPoint );

			offPoint.x = 0;
			offPoint.y = StepLenth;
			OnMouseAction( startpoint, offPoint );

			mouse_event( MOUSEEVENTF_RIGHTUP  , 0 , 0 , 0 , 0 );//右键抬起
			break;
		}
	case WM_USER_MOUSEMOTION_RIGHTDOWN:
		{
			offPoint.x = StepLenth;
			offPoint.y = 0;

			SetCursorPos( startpoint.x , startpoint.y );
			mouse_event( MOUSEEVENTF_RIGHTDOWN , 0 , 0 , 0 , 0 );//右键键按下
			Sleep( Delay );

			OnMouseAction( startpoint , offPoint );

			offPoint.x = 0;
			offPoint.y = StepLenth;
			OnMouseAction( startpoint, offPoint );


			mouse_event( MOUSEEVENTF_RIGHTUP  , 0 , 0 , 0 , 0 );//右键抬起
			break;
		}
	case WM_USER_MOUSEMOTION_LEFTRIGHT:
		{

			offPoint.x = -StepLenth;
			offPoint.y = 0;

			SetCursorPos( startpoint.x , startpoint.y );
			mouse_event( MOUSEEVENTF_RIGHTDOWN , 0 , 0 , 0 , 0 );//右键键按下
			Sleep( Delay );

			OnMouseAction( startpoint , offPoint );

			offPoint.x = StepLenth;
			offPoint.y = 0;

			OnMouseAction( startpoint, offPoint );

			mouse_event( MOUSEEVENTF_RIGHTUP  , 0 , 0 , 0 , 0 );//右键抬起
			break;
		}
	case WM_USER_MOUSEMOTION_LEFTUP:
		{
			offPoint.x = -StepLenth;
			offPoint.y = 0;

			SetCursorPos( startpoint.x , startpoint.y );
			mouse_event( MOUSEEVENTF_RIGHTDOWN , 0 , 0 , 0 , 0 );//右键键按下
			Sleep( Delay );

			OnMouseAction( startpoint , offPoint );

			offPoint.x = 0;
			offPoint.y = -StepLenth;
			OnMouseAction( startpoint , offPoint );

			mouse_event( MOUSEEVENTF_RIGHTUP  , 0 , 0 , 0 , 0 );//右键抬起
			break;
		}
	case WM_USER_MOUSEMOTION_RIGHTUP:
		{
			offPoint.x = StepLenth;
			offPoint.y = 0;

			SetCursorPos( startpoint.x , startpoint.y );
			mouse_event( MOUSEEVENTF_RIGHTDOWN , 0 , 0 , 0 , 0 );//右键键按下
			Sleep( Delay );

			OnMouseAction( startpoint , offPoint );

			offPoint.x = 0;
			offPoint.y = -StepLenth;
			OnMouseAction( startpoint , offPoint );

			mouse_event( MOUSEEVENTF_RIGHTUP  , 0 , 0 , 0 , 0 );//右键抬起
			break;
		}
	case WM_USER_MOUSEMOTION_RIGHTLEFT:
		{
			offPoint.x = StepLenth;
			offPoint.y = 0;

			SetCursorPos( startpoint.x , startpoint.y );
			mouse_event( MOUSEEVENTF_RIGHTDOWN , 0 , 0 , 0 , 0 );//右键键按下
			Sleep( Delay );

			OnMouseAction( startpoint , offPoint );

			offPoint.x = -StepLenth;
			offPoint.y = 0;

			OnMouseAction( startpoint , offPoint );

			mouse_event( MOUSEEVENTF_RIGHTUP  , 0 , 0 , 0 , 0 );//右键抬起
			break;
		}
	default:
		{
			return 0;
			break;
		}
	}

	return 1;
}