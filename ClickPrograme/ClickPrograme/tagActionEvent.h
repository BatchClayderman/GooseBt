#pragma once

//�����Ϊ����
//�����У�������˫�����ƶ������
//��λ�У�������Ҽ�
//

#define		MOUSE_LEFT_MOVE			0x1001	//��������ƶ�
#define		MOUSE_LEFT_CLICK		0x1002	//�������
#define		MOUSE_LEFT_DBCLICK		0x1004	//���˫��
#define		MOUSE_LEFT_RANDOM		0x1008	//������

#define		MOUSE_RIGHT_MOVE		0x2001	//�����Ҽ��ƶ�
#define		MOUSE_RIGHT_CLICK		0x2002	//�Ҽ�����
#define		MOUSE_RIGHT_DBCLICK		0x2004	//�Ҽ�˫��
#define		MOUSE_RIGHT_RANDOM		0x2008	//�Ҽ����

//�ж������Ϊ������
#define		MOUSE_MASK_MOVE			0x0001	//����ƶ�
#define		MOUSE_MASK_CLICK		0x0002	//������Ϊ
#define		MOUSE_MASK_DBCLICK		0x0004	//˫����Ϊ
#define		MOUSE_MASK_RANDOM		0x0008	//�������
//�ж����Ҽ�������
#define		MOUSE_MASK_LEFT			0x1000	//���
#define		MOUSE_MASK_RIGHT		0x2000	//�Ҽ�

//�¼����
#define	 ACTIONTYPE_MOUSE			1
#define	 ACTIONTYPE_KEYBOARD		2

//Ĭ����ֵ
#define DefaultSleepTime 2000
#define	DefaultLoopTime  1
#define Delay			 1000		//Ϊ�����������ƣ��ӳ�500����
#define StepLenth		 100					//����ƶ�����

enum RunState
{
	Run = 0,		//������
	Pause,			//��ͣ
	Stop			//ֹͣ
};
struct tagMouseAction 
{
	CPoint	m_StartPoint;		//��ʼλ��
	CPoint	m_EndPoint;			//����λ��
	int		m_iActionType;		//��������(���DOWN���Ҽ�DOWN�����UP���Ҽ�UP��������˫��)
};
struct tagKeyAction 
{
	CString m_strInput;			//��������
	int		m_iSleepTime;		//��ʱ
};
struct tagActionEvent 
{
	//CString			m_strActionName;	//����Ķ�������
	tagMouseAction		m_tagMouseEvent;	//����¼�
	tagKeyAction		m_tagKeyEvent;		//�����¼�
	int					m_iEventType;		//�¼����
	int					m_iSleepTime;		//��ʱ
	int					m_iLoopTime;		//ѭ������
	void	InitData()
	{//��ʼ������
		m_tagMouseEvent.m_StartPoint.x = 0;
		m_tagMouseEvent.m_StartPoint.y = 0;

		m_tagMouseEvent.m_EndPoint.x = 0;
		m_tagMouseEvent.m_EndPoint.y = 0;

		m_tagMouseEvent.m_iActionType = MOUSE_LEFT_CLICK;

		m_iEventType = ACTIONTYPE_MOUSE;
		m_iSleepTime = DefaultSleepTime;
		m_iLoopTime = DefaultLoopTime;
	};

};