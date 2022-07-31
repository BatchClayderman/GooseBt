// ClickProgrameDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "tagActionEvent.h"
#include "DlgInput.h"

#define  WM_USER_EDIT_END					WM_USER+101	

#define  WM_USER_MOUSEMOTION				WM_USER+1
#define  WM_USER_MOUSEMOTION_UP			WM_USER+10		//������ƣ�����
#define  WM_USER_MOUSEMOTION_DOWN			WM_USER+11		//������ƣ�����
#define  WM_USER_MOUSEMOTION_LEFT			WM_USER+12		//������ƣ�����
#define  WM_USER_MOUSEMOTION_RIGHT			WM_USER+13		//������ƣ�����
#define  WM_USER_MOUSEMOTION_DOWNRIGHT		WM_USER+14		//������ƣ�����
#define  WM_USER_MOUSEMOTION_DOWNLEFT		WM_USER+15		//������ƣ�����
#define  WM_USER_MOUSEMOTION_DOWNUP		WM_USER+16		//������ƣ�����
#define  WM_USER_MOUSEMOTION_LEFTDOWN		WM_USER+17		//������ƣ�����
#define  WM_USER_MOUSEMOTION_RIGHTDOWN		WM_USER+18		//������ƣ�����
#define  WM_USER_MOUSEMOTION_LEFTRIGHT		WM_USER+19		//������ƣ�����
#define  WM_USER_MOUSEMOTION_LEFTUP		WM_USER+20		//������ƣ�����
#define  WM_USER_MOUSEMOTION_RIGHTUP		WM_USER+21		//������ƣ�����
#define  WM_USER_MOUSEMOTION_RIGHTLEFT		WM_USER+22		//������ƣ�����

#define  WM_USER_MOUSECONTROL				WM_USER+200		//������
#define  WM_USER_MOUSECONTROL_LEFTDOWN		WM_USER+201
#define  WM_USER_MOUSECONTROL_LEFTUP		WM_USER+202
#define  WM_USER_MOUSECONTROL_MOVE			WM_USER+203
#define  WM_USER_MOUSECONTROL_RIGHTDOWN		WM_USER+204
#define  WM_USER_MOUSECONTROL_RIGHTUP		WM_USER+205
#define  WM_USER_MOUSECONTROL_MIDDOWN		WM_USER+206
#define  WM_USER_MOUSECONTROL_MIDUP			WM_USER+207
//#define  WM_USER_MOUSECONTROL_RIGHTMOVE		WM_USER+206

#define  CG_ACTION_RUN		100		//��ʼ����
#define  CG_ACTION_PAUSE	101		//��ͣ����
#define  CG_ACTION_STOP		102		//ֹͣ����
#define  CG_ACTION_RECORDMOUSE_CLICK	103	//¼�������Ϊ
#define  CG_ACTION_RECORDMOUSE_MOVE		104	//¼�������Ϊ


#define	 KeyInputStart	'R'	//���ж�Ӧ���ַ�
#define	 KeyInputPause	'P'	//��ͣ��Ӧ���ַ�
#define	 KeyInputStop	'S'	//ֹͣ��Ӧ���ַ�
#define  KeyInputRecordMove		VK_F1	//¼����Ϊ�ƶ�
#define  KeyInputRecordClick	'A'		//¼����Ϊ���


//���ƫ����������
#define CG_CURSOR_RANDOM_X 100
#define CG_CURSOR_RANDOM_Y 100

// CClickProgrameDlg dialog
class CClickProgrameDlg : public CDialog
{
// Construction
public:
	CClickProgrameDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CLICKPROGRAME_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	CWnd * m_pWnd;
	int m_nHotKeyID;
	HCURSOR m_hCurs; 
	//HWND m_hWnd;
	
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnSetText(WPARAM wp,LPARAM lp);
	afx_msg LRESULT OnHotKey(WPARAM wp,LPARAM lp); //�����Ϣ������ 
	afx_msg LRESULT OnRefreshData(WPARAM wp , LPARAM lp);
	afx_msg LRESULT OnMouseMotion( WPARAM wp , LPARAM lp );//���������Ӧ����
	afx_msg LRESULT OnMouseControlLeftDown( WPARAM wp , LPARAM lp );//��������Ӧ����
	afx_msg LRESULT OnMouseControlLeftUp( WPARAM wp , LPARAM lp );//��������Ӧ����
	afx_msg LRESULT OnMouseControlMove( WPARAM wp , LPARAM lp );//��������Ӧ����
	afx_msg LRESULT OnMouseControlRightDown( WPARAM wp , LPARAM lp );//��������Ӧ����
	afx_msg LRESULT OnMouseControlRightUp( WPARAM wp , LPARAM lp );//��������Ӧ����
	afx_msg LRESULT OnMouseControlMidDown( WPARAM wp , LPARAM lp );//��������Ӧ����
	afx_msg LRESULT OnMouseControlMidUp( WPARAM wp , LPARAM lp );//��������Ӧ����
	//afx_msg LRESULT OnMouseControlRightMove( WPARAM wp , LPARAM lp );//��������Ӧ����
	//afx_msg LRESULT OnEditEnd(WPARAM   wParam,LPARAM   lParam);
	DECLARE_MESSAGE_MAP()
public:
	//int m_nIndex;
	int m_iRecordState;
	RunState m_eState;

	tagActionEvent m_tagEventRecord;
	CDlgInput * m_pdlgInput;
	CListCtrl m_wndListAction;
	// �ڶ�����ʼ����ʱ�������

	
	void SetHotKey(void);
	void SetStopFlag(void);
	int StartRandom( tagActionEvent * pNewEvent );
	int SingleClick( tagActionEvent * pNewEvent );
	int DoubleClick( tagActionEvent * pNewEvent );
	int MouseMove( tagActionEvent * pNewEvent );
	int RecordAction( );
	int RecordActionB();
	void ReFreshList();
	int InsertAction( tagActionEvent * pNewEvent );
	int RunAllAction();
	void PeepMessage();

	afx_msg void OnDestroy();
	afx_msg void OnNMClickListAction(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnBnClickedButRun();
	afx_msg void OnBnClickedButStop();
	afx_msg void OnBnClickedButDelete();
	int m_iLoopCount;
};