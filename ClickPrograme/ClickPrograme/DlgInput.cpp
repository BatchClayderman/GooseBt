// DlgInput.cpp : implementation file
//

#include "stdafx.h"
#include "ClickPrograme.h"
#include "DlgInput.h"
#include "ClickProgrameDlg.h"

// CDlgInput dialog

#define WM_TESTWK 0x3108

IMPLEMENT_DYNAMIC(CDlgInput, CDialog)

CDlgInput::CDlgInput( CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInput::IDD, pParent)
	, m_edit_Start_X(0)
	, m_edit_Start_Y(0)
	, m_edit_End_X(0)
	, m_edit_End_Y(0)
	, m_editSleepTime(0)
	, m_editLoopTime(0)
{
	Create( CDlgInput::IDD , pParent );
	//m_hParentWnd = hwnd;
}

CDlgInput::~CDlgInput()
{

}

void CDlgInput::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_S_X, m_edit_Start_X);
	DDV_MinMaxInt(pDX, m_edit_Start_X, 0, 65535);
	DDX_Text(pDX, IDC_EDIT_S_Y, m_edit_Start_Y);
	DDV_MinMaxInt(pDX, m_edit_Start_Y, 0, 65535);
	DDX_Text(pDX, IDC_EDIT_E_X, m_edit_End_X);
	DDV_MinMaxInt(pDX, m_edit_End_X, 0, 65535);
	DDX_Text(pDX, IDC_EDIT_E_Y, m_edit_End_Y);
	DDV_MinMaxInt(pDX, m_edit_End_Y, 0, 65535);
	DDX_Control(pDX, IDC_COMBO_EVENT, m_cmbEventType);
	DDX_Control(pDX, IDC_COMBO_ACTION, m_cmbActionType);
	DDX_Text(pDX, IDC_EDIT_SLEEP, m_editSleepTime);
	DDV_MinMaxInt(pDX, m_editSleepTime, 0, 65535);
	DDX_Text(pDX, IDC_EDIT_LOOP, m_editLoopTime);
	DDV_MinMaxInt(pDX, m_editLoopTime, 0, 65535);
}


BEGIN_MESSAGE_MAP(CDlgInput, CDialog)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDOK, &CDlgInput::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgInput message handlers
BOOL CDlgInput::OnInitDialog()
{
	CDialog::OnInitDialog();

	////初始化事件
	//m_cmbEventType.InsertString( 0 , _T("鼠标事件"));

	m_cmbEventType.InsertString( 0 , _T("鼠标事件"));

	m_cmbActionType.InsertString( LEFT_MOVE , _T("左键移动"));
	m_cmbActionType.InsertString( RIGHT_MOVE , _T("右键移动"));
	m_cmbActionType.InsertString( LEFT_CLICK , _T("左键单击"));
	m_cmbActionType.InsertString( RIGHT_CLICK , _T("右键单击"));
	m_cmbActionType.InsertString( LEFT_DBCLICK , _T("左键双击"));
	m_cmbActionType.InsertString( RIGHT_DBCLICK , _T("右键双击"));
	m_cmbActionType.InsertString( LEFT_RANDOM , _T("左键随机"));
	m_cmbActionType.InsertString( RIGHT_RANDOM , _T("右键随机"));

	return TRUE;  // return TRUE  unless you set the focus to a control
}
void CDlgInput::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	TRACE(_T("Show Window"));

	UpdateData( FALSE );
	// TODO: Add your message handler code here
}
int CDlgInput::InitEventData( int nIndex ,  tagActionEvent * pEvent )
{
	m_nIndex = nIndex;
	m_tagSourceEvent = pEvent;
	//m_tagSourceEvent = *pEvent;

	if ( m_tagSourceEvent->m_iEventType == ACTIONTYPE_MOUSE )
	{//鼠标行为

		m_cmbEventType.SetCurSel( MOUSEEVENT );
		m_edit_Start_X = m_tagSourceEvent->m_tagMouseEvent.m_StartPoint.x;
		m_edit_Start_Y = m_tagSourceEvent->m_tagMouseEvent.m_StartPoint.y;
		m_edit_End_X = m_tagSourceEvent->m_tagMouseEvent.m_EndPoint.x;
		m_edit_End_Y = m_tagSourceEvent->m_tagMouseEvent.m_EndPoint.y;
		m_editLoopTime = m_tagSourceEvent->m_iLoopTime;
		m_editSleepTime = m_tagSourceEvent->m_iSleepTime;

		if ( m_tagSourceEvent->m_tagMouseEvent.m_iActionType & MOUSE_MASK_RANDOM )
		{//随机
			if ( m_tagSourceEvent->m_tagMouseEvent.m_iActionType & MOUSE_MASK_LEFT )
			{
				m_cmbActionType.SetCurSel( LEFT_RANDOM );
			}
			else
			{
				m_cmbActionType.SetCurSel( RIGHT_RANDOM );
			}
			//GetDlgItem( IDC_EDIT_S_X )->SETITEM
		}
		else if ( m_tagSourceEvent->m_tagMouseEvent.m_iActionType & MOUSE_MASK_DBCLICK )
		{//双击
			if ( m_tagSourceEvent->m_tagMouseEvent.m_iActionType & MOUSE_MASK_LEFT )
			{
				m_cmbActionType.SetCurSel( LEFT_DBCLICK );
			}
			else
			{
				m_cmbActionType.SetCurSel( RIGHT_DBCLICK );
			}
		}
		else if ( m_tagSourceEvent->m_tagMouseEvent.m_iActionType & MOUSE_MASK_CLICK )
		{//单击
			if ( m_tagSourceEvent->m_tagMouseEvent.m_iActionType & MOUSE_MASK_LEFT )
			{
				m_cmbActionType.SetCurSel( LEFT_CLICK );
			}
			else
			{
				m_cmbActionType.SetCurSel( RIGHT_CLICK );
			}
		}
		else if ( m_tagSourceEvent->m_tagMouseEvent.m_iActionType & MOUSE_MASK_MOVE )
		{//移动
			if ( m_tagSourceEvent->m_tagMouseEvent.m_iActionType & MOUSE_MASK_LEFT )
			{
				m_cmbActionType.SetCurSel( LEFT_MOVE );
			}
			else
			{
				m_cmbActionType.SetCurSel( RIGHT_MOVE );
			}
		}
	}
	else
	{//键盘行为


	}
	return 1;
}
void CDlgInput::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData( TRUE );

