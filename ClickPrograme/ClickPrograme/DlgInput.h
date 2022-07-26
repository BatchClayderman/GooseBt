#pragma once
#include "afxwin.h"

#include "tagActionEvent.h"


// CDlgInput dialog
const int MOUSEEVENT = 0;
//m_cmbActionType.InsertString( 0 , _T("×ó¼üÒÆ¶¯"));
//m_cmbActionType.InsertString( 1 , _T("ÓÒ¼üÒÆ¶¯"));
//m_cmbActionType.InsertString( 2 , _T("×ó¼üµ¥»÷"));
//m_cmbActionType.InsertString( 3 , _T("ÓÒ¼üµ¥»÷"));
//m_cmbActionType.InsertString( 4 , _T("×ó¼üË«»÷"));
//m_cmbActionType.InsertString( 5 , _T("ÓÒ¼üË«»÷"));
//m_cmbActionType.InsertString( 6 , _T("×ó¼üËæ»ú"));
//m_cmbActionType.InsertString( 7 , _T("ÓÒ¼üËæ»ú"));
const int LEFT_MOVE = 0;
const int RIGHT_MOVE = 1;
const int LEFT_CLICK = 2;
const int RIGHT_CLICK = 3;
const int LEFT_DBCLICK = 4;
const int RIGHT_DBCLICK = 5;
const int LEFT_RANDOM = 6;
const int RIGHT_RANDOM = 7;


class CDlgInput : public CDialog
{
	DECLARE_DYNAMIC(CDlgInput)

public:
	CDlgInput( CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgInput();

// Dialog Data
	enum { IDD = IDD_DIALOG_INPUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	//HWND m_hParentWnd;
	int m_edit_Start_X;		//ÆðÊ¼Î»ÖÃX×ø±ê
	int m_edit_Start_Y;		//ÆðÊ¼Î»ÖÃY×ø±ê
	int m_edit_End_X;		//ÖÕÖ¹Î»ÖÃX×ø±ê
	int m_edit_End_Y;		//ÖÕÖ¹Î»ÖÃY×ø±ê
	CComboBox m_cmbEventType;
	CComboBox m_cmbActionType;
	int m_editSleepTime;
	int m_editLoopTime;
	int m_nIndex;
	tagActionEvent * m_tagSourceEvent;
	int InitEventData(  int nIndex , tagActionEvent * pEvent );
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedOk();
};
