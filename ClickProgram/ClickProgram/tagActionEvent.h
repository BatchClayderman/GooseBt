#pragma once

//鼠标行为类型
//动作有：单击、双击、移动、随机
//键位有：左键、右键
//

#define		MOUSE_LEFT_MOVE			0x1001	//按下左键移动
#define		MOUSE_LEFT_CLICK		0x1002	//左键单击
#define		MOUSE_LEFT_DBCLICK		0x1004	//左键双击
#define		MOUSE_LEFT_RANDOM		0x1008	//左键随机

#define		MOUSE_RIGHT_MOVE		0x2001	//按下右键移动
#define		MOUSE_RIGHT_CLICK		0x2002	//右键单击
#define		MOUSE_RIGHT_DBCLICK		0x2004	//右键双击
#define		MOUSE_RIGHT_RANDOM		0x2008	//右键随机

//判断鼠标行为的掩码
#define		MOUSE_MASK_MOVE			0x0001	//鼠标移动
#define		MOUSE_MASK_CLICK		0x0002	//单击行为
#define		MOUSE_MASK_DBCLICK		0x0004	//双击行为
#define		MOUSE_MASK_RANDOM		0x0008	//随机测试
//判断左右键的掩码
#define		MOUSE_MASK_LEFT			0x1000	//左键
#define		MOUSE_MASK_RIGHT		0x2000	//右键

//事件类别
#define	 ACTIONTYPE_MOUSE			1
#define	 ACTIONTYPE_KEYBOARD		2

//默认数值
#define DefaultSleepTime 2000
#define	DefaultLoopTime  1
#define Delay			 1000		//为了配合鼠标手势，延迟500毫秒
#define StepLenth		 100					//鼠标移动步长

enum RunState
{
	Run = 0,		//运行中
	Pause,			//暂停
	Stop			//停止
};
struct tagMouseAction 
{
	CPoint	m_StartPoint;		//起始位置
	CPoint	m_EndPoint;			//结束位置
	int		m_iActionType;		//动作类型(左键DOWN、右键DOWN、左键UP、右键UP、单击、双击)
};
struct tagKeyAction 
{
	CString m_strInput;			//输入内容
	int		m_iSleepTime;		//延时
};
struct tagActionEvent 
{
	//CString			m_strActionName;	//输入的动作名称
	tagMouseAction		m_tagMouseEvent;	//鼠标事件
	tagKeyAction		m_tagKeyEvent;		//键盘事件
	int					m_iEventType;		//事件类别
	int					m_iSleepTime;		//延时
	int					m_iLoopTime;		//循环次数
	void	InitData()
	{//初始化数据
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