//	m_tagSourceEvent->m_iEventType = m_cmbActionType;
	m_tagSourceEvent->m_iLoopTime = m_editLoopTime;
	m_tagSourceEvent->m_iSleepTime = m_editSleepTime;
	switch ( m_cmbActionType.GetCurSel() )
	{
	case LEFT_MOVE:
		{
			m_tagSourceEvent->m_tagMouseEvent.m_iActionType = MOUSE_LEFT_MOVE;
			break;
		}
	case RIGHT_MOVE:
		{
			m_tagSourceEvent->m_tagMouseEvent.m_iActionType = MOUSE_RIGHT_MOVE;
			break;
		}
	case LEFT_CLICK:
		{
			m_tagSourceEvent->m_tagMouseEvent.m_iActionType = MOUSE_LEFT_CLICK;
			break;
		}
	case RIGHT_CLICK:
		{
			m_tagSourceEvent->m_tagMouseEvent.m_iActionType = MOUSE_RIGHT_CLICK;
			break;
		}
	case LEFT_DBCLICK:
		{
			m_tagSourceEvent->m_tagMouseEvent.m_iActionType = MOUSE_LEFT_DBCLICK;
			break;
		}
	case RIGHT_DBCLICK:
		{
			m_tagSourceEvent->m_tagMouseEvent.m_iActionType = MOUSE_RIGHT_DBCLICK;
			break;
		}
	case LEFT_RANDOM:
		{
			m_tagSourceEvent->m_tagMouseEvent.m_iActionType = MOUSE_LEFT_RANDOM;
			break;
		}
	case RIGHT_RANDOM:
		{
			m_tagSourceEvent->m_tagMouseEvent.m_iActionType = MOUSE_RIGHT_RANDOM;
			break;
		}
	}
	m_tagSourceEvent->m_tagMouseEvent.m_StartPoint.x = m_edit_Start_X;
	m_tagSourceEvent->m_tagMouseEvent.m_StartPoint.y = m_edit_Start_Y;
	m_tagSourceEvent->m_tagMouseEvent.m_EndPoint.x = m_edit_End_X;
	m_tagSourceEvent->m_tagMouseEvent.m_EndPoint.y = m_edit_End_Y;
	

	HWND   hwnd = NULL;   
	while(1){     
		//hwnd = ::FindWindow(_T("SE_AxControl") ,NULL );
		//hwnd = ::GetParent(NULL);
		hwnd = ::FindWindow(NULL, _T("ClickPrograme"));
			if(hwnd!=NULL)   break;   
	}
	CRect rect1;		//窗口的屏幕坐标
	CRect rect2;		//窗口的大小
	//FindWindow( "鼠标测试")
	//HWND hParentWnd;
	//HWND hwnd = GetSafeHwnd();
	//hwnd = GetActiveWindow();
	//hParentWnd = ::GetParent( hwnd );
	//		BringWindowToTop( hwnd );
	::GetWindowRect( hwnd , rect1 );
	::GetClientRect( hwnd , rect2 );
	
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;

	::GetScrollInfo( hwnd , SB_CTL , &si );



	::SendMessageW( hwnd , WM_USER_EDIT_END, m_nIndex, (LPARAM )&m_tagSourceEvent  );
	//OnOK();
	ShowWindow( SW_HIDE );
}
