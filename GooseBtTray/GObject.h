#include <iostream>
#pragma once
#define FLAGS_NORMAL                 0x00000000
#define FLAGS_SECURE_THREAD          0x00000001
#define FLAGS_SECURE_PROCESS         0x00000002
#define FLAGS_SECURE_ACTION          0x00000004
#define FLAGS_SECURE_CORE            0x00000008
#define FLAGS_NO_WARNINGS            0x00000010
#define FLAGS_NO_ALERTS              0x00000020
#define FLAGS_NO_LOGS                0x00000040
#define FLAGS_NO_FAILED              0x00000080
#define FLAGS_FAILED_LOGOFF          0x00000100
#define FLAGS_FAILED_SHUTDOWN        0x00000200
#define FLAGS_FAILED_REBOOT          0x00000400
#define FLAGS_FAILED_ERRORSCREEN     0x00000800
typedef int GStatus;
typedef short int GOpFlags;
typedef char GOptions;
typedef int GFlags;
using namespace std;

struct GEvent
{
	short int type;// 1->"Process" 2->"File" 3->"Registry" 4->"COM" 5->"Others"
	char typeName[12];
	short int action;// 1->"OnCreate" 2->"OnRead 3->"OnChange" 4->"OnDelete" 5->"OnRename" 6->"OnExec"
	char actionName[12];
	long int time;
	bool isSystemProcess, isFolder;
	string imagename, tid, pid, ppid, purename, extname, filename, filepath, dirname, dirpath, regpath, compath;
};

struct GEnvironment
{
	char platform[16];
	char os[16];
	char ver[16];
	char other[16];
};

class GProcessAction
{
	GStatus CallCreate(GOpFlags opFlag);
	GStatus CallSuspend(GOpFlags opFlag);
	GStatus CallResume(GOpFlags opFlag);
	GStatus CallTerminate(GOpFlags opFlag);
	GStatus CallNtShutdown(GOpFlags opFlag);
	GStatus CallNtReboot(GOpFlags opFlag);
	GStatus CallZwShutdownSystem(GOpFlags opFlag);
	GStatus CallZwRebootSystem(GOpFlags opFlag);
};

class GFileAction
{
	GStatus CallNtDeleteFile(GOpFlags opFlag);
};

class GRegistryAction
{
	
};

class GCOMAction
{

};

class GOthersAction
{

};

class GAction
{
	GProcessAction  process;
	GFileAction     file;
	GRegistryAction reg;
	GCOMAction      com;
};

struct GObject
{
	GEvent source, target;// Save Events including action type, process information, time, etc.
	GEnvironment env;// Your environment
	GAction action;// Some functions you can call directly
	GOptions options;// Your decisions
	GFlags flags;// Your flags
};

void OnCreate(GObject object)//监控到文件创建行为
{
	if (object.target.dirpath == "C:\\Windows\\system32")//如果目标位于系统目录内
	{
		object.options = '1';//阻止
		object.flags = FLAGS_NO_ALERTS | FLAGS_SECURE_THREAD;//拦截成功后不通知并对同一线程不再询问
	}
	else//如果目标不在系统目录内
	{
		object.options = '0';//放行
		object.flags = FLAGS_NO_WARNINGS | FLAGS_NO_ALERTS;//静默处理
	}
}

void OnDelete(GObject object)//监控到文件删除行为
{
	if (object.source.imagename == "rd.exe")//如果是进程 rd.exe 正在尝试删除文件
	{
		if (object.target.dirpath == "C:\\Windows\\system32")//如果目标位于系统目录内
		{
			object.options = '2';//拦截
			object.flags = FLAGS_NO_WARNINGS | FLAGS_FAILED_ERRORSCREEN;//拦截后发出提醒，失败则调用蓝屏
		}
		else//如果目标不在系统目录内
		{
			object.options = '0';//放行
			object.flags = FLAGS_NO_ALERTS | FLAGS_SECURE_PROCESS;//询问用户是否默认放心并设置对该进程不再询问
		}
	}
	else
	{
		object.options = 'Q';//对其它程序的删除都采取询问
		object.flags = FLAGS_NORMAL;//正常询问方式（缺省值）
	}
}