#include <iostream>
#ifdef WIN32
#include <Windows.h>
#include <WinInet.h>
#elif (defined _WIN64 || defined WIN64)
#include <Windows.h>
#include <WinInet.h>
#ifndef _WIN32
#define _WIN32 1
#endif
#ifndef WIN32
#define WIN32 1
#endif
#endif//_Windows_H
#include <thread>
#include <tchar.h>
#include <TlHelp32.h>
#include <conio.h>
#include <fstream>
#include <algorithm>
#include <vector>
#include <time.h>


/* 主程序预编译 */
#ifndef _wmip_H
#define _wmip_H
#ifndef NULL
#define NULL 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef EOF
#define EOF (-1)
#endif
#ifndef OK
#ifdef TRUE
#define OK TRUE
#else
#define OK 1
#endif
#endif
#ifndef ERROR
#if defined FALSE
#define ERROR FALSE
#elif defined NULL
#define ERROR NULL
#else
#define FALSE 0
#endif
#endif
#ifndef VOID
#define VOID void
#endif
#ifdef Platform
#undef Platform
#endif
#ifndef KMDF_DRIVER
#define KMDF_DRIVER TRUE
#endif

#ifdef WIN32
#ifndef Platform
#define Platform TRUE
#endif//由于前方已处理 Win 64，因此不需要再处理 Win 64
#endif
#ifdef WIN16
#ifdef Platform
#undef Platform
#endif
#define Platform FALSE
#endif//暂不支持 16 位（子）系统
#ifdef linux
#ifdef Platform
#undef Platform
#endif
#define Platform FALSE
#endif
#ifdef _LINUX
#ifdef Platform
#undef Platform
#endif
#define Platform FALSE
#endif
#ifdef _UNIX
#ifdef Platform
#undef Platform
#endif
#define Platform FALSE
#endif
#ifdef __sum
#ifdef Platform
#undef Platform
#endif
#define Platform FALSE
#endif
#ifndef Platform
#define Platform FALSE
#endif
#ifdef EQ
#undef EQ
#endif
#define EQ(a, b) ((a) == (b))//相等
#ifdef LT
#undef LT
#endif
#define LT(a, b) ((a) < (b))//小于
#ifdef LQ
#undef LQ
#endif
#define LQ(a, b) ((a) <= (b))//小于等于
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif//最大值
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif//最小值
#ifndef _WIN32_WINNT
#if EQ(Platform, TRUE)
#define _WIN32_WINNT 0x0001
#else
#define _WIN32_WINNT 0x0000
#endif
#endif

#ifdef PLF
#undef PLF
#endif
#define PLF TRUE//平台检测
#ifndef IDTRYAGAIN
#undef PLF
#define PLF FALSE
#endif
#ifndef IDCONTINUE
#undef PLF
#define PLF FALSE
#endif
#ifndef MB_CANCELTRYCONTINUE
#undef PLF
#define PLF FALSE
#endif
#if (PLF == FALSE)
#undef IDTRYAGAIN
#undef IDCONTINUE
#undef MB_CANCELTRYCONTINUE//去除所有声明
#ifdef IDABORT
#define IDTRYAGAIN IDRETRY
#else
#define IDTRYAGAIN 10
#endif
#ifdef IDRETRY
#define IDCONTINUE IDIGNORE
#else
#define IDCONTINUE 11
#endif
#ifdef MB_ABORTRETRYIGNORE
#define MB_CANCELTRYCONTINUE MB_ABORTRETRYIGNORE
#else
#define MB_CANCELTRYCONTINUE 0x00000006L
#endif
#endif//如果运行平台不支持 MB_CANCELTRYCONTINUE 及其衍生物，转用 MB_ABORTRETRYIGNORE
#ifdef PLF
#undef PLF
#endif

#ifndef LogName
#define LogName "wmipLog.log"
#endif
#ifndef WmipTextTitle
#define WmipTextTitle TEXT("wmip")
#endif
#ifndef MAX_PATH
#define MAX_PATH 260
#endif
#ifndef EXIT_OUT_OF_SCHEDULE
#define EXIT_OUT_OF_SCHEDULE 9009
#endif
#ifndef EXIT_FILE_UNEXISTED
#define EXIT_FILE_UNEXISTED 3
#endif
#ifndef EXIT_WITH_NONE_SELECTED
#define EXIT_WITH_NONE_SELECTED 2
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef EXIT_GRAMMAR_ERROR
#ifdef EOF
#define EXIT_GRAMMAR_ERROR EOF
#else
#define EXIT_GRAMMAR_ERROR (-1)
#endif
#endif
#ifndef EXIT_OPTIONS_TOO_LESS
#define EXIT_OPTIONS_TOO_LESS (-2)
#endif
#ifndef EXIT_UNSAFELY
#define EXIT_UNSAFELY (-3)
#endif
#ifndef EXIT_MEMERROR
#define EXIT_MEMERROR (-4)
#endif
#ifndef EXIT_DRIVER_ERROR
#define EXIT_DRIVER_ERROR (-5)
#endif
#ifndef Large
#define Large 8000
#endif
#ifndef Small
#define Small 500
#endif
#ifndef MaxTime
#define MaxTime 100000
#endif
#ifndef DefaultTime
#define DefaultTime 5000
#endif
#ifndef MinTime
#define MinTime 500
#endif
#ifndef gapTime
#define gapTime 60000
#endif

#ifndef OBJ_INHERIT
#define OBJ_INHERIT             0x00000002L
#endif
#ifndef OBJ_PERMANENT
#define OBJ_PERMANENT           0x00000010L
#endif
#ifndef OBJ_EXCLUSIVE
#define OBJ_EXCLUSIVE           0x00000020L
#endif
#ifndef OBJ_CASE_INSENSITIVE
#define OBJ_CASE_INSENSITIVE    0x00000040L
#endif
#ifndef OBJ_OPENIF
#define OBJ_OPENIF              0x00000080L
#endif
#ifndef OBJ_OPENLINK
#define OBJ_OPENLINK            0x00000100L
#endif
#ifndef OBJ_KERNEL_HANDLE
#define OBJ_KERNEL_HANDLE       0x00000200L
#endif
#ifndef OBJ_FORCE_ACCESS_CHECK
#define OBJ_FORCE_ACCESS_CHECK  0x00000400L
#endif
#ifndef OBJ_VALID_ATTRIBUTES
#define OBJ_VALID_ATTRIBUTES    0x000007F2L
#endif
#ifdef InitializeObjectAttributes
#undef InitializeObjectAttributes
#endif

#if defined(_MSC_VER)
#define JL_SIZE_T_SPECIFIER "%Iu"
#define JL_SSIZE_T_SPECIFIER "%Id"
#define JL_PTRDIFF_T_SPECIFIER "%Id"
#elif defined(__GNUC__)
#define JL_SIZE_T_SPECIFIER "%zu"
#define JL_SSIZE_T_SPECIFIER "%zd"
#define JL_PTRDIFF_T_SPECIFIER "%zd"
#else//计算出使用的 size_t 及其衍生物应该使用的 % 符
#if NUMBITS == 32
#define JL_SIZE_T_SPECIFIER something_unsigned
#define JL_SSIZE_T_SPECIFIER something_signed
#define JL_PTRDIFF_T_SPECIFIER something_signed
#else
#define JL_SIZE_T_SPECIFIER something_bigger_unsigned
#define JL_SSIZE_T_SPECIFIER something_bigger_signed
#define JL_PTRDIFF_T_SPECIFIER something-bigger_signed
#endif
#endif

#ifdef ForeMem
#undef ForeMem
#endif
#define ForeMem 66//预运行测试用的内存值（单位：MB）
using namespace std;
#pragma comment(lib, "Wininet.lib")
#pragma comment(lib, "urlmon.lib")//编译时请务必使用静态编译
#endif//_wmip_H


/* zip 解压预编译 */
#ifndef _unzip_H
#define _unzip_H
#ifndef _zip_H
DECLARE_HANDLE(HZIP);
#endif
#ifndef BUFREADCOMMENT
#define BUFREADCOMMENT (0x400)
#endif
#ifndef ZR_OK
#define ZR_OK         0x00000000
#endif
#ifndef ZR_RECENT
#define ZR_RECENT     0x00000001
#endif
#ifndef ZR_GENMASK
#define ZR_GENMASK    0x0000FF00
#endif
#ifndef ZR_NODUPH
#define ZR_NODUPH     0x00000100
#endif
#ifndef ZR_NOFILE
#define ZR_NOFILE     0x00000200
#endif
#ifndef ZR_NOALLOC
#define ZR_NOALLOC    0x00000300
#endif
#ifndef ZR_WRITE
#define ZR_WRITE      0x00000400
#endif
#ifndef ZR_NOTFOUND
#define ZR_NOTFOUND   0x00000500
#endif
#ifndef ZR_MORE
#define ZR_MORE       0x00000600
#endif
#ifndef ZR_CORRUPT
#define ZR_CORRUPT    0x00000700
#endif
#ifndef ZR_READ
#define ZR_READ       0x00000800
#endif
#ifndef ZR_PASSWORD
#define ZR_PASSWORD   0x00001000
#endif
#ifndef ZR_CALLERMASK
#define ZR_CALLERMASK 0x00FF0000
#endif
#ifndef ZR_ARGS
#define ZR_ARGS       0x00010000
#endif
#ifndef ZR_NOTMMAP
#define ZR_NOTMMAP    0x00020000
#endif
#ifndef ZR_MEMSIZE
#define ZR_MEMSIZE    0x00030000
#endif
#ifndef ZR_FAILED
#define ZR_FAILED     0x00040000
#endif
#ifndef ZR_ENDED
#define ZR_ENDED      0x00050000
#endif
#ifndef ZR_MISSIZE
#define ZR_MISSIZE    0x00060000
#endif
#ifndef ZR_PARTIALUNZ
#define ZR_PARTIALUNZ 0x00070000
#endif
#ifndef ZR_ZMODE
#define ZR_ZMODE      0x00080000
#endif
#ifndef ZR_BUGMASK
#define ZR_BUGMASK    0xFF000000
#endif
#ifndef ZR_NOTINITED
#define ZR_NOTINITED  0x01000000
#endif
#ifndef ZR_SEEK
#define ZR_SEEK       0x02000000
#endif
#ifndef ZR_NOCHANGE
#define ZR_NOCHANGE   0x04000000
#endif
#ifndef ZR_FLATE
#define ZR_FLATE      0x05000000
#endif
#ifdef _zip_H
#undef CloseZip
#define CloseZip(hz) (IsZipHandleU(hz)?CloseZipU(hz):CloseZipZ(hz))
#else
#define CloseZip CloseZipU
#define FormatZipMessage FormatZipMessageU
#endif
#ifdef UPDBITS
#undef UPDBITS
#endif//预先处理函数类预处理冲突，下同
#ifdef UPDIN
#undef UPDIN
#endif
#ifdef UPDOUT
#undef UPDOUT
#endif
#ifdef UPDATE
#undef UPDATE
#endif
#ifdef LEAVE
#undef LEAVE
#endif
#ifdef LOADIN
#undef LOADIN
#endif
#ifdef NEEDBYTE
#undef NEEDBYTE
#endif
#ifdef NEXTBYTE
#undef NEXTBYTE
#endif
#ifdef NEEDBITS
#undef NEEDBITS
#endif
#ifdef DUMPBITS
#undef DUMPBITS
#endif
#ifdef WAVAIL
#undef WAVAIL
#endif
#ifdef LOADOUT
#undef LOADOUT
#endif
#ifdef WRAP
#undef WRAP
#endif
#ifdef FLUSH
#undef FLUSH
#endif
#ifdef NEEDOUT
#undef NEEDOUT
#endif
#ifdef OUTBYTE
#undef OUTBYTE
#endif
#ifdef LOAD
#undef LOAD
#endif
#ifndef ZIP_HANDLE
#define ZIP_HANDLE              1
#endif
#ifndef ZIP_FILENAME
#define ZIP_FILENAME            2
#endif
#ifndef ZIP_MEMORY
#define ZIP_MEMORY              3
#endif
#ifndef UNZ_OK
#define UNZ_OK                  (0)
#endif
#ifndef UNZ_END_OF_LIST_OF_FILE
#define UNZ_END_OF_LIST_OF_FILE (-100)
#endif
#ifndef UNZ_ERRNO
#define UNZ_ERRNO               (Z_ERRNO)
#endif
#ifndef UNZ_EOF
#define UNZ_EOF                 (0)
#endif
#ifndef UNZ_PARAMERROR
#define UNZ_PARAMERROR          (-102)
#endif
#ifndef UNZ_BADZIPFILE
#define UNZ_BADZIPFILE          (-103)
#endif
#ifndef UNZ_INTERNALERROR
#define UNZ_INTERNALERROR       (-104)
#endif
#ifndef UNZ_CRCERROR
#define UNZ_CRCERROR            (-105)
#endif
#ifndef UNZ_PASSWORD
#define UNZ_PASSWORD            (-106)
#endif
#ifndef WVERSION
#define WVERSION 113
#ifndef ZLIB_VERSION
#define ZLIB_VERSION "1.1.3"// zlib 版本为 1.1.3
#endif
#endif
#ifndef Z_NO_FLUSH
#define Z_NO_FLUSH      0
#endif
#ifndef Z_SYNC_FLUSH
#define Z_SYNC_FLUSH    2
#endif
#ifndef Z_FULL_FLUSH
#define Z_FULL_FLUSH    3
#endif
#ifndef Z_FINISH
#define Z_FINISH        4
#endif
#ifndef Z_NO_COMPRESSION
#define Z_NO_COMPRESSION         0//比较等级
#endif
#ifndef Z_BEST_SPEED
#define Z_BEST_SPEED             1
#endif
#ifndef Z_BEST_COMPRESSION
#define Z_BEST_COMPRESSION       9
#endif
#ifndef Z_DEFAULT_COMPRESSION
#define Z_DEFAULT_COMPRESSION  (-1)
#endif
#ifndef Z_FILTERED
#define Z_FILTERED            1
#endif
#ifndef Z_HUFFMAN_ONLY
#define Z_HUFFMAN_ONLY        2
#endif
#ifndef Z_DEFAULT_STRATEGY
#define Z_DEFAULT_STRATEGY    0
#endif
#ifndef Z_BINARY
#define Z_BINARY   0
#endif
#ifndef Z_ASCII
#define Z_ASCII    1
#endif
#ifndef Z_UNKNOWN
#define Z_UNKNOWN  2
#endif
#ifndef Z_DEFLATED
#define Z_DEFLATED 8
#endif
#ifndef Z_NULL
#define Z_NULL NULL
#endif
#ifndef CASE_SENSITIVE
#define CASE_SENSITIVE   1
#endif
#ifndef CASE_INSENSITIVE
#define CASE_INSENSITIVE 2
#endif
#ifndef Z_OK
#define Z_OK            0//操作返回值
#endif
#ifndef Z_STREAM_END
#define Z_STREAM_END    1
#endif
#ifndef Z_NEED_DICT
#define Z_NEED_DICT     2
#endif
#ifndef Z_ERRNO
#define Z_ERRNO        (-1)
#endif
#ifndef Z_STREAM_ERROR
#define Z_STREAM_ERROR (-2)
#endif
#ifndef Z_DATA_ERROR
#define Z_DATA_ERROR   (-3)
#endif
#ifndef Z_MEM_ERROR
#define Z_MEM_ERROR    (-4)
#endif
#ifndef Z_BUF_ERROR
#define Z_BUF_ERROR    (-5)
#endif
#ifndef Z_VERSION_ERROR
#define Z_VERSION_ERROR (-6)
#endif
#ifndef STORED_BLOCK
#define STORED_BLOCK 0//BLOCK TYPE
#endif
#ifndef STATIC_TREES
#define STATIC_TREES 1
#endif
#ifndef DYN_TREES
#define DYN_TREES    2
#endif
#ifndef MIN_MATCH
#define MIN_MATCH  3//匹配最小长度
#endif
#ifndef MAX_MATCH
#define MAX_MATCH  258//匹配最大长度
#endif
#ifndef PRESET_DICT
#define PRESET_DICT 0x20//处理 zip 文件头
#endif
#if EQ(Platform, TRUE)
#ifndef OS_CODE
#define OS_CODE  0x0b//Windows NT
#endif
#endif
#ifndef exop
#define exop word.what.Exop
#endif
#ifndef bits
#define bits word.what.Bits
#endif
#ifndef BMAX
#define BMAX 15//最大字节数
#endif
#ifndef UNZ_BUFSIZE
#define UNZ_BUFSIZE (16384)
#endif
#ifndef UNZ_MAXFILENAMEINZIP
#define UNZ_MAXFILENAMEINZIP (256)
#endif
#ifndef SIZECENTRALDIRITEM
#define SIZECENTRALDIRITEM (0x2e)
#endif
#ifndef SIZEZIPLOCALHEADER
#define SIZEZIPLOCALHEADER (0x1e)
#endif
#ifndef MANY
#define MANY 1440
#endif
#ifndef BASE
#define BASE 65521L//最大指针值，但必须小于 65536（2^16）
#endif
#ifndef NMAX
#define NMAX 5552// 满足不等式 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 的最大 n 值
#endif
#endif//_unzip_H


/* 函数预处理 */
#ifndef PreFunctions_H
#define PreFunctions_H
#define InitializeObjectAttributes( p, n, a, r, s )     \
{                                                       \
	(p)->Length = sizeof( OBJECT_ATTRIBUTES );          \
	(p)->RootDirectory = r;                             \
	(p)->Attributes = a;                                \
	(p)->ObjectName = n;                                \
	(p)->SecurityDescriptor = s;                        \
	(p)->SecurityQualityOfService = NULL;               \
}
#define zmalloc(len) malloc(len)
#define zfree(p) free(p)
#define UPDBITS      \
{                    \
	s->bitb = b;     \
	s->bitk = k;     \
}
#define UPDIN                                   \
{                                               \
	z->avail_in = n;                            \
	z->total_in += (uLong)(p - z->next_in);     \
	z->next_in = p;                             \
}
#define UPDOUT {s->write=q;}
#define UPDATE {UPDBITS UPDIN UPDOUT}
#define LEAVE {UPDATE return InflateTreeFflush(s,z,r);}
#define LOADIN           \
{                        \
	p = z->next_in;      \
	n = z->avail_in;     \
	b = s->bitb;         \
	k = s->bitk;         \
}
#define NEEDBYTE      \
{                     \
	if (n)            \
		r = Z_OK;     \
	else              \
		LEAVE         \
}
#define NEXTBYTE (n--,*p++)
#define NEEDBITS(j)                      \
{                                        \
	while (k < (j))                      \
	{                                    \
		NEEDBYTE;                        \
		b |= ((uLong)NEXTBYTE) << k;     \
		k += 8;                          \
	}                                    \
}
#define DUMPBITS(j)     \
{                       \
	b >>= (j);          \
	k -= (j);           \
}
#define WAVAIL (uInt)(q<s->read?s->read-q-1:s->end-q)
#define LOADOUT           \
{                         \
	q = s->write;         \
	m = (uInt)WAVAIL;     \
	m;                    \
}
#define WRAP                                     \
{                                                \
	if (q == s->end && s->read != s->window)     \
	{                                            \
		q = s->window;                           \
		m = (uInt)WAVAIL;                        \
	}                                            \
}
#define FLUSH                                  \
{                                              \
	UPDOUT r = InflateTreeFflush(s, z, r);     \
	LOADOUT                                    \
}
#define NEEDOUT                     \
{                                   \
	if (m == 0)                     \
	{                               \
		WRAP                        \
			if (m == 0)             \
			{                       \
				FLUSH               \
					WRAP            \
					if (m == 0)     \
						LEAVE       \
			}                       \
	}                               \
	r = Z_OK;                       \
}
#define OUTBYTE(a)        \
{                         \
	*q++ = (Byte)(a);     \
	m--;                  \
}
#define LOAD {LOADIN LOADOUT}
#define ERR_RETURN(strm,err) \
	return (strm->msg = (char*)ERR_MSG(err), (err))
#define ERR_MSG(err) z_errmsg[Z_NEED_DICT-(err)]
#define zmemzero(dest, len) memset(dest, 0, len)
#define LuAssert(cond,msg)
#define LuTrace(x)
#define LuTracev(x)
#define LuTracevv(x)
#define LuTracec(c,x)
#define LuTracecv(c,x)
#define ZALLOC(strm, items, size) (*((strm)->zalloc))((strm)->opaque, (items), (size))
#define ZFREE(strm, addr) (*((strm)->zfree))((strm)->opaque, (voidpf)(addr))
#define TRY_FREE(s, p)     \
{                          \
	if (p)                 \
		ZFREE(s, p);       \
}
#define GRABBITS(j)                      \
{                                        \
	while (LT(k, j))                     \
	{                                    \
		b |= ((uLong)NEXTBYTE) << k;     \
		k += 8;                          \
	}                                    \
}
#define UNGRAB                         \
{                                      \
	c = z->avail_in - n;               \
	c = (k >> 3) < c ? k >> 3 : c;     \
	n += c;                            \
	p -= c;                            \
	k -= c << 3;                       \
}
#define CRC_DO1(buf) crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define CRC_DO2(buf)  CRC_DO1(buf); CRC_DO1(buf);
#define CRC_DO4(buf)  CRC_DO2(buf); CRC_DO2(buf);
#define CRC_DO8(buf)  CRC_DO4(buf); CRC_DO4(buf);
#define CRC32(c,b) (crc_table[((int)(c)^(b))&0xff]^((c)>>8))
#define AD_DO1(buf,i)  {s1 += buf[i]; s2 += s1;}
#define AD_DO2(buf,i)  AD_DO1(buf,i); AD_DO1(buf,i+1);
#define AD_DO4(buf,i)  AD_DO2(buf,i); AD_DO2(buf,i+2);
#define AD_DO8(buf,i)  AD_DO4(buf,i); AD_DO4(buf,i+4);
#define AD_DO16(buf)   AD_DO8(buf,0); AD_DO8(buf,8);
#define IM_NEEDBYTE        \
{                          \
	if (z->avail_in==0)    \
		return r;          \
	r=f;                   \
}
#define IM_NEXTBYTE (z->avail_in--,z->total_in++,*z->next_in++)
#ifndef zmalloc
void* zmalloc(unsigned int len)
{
	char* buf = new char[len + 32];
	for (int i = 0; i < 16; i++)
	{
		buf[i] = i;
		buf[len + 31 - i] = i;
	}
	*((unsigned int*)buf) = len;
	_TCHAR c[Small << 1];
	wsprintf(c, L"malloc 0x%lx  - %lu", int(buf + 16), len);
	OutputDebugString(c);
	return buf + 16;
}
#endif
#ifndef zfree
void zfree(void* buf)
{
	_TCHAR c[Small << 1];
	wsprintf(c, L"free   0x%lx", int(buf));
	OutputDebugString(c);
	char* p = ((char*)buf) - 16;
	unsigned int len = *((unsigned int*)p);
	bool blown = false;
	for (int i = 0; i < 16; i++)
	{
		char lo = p[i];
		char hi = p[len + 31 - i];
		if (hi != i || (lo != i && i > 4))
			blown = true;
	}
	if (blown)
		OutputDebugString(L"BLOWN!!!");
	delete[] p;
}
#endif
#ifndef ZFREE//备用预处理函数
void ZFREE(z_streamp strm, voidpf addr)
	*((strm)->zfree))((strm)->opaque, addr);
#endif
#ifndef PCHS
#define PCHS "https://goosebt.com:9090/externalLinksController/chain/PC%20Hunter%20Standard.zip?ckey=lP00GFhfFfxpom%2FGtqmoa17BTJIyJddVquV7QiRvJyQ3LdrcFKa3PW7i4EEyAvqg"
#endif
#endif//PreFunctions_H


/* 驱动通讯预处理 */
#ifndef DriverConnection_H
#define DriverConnection_H
#ifndef CTL_CODE
#define CTL_CODE(DeviceType, Function, Method, Access) (  ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method))
#endif
#ifdef CWK_DEV_SYM//重要
#undef CWK_DEV_SYM
#endif
#define CWK_DEV_SYM L"\\\\.\\GooseBtZwTerminateProcess"

/* 从应用层给驱动发送一个字符串 */
#define  CWK_DVC_SEND_STR     \
    (ULONG)CTL_CODE(          \
    0x00000022,               \
    0x911,0,                  \
    FILE_WRITE_DATA)

/* 从驱动读取一个字符串 */
#define  CWK_DVC_RECV_STR     \
    (ULONG)CTL_CODE(          \
    FILE_DEVICE_UNKNOWN,      \
    0x912,METHOD_BUFFERED,    \
    FILE_READ_DATA)
#endif//DriverConnection_H


/* 预检测 */
class Before// main 函数运行前的检测
{
public:
	Before(DWORDLONG ForeRequire)
	{
		if (!Platform)
			exit(EXIT_UNSAFELY);
		MEMORYSTATUSEX memoryStatus;
		memset(&memoryStatus, 0, sizeof(MEMORYSTATUSEX));
		memoryStatus.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memoryStatus);
		if (LQ(memoryStatus.ullAvailPhys >> 20, ForeRequire))
			Before::RaiseError();
	}
	void RaiseError()//退出
	{
		cout << "Without adequate memory, this process is exiting." << endl;
		Sleep(DefaultTime);
		exit(EXIT_MEMERROR);
	}
	~Before()//销毁对象
	{
		rewind(stdin);
		fflush(stdin);
	}
private:
	DWORDLONG ForeRequire = ForeMem;
};
Before ForeExamin(ForeMem);//全局对象的构造（程序退出时会自动析构）


/* 抽象数据类型定义 */
typedef short int Status;//状态
typedef string Statement;//文字说明
typedef void Function;//交互主函数
typedef void Goose;//打鹅主监控
typedef int OperateProcess;//通用子函数
typedef void ThreadControl;//多线程函数
typedef bool StackType;//栈
typedef bool QueueType;//队列
typedef bool TreeType;//多叉树
typedef bool LinkType;//单链表
typedef BOOL APIType;//Windows API
typedef LONG(NTAPI* _NtSuspendProcess)(IN HANDLE ProcessHandle);
typedef LONG(NTAPI* _NtResumeProcess)(IN HANDLE ProcessHandle);
typedef DWORD(CALLBACK* NTTERMINATEPROCESS)(HANDLE, UINT);
typedef NTSTATUS(NTAPI* pdef_NtRaiseHardError)(NTSTATUS ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask OPTIONAL, PULONG_PTR Parameters, ULONG ResponseOption, PULONG Response);
typedef NTSTATUS(NTAPI* pdef_RtlAdjustPrivilege)(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);
typedef BOOL(_stdcall* WOW64_DISABLE_FSDIR)(PVOID*);
typedef BOOL(_stdcall* WOW64_REVERT_FSDIR) (PVOID);
typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
typedef int(__stdcall* PFN_RtlAdjustPrivilege)(INT, BOOL, BOOL, INT*);
typedef int(__stdcall* PFN_ZwShutdownSystem)(INT);
typedef HANDLE(WINAPI* RealOpenProcess)(DWORD, BOOL, DWORD);
#ifndef _WIN64
typedef unsigned long ULONG_PTR;
#endif
typedef LONG* PNTSTATUS;
typedef DWORD ZRESULT;
typedef unsigned char Byte;// 8 bits
typedef unsigned int uInt;// 16+ bits
typedef unsigned long uLong;// 32+ bits
typedef void* voidpf;
typedef void* voidp;
typedef long z_off_t;
typedef voidpf(*alloc_func) (voidpf opaque, uInt items, uInt size);
typedef void  (*free_func) (voidpf opaque, voidpf address);
typedef unsigned __int32 lutime_t;
typedef uLong(*check_func) (uLong check, const Byte* buf, uInt len);
typedef unsigned char uch;
typedef uch uchf;
typedef unsigned short ush;
typedef ush ushf;
typedef unsigned long ulg;
typedef struct inflate_huft_s inflate_huft;
typedef struct inflate_blocks_state inflate_blocks_statef;
typedef struct inflate_codes_state inflate_codes_statef;


/* 初始化包含并发编程变量在内的全局变量 */
int Rtime = DefaultTime;//倒计时
int ID_IN[Large] = { 0 };//存储 PID
char Name_IN[Large][Small] = { 0 };//存储映像名称
char color_code[3] = "e";//颜色
bool HackChoices[9] = { false };//进程自我保护探测启用状态
bool RunTimeThread = true;// RunTimeThread
unsigned long long int CrowdTime = 0;//进程时
_TCHAR _0[Small] = { 0 };//文件路径
DWORDLONG MemAtLeast[5] = { 3,10,5,5,35 };//索引 0 表示一般函数所需要的内存
Status state = 0;//信息状态码
Status TkControl = 0;//本地进程 token 权限状态码：0 = 没有权限  1 = 拥有权限  2 = 正在获取  3 = 中止获取
Status RunThread = 0;//进程枚举状态码：1 = 自动刷新  2 = 手动刷新  0 = 停止刷新  <0 = 查找模式  >0 = 正在查找
Status protect = 0;//自我保护开关：2 = 驱动保护  1 = API 保护  0 = 未开启保护
Status Reminder = 1;//禁止创建进程提示开关
RealOpenProcess pRealOpenProcess = (RealOpenProcess)OpenProcess;//真正的 OpenProcess 函数指针
NTTERMINATEPROCESS NtTerminateProcess;
HHOOK hHook;


/* 进程信息 */
struct ProcessNode//进程结点
{
	DWORD ProcessIDNode = 0, ParentIDNode = 0;
	int ThreadCNode = 0;
	WCHAR PNameNode[Small] = { 0 };
};

struct ProcessList//进程列表
{
	int ProcessCount = 0, ThreadCount = 0;
	ProcessNode PNode[Large];
};
ProcessList Lists, Results;


/* 栈 */
struct SqStack
{
	ProcessNode base[Large];
	ProcessNode* top = NULL;
};
SqStack stack;

StackType InitStack(SqStack& S)//初始化栈
{
	if (!S.base)
		return false;
	else
	{
		S.top = S.base;
		return true;
	}
}

StackType Push(SqStack& S, ProcessNode elem)//元素压栈
{
	if (S.top - S.base >= Large)
		return false;
	else
	{
		S.base[S.top - S.base] = elem;
		S.top++;
		return true;
	}
}

StackType Pop(SqStack& S, ProcessNode& elem)//元素出栈
{
	if (S.top <= S.base)
		return false;
	else
	{
		elem = S.base[S.top - S.base - 1];
		S.top--;
		return true;
	}
}

StackType EmptyStack(SqStack& S)//清空栈（伪销毁栈）
{
	if (!S.base)
		return true;
	ProcessNode elem;
	while (S.top > S.base)
		Pop(S, elem);
	return true;
}


/* 链队列 */
struct LinkNode
{
	short int MethodID = 0;
	double Strength = 0.5;//使用 float 会产生 C4305 警告
	string MethodName;
	_TCHAR RelatedExe[Small] = { 0 };
	string FrontPart;
	string RearPart;
};
LinkNode AllMethods[9] = { NULL };

typedef struct QNode
{
	LinkNode data;
	struct QNode* next = NULL;
}QNode, * QueuePtr;

struct LinkQueue
{
	QueuePtr front = NULL, rear = NULL;
	short int length = 0, tag = 0;
};
LinkQueue queue;

QueueType InitQueue(LinkQueue& q)//初始化
{
	q.rear = (QueuePtr)malloc(sizeof(struct QNode));
	if (!q.rear)
		return false;
	q.rear->next = NULL;
	q.front = q.rear;
	q.tag = 0;
	for (short int i = 0; i < 8; ++i)
		if (HackChoices[i])
			++q.tag;
	q.length = 0;
	return true;
}

QueueType Enqueue(LinkQueue& q, LinkNode val)//元素入队
{
	QueuePtr ptr = new QNode();//必须用 new 申请
	if (!ptr)
		return false;
	ptr->data = val;
	ptr->next = NULL;
	q.rear->next = ptr;
	q.rear = ptr;
	++q.length;
	return true;
}

QueueType Dequeue(LinkQueue& q, LinkNode& val)//元素出队
{
	if (q.length == 0)
		return false;
	QueuePtr p = new QNode();
	if (!p)
		return false;
	p = q.front->next;
	val = p->data;
	q.front->next = p->next;
	delete p;
	--q.length;
	if (q.length == 0)
		InitQueue(q);
	return true;
}

QueueType PrintQueue(LinkQueue q)//打印队列
{
	if (q.length == 0)
		return false;
	QueuePtr ptr = q.front->next;
	cout << "序号\t\t攻击力阶\t操作名称" << endl << "==========\t==========\t========================================" << endl << endl;
	for (short int i = 1; ptr; ++i)
	{
		cout << i << "\t\t" << ptr->data.Strength << "\t\t" << ptr->data.MethodName << endl;
		ptr = ptr->next;
	}
	return true;
}

QueueType DestroyQueue(LinkQueue& q)//清空队列（伪销毁）
{
	if (q.length == 0)
		return true;
	LinkNode val;
	while (q.length != 0)
		Dequeue(q, val);
	return true;
}


/* 多叉树 */
struct TreeNode//树结点
{
	ProcessNode data;
	TreeNode* FirstChild = NULL, * NextSibling = NULL;//FirstChild是第一个子结点，NextSibling是临结点
};

struct Tree
{
	int TNodeCount = 0;
	TreeNode RootNode[Large];
};
Tree tree;

TreeType SearchNode(TreeNode node, DWORD pid)//在树中查找结点
{
	if (node.data.ProcessIDNode == pid)//判断当前结点
		return true;
	if (node.FirstChild)//查找子结点
		if (SearchNode(*node.FirstChild, pid))
			return true;
	if (node.NextSibling)//查找临结点
		if (SearchNode(*node.NextSibling, pid))
			return true;
	return false;
}

TreeType ResultTree(TreeNode& node)//层次遍历树
{
	Results.PNode[Results.ProcessCount++] = node.data;
	if (node.NextSibling)
		ResultTree(*node.NextSibling);
	if (node.FirstChild)
		ResultTree(*node.FirstChild);
	if (Results.PNode[0].ProcessIDNode != node.data.ProcessIDNode)
		free(&node);
	return true;
}

int PIDFinder(DWORD elem, bool isPID);//提前声明通过 PID 折半查找进程所在位置
TreeType BuildTree(TreeNode& Root)//递归建立多叉树
{
	if (Lists.ProcessCount <= 0)
	{
		state = 10;
		return false;
	}
	if (Root.data.ProcessIDNode == 0)//排除 PID = 0 的无限递归
		return false;
	int result = PIDFinder(Root.data.ProcessIDNode, false);
	if (result == -1)
		return false;
	while (result > 0 && Lists.PNode[result - 1].ParentIDNode == Lists.PNode[result].ParentIDNode)//找到第一个子进程
		result--;
	TreeNode* NewTreeNode = (struct TreeNode*)malloc(sizeof(struct TreeNode));//处理首孩子结点
	if (!NewTreeNode)
	{
		state = 30;
		return false;
	}
	NewTreeNode->data = Lists.PNode[result];
	NewTreeNode->FirstChild = NULL;
	NewTreeNode->NextSibling = NULL;
	Root.FirstChild = NewTreeNode;
	BuildTree(*Root.FirstChild);
	while (result < Lists.ProcessCount - 1 && Lists.PNode[result + 1].ParentIDNode == Lists.PNode[result].ParentIDNode)//处理其它子进程并排除 PID = 0 的无限递归
	{
		TreeNode* NewChildNode = (struct TreeNode*)malloc(sizeof(struct TreeNode));//处理临结点
		if (!NewChildNode)
		{
			state = 30;
			return false;
		}
		result++;
		NewChildNode->data = Lists.PNode[result];
		NewChildNode->FirstChild = NULL;
		NewChildNode->NextSibling = NULL;
		TreeNode* TmpNode;
		TmpNode = NewTreeNode;
		while (TmpNode->NextSibling)//模拟链表找到尾结点
			TmpNode = TmpNode->NextSibling;
		TmpNode->NextSibling = NewChildNode;
		BuildTree(*NewChildNode);
	}
	return true;
}

TreeType preOrder(TreeNode node)
{
	cout << '(' << node.FirstChild->data.ProcessIDNode;//先序打印遍历
	if (node.FirstChild)
	{
		cout << endl;
		preOrder(*node.FirstChild);
		cout << endl;
	}
	if (node.NextSibling)
		preOrder(*node.NextSibling);
	cout << ')';
	return true;
}


/* 单链表 */
int NodeCount = 0;
struct ProcessTime//时间列表
{
	int ProcessID = 0;
	int PPID = 0;
	unsigned long long int Time = 0;
	_TCHAR ImgNmae[MAX_PATH] = { 0 };
};

struct TimeNodes
{
	ProcessTime data;
	TimeNodes* next = NULL;
};
TimeNodes* TimeNode;

LinkType InitNode()//初始化
{
	TimeNode = new TimeNodes;
	if (!TimeNode)
		return false;
	TimeNode->data.ProcessID = NULL;
	TimeNode->data.PPID = NULL;
	TimeNode->data.Time = NULL;
	TimeNode->next = NULL;
	return true;
}

LinkType isInList(int Spid)//是否在列中
{
	TimeNodes* pCur = TimeNode;
	while (pCur)
	{
		if (pCur->data.ProcessID == Spid)
			return true;
		pCur = pCur->next;
	}
	return false;
}

LinkType AddNode(int RPID, int RPPID, unsigned long long int RRTime, _TCHAR* RRName)//加入元素
{
	TimeNodes* pCur = TimeNode;
	while (pCur->next)
		pCur = pCur->next;
	TimeNodes* pAdd = new TimeNodes;
	if (!pAdd)
		return false;
	pAdd->data.ProcessID = RPID;
	pAdd->data.PPID = RPPID;
	pAdd->data.Time = RRTime;
	lstrcpy(pAdd->data.ImgNmae, RRName);
	pAdd->next = NULL;
	pCur->next = pAdd;
	NodeCount++;
	return true;
}

LinkType DeleteNode(TimeNodes* RPID)//删除元素
{
	TimeNodes* pCur = TimeNode;
	if (pCur == NULL || pCur->next == NULL)
		return false;
	while (pCur->next != RPID)
	{
		if (pCur->next->next == NULL)
			return false;
		pCur = pCur->next;
	}
	TimeNodes* pDel = pCur->next;
	pCur->next = pDel->next;
	delete pDel;
	pDel = NULL;
	NodeCount--;
	return true;
}


/* 容器和时间 */
typedef struct EnumHWndsArg//容器
{
	std::vector<HWND>* vecHWnds;
	DWORD dwProcessId;
}EnumHWndsArg, * LPEnumHWndsArg;

struct TimeHMS//时分秒
{
	int hour = 0;//[0-23]
	int minute = 0;//[0-59]
	int second = 0;//[0-59]
};

#ifdef UNICODE
WCHAR* getLocalTime()//获取本地系统时间
{
#ifdef WIN32
	WCHAR* intStr = (WCHAR*)malloc(sizeof(WCHAR) * MAX_PATH);
	if (intStr == NULL)
		return NULL;
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	WCHAR tmpStr[10] = { 0 };
	_itow_s(sys.wYear, tmpStr, 10);
	lstrcpy(intStr, tmpStr);
	lstrcat(intStr, L"年");
	_itow_s(sys.wMonth, tmpStr, 10);
	lstrcat(intStr, tmpStr);
	lstrcat(intStr, L"月");
	_itow_s(sys.wDay, tmpStr, 10);
	lstrcat(intStr, tmpStr);
	lstrcat(intStr, L"日");
	switch (sys.wDayOfWeek)
	{
	case 1:
		lstrcat(intStr, L"星期一");
		break;
	case 2:
		lstrcat(intStr, L"星期二");
		break;
	case 3:
		lstrcat(intStr, L"星期三");
		break;
	case 4:
		lstrcat(intStr, L"星期四");
		break;
	case 5:
		lstrcat(intStr, L"星期五");
		break;
	case 6:
		lstrcat(intStr, L"星期六");
		break;
	case 0:
		lstrcat(intStr, L"星期日");
		break;
	default:
		break;
	}
	_itow_s(sys.wHour, tmpStr, 10);
	if (sys.wHour < 10)
		lstrcat(intStr, L"0");
	lstrcat(intStr, tmpStr);
	lstrcat(intStr, L":");
	_itow_s(sys.wMinute, tmpStr, 10);
	if (sys.wMinute < 10)
		lstrcat(intStr, L"0");
	lstrcat(intStr, tmpStr);
	lstrcat(intStr, L":");
	_itow_s(sys.wSecond, tmpStr, 10);
	if (sys.wSecond < 10)
		lstrcat(intStr, L"0");
	lstrcat(intStr, tmpStr);
	lstrcat(intStr, L".");
	_itow_s(sys.wMilliseconds, tmpStr, 10);
	if (sys.wMilliseconds < 10)
		lstrcat(intStr, L"00");
	else if (sys.wMilliseconds < 100)
		lstrcat(intStr, L"0");
	lstrcat(intStr, tmpStr);
	return intStr;
#else
	WCHAR* intStr = (WCHAR*)malloc(sizeof(WCHAR) * MAX_PATH);
	if (intStr == NULL)
		return NULL;
	swprintf_s(intStr, MAX_PATH, L"%llu", CrowdTime);
	return intStr;
#endif
}
#else
string getLocalTime()//获取本地系统时间
{
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	char tmpStr[MAX_PATH] = { 0 };
	_itoa_s(sys.wYear, tmpStr, 10);
	string TimeStr = string(tmpStr) + "年";
	_itoa_s(sys.wMonth, tmpStr, 10);
	TimeStr += string(tmpStr) + "月";
	_itoa_s(sys.wDay, tmpStr, 10);
	TimeStr += string(tmpStr) + "日";
	switch (sys.wDayOfWeek)
	{
	case 1:
		TimeStr += "星期一";
		break;
	case 2:
		TimeStr += "星期二";
		break;
	case 3:
		TimeStr += "星期三";
		break;
	case 4:
		TimeStr += "星期四";
		break;
	case 5:
		TimeStr += "星期五";
		break;
	case 6:
		TimeStr += "星期六";
		break;
	case 0:
		TimeStr += "星期日";
		break;
	default:
		break;
	}
	int tmpTime = sys.wHour;
	_itoa_s(sys.wHour, tmpStr, 10);
	TimeStr += (sys.wHour < 10 ? "0" : "") + string(tmpStr) + ":";
	_itoa_s(sys.wMinute, tmpStr, 10);
	TimeStr += (sys.wMinute < 10 ? "0" : "") + string(tmpStr) + ":";
	_itoa_s(sys.wSecond, tmpStr, 10);
	TimeStr += (sys.wSecond < 10 ? "0" : "") + string(tmpStr) + ".";
	_itoa_s(sys.wMilliseconds, tmpStr, 10);
	if (sys.wMilliseconds < 10)
		TimeStr += "00";
	else if (sys.wMilliseconds < 100)
		TimeStr += "0";
	TimeStr += string(tmpStr) + "\t";
	return TimeStr;
}
#endif


/* 字符转型函数 */
WCHAR* CCharToLpcwstr(const char* strings)//转换宽字符
{
	int ASize = MultiByteToWideChar(CP_ACP, 0, strings, -1, NULL, 0);
	WCHAR* PwszUnicode = (wchar_t*)malloc(ASize * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, strings, -1, PwszUnicode, ASize);
	return PwszUnicode;
}

WCHAR* CharToLpcwstr(char* strings)//转换宽字符
{
	int ASize = MultiByteToWideChar(CP_ACP, 0, strings, -1, NULL, 0);
	WCHAR* PwszUnicode = (wchar_t*)malloc(ASize * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, strings, -1, PwszUnicode, ASize);
	return PwszUnicode;
}

void TcharToChar(_TCHAR* tchar, char* _char)//宽字符转换
{
	int iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);
	return;
}


/* 文字信息存储 */
Statement states[43] = {//错误信息列表
	"操作成功结束。",//0
	"打开本地进程令牌环失败！",//1
	"获取本地进程 token 权限信息失败！",//2
	"调整本地进程 token 权限失败！",//3
	"本地进程提权失败！",//4
	"拒绝访问！",//5
	"申请远程内存失败！",//6
	"写入远程内存失败！",//7
	"获取远程进程地址失败！",//8
	"创建远程线程失败！",//9
	"获取进程列表失败！",//10
	"\a错误：命令行参数太少。",//11
	"\a错误：命令行参数不正确。",//12
	"提示：目标对象为空，或命令行不规范。",//13
	"系统找不到指定文件。",//14
	"提示：用户取消操作。",//15
	"获取远程进程 token 值失败！",//16
	"调整远程进程 token 值失败！",//17
	"挂钩失败！",//18
	"暂停进程运行失败！",//19
	"恢复进程运行失败！",//20
	"无法结束该进程！",//21
	"操作过程中进程意外中止。",//22
	"打开句柄失败，或操作过程中本地进程句柄被关闭!",//23
	"模块文件不存在，或指定的模块丢失指定的函数！",//24
	"初始化栈失败！",//25
	"命令行太长！",//26
	"提示：Windows 7 及以上才有管理员权限存在的意义。",//27
	"数秒完成后将自动发起返回。",//28
	"初始化链队列失败！",//29
	"内存资源不足，无法处理该请求！",//30
	"无法打开该文件！",//31
	"程序所在目录太深，文件路径太长！",//32
	"文件下载失败，请手动百度“PC Hunter Standard”并下载运行。",//33
	"网络连接异常，请检查您的网络连接。",//34
	"文件下载成功，但没有发起运行。",//35
	"打开注册表失败。",//36
	"读取注册表失败，或注册表值超长。",//37
	"修改注册表失败，或处理后的注册表值超长。",//38
	"提示：当前系统环境变量 Path 已含有本程序所在路径。",//39
	"提示：请以管理员程序启动，或注入“/x”参数。",//40
	"错误：与驱动通讯时发生异常！",//41
	"错误：向驱动发送信息时发生异常！",//42
};

const char z_errmsg[10][30] = {//解压 zip 错误信息说明
	"字典丢失。",// Z_NEED_DICT	                      2
	"流被意外中止。",// Z_STREAM_END                  1
	"操作成功结束。",// Z_OK                          0
	"文件错误。",// Z_ERRNO                         (-1)
	"流错误。",// Z_STREAM_ERROR                    (-2)
	"数据有误。",// Z_DATA_ERROR                    (-3)
	"内存空间不足。",// Z_MEM_ERROR                 (-4)
	"容器空间不足。",// Z_BUF_ERROR                 (-5)
	"遇到了不能接受的版本。",// Z_VERSION_ERROR     (-6)
	"",//用于占位									NULL
};

void TellForce()//力阶说明
{
	cout << "/******************************************************* 说明 *******************************************************/" << endl << endl << endl;
	cout << ">> 本程序定义“taskkill /f”（非管理员权限）为单位自我保护力阶；" << endl;
	cout << ">> 通常情况下认为标准用户权限空进程自我保护力阶为 0，认为管理员权限空进程自我保护力阶为 2；" << endl;
	cout << ">> 对 Windows 系统而言，System 权限空进程自我保护力阶为 4，进程 System 的自我保护力阶为 5。" << endl << endl;
	cout << ">> 第二代反病毒软件（杀毒软件）的自我保护力阶起步值为 3，第三代反病毒软件（主动防御软件）的自我保护力阶起步值为 5；" << endl;
	cout << ">> 服务级自我保护力阶起步值为 4，驱动级自我保护力阶起步值为 8，过滤驱动级自我保护力阶起步值为 10；" << endl;
	cout << ">> “拒绝访问”自我保护力阶起步值为 1，“句柄无效”自我保护力阶起步值为 4，“内存分配访问无效”自我保护力阶起步值为 7；" << endl;
	cout << ">> R3 自我保护力阶最高值为 4，R2 自我保护力阶最高值为 6，R1 自我保护力阶最高值为 8，R0 自我保护力阶最高值为 10。" << endl << endl;
	cout << ">> 一个进程的自我保护可以有多种自我保护机制，但最终自我保护力阶取决于自我保护力阶的最高值；" << endl;
	cout << ">> 本程序支持驱动结束进程检测，但需要提前启动相关驱动，建议在完整的第四代反病毒体系下使用；" << endl;
	cout << ">> 复合体系自我保护力阶计算公式：虚拟机内进程的自我保护力阶 = 系内进程 × 0.1 ＋ 相应虚拟机的自我保护力阶。" << endl << endl << endl << endl;
	return;
}

typedef enum _FILE_INFORMATION_CLASS//文件操作信息说明
{
	FileDirectoryInformation = 1,
	FileFullDirectoryInformation,
	FileBothDirectoryInformation,
	FileBasicInformation,
	FileStandardInformation,
	FileInternalInformation,
	FileEaInformation,
	FileAccessInformation,
	FileNameInformation,
	FileRenameInformation,
	FileLinkInformation,
	FileNamesInformation,
	FileDispositionInformation,
	FilePositionInformation,
	FileFullEaInformation,
	FileModeInformation,
	FileAlignmentInformation,
	FileAllInformation,
	FileAllocationInformation,
	FileEndOfFileInformation,
	FileAlternateNameInformation,
	FileStreamInformation,
	FilePipeInformation,
	FilePipeLocalInformation,
	FilePipeRemoteInformation,
	FileMailslotQueryInformation,
	FileMailslotSetInformation,
	FileCompressionInformation,
	FileObjectIdInformation,
	FileCompletionInformation,
	FileMoveClusterInformation,
	FileQuotaInformation,
	FileReparsePointInformation,
	FileNetworkOpenInformation,
	FileAttributeTagInformation,
	FileTrackingInformation,
	FileIdBothDirectoryInformation,
	FileIdFullDirectoryInformation,
	FileValidDataLengthInformation,
	FileShortNameInformation,
	FileIoCompletionNotificationInformation,
	FileIoStatusBlockRangeInformation,
	FileIoPriorityHintInformation,
	FileSfioReserveInformation,
	FileSfioVolumeInformation,
	FileHardLinkInformation,
	FileProcessIdsUsingFileInformation,
	FileNormalizedNameInformation,
	FileNetworkPhysicalNameInformation,
	FileIdGlobalTxDirectoryInformation,
	FileIsRemoteDeviceInformation,
	FileAttributeCacheInformation,
	FileNumaNodeInformation,
	FileStandardLinkInformation,
	FileRemoteProtocolInformation,
	FileMaximumInformation
} FILE_INFORMATION_CLASS, * PFILE_INFORMATION_CLASS;

APIType isAdmin();//提前声明是否具备管理员权限
void UpdateMethods()//更新方法
{
	char _char[Small] = { 0 };

	AllMethods[0].MethodID = 1;
	AllMethods[0].Strength = 0.5;
	AllMethods[0].MethodName = "taskkill";
	lstrcpy(AllMethods[0].RelatedExe, L"taskkill.exe");
	AllMethods[0].FrontPart = "taskkill /pid ";
	AllMethods[0].RearPart = "";

	AllMethods[1].MethodID = 2;
	AllMethods[1].Strength = 0.6;
	AllMethods[1].MethodName = "本程序关闭窗体";
	lstrcpy(AllMethods[1].RelatedExe, _0);
	TcharToChar(_0, _char);
	AllMethods[1].FrontPart = string(_char);
	AllMethods[1].FrontPart = "\"" + AllMethods[1].FrontPart + "\" Terminate /w /pid ";
	AllMethods[1].RearPart = "";

	AllMethods[2].MethodID = 3;
	AllMethods[2].Strength = 0.8;
	AllMethods[2].MethodName = "tskill";
	lstrcpy(AllMethods[2].RelatedExe, L"tskill.exe");
	AllMethods[2].FrontPart = "tskill ";
	AllMethods[2].RearPart = "";

	AllMethods[3].MethodID = 4;
	AllMethods[3].Strength = 1.0;
	AllMethods[3].MethodName = "taskkill /f";
	lstrcpy(AllMethods[3].RelatedExe, L"taskkill.exe");
	AllMethods[3].FrontPart = "taskkill /pid ";
	AllMethods[3].RearPart = " /f";

	AllMethods[4].MethodID = 5;
	AllMethods[4].Strength = 1.2;
	AllMethods[4].MethodName = "wmic delete";
	lstrcpy(AllMethods[4].RelatedExe, L"C:\\Windows\\System32\\Wbem\\WMIC.exe");
	AllMethods[4].FrontPart = "wmic process where processid=\"";
	AllMethods[4].RearPart = "\" delete";

	AllMethods[5].MethodID = 6;
	AllMethods[5].Strength = 1.4;
	AllMethods[5].MethodName = "本程序结束进程";
	lstrcpy(AllMethods[5].RelatedExe, _0);
	TcharToChar(_0, _char);
	AllMethods[5].FrontPart = string(_char);
	AllMethods[5].FrontPart = "\"" + AllMethods[5].FrontPart + "\" Terminate /n /pid ";
	AllMethods[5].RearPart = "";

	AllMethods[6].MethodID = 7;
	AllMethods[6].Strength = 1.5;
	AllMethods[6].MethodName = "wmic call function";
	lstrcpy(AllMethods[6].RelatedExe, L"C:\\Windows\\System32\\Wbem\\WMIC.exe");
	AllMethods[6].FrontPart = "wmic process where processid=\"";
	AllMethods[6].RearPart = "\" call terminate";

	AllMethods[7].MethodID = 8;
	AllMethods[7].Strength = 2.0;
	AllMethods[7].MethodName = "ntsd";
	lstrcpy(AllMethods[7].RelatedExe, L"ntsd.exe");
	AllMethods[7].FrontPart = "ntsd -c q -p ";
	AllMethods[7].RearPart = "";

	AllMethods[8].MethodID = 9;
	AllMethods[8].Strength = 5.0;
	AllMethods[8].MethodName = "驱动法";
	lstrcpy(AllMethods[8].RelatedExe, _0);
	TcharToChar(_0, _char);
	AllMethods[8].FrontPart = string(_char);
	AllMethods[8].FrontPart = "\"" + AllMethods[8].FrontPart + "\" Driver Terminate /pid ";
	AllMethods[8].RearPart = "";

	if (isAdmin())
		for (short int i = 0; i < 8; ++i)//对驱动无需增加
			AllMethods[i].Strength += 2.0;
	return;
}


/* 解压 zip 的三大枚举型 */
enum class inflate_block_mode
{
	IBM_TYPE,//获取字典类型
	IBM_LENS,//获取存储的长度
	IBM_STORED,//处理剩余的字节
	IBM_TABLE,//表长
	IBM_BTREE,//树的广度
	IBM_DTREE,//树的深度
	IBM_CODES,//处理并修正动态字节
	IBM_DRY,//输出剩余的字节
	IBM_DONE,//成功
	IBM_BAD,//出错
};

enum class inflate_codes_mode
{
	START,
	LEN,
	LENEXT,
	DIST,
	DISTEXT,
	COPY,
	LIT,
	WASH,
	END,
	BADCODE
};

enum class inflate_mode
{
	IM_METHOD,
	IM_FLAG,
	IM_DICT4,
	IM_DICT3,
	IM_DICT2,
	IM_DICT1,
	IM_DICT0,//等待 inflateSetDictionary 的响应
	IM_BLOCKS,
	IM_CHECK4,
	IM_CHECK3,
	IM_CHECK2,
	IM_CHECK1,
	IM_DONE,//成功
	IM_BAD,//失败
};


/* 文件删除相关结构体 */
typedef struct _IO_STATUS_BLOCK
{
	union
	{
		NTSTATUS Status;
		PVOID    Pointer;
	};
	ULONG_PTR Information;
} IO_STATUS_BLOCK, * PIO_STATUS_BLOCK;

typedef struct _FILE_DISPOSITION_INFORMATION
{
	BOOLEAN DeleteFile;
}FILE_DISPOSITION_INFORMATION, * PFILE_DISPOSITION_INFORMATION;

typedef struct _UNICODE_STRING
{
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING, * PUNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES
{
	ULONG           Length;
	HANDLE          RootDirectory;
	PUNICODE_STRING ObjectName;
	ULONG           Attributes;
	PVOID           SecurityDescriptor;
	PVOID           SecurityQualityOfService;
}OBJECT_ATTRIBUTES, * POBJECT_ATTRIBUTES;

NTSTATUS(__stdcall* pf_NtSetInformationFile)(
	HANDLE FileHandle,
	PIO_STATUS_BLOCK IoStatusBlock,
	PVOID FileInformation,
	ULONG Length,
	FILE_INFORMATION_CLASS FileInformationClass
	);
NTSTATUS(__stdcall* pf_NtDeleteFile)(
	POBJECT_ATTRIBUTES ObjectAttributes
	);
VOID(__stdcall* pf_RtlInitUnicodeString)(
	PUNICODE_STRING DestinationString,
	PCWSTR SourceString
	);
struct stat buf;


/* 解压 zip 用到的哈夫曼树相关结构体 */
typedef struct
{
	int index;//zip 入口
	TCHAR name[Small];//zip 名称
	DWORD attr;//zip 属性
	FILETIME atime, ctime, mtime;
	long comp_size, unc_size;
} ZIPENTRY;

typedef struct tm_unz_s
{
	unsigned int tm_sec;// [0,59]
	unsigned int tm_min;// [0,59]
	unsigned int tm_hour;// [0,23]
	unsigned int tm_mday;// [1,31]
	unsigned int tm_mon;// [0,11]
	unsigned int tm_year;// [1980, 2044]
} tm_unz;

typedef struct unz_global_info_s
{
	unsigned long number_entry;//此磁盘上中心目录中的条目总数
	unsigned long size_comment;//压缩文件的全局注释大小
} unz_global_info;

typedef struct unz_file_info_s
{
	unsigned long version;//版本号                        2 bytes
	unsigned long version_needed;//需要提取的版本         2 bytes
	unsigned long flag;//通用位标志                       2 bytes
	unsigned long compression_method;//压缩方法           2 bytes
	unsigned long dosDate;// DOS 中显示的最后修改日期     4 bytes
	unsigned long crc;// crc-32                           4 bytes
	unsigned long compressed_size;//压缩部分大小          4 bytes
	unsigned long uncompressed_size;//未压缩部分大小      4 bytes
	unsigned long size_filename;//文件名长度              2 bytes
	unsigned long size_file_extra;//额外字段长度          2 bytes（可能包含文件数据隐写）
	unsigned long size_file_comment;//文件注释长度        2 bytes
	unsigned long disk_num_start;//磁盘号开始             2 bytes
	unsigned long internal_fa;//内部文件属性              2 bytes（解压缩要保留原有属性）
	unsigned long external_fa;//外部文件属性              4 bytes
	tm_unz tmu_date;
} unz_file_info;

typedef struct z_stream_s
{
	Byte* next_in;
	uInt avail_in;
	uLong total_in;
	Byte* next_out;
	uInt avail_out;
	uLong total_out;
	char* ErrorMessages;
	struct internal_state* state;
	alloc_func zalloc;
	free_func zfree;
	voidpf opaque;
	int data_type;
	uLong adler;
	uLong reserved;
} z_stream;
typedef z_stream* z_streamp;//必须放在这里

struct inflate_huft_s//哈夫曼树
{
	union
	{
		struct
		{
			Byte Exop;//额外位或操作数
			Byte Bits;//此代码或子代码中的位数
		} what;
		uInt pad;//焊盘结构 
	} word;//字典
	uInt base;
};

struct inflate_codes_state
{
	inflate_codes_mode mode;
	uInt len;
	union
	{
		struct
		{
			const inflate_huft* tree;
			uInt need;
		} code;
		uInt lit;
		struct
		{
			uInt get;
			uInt dist;
		} copy;
	} sub;
	Byte lbits;//每个分支解码的树位
	Byte dbits;//每个分支的数据树位解码器
	const inflate_huft* ltree;
	const inflate_huft* dtree;
};

struct internal_state
{
	inflate_mode mode;
	union
	{
		uInt method;
		struct
		{
			uLong was;
			uLong need;
		}
		check;
		uInt marker;
	} sub;
	int nowrap;
	uInt wbits;
	inflate_blocks_statef* blocks;
};

typedef struct unz_file_info_internal_s
{
	uLong offset_curfile;//本地头的相对偏移量 4 字节
} unz_file_info_internal;

typedef struct
{
	bool is_handle;//判断是句柄或寄存器
	bool canseek;//是否可被检测
	HANDLE h;//如果是句柄
	bool herr;
	unsigned long initial_offset;
	bool mustclosehandle;
	void* buf;//寄存器
	unsigned int len, pos;//如果不是内存块
} LUFILE;

typedef struct
{
	char* read_buffer;//压缩数据的内部缓冲区
	z_stream stream;// zip 流
	uLong pos_in_zipfile;//在 zip 文件上的字节位置，用于 fseek
	uLong stream_initialised;//流结构初始化时设置的标志
	uLong offset_local_extrafield;//本地额外字段的偏移量
	uInt  size_local_extrafield;//本地额外字段的大小
	uLong pos_local_extrafield;//在 read 的本地额外字段中的位置
	uLong crc32;//所有未压缩数据的 crc32
	uLong crc32_wait;// crc32 我们必须在全部减压后获得
	uLong rest_read_compressed;//要解压缩的字节数
	uLong rest_read_uncompressed;//反编译后要获取的字节数
	LUFILE* file;// zip 文件的 IO 结构
	uLong compression_method;//压缩方法
	uLong byte_before_the_zipfile;// zip 文件之前的字节
	bool encrypted;//判断压缩包是否被加密
	unsigned long keys[3];//解密密钥，由 unzOpenCurrentFile() 初始化
	int encheadleft;//对 unzReadCurrentFile() 的第一次调用将首先读取既定加密头字节
	char crcenctest;//如果加密则检查加密缓冲区
} file_in_zip_read_info_s;

typedef struct//包含有关 zip 文件的内部信息
{
	LUFILE* file;//类似注释
	unz_global_info gi;
	uLong byte_before_the_zipfile;
	uLong num_file;
	uLong pos_in_central_dir;
	uLong current_file_ok;
	uLong central_pos;
	uLong size_central_dir;
	uLong offset_central_dir;
	unz_file_info cur_file_info;
	unz_file_info_internal cur_file_info_internal;
	file_in_zip_read_info_s* pfile_in_zip_read;
} unz_s, * unzFile;


/* 与驱动通信函数 */
int DriverConnector(char* msg, bool test)
{
	HANDLE device = NULL;
	ULONG ret_len;
	int ret = EXIT_SUCCESS;

	/* 打开设备，每次要操作驱动对象时，先以此为例子打开设备 */
	device = CreateFileW(CWK_DEV_SYM, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, 0);
	if (device == INVALID_HANDLE_VALUE)
	{
		state = 41;
		cout << states[state] << "（" << msg << "）" << endl;
		return EXIT_DRIVER_ERROR;
	}

	if (test)//如果只是测试模式
	{
		/* 发送测试指令 */
		char testMsg[] = "/test";
		if (DeviceIoControl(device, CWK_DVC_SEND_STR, testMsg, (DWORD)(strlen(testMsg) + 1), NULL, 0, &ret_len, 0))
			cout << "与驱动通讯成功！" << endl;
		else
		{
			state = 42;
			cout << states[state] << endl;
			ret = EXIT_DRIVER_ERROR;
		}
	}
	else//如果不是测试模式
	{
		/* 发送结束指令 */
		if (DeviceIoControl(device, CWK_DVC_SEND_STR, msg, (DWORD)(strlen(msg) + 1), NULL, 0, &ret_len, 0))
		{
#if (defined _WIN64 || defined WIN64)
			cout << "通知驱动，结束进程——“" << msg << "”！" << endl;
#else
			if (strcmp(msg, "/DR") == 0 || strcmp(msg, "/Dr") == 0
				|| strcmp(msg, "/dR") == 0 || strcmp(msg, "/dr") == 0
				|| strcmp(msg, "-DR") == 0 || strcmp(msg, "-Dr") == 0
				|| strcmp(msg, "-dR") == 0 || strcmp(msg, "-dr") == 0
				)
				cout << "通知驱动，驱动重启！" << endl;
			else if (strcmp(msg, "/DS") == 0 || strcmp(msg, "/Ds") == 0
				|| strcmp(msg, "/dS") == 0 || strcmp(msg, "/ds") == 0
				|| strcmp(msg, "-DS") == 0 || strcmp(msg, "-Ds") == 0
				|| strcmp(msg, "-dS") == 0 || strcmp(msg, "-ds") == 0
				)
				cout << "通知驱动，驱动关机！" << endl;
			else
				cout << "通知驱动，结束进程——“" << msg << "”！" << endl;
#endif
		}
		else
		{
			cout << "\a错误：向驱动发送信息时发生异常！（" << msg << "）" << endl;
			ret = EXIT_DRIVER_ERROR;
		}
	}

	CloseHandle(device);//记得关闭
	return ret;
}


/* Windows Hook */
LRESULT __stdcall CBTHookProc1(long nCode, WPARAM wParam, LPARAM lParam)//并发编程 Funtion1
{
	if (nCode == HCBT_ACTIVATE)
	{
		SetDlgItemText((HWND)wParam, IDYES, RunThread == 2 ? TEXT("自动刷新") : TEXT("手动刷新"));
		SetDlgItemText((HWND)wParam, IDNO, TEXT("查找进程"));
		SetDlgItemText((HWND)wParam, IDCANCEL, TEXT("中止遍历"));
		UnhookWindowsHookEx(hHook);
	}
	return 0;
}

LRESULT __stdcall CBTHookProc2(long nCode, WPARAM wParam, LPARAM lParam)//并发编程 FunctionC
{
	if (nCode == HCBT_ACTIVATE)
	{
		SetDlgItemText((HWND)wParam, IDABORT, TEXT("循环提权"));
		UnhookWindowsHookEx(hHook);
	}
	return 0;
}

LRESULT __stdcall CBTHookProc3(long nCode, WPARAM wParam, LPARAM lParam)//禁止创建进程
{
	if (nCode == HCBT_ACTIVATE)
	{
		SetDlgItemText((HWND)wParam, IDCANCEL, TEXT("不再提示"));
		UnhookWindowsHookEx(hHook);
	}
	return 0;
}


/* Windows API */
APIType EnableDebugPriv(const char* name)//本地进程提权函数
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	LUID luid;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))//打开进程令牌环
	{
		state = 1;
		return FALSE;
	}
	if (!LookupPrivilegeValue(NULL, CCharToLpcwstr(name), &luid))//获取进程 token 权限信息
	{
		state = 2;
		return FALSE;
	}
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	tp.Privileges[0].Luid = luid;
	if (!AdjustTokenPrivileges(hToken, 0, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))//调整权限
	{
		state = 3;
		return FALSE;
	}
	return TRUE;
}

APIType SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)//调整远程进程权限
{
	BOOL bRet = FALSE;
	LUID luid;
	TOKEN_PRIVILEGES tp;
	bRet = LookupPrivilegeValue(NULL, lpszPrivilege, &luid);
	if (!bRet)
		return bRet;
	tp.PrivilegeCount = 1;
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = NULL;
	bRet = AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL);
	if (!bRet)
		return bRet;
	return TRUE;
}

APIType isAdmin()//是否具备管理员权限
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

APIType IsAboveVistaVersion(DWORD Win)//判断系统版本是否为 Win 或以上
{
	if (LQ(Win, _WIN32_WINNT))//返回的是 bool
		return TRUE;
	else
		return FALSE;
}

APIType is64BitOS()//判断系统是否为 64 位系统
{
	SYSTEM_INFO cur_system_info;
	GetNativeSystemInfo(&cur_system_info);
	WORD system_str = cur_system_info.wProcessorArchitecture;
	if (system_str == PROCESSOR_ARCHITECTURE_IA64 || system_str == PROCESSOR_ARCHITECTURE_AMD64)
		return TRUE;
	else
		return FALSE;
}

APIType IsWow64()//判断是否为 Wow64
{
	APIType bIsWow64 = FALSE;
	LPFN_ISWOW64PROCESS fnIsWow64Process;
	HMODULE hmodule = ::GetModuleHandle(TEXT("kernel32"));
	if (!hmodule)
	{
		state = 14;
		return FALSE;
	}
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)::GetProcAddress(hmodule, "IsWow64Process");
	if (NULL != fnIsWow64Process)
		if (!fnIsWow64Process(::GetCurrentProcess(), &bIsWow64))
			state = 23;
	return bIsWow64;
}

APIType WOW64FsDir(BOOL bDisable)//关闭重定向
{
	static WOW64_DISABLE_FSDIR pfnDisable = NULL;
	static WOW64_REVERT_FSDIR pfnrevert = NULL;
	static PVOID OldValue = NULL;
	static BOOL bInit = FALSE;
	if (!bInit && IsWow64())//if (!bInit && OVI_IS64(g_OsVer))
	{
		HMODULE hMod = LoadLibrary(TEXT("kernel32.dll"));
		if (hMod)
		{
			pfnDisable = (WOW64_DISABLE_FSDIR)GetProcAddress(hMod, "Wow64DisableWow64FsRedirection");
			pfnrevert = (WOW64_REVERT_FSDIR)GetProcAddress(hMod, "Wow64RevertWow64FsRedirection");
		}
		if (pfnDisable == NULL || pfnrevert == NULL)
			return FALSE;
		bInit = TRUE;
	}
	if (IsWow64())//if (OVI_IS64(g_OsVer))
	{
		if (bDisable)
			return pfnDisable(&OldValue);
		else
			return pfnrevert(OldValue);
	}
	return TRUE;
}

APIType injectit(char* DllPath, DWORD dwRemoteProcessld)//注入主函数
{
	HANDLE hrp;
	state = 0;
	if ((hrp = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwRemoteProcessld)) == NULL)
	{
		state = 5;
		return FALSE;
	}
	char* psLibFileRemote;
	/* 使用 VirtualAllocEx 函数在远程进程的内存地址空间分配DLL文件名缓冲 */
	psLibFileRemote = (char*)VirtualAllocEx(hrp, NULL, (size_t)lstrlen(CCharToLpcwstr(DllPath)) + 1, MEM_COMMIT, PAGE_READWRITE);
	if (psLibFileRemote == NULL)
	{
		state = 6;
		return FALSE;
	}
	/* 使用 WriteProcessMemory 函数将DLL的路径名复制到远程的内存空间 */
	if (WriteProcessMemory(hrp, psLibFileRemote, (void*)DllPath, (size_t)lstrlen(CCharToLpcwstr(DllPath)) + 1, NULL) == 0)
	{
		state = 7;
		return FALSE;
	}
	/* 计算 LoadLibraryA 的入口地址 */
	HMODULE hmtmp = GetModuleHandle(TEXT("Kernel32"));
	if (hmtmp == NULL)
	{
		state = 8;
		return FALSE;
	}
	PTHREAD_START_ROUTINE pfnStartAddr = (PTHREAD_START_ROUTINE)
		GetProcAddress(hmtmp, "LoadLibraryA");
	if (pfnStartAddr == NULL)
	{
		state = 8;
		return FALSE;
	}
	/* pfnStartAddr 地址就是 LoadLibraryA 的入口地址 */
	HANDLE hrt = CreateRemoteThread(hrp, NULL, 0, pfnStartAddr, psLibFileRemote, 0, NULL);
	if (hrt == NULL)
	{
		state = 9;
		return FALSE;
	}
	WaitForSingleObject(hrt, INFINITE);
	CloseHandle(hrp);
	CloseHandle(hrt);
	return TRUE;
}

APIType GetAdminPermission(_TCHAR* tchar, _TCHAR* _option)//以管理员权限启动
{
	if (!IsAboveVistaVersion(_WIN32_WINNT_WIN7))
	{
		state = 27;
		return FALSE;
	}
	SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
	sei.lpVerb = TEXT("runas");
	sei.lpFile = tchar;
	sei.lpParameters = _option;
	sei.nShow = SW_SHOWNORMAL;
	if (!ShellExecuteEx(&sei))
	{
		DWORD dwStatus = GetLastError();
		if (dwStatus == ERROR_CANCELLED)
			state = 15;
		else if (dwStatus == ERROR_FILE_NOT_FOUND)
			state = 14;
		else
			state = 5;
		return FALSE;
	}
	state = 0;
	return TRUE;
}

APIType CmdConsole()//全屏启动 cmd
{
	if (lstrlen(L"start /realtime \"命令提示符\" \"") + lstrlen(_0) + lstrlen(L"\" cmd") + 1 >= Small)
		return FALSE;
	_TCHAR Cmdline[Small] = L"start /realtime \"命令提示符\" \"";
	char _Cmdline[Small] = { 0 };
	lstrcat(Cmdline, _0);
	lstrcat(Cmdline, L"\" cmd");
	TcharToChar(Cmdline, _Cmdline);
	system(_Cmdline);
	return TRUE;
}

APIType isAdequateMem(DWORDLONG RequireMBytes)//检测内存是否足够
{
	MEMORYSTATUSEX memoryStatus;
	memset(&memoryStatus, 0, sizeof(MEMORYSTATUSEX));
	memoryStatus.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memoryStatus);
	if (LT(RequireMBytes, memoryStatus.ullAvailPhys >> 20))
		return TRUE;
	else
		return FALSE;
}

APIType RemoveFile(char* FileName)
{
	if (LQ(Small - 1, strlen(FileName) + strlen("\\??\\")))
	{
		state = 32;
		return FALSE;
	}
	FILE_DISPOSITION_INFORMATION fi = { 1 };
	IO_STATUS_BLOCK bs = { 0 };
	OBJECT_ATTRIBUTES ob;
	UNICODE_STRING str;
	HMODULE hmodule = LoadLibrary(L"ntdll.dll");
	if (hmodule == NULL)
	{
		state = 24;
		return FALSE;
	}
	pf_NtSetInformationFile = (NTSTATUS(__stdcall*)(
		HANDLE,
		PIO_STATUS_BLOCK,
		PVOID,
		ULONG,
		FILE_INFORMATION_CLASS))GetProcAddress(hmodule, "NtSetInformationFile");
	pf_NtDeleteFile = (NTSTATUS(__stdcall*)(POBJECT_ATTRIBUTES)) GetProcAddress(hmodule, "NtDeleteFile");
	pf_RtlInitUnicodeString = (VOID(__stdcall*)(PUNICODE_STRING, PCWSTR)) GetProcAddress(hmodule, "RtlInitUnicodeString");
	char _FileName[Small] = { 0 };
	strcat_s(_FileName, "\\??\\");
	strcat_s(_FileName, FileName);
	pf_RtlInitUnicodeString(&str, CharToLpcwstr(_FileName));
	InitializeObjectAttributes(&ob, &str, OBJ_CASE_INSENSITIVE, NULL, NULL);
	return TRUE;
}

APIType isConnected()//检查网络连接
{
	DWORD flag;
	return InternetGetConnectedState(&flag, 0);
}

APIType FileDownload(const char* Url, const char* SavePath)//将 Url 指向的地址的文件下载到 SavePath 指向的本地文件
{
	if (!isConnected())
	{
		state = 34;
		return FALSE;
	}
	DeleteUrlCacheEntry(CCharToLpcwstr(Url));
	BOOL bRet;
	if (URLDownloadToFile(NULL, CCharToLpcwstr(Url), CCharToLpcwstr(SavePath), 0, NULL) == S_OK)
		bRet = TRUE;
	else
		bRet = FALSE;
	DeleteUrlCacheEntry(CCharToLpcwstr(Url));
	if (!bRet)
		state = 33;
	return bRet;
}

APIType FindFirstFileExists(LPCTSTR lpPath, BOOL dwFilter)//检查文件是否存在
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(lpPath, &fd);
	BOOL bFilter = (FALSE == dwFilter) ? TRUE : fd.dwFileAttributes & dwFilter;
	BOOL RetValue = ((hFind != INVALID_HANDLE_VALUE) && bFilter) ? TRUE : FALSE;
	if (RetValue == FALSE)
	{
		TCHAR SystemPath[Small];
		GetSystemDirectory(SystemPath, Small);
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

APIType NtShutdownOrReboot(const int _SH)//强制关机或重启
{
	state = 0;
	if (MessageBox(NULL, TEXT("警告：操作具有高度风险，是否继续？\n如要继续，请保存您的所有数据，然后选择“是”。"), WmipTextTitle, MB_YESNO | MB_ICONWARNING | MB_TOPMOST) == IDNO)
	{
		state = 15;
		return FALSE;
	}
	HANDLE HToken;
	TOKEN_PRIVILEGES tkp;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &HToken))
	{
		state = 2;
		return FALSE;
	}
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(HToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
	if (GetLastError() != ERROR_SUCCESS)
	{
		state = 3;
		return FALSE;
	}
	const int SE_SHUTDOWN_PRIVILEGE = 0x13;
	HMODULE hModule = ::LoadLibrary(_T("ntdll.dll"));
	if (hModule)
	{
		PFN_RtlAdjustPrivilege pfnRtl = (PFN_RtlAdjustPrivilege)GetProcAddress(hModule, "RtlAdjustPrivilege");
		PFN_ZwShutdownSystem pfnShutdown = (PFN_ZwShutdownSystem)GetProcAddress(hModule, "ZwShutdownSystem");
		if (pfnRtl != NULL && pfnShutdown != NULL)
		{
			int en = 0, nRet = pfnRtl(SE_SHUTDOWN_PRIVILEGE, TRUE, TRUE, &en);
			if (nRet == 0x0C000007C)
				nRet = pfnRtl(SE_SHUTDOWN_PRIVILEGE, TRUE, FALSE, &en);
			nRet = pfnShutdown(_SH);// 1 = 强制重启  2 = 强制关机
			state = 0;
			return TRUE;
		}
		else
		{
			state = 24;
			return FALSE;
		}
	}
	else
	{
		state = 18;
		return FALSE;
	}
}

APIType CallErrorScreen()//蓝屏
{
	state = 0;
	if (MessageBox(NULL, TEXT("警告：操作具有高度风险，是否继续？\n如要继续，请保存您的所有数据，然后选择“是”。"), WmipTextTitle, MB_YESNO | MB_ICONWARNING | MB_TOPMOST) == IDNO)
	{
		state = 15;
		return FALSE;
	}
	BOOLEAN bEnabled;
	ULONG uResp;
	HMODULE hmodule = LoadLibraryA("ntdll.dll");
	if (hmodule == NULL)
	{
		state = 14;
		return FALSE;
	}
	LPVOID lpFuncAddress = GetProcAddress(hmodule, "RtlAdjustPrivilege");
	if (lpFuncAddress == NULL)
	{
		state = 24;
		return FALSE;
	}
	hmodule = GetModuleHandle(L"ntdll.dll");
	if (hmodule == NULL)
	{
		state = 14;
		return FALSE;
	}
	LPVOID lpFuncAddress2 = GetProcAddress(hmodule, "NtRaiseHardError");
	if (lpFuncAddress2 == NULL)
	{
		state = 24;
		return FALSE;
	}
	pdef_RtlAdjustPrivilege NtCall = (pdef_RtlAdjustPrivilege)lpFuncAddress;
	pdef_NtRaiseHardError NtCall2 = (pdef_NtRaiseHardError)lpFuncAddress2;
	NTSTATUS NtRet = NtCall(19, TRUE, FALSE, &bEnabled);
	NtCall2(STATUS_FLOAT_MULTIPLE_FAULTS, 0, 0, 0, 6, &uResp);
	return TRUE;
}

APIType CALLBACK lpEnumFunc(HWND hwnd, LPARAM lParam)//窗体
{
	EnumHWndsArg* pArg = (LPEnumHWndsArg)lParam;
	DWORD processId;
	GetWindowThreadProcessId(hwnd, &processId);
	if (processId == pArg->dwProcessId)
		pArg->vecHWnds->push_back(hwnd);
	return TRUE;
}

APIType GetHWndsByProcessID(DWORD processID, vector<HWND>& vecHWnds)
{
	EnumHWndsArg wi;
	wi.dwProcessId = processID;
	wi.vecHWnds = &vecHWnds;
	EnumWindows(lpEnumFunc, (LPARAM)&wi);
	return TRUE;
}

HANDLE WINAPI MyOpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId)//MyOpenProcess 函数的实现
{
	if (_getpid() == dwProcessId)//如果要打开本进程，返回 NULL
		return NULL;
	return pRealOpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
}

APIType IatHook(
	const char* DllName,
	const char* FunName,
#ifdef _WIN64
	unsigned long long RealAddr
#else
	DWORD RealAddr
#endif
)//IAT HOOK 函数的实现
{
	/* 查找相同的 DLL */
	HANDLE pBegin = GetModuleHandle(NULL);
	PBYTE pBegin2 = (PBYTE)pBegin;
	PIMAGE_DOS_HEADER DOS = PIMAGE_DOS_HEADER(pBegin2);
	PIMAGE_NT_HEADERS NT = PIMAGE_NT_HEADERS(pBegin2 + DOS->e_lfanew);
	PIMAGE_OPTIONAL_HEADER OPTION = &(NT->OptionalHeader);
	PIMAGE_IMPORT_DESCRIPTOR IMPORT = PIMAGE_IMPORT_DESCRIPTOR(OPTION->DataDirectory[1].VirtualAddress + pBegin2);
	while (IMPORT->Name)
	{
		char* OurDllName = (char*)(IMPORT->Name + pBegin2);
		if (0 == _strcmpi(DllName, OurDllName))
			break;
		IMPORT++;
	}
	/* 查找相同的 API 函数 */
	PIMAGE_IMPORT_BY_NAME pImportByName = NULL;
	PIMAGE_THUNK_DATA pOriginalThunk = (PIMAGE_THUNK_DATA)(IMPORT->OriginalFirstThunk + pBegin2);
	PIMAGE_THUNK_DATA pFirstThunk = (PIMAGE_THUNK_DATA)(IMPORT->FirstThunk + pBegin2);
	while (pOriginalThunk->u1.Function)//注意是 Function
	{
#ifdef _WIN64
		ULONGLONG u1 = pOriginalThunk->u1.Ordinal;//注意是 Ordinal
#else
		DWORD u1 = pOriginalThunk->u1.Ordinal;//注意是 Ordinal
#endif
		if ((u1 & IMAGE_ORDINAL_FLAG) != IMAGE_ORDINAL_FLAG)//说明 MSB 不为 1，不是以序号导入
		{
			pImportByName = (PIMAGE_IMPORT_BY_NAME)((DWORD)pOriginalThunk->u1.AddressOfData + pBegin2);
			char* OurFunName = (char*)(pImportByName->Name);//也可以用如下计算方法
			//char* OurFunName2 = (char*)((DWORD)pOriginalThunk->u1.AddressOfData + pBegin2 + 2);  
			if (0 == _strcmpi(FunName, OurFunName))//找到 OpenProcess
			{
				MEMORY_BASIC_INFORMATION mbi_thunk;
				VirtualQuery(pFirstThunk, &mbi_thunk, sizeof(MEMORY_BASIC_INFORMATION));//获取以 pFirstThunk 开始的内存的信息并将其保存到 MEMORY_BASIC_INFORMATION 结构中
				VirtualProtect(mbi_thunk.BaseAddress, mbi_thunk.RegionSize, PAGE_READWRITE, &mbi_thunk.Protect);
				DWORD dwOLD;
				VirtualProtect(pFirstThunk, sizeof(DWORD), PAGE_READWRITE, &dwOLD);//修改以pFirstThunk开始的内存的的保护属性为PAGE_READWRITE并将原保护属性保存到&dwOLD中
				pFirstThunk->u1.Function = (DWORD)RealAddr;//更改真正OpenProcess的地址为自己写的MyOpenProcess函数的地址
				PDWORD tmp = (PDWORD)malloc(sizeof(PDWORD));//恢复之前更改的内存的保护属性为人家自己的
				if (tmp)
				{
					VirtualProtect(pFirstThunk, sizeof(DWORD), dwOLD, tmp);
					free(tmp);
				}
				else
					return FALSE;
				break;
			}
		}
		pOriginalThunk++;
		pFirstThunk++;
	}
	return TRUE;
}

APIType SelfProtection()//加载自我保护
{
	if (!isAdequateMem(35))
	{
		MessageBox(NULL, TEXT("内存资源不足，程序将停止运行自我保护。\n为了释放内存，建议您重新启动本程序。"), WmipTextTitle, MB_OK | MB_ICONERROR | MB_TOPMOST);
		protect = 0;
		return FALSE;
	}
#ifdef _WIN64
	return IatHook("Kernel32.dll", "OpenProcess", (unsigned long long)MyOpenProcess);
#else
	return IatHook("Kernel32.dll", "OpenProcess", (DWORD)MyOpenProcess);
#endif
}


/* 数据结构算法 */
void ImageSort()//映像名称排序
{
	if (Lists.ProcessCount <= 0)
	{
		state = 10;
		return;
	}
	int i, j, l = Lists.ProcessCount - 1;
	ProcessNode tmp;
	char tmp1[Small] = { 0 }, tmp2[Small] = { 0 };
	while (l > 0)
	{
		for (i = 0, j = 0; i < l; ++i)
		{
			TcharToChar(Lists.PNode[i].PNameNode, tmp1);
			TcharToChar(Lists.PNode[i + 1].PNameNode, tmp2);
			if (strcmp(tmp1, tmp2) > 0)
			{
				tmp = Lists.PNode[i];
				Lists.PNode[i] = Lists.PNode[i + 1];
				Lists.PNode[i + 1] = tmp;
				j = i;
			}
		}
		l = j;
	}
	return;
}

void BubbleSort(char choice)//冒泡排序
{
	if (Lists.ProcessCount <= 0)
	{
		state = 10;
		return;
	}
	if ('4' == choice)
	{
		ImageSort();
		return;
	}
	int i, j, l = Lists.ProcessCount - 1;
	ProcessNode tmp;
	while (l > 0)
	{
		for (i = 0, j = 0; i < l; ++i)
		{
			switch (choice)
			{
			case '1':
				if (Lists.PNode[i].ProcessIDNode > Lists.PNode[i + 1].ProcessIDNode)
				{
					tmp = Lists.PNode[i];
					Lists.PNode[i] = Lists.PNode[i + 1];
					Lists.PNode[i + 1] = tmp;
					j = i;
				}
				break;
			case '2':
				if (Lists.PNode[i].ParentIDNode > Lists.PNode[i + 1].ParentIDNode)
				{
					tmp = Lists.PNode[i];
					Lists.PNode[i] = Lists.PNode[i + 1];
					Lists.PNode[i + 1] = tmp;
					j = i;
				}
				break;
			case '3':
				if (Lists.PNode[i].ThreadCNode > Lists.PNode[i + 1].ThreadCNode)
				{
					tmp = Lists.PNode[i];
					Lists.PNode[i] = Lists.PNode[i + 1];
					Lists.PNode[i + 1] = tmp;
					j = i;
				}
				break;
			default:
				return;
			}
		}
		l = j;
	}
	return;
}

void ReverseSort(ProcessList& L)//倒置
{
	if (L.ProcessCount == 0)
		return;
	if (!InitStack(stack))
	{
		state = 25;
		return;
	}
	for (int i = 0; i < L.ProcessCount; ++i)
		Push(stack, L.PNode[i]);
	for (int i = 0; i < L.ProcessCount; ++i)
		Pop(stack, L.PNode[i]);
	EmptyStack(stack);
	return;
}

void ShowProcess()//打印 Lists 进程
{
	RunThread += 10;
	cout << "PID\t\tPPID\t\t线程数\t\t进程名" << endl << "==========\t==========\t==========\t========================================" << endl << endl;
	for (int i = 0; i < Lists.ProcessCount; ++i)
	{
		cout << Lists.PNode[i].ProcessIDNode << "\t\t" << Lists.PNode[i].ParentIDNode << "\t\t" << Lists.PNode[i].ThreadCNode << "\t\t";
		wcout << Lists.PNode[i].PNameNode << endl;
	}
	cout << endl << endl << "当前共有 " << Lists.ProcessCount << " 个进程，共有 " << Lists.ThreadCount << " 条线程。" << endl;
	RunThread -= 10;
	return;
}

void ShowResults()//打印 Result 进程
{
	cout << "PID\t\tPPID\t\t线程数\t\t进程名" << endl << "==========\t==========\t==========\t========================================" << endl << endl;
	for (int i = 0; i < Results.ProcessCount; ++i)
	{
		cout << Results.PNode[i].ProcessIDNode << "\t\t" << Results.PNode[i].ParentIDNode << "\t\t" << Results.PNode[i].ThreadCNode << "\t\t";
		wcout << Results.PNode[i].PNameNode << endl;
	}
	cout << endl << endl << "当前共有 " << Lists.ProcessCount << " 个进程，共有 " << Lists.ThreadCount << " 条线程。" << endl;
	cout << "其中，查找到 " << Results.ProcessCount << " 个进程，共包含有 " << Results.ThreadCount << " 条线程。" << endl;
	return;
}

int PIDFinder(DWORD elem, bool isPID)//通过 PID 折半查找进程所在位置
{
	int high = Lists.ProcessCount - 1, low = 0, mid;
	while (low <= high)
	{
		mid = low + ((high - low) >> 1);
		if (EQ(elem, (isPID ? Lists.PNode[mid].ProcessIDNode : Lists.PNode[mid].ParentIDNode)))
			return mid;
		else if (LT(elem, (isPID ? Lists.PNode[mid].ProcessIDNode : Lists.PNode[mid].ParentIDNode)))
			high = mid - 1;
		else
			low = mid + 1;
	}
	return -1;
}

int BinarySearch(DWORD elem)//二分查找插入位置
{
	if (Results.ProcessCount == 0)
		return 0;
	int low = 0, high = Results.ProcessCount - 1, mid = 0;
	while (low <= high)
	{
		mid = low + ((high - low) >> 1);
		if (EQ(Results.PNode[mid].ProcessIDNode, elem))
			return -1;//重复元素返回 -1
		else if (LT(elem, Results.PNode[mid].ProcessIDNode))
			high = mid - 1;
		else
			low = mid + 1;
	}
	return low;
}

bool InsertSort(ProcessNode PNodeElem)//插入元素
{
	if (Results.ProcessCount < 0 || Results.ProcessCount - 1 >= Large || PNodeElem.ProcessIDNode < 0)
		return false;
	int index = BinarySearch(PNodeElem.ProcessIDNode), i = Results.ProcessCount;
	if (index == -1)
		return false;
	for (i = Results.ProcessCount; i > index; --i)
		Results.PNode[i] = Results.PNode[i - 1];
	Results.PNode[i] = PNodeElem;
	Results.ProcessCount++;
	Results.ThreadCount += PNodeElem.ThreadCNode;
	return true;
}

int match_string(string m_str, string w_str)//处理通配符
{
	transform(m_str.begin(), m_str.end(), m_str.begin(), tolower);//在 m_str 中匹配 w_str
	transform(w_str.begin(), w_str.end(), w_str.begin(), tolower);
	size_t m_len = m_str.size(), w_len = w_str.size();
	vector<vector<int> > b_dp(w_len + 1, vector<int>(m_len + 1, 0));//多加一行一列作为初始初值所用
	b_dp[0][0] = 1;
	for (size_t i = 1; i <= w_len; i++)
	{
		char ch = w_str[i - 1];
		b_dp[i][0] = int(b_dp[i - 1][0] && (ch == '*'));//设置每次循环的初值，即当星号不出现在首位时，匹配字符串的初值都为false
		for (size_t j = 1; j <= m_len; j++)
		{
			char ch2 = m_str[j - 1];
			if (ch == '*')
				b_dp[i][j] = int(b_dp[i - 1][j] || b_dp[i][j - 1]);//当匹配字符为*号时，状态取决于上面状态和左边状态的值
			else
				b_dp[i][j] = int(b_dp[i - 1][j - 1] && (ch == '?' || ch2 == ch));//决定于上一次和本次
		}
	}
	return b_dp[w_len][m_len];//匹配成功的返回值是 1
}

void NameFinder(char* pn)//由进程名称获取进程标识符
{
	for (int i = 0; i < Lists.ProcessCount; ++i)
	{
		char tmp[Small] = { 0 };
		TcharToChar(Lists.PNode[i].PNameNode, tmp);
		string str1(pn);
		string str2(tmp);
		if (match_string(str2, str1) == 1 && Results.ProcessCount < Large)
			InsertSort(Lists.PNode[i]);
	}
	return;
}

bool isIncludeSpace(_TCHAR* arg)//是否含有空格
{
	for (int i = 0; i < lstrlen(arg); ++i)
		if (EQ(arg[i], ' '))
			return true;
	return false;
}

void GetProcess()//获取进程
{
	Lists.ProcessCount = 0;
	Lists.ThreadCount = 0;
	HANDLE hProceessnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);
	BOOL hProcess = Process32First(hProceessnap, &pe32);
	while (hProcess)
	{
		Lists.PNode[Lists.ProcessCount].ProcessIDNode = pe32.th32ProcessID;
		Lists.PNode[Lists.ProcessCount].ParentIDNode = pe32.th32ParentProcessID;
		Lists.PNode[Lists.ProcessCount].ThreadCNode = pe32.cntThreads;
		wcscpy_s(Lists.PNode[Lists.ProcessCount].PNameNode, pe32.szExeFile);
		hProcess = Process32Next(hProceessnap, &pe32);
		Lists.ProcessCount++;
		Lists.ThreadCount += pe32.cntThreads;
		if (hProcess && Lists.ProcessCount >= Large)
		{
			state = 30;
			cout << "\a警告：进程数超过可保存的最大值，超过部分将会被裁剪。" << endl;
			Sleep(Rtime);
			break;
		}
	}
	return;
}

void FindProcess()//查找进程
{
	system("cls");
	GetProcess();
	int result = 0, i = 0;
	if (Lists.ProcessCount <= 0)
	{
		cout << "\a错误：" << states[10] << "请按任意键返回。" << endl;
		system("pause>nul&cls");
		return;
	}
	else
	{
		BubbleSort('1');
		ShowProcess();
	}
	Results.ProcessCount = 0;
	i = 0;
	while (i < Large && Results.ProcessCount < Large)
	{
		cout << "请输入第 " << i + 1 << " 个进程 ID（输入非法字符或负整数退出输入）：";
		rewind(stdin);
		fflush(stdin);
		if (scanf_s("%d", &ID_IN[i]) != 1 || ID_IN[i] < 0)
			break;
		result = PIDFinder(ID_IN[i], true);
		if (result != -1)
			InsertSort(Lists.PNode[result]);
		i++;
	}
	i = 0;
	cout << endl << "说明：依照 Microsoft Windows 相关规定，进程名不区分大小写。" << endl;
	cout << "本程序支持通配符，通配符说明：“?”代表一个字符，“*”代表不定数目（含 0 ）个字符。" << endl;
	cout << "录入完成后，输入“\\/:\"<>|”中的字符之一回车即可退出输入。" << endl;
	while (i < Large && Results.ProcessCount < Large)
	{
		cout << "请输入第 " << i + 1 << " 个映像名称：";
		rewind(stdin);
		fflush(stdin);
		fgets(Name_IN[i], sizeof(Name_IN[i]), stdin);
		Name_IN[i][strlen(Name_IN[i]) - 1] = 0;
		if (strcmp(Name_IN[i], "\\") == 0 || strcmp(Name_IN[i], "/") == 0 || strcmp(Name_IN[i], ":") == 0 || strcmp(Name_IN[i], "\"") == 0
			|| strcmp(Name_IN[i], "<") == 0 || strcmp(Name_IN[i], ">") == 0 || strcmp(Name_IN[i], "|") == 0
			)
			break;
		NameFinder(Name_IN[i]);
		i++;
	}
	rewind(stdin);
	fflush(stdin);
	system("cls");
	if (Results.ProcessCount == 0)
	{
		cout << "输入为空或没有结果，" << states[28] << endl;
		Sleep(Rtime);
		return;
	}
	else
	{
		cout << "PID\t\tPPID\t\t线程数\t\t进程名" << endl << "==========\t==========\t==========\t========================================" << endl << endl;
		for (i = 0; i < Results.ProcessCount; ++i)
		{
			cout << Results.PNode[i].ProcessIDNode << "\t\t" << Results.PNode[i].ParentIDNode << "\t\t" << Results.PNode[i].ThreadCNode << "\t\t";
			wcout << Results.PNode[i].PNameNode << endl;
		}
	}
	cout << endl << endl << "当前共有 " << Lists.ProcessCount << " 个进程，其中有 " << Results.ProcessCount << " 个查找结果。" << endl;
	return;
}


/* 处理命令行参数 */
void GetCommandResult(int arc, _TCHAR* arg[])//命令行操作对象参数处理
{
	GetProcess();
	if (Lists.ProcessCount <= 0)
	{
		cout << "\a错误：" << states[10] << endl;
		exit(EXIT_FAILURE);
	}
	BubbleSort('1');
	if (_wcsicmp(arg[3], L"/all") == 0 || _wcsicmp(arg[3], L"-all") == 0)
	{
		Results = Lists;
		return;
	}
	int i = 3, tmp;
im:
	if (_wcsicmp(arg[i], L"/im") == 0 || _wcsicmp(arg[i], L"-im") == 0)
	{
		while (_wcsicmp(arg[i + 1], L"/pid") || _wcsicmp(arg[i + 1], L"-pid"))
		{
			i++;
			if (i > Large + 2 || i >= arc || Results.ProcessCount >= Large)
				goto over;
			TcharToChar(arg[i], Name_IN[i - 3]);
			NameFinder(Name_IN[i - 3]);
			if (i + 1 >= arc)
				goto over;
		}
		i++;
		goto pid;
	}
pid:
	if (_wcsicmp(arg[i], L"/pid") == 0 || _wcsicmp(arg[i], L"-pid") == 0)
	{
		while (_wcsicmp(arg[i + 1], L"/im") || _wcsicmp(arg[i + 1], L"-im"))
		{
			i++;
			if (i > Large + 2 || i >= arc || Results.ProcessCount >= Large)
				goto over;
			rewind(stdin);
			fflush(stdin);
			tmp = _wtoi(arg[i]);
			tmp = PIDFinder(tmp, true);
			if (tmp != -1)
				InsertSort(Lists.PNode[tmp]);
			if (i + 1 >= arc)
				goto over;
		}
		rewind(stdin);
		fflush(stdin);
		i++;
		goto im;
	}
over:
	return;
}

int DelayRun(int arc, _TCHAR* arg[])//延迟执行算法函数
{
	if (arc < 3)
	{
		state = 11;
		cout << states[state] << endl;
		return EXIT_FAILURE;
	}
	if (LT('9', arg[2][0]) || LT(arg[2][0], '0'))//排除不可读入的情况
	{
		state = 12;
		cout << states[state] << endl;
		return EXIT_FAILURE;
	}
	int SleepTime = _wtoi(arg[2]) * 1000;
	if (SleepTime < 0 || SleepTime >= 86400)
	{
		state = 12;
		cout << states[state] << endl;
		return EXIT_FAILURE;
	}
	for (int i = 0; i < arc; ++i)
		if (LQ(Small - 1, lstrlen(arg[i])))
		{
			state = 26;
			cout << "\a错误：" << states[state] << endl;
			return EXIT_FAILURE;
		}
	int lslen = lstrlen(arg[0]) + arc;
	for (int i = 3; i < arc; ++i)
		lslen += lstrlen(arg[i]) + (isIncludeSpace(arg[i]) ? 2 : 0) + int(strlen("call \""));
	if (LQ(Large - 1, lslen))
	{
		state = 26;
		cout << "\a错误：" << states[state] << endl;
		return EXIT_FAILURE;
	}
	char Commandline[Large] = { 0 }, tmp[Small] = { 0 }, myplace[Small] = { 0 };
	TcharToChar(arg[0], tmp);
	strcat_s(myplace, tmp);
	if (_wcsicmp(arg[1], L"/p") == 0 || _wcsicmp(arg[1], L"-p") == 0)
	{
		strcat_s(Commandline, "call \"");
		strcat_s(Commandline, myplace);
		strcat_s(Commandline, "\"");
	}
	for (int i = 3; i < arc; ++i)
	{
		TcharToChar(arg[i], tmp);
		if (isIncludeSpace(arg[i]))
		{
			strcat_s(Commandline, " \"");
			strcat_s(Commandline, tmp);
			strcat_s(Commandline, "\"");
		}
		else
		{
			strcat_s(Commandline, " ");
			strcat_s(Commandline, tmp);
		}
	}
	Sleep(SleepTime);
	if (_wcsicmp(arg[1], L"/p") == 0 || _wcsicmp(arg[1], L"-p") == 0)
		return system(Commandline);
	else if (_wcsicmp(arg[1], L"/ph") == 0 || _wcsicmp(arg[1], L"-ph") == 0)
		ShellExecuteA(NULL, ("open"), (myplace), (Commandline), NULL, SW_HIDE);
	else if (_wcsicmp(arg[1], L"/ps") == 0 || _wcsicmp(arg[1], L"-ps") == 0)
		ShellExecuteA(NULL, ("open"), (myplace), (Commandline), NULL, SW_SHOWNORMAL);
	cout << states[0] << endl;
	return EXIT_SUCCESS;
}


/* 解压 zip 的信息预处理部分 */
const uInt inflate_mask[17] = {
	0x0000,
	0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f, 0x00ff,
	0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff, 0xffff
};
const uInt fixed_bl = 9, fixed_bd = 5;
const uInt border[] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };//编码顺序

struct inflate_blocks_state
{
	inflate_block_mode mode;
	union
	{
		uInt left;
		struct
		{
			uInt table;
			uInt index;
			uInt* blens;
			uInt bb;
			inflate_huft* tb;
		} trees;
		struct
		{
			inflate_codes_statef
				* codes;
		} decode;
	} sub;
	uInt last;//此块是否为最后一个块
	uInt bitk;//位缓冲区中的位
	uLong bitb;//位缓冲区
	inflate_huft* hufts;//树空间的单个 malloc
	Byte* window;//滑动窗口
	Byte* end;//滑动窗口后一个字节
	Byte* read;//窗口的读指针
	Byte* write;//窗口的写指针
	check_func checkfn;//检查功能
	uLong check;//检查输出
};

const inflate_huft fixed_tl[] = {
	{{{96,7}},256}, {{{0,8}},80}, {{{0,8}},16}, {{{84,8}},115},
	{{{82,7}},31}, {{{0,8}},112}, {{{0,8}},48}, {{{0,9}},192},
	{{{80,7}},10}, {{{0,8}},96}, {{{0,8}},32}, {{{0,9}},160},
	{{{0,8}},0}, {{{0,8}},128}, {{{0,8}},64}, {{{0,9}},224},
	{{{80,7}},6}, {{{0,8}},88}, {{{0,8}},24}, {{{0,9}},144},
	{{{83,7}},59}, {{{0,8}},120}, {{{0,8}},56}, {{{0,9}},208},
	{{{81,7}},17}, {{{0,8}},104}, {{{0,8}},40}, {{{0,9}},176},
	{{{0,8}},8}, {{{0,8}},136}, {{{0,8}},72}, {{{0,9}},240},
	{{{80,7}},4}, {{{0,8}},84}, {{{0,8}},20}, {{{85,8}},227},
	{{{83,7}},43}, {{{0,8}},116}, {{{0,8}},52}, {{{0,9}},200},
	{{{81,7}},13}, {{{0,8}},100}, {{{0,8}},36}, {{{0,9}},168},
	{{{0,8}},4}, {{{0,8}},132}, {{{0,8}},68}, {{{0,9}},232},
	{{{80,7}},8}, {{{0,8}},92}, {{{0,8}},28}, {{{0,9}},152},
	{{{84,7}},83}, {{{0,8}},124}, {{{0,8}},60}, {{{0,9}},216},
	{{{82,7}},23}, {{{0,8}},108}, {{{0,8}},44}, {{{0,9}},184},
	{{{0,8}},12}, {{{0,8}},140}, {{{0,8}},76}, {{{0,9}},248},
	{{{80,7}},3}, {{{0,8}},82}, {{{0,8}},18}, {{{85,8}},163},
	{{{83,7}},35}, {{{0,8}},114}, {{{0,8}},50}, {{{0,9}},196},
	{{{81,7}},11}, {{{0,8}},98}, {{{0,8}},34}, {{{0,9}},164},
	{{{0,8}},2}, {{{0,8}},130}, {{{0,8}},66}, {{{0,9}},228},
	{{{80,7}},7}, {{{0,8}},90}, {{{0,8}},26}, {{{0,9}},148},
	{{{84,7}},67}, {{{0,8}},122}, {{{0,8}},58}, {{{0,9}},212},
	{{{82,7}},19}, {{{0,8}},106}, {{{0,8}},42}, {{{0,9}},180},
	{{{0,8}},10}, {{{0,8}},138}, {{{0,8}},74}, {{{0,9}},244},
	{{{80,7}},5}, {{{0,8}},86}, {{{0,8}},22}, {{{192,8}},0},
	{{{83,7}},51}, {{{0,8}},118}, {{{0,8}},54}, {{{0,9}},204},
	{{{81,7}},15}, {{{0,8}},102}, {{{0,8}},38}, {{{0,9}},172},
	{{{0,8}},6}, {{{0,8}},134}, {{{0,8}},70}, {{{0,9}},236},
	{{{80,7}},9}, {{{0,8}},94}, {{{0,8}},30}, {{{0,9}},156},
	{{{84,7}},99}, {{{0,8}},126}, {{{0,8}},62}, {{{0,9}},220},
	{{{82,7}},27}, {{{0,8}},110}, {{{0,8}},46}, {{{0,9}},188},
	{{{0,8}},14}, {{{0,8}},142}, {{{0,8}},78}, {{{0,9}},252},
	{{{96,7}},256}, {{{0,8}},81}, {{{0,8}},17}, {{{85,8}},131},
	{{{82,7}},31}, {{{0,8}},113}, {{{0,8}},49}, {{{0,9}},194},
	{{{80,7}},10}, {{{0,8}},97}, {{{0,8}},33}, {{{0,9}},162},
	{{{0,8}},1}, {{{0,8}},129}, {{{0,8}},65}, {{{0,9}},226},
	{{{80,7}},6}, {{{0,8}},89}, {{{0,8}},25}, {{{0,9}},146},
	{{{83,7}},59}, {{{0,8}},121}, {{{0,8}},57}, {{{0,9}},210},
	{{{81,7}},17}, {{{0,8}},105}, {{{0,8}},41}, {{{0,9}},178},
	{{{0,8}},9}, {{{0,8}},137}, {{{0,8}},73}, {{{0,9}},242},
	{{{80,7}},4}, {{{0,8}},85}, {{{0,8}},21}, {{{80,8}},258},
	{{{83,7}},43}, {{{0,8}},117}, {{{0,8}},53}, {{{0,9}},202},
	{{{81,7}},13}, {{{0,8}},101}, {{{0,8}},37}, {{{0,9}},170},
	{{{0,8}},5}, {{{0,8}},133}, {{{0,8}},69}, {{{0,9}},234},
	{{{80,7}},8}, {{{0,8}},93}, {{{0,8}},29}, {{{0,9}},154},
	{{{84,7}},83}, {{{0,8}},125}, {{{0,8}},61}, {{{0,9}},218},
	{{{82,7}},23}, {{{0,8}},109}, {{{0,8}},45}, {{{0,9}},186},
	{{{0,8}},13}, {{{0,8}},141}, {{{0,8}},77}, {{{0,9}},250},
	{{{80,7}},3}, {{{0,8}},83}, {{{0,8}},19}, {{{85,8}},195},
	{{{83,7}},35}, {{{0,8}},115}, {{{0,8}},51}, {{{0,9}},198},
	{{{81,7}},11}, {{{0,8}},99}, {{{0,8}},35}, {{{0,9}},166},
	{{{0,8}},3}, {{{0,8}},131}, {{{0,8}},67}, {{{0,9}},230},
	{{{80,7}},7}, {{{0,8}},91}, {{{0,8}},27}, {{{0,9}},150},
	{{{84,7}},67}, {{{0,8}},123}, {{{0,8}},59}, {{{0,9}},214},
	{{{82,7}},19}, {{{0,8}},107}, {{{0,8}},43}, {{{0,9}},182},
	{{{0,8}},11}, {{{0,8}},139}, {{{0,8}},75}, {{{0,9}},246},
	{{{80,7}},5}, {{{0,8}},87}, {{{0,8}},23}, {{{192,8}},0},
	{{{83,7}},51}, {{{0,8}},119}, {{{0,8}},55}, {{{0,9}},206},
	{{{81,7}},15}, {{{0,8}},103}, {{{0,8}},39}, {{{0,9}},174},
	{{{0,8}},7}, {{{0,8}},135}, {{{0,8}},71}, {{{0,9}},238},
	{{{80,7}},9}, {{{0,8}},95}, {{{0,8}},31}, {{{0,9}},158},
	{{{84,7}},99}, {{{0,8}},127}, {{{0,8}},63}, {{{0,9}},222},
	{{{82,7}},27}, {{{0,8}},111}, {{{0,8}},47}, {{{0,9}},190},
	{{{0,8}},15}, {{{0,8}},143}, {{{0,8}},79}, {{{0,9}},254},
	{{{96,7}},256}, {{{0,8}},80}, {{{0,8}},16}, {{{84,8}},115},
	{{{82,7}},31}, {{{0,8}},112}, {{{0,8}},48}, {{{0,9}},193},
	{{{80,7}},10}, {{{0,8}},96}, {{{0,8}},32}, {{{0,9}},161},
	{{{0,8}},0}, {{{0,8}},128}, {{{0,8}},64}, {{{0,9}},225},
	{{{80,7}},6}, {{{0,8}},88}, {{{0,8}},24}, {{{0,9}},145},
	{{{83,7}},59}, {{{0,8}},120}, {{{0,8}},56}, {{{0,9}},209},
	{{{81,7}},17}, {{{0,8}},104}, {{{0,8}},40}, {{{0,9}},177},
	{{{0,8}},8}, {{{0,8}},136}, {{{0,8}},72}, {{{0,9}},241},
	{{{80,7}},4}, {{{0,8}},84}, {{{0,8}},20}, {{{85,8}},227},
	{{{83,7}},43}, {{{0,8}},116}, {{{0,8}},52}, {{{0,9}},201},
	{{{81,7}},13}, {{{0,8}},100}, {{{0,8}},36}, {{{0,9}},169},
	{{{0,8}},4}, {{{0,8}},132}, {{{0,8}},68}, {{{0,9}},233},
	{{{80,7}},8}, {{{0,8}},92}, {{{0,8}},28}, {{{0,9}},153},
	{{{84,7}},83}, {{{0,8}},124}, {{{0,8}},60}, {{{0,9}},217},
	{{{82,7}},23}, {{{0,8}},108}, {{{0,8}},44}, {{{0,9}},185},
	{{{0,8}},12}, {{{0,8}},140}, {{{0,8}},76}, {{{0,9}},249},
	{{{80,7}},3}, {{{0,8}},82}, {{{0,8}},18}, {{{85,8}},163},
	{{{83,7}},35}, {{{0,8}},114}, {{{0,8}},50}, {{{0,9}},197},
	{{{81,7}},11}, {{{0,8}},98}, {{{0,8}},34}, {{{0,9}},165},
	{{{0,8}},2}, {{{0,8}},130}, {{{0,8}},66}, {{{0,9}},229},
	{{{80,7}},7}, {{{0,8}},90}, {{{0,8}},26}, {{{0,9}},149},
	{{{84,7}},67}, {{{0,8}},122}, {{{0,8}},58}, {{{0,9}},213},
	{{{82,7}},19}, {{{0,8}},106}, {{{0,8}},42}, {{{0,9}},181},
	{{{0,8}},10}, {{{0,8}},138}, {{{0,8}},74}, {{{0,9}},245},
	{{{80,7}},5}, {{{0,8}},86}, {{{0,8}},22}, {{{192,8}},0},
	{{{83,7}},51}, {{{0,8}},118}, {{{0,8}},54}, {{{0,9}},205},
	{{{81,7}},15}, {{{0,8}},102}, {{{0,8}},38}, {{{0,9}},173},
	{{{0,8}},6}, {{{0,8}},134}, {{{0,8}},70}, {{{0,9}},237},
	{{{80,7}},9}, {{{0,8}},94}, {{{0,8}},30}, {{{0,9}},157},
	{{{84,7}},99}, {{{0,8}},126}, {{{0,8}},62}, {{{0,9}},221},
	{{{82,7}},27}, {{{0,8}},110}, {{{0,8}},46}, {{{0,9}},189},
	{{{0,8}},14}, {{{0,8}},142}, {{{0,8}},78}, {{{0,9}},253},
	{{{96,7}},256}, {{{0,8}},81}, {{{0,8}},17}, {{{85,8}},131},
	{{{82,7}},31}, {{{0,8}},113}, {{{0,8}},49}, {{{0,9}},195},
	{{{80,7}},10}, {{{0,8}},97}, {{{0,8}},33}, {{{0,9}},163},
	{{{0,8}},1}, {{{0,8}},129}, {{{0,8}},65}, {{{0,9}},227},
	{{{80,7}},6}, {{{0,8}},89}, {{{0,8}},25}, {{{0,9}},147},
	{{{83,7}},59}, {{{0,8}},121}, {{{0,8}},57}, {{{0,9}},211},
	{{{81,7}},17}, {{{0,8}},105}, {{{0,8}},41}, {{{0,9}},179},
	{{{0,8}},9}, {{{0,8}},137}, {{{0,8}},73}, {{{0,9}},243},
	{{{80,7}},4}, {{{0,8}},85}, {{{0,8}},21}, {{{80,8}},258},
	{{{83,7}},43}, {{{0,8}},117}, {{{0,8}},53}, {{{0,9}},203},
	{{{81,7}},13}, {{{0,8}},101}, {{{0,8}},37}, {{{0,9}},171},
	{{{0,8}},5}, {{{0,8}},133}, {{{0,8}},69}, {{{0,9}},235},
	{{{80,7}},8}, {{{0,8}},93}, {{{0,8}},29}, {{{0,9}},155},
	{{{84,7}},83}, {{{0,8}},125}, {{{0,8}},61}, {{{0,9}},219},
	{{{82,7}},23}, {{{0,8}},109}, {{{0,8}},45}, {{{0,9}},187},
	{{{0,8}},13}, {{{0,8}},141}, {{{0,8}},77}, {{{0,9}},251},
	{{{80,7}},3}, {{{0,8}},83}, {{{0,8}},19}, {{{85,8}},195},
	{{{83,7}},35}, {{{0,8}},115}, {{{0,8}},51}, {{{0,9}},199},
	{{{81,7}},11}, {{{0,8}},99}, {{{0,8}},35}, {{{0,9}},167},
	{{{0,8}},3}, {{{0,8}},131}, {{{0,8}},67}, {{{0,9}},231},
	{{{80,7}},7}, {{{0,8}},91}, {{{0,8}},27}, {{{0,9}},151},
	{{{84,7}},67}, {{{0,8}},123}, {{{0,8}},59}, {{{0,9}},215},
	{{{82,7}},19}, {{{0,8}},107}, {{{0,8}},43}, {{{0,9}},183},
	{{{0,8}},11}, {{{0,8}},139}, {{{0,8}},75}, {{{0,9}},247},
	{{{80,7}},5}, {{{0,8}},87}, {{{0,8}},23}, {{{192,8}},0},
	{{{83,7}},51}, {{{0,8}},119}, {{{0,8}},55}, {{{0,9}},207},
	{{{81,7}},15}, {{{0,8}},103}, {{{0,8}},39}, {{{0,9}},175},
	{{{0,8}},7}, {{{0,8}},135}, {{{0,8}},71}, {{{0,9}},239},
	{{{80,7}},9}, {{{0,8}},95}, {{{0,8}},31}, {{{0,9}},159},
	{{{84,7}},99}, {{{0,8}},127}, {{{0,8}},63}, {{{0,9}},223},
	{{{82,7}},27}, {{{0,8}},111}, {{{0,8}},47}, {{{0,9}},191},
	{{{0,8}},15}, {{{0,8}},143}, {{{0,8}},79}, {{{0,9}},255}
};

const inflate_huft fixed_td[] = {
	{{{80,5}},1}, {{{87,5}},257}, {{{83,5}},17}, {{{91,5}},4097},
	{{{81,5}},5}, {{{89,5}},1025}, {{{85,5}},65}, {{{93,5}},16385},
	{{{80,5}},3}, {{{88,5}},513}, {{{84,5}},33}, {{{92,5}},8193},
	{{{82,5}},9}, {{{90,5}},2049}, {{{86,5}},129}, {{{192,5}},24577},
	{{{80,5}},2}, {{{87,5}},385}, {{{83,5}},25}, {{{91,5}},6145},
	{{{81,5}},7}, {{{89,5}},1537}, {{{85,5}},97}, {{{93,5}},24577},
	{{{80,5}},4}, {{{88,5}},769}, {{{84,5}},49}, {{{92,5}},12289},
	{{{82,5}},13}, {{{90,5}},3073}, {{{86,5}},193}, {{{192,5}},24577}
};

const uInt cplens[31] = {// pkzip 的 deflate 表 appnote.txt 文件
	3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
	35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0
};

const uInt cplext[31] = {
		0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
		3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 112, 112
};

const uInt cpdist[30] = {
		1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
		257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
		8193, 12289, 16385, 24577
};

const uInt cpdext[30] = {//距离码的额外位
		0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
		7, 7, 8, 8, 9, 9, 10, 10, 11, 11,
		12, 12, 13, 13
};

const uLong crc_table[256] = {
  0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
  0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
  0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
  0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
  0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
  0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
  0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
  0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
  0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
  0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
  0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
  0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
  0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
  0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
  0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
  0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
  0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
  0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
  0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
  0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
  0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
  0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
  0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
  0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
  0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
  0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
  0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
  0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
  0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
  0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
  0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
  0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
  0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
  0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
  0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
  0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
  0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
  0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
  0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
  0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
  0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
  0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
  0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
  0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
  0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
  0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
  0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
  0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
  0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
  0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
  0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
  0x2d02ef8dL
};


/* 解压 zip 的主要部分 */
int InflateTreesBits(uInt*, uInt*, inflate_huft**, inflate_huft*, z_streamp);//提前声明相关函数
int InflateTreesDynamic(uInt, uInt, uInt*, uInt*, uInt*, inflate_huft**, inflate_huft**, inflate_huft*, z_streamp);//提前声明相关函数
int InflateTreesFixed(uInt*, uInt*, const inflate_huft**, const inflate_huft**, z_streamp);//提前声明相关函数
int InflateTreesFast(uInt, uInt, const inflate_huft*, const inflate_huft*, inflate_blocks_statef*, z_streamp);//提前声明相关函数

int InflateTreeFflush(inflate_blocks_statef* s, z_streamp z, int r)
{
	uInt n;//尽可能多地从滑动窗口复制到输出区域
	Byte* p = z->next_out, * q = s->read;//源指针和目标指针的本地副本
	n = (uInt)((q <= s->write ? s->write : s->end) - q);//计算要复制到窗口末尾的字节数
	if (n > z->avail_out)
		n = z->avail_out;
	if (n && r == Z_BUF_ERROR)
		r = Z_OK;
	z->avail_out -= n;//更新计数器
	z->total_out += n;
	if (s->checkfn != Z_NULL)//更新检测信息
		z->adler = s->check = (*s->checkfn)(s->check, q, n);
	if (n != 0)//复制到窗口末端并避免激活 CodeGuard
	{
		memcpy(p, q, n);
		p += n;
		q += n;
	}
	if (q == s->end)//在窗口开始处查看是否有更多需要复制的内容
	{
		q = s->window;
		if (s->write == s->end)
			s->write = s->window;//交换指针
		n = (uInt)(s->write - q);//计算要复制的字节数
		if (n > z->avail_out)
			n = z->avail_out;
		if (n && r == Z_BUF_ERROR)
			r = Z_OK;
		z->avail_out -= n;//更新计数器
		z->total_out += n;
		if (s->checkfn != Z_NULL)//更新检测信息
			z->adler = s->check = (*s->checkfn)(s->check, q, n);
		if (n != 0)
		{
			memcpy(p, q, n);
			p += n;
			q += n;
		}
	}
	z->next_out = p;//更新指针域
	s->read = q;
	return r;
}

inflate_codes_statef* inflate_codes_new(
	uInt bl, uInt bd,
	const inflate_huft* tl,
	const inflate_huft* td,//这里可能需要 Borland C++ 的单独声明
	z_streamp z
)
{
	inflate_codes_statef* c;
	if ((c = (inflate_codes_statef*)
		ZALLOC(z, 1, sizeof(struct inflate_codes_state))) != Z_NULL)
	{
		c->mode = inflate_codes_mode::START;
		c->lbits = (Byte)bl;
		c->dbits = (Byte)bd;
		c->ltree = tl;
		c->dtree = td;
		LuTracev((stderr, "内部信息：新编码\n"));
	}
	return c;
}

int inflate_codes(inflate_blocks_statef* s, z_streamp z, int r)
{
	uInt j;//临时变量
	const inflate_huft* t;//临时指针
	uLong b;
	uInt e, k, m, n;
	Byte* p, * q, * f;
	inflate_codes_statef* c = s->sub.decode.codes;//zip 专用状态码
	LOAD
		for (;;)//死循环
			switch (c->mode)
			{
			case inflate_codes_mode::START:
#ifndef SLOW
				if (m >= 258 && n >= 10)
				{
					UPDATE
						r = InflateTreesFast(c->lbits, c->dbits, c->ltree, c->dtree, s, z);
					LOAD
						if (r != Z_OK)
						{
							c->mode = r == Z_STREAM_END ? inflate_codes_mode::WASH : inflate_codes_mode::BADCODE;
							break;
						}
				}
#endif//SLOW
				c->sub.code.need = c->lbits;
				c->sub.code.tree = c->ltree;
				c->mode = inflate_codes_mode::LEN;
			case inflate_codes_mode::LEN:
				j = c->sub.code.need;
				NEEDBITS(j)
					t = c->sub.code.tree + ((uInt)b & inflate_mask[j]);
				DUMPBITS(t->bits)
					e = (uInt)(t->exop);
				if (e == 0)
				{
					c->sub.lit = t->base;
					LuTracevv((stderr, t->base >= 0x20 && t->base < 0x7f ?
						"内部信息：* literal = '%c'\n" :
						"内部信息：* literal = 0x%02x\n", t->base));
					c->mode = inflate_codes_mode::LIT;
					break;
				}
				if (e & 16)
				{
					c->sub.copy.get = e & 15;
					c->len = t->base;
					c->mode = inflate_codes_mode::LENEXT;
					break;
				}
				if ((e & 64) == 0)
				{
					c->sub.code.need = e;
					c->sub.code.tree = t + t->base;
					break;
				}
				if (e & 32)
				{
					LuTracevv((stderr, "内部信息：块的尾部\n"));
					c->mode = inflate_codes_mode::WASH;
					break;
				}
				c->mode = inflate_codes_mode::BADCODE;//处理非法格
				z->ErrorMessages = (char*)"非法格";
				r = Z_DATA_ERROR;
				LEAVE
			case inflate_codes_mode::LENEXT:
				j = c->sub.copy.get;
				NEEDBITS(j)
					c->len += (uInt)b & inflate_mask[j];
				DUMPBITS(j)
					c->sub.code.need = c->dbits;
				c->sub.code.tree = c->dtree;
				LuTracevv((stderr, "内部信息：长度 %u\n", c->len));
				c->mode = inflate_codes_mode::DIST;
			case inflate_codes_mode::DIST:
				j = c->sub.code.need;
				NEEDBITS(j)
					t = c->sub.code.tree + ((uInt)b & inflate_mask[j]);
				DUMPBITS(t->bits)
					e = (uInt)(t->exop);
				if (e & 16)
				{
					c->sub.copy.get = e & 15;
					c->sub.copy.dist = t->base;
					c->mode = inflate_codes_mode::DISTEXT;
					break;
				}
				if ((e & 64) == 0)
				{
					c->sub.code.need = e;
					c->sub.code.tree = t + t->base;
					break;
				}
				c->mode = inflate_codes_mode::BADCODE;
				z->ErrorMessages = (char*)"非法距离";
				r = Z_DATA_ERROR;
				LEAVE
			case inflate_codes_mode::DISTEXT:
				j = c->sub.copy.get;
				NEEDBITS(j)
					c->sub.copy.dist += (uInt)b & inflate_mask[j];
				DUMPBITS(j)
					LuTracevv((stderr, "内部信息：距离 %u\n", c->sub.copy.dist));
				c->mode = inflate_codes_mode::COPY;
			case inflate_codes_mode::COPY:
				f = q - c->sub.copy.dist;
				while (LT(f, s->window))//此处 while 相当于 if
					f += s->end - s->window;
				while (c->len)
				{
					NEEDOUT
						OUTBYTE(*f++)
						if (f == s->end)
							f = s->window;
					c->len--;
				}
				c->mode = inflate_codes_mode::START;
				break;
			case inflate_codes_mode::LIT:
				NEEDOUT
					OUTBYTE(c->sub.lit)
					c->mode = inflate_codes_mode::START;
				break;
			case inflate_codes_mode::WASH:
				if (k > 7)
				{
					if (k < 16)
						MessageBox(NULL, TEXT("代码占用了太多字节，请注意。"), WmipTextTitle, MB_OK | MB_ICONWARNING | MB_TOPMOST);
					k -= 8;
					n++;
					p--;//确保可以有至少一个返回值
				}
				FLUSH
					if (s->read != s->write)
						LEAVE
						c->mode = inflate_codes_mode::END;
			case inflate_codes_mode::END:
				r = Z_STREAM_END;
				LEAVE
			case inflate_codes_mode::BADCODE:
				r = Z_DATA_ERROR;
				LEAVE
			default:
				r = Z_STREAM_ERROR;
				LEAVE
			}
}

void FreeInflateCodes(inflate_codes_statef* c, z_streamp z)
{
	ZFREE(z, c);
	LuTracev((stderr, "内部信息：已释放编码\n"));
}

void inflate_blocks_reset(inflate_blocks_statef* s, z_streamp z, uLong* c)
{
	if (c != Z_NULL)
		*c = s->check;
	if (s->mode == inflate_block_mode::IBM_BTREE || s->mode == inflate_block_mode::IBM_DTREE)
		ZFREE(z, s->sub.trees.blens);
	if (s->mode == inflate_block_mode::IBM_CODES)
		FreeInflateCodes(s->sub.decode.codes, z);
	s->mode = inflate_block_mode::IBM_TYPE;
	s->bitk = 0;
	s->bitb = 0;
	s->read = s->write = s->window;
	if (s->checkfn != Z_NULL)
		z->adler = s->check = (*s->checkfn)(0L, (const Byte*)Z_NULL, 0);
	LuTracev((stderr, "内部信息：块已被重置\n"));
}

inflate_blocks_statef* inflate_blocks_new(z_streamp z, check_func c, uInt w)
{
	inflate_blocks_statef* s;

	if ((s = (inflate_blocks_statef*)ZALLOC
	(z, 1, sizeof(struct inflate_blocks_state))) == Z_NULL)
		return s;
	if ((s->hufts =
		(inflate_huft*)ZALLOC(z, sizeof(inflate_huft), MANY)) == Z_NULL)
	{
		ZFREE(z, s);
		return Z_NULL;
	}
	if ((s->window = (Byte*)ZALLOC(z, 1, w)) == Z_NULL)
	{
		ZFREE(z, s->hufts);
		ZFREE(z, s);
		return Z_NULL;
	}
	s->end = s->window + w;
	s->checkfn = c;
	s->mode = inflate_block_mode::IBM_TYPE;
	LuTracev((stderr, "内部信息：块已被分配\n"));
	inflate_blocks_reset(s, z, Z_NULL);
	return s;
}

int inflate_blocks(inflate_blocks_statef* s, z_streamp z, int r)
{
	uInt t;//临时变量
	uLong b;//存放比特
	uInt k;//位缓冲区中的位
	Byte* p;//输入数据指针
	uInt n;//剩余字节
	Byte* q;//输出窗口写入指针
	uInt m;//窗口或读取指针结尾的字节数
	LOAD//将输入输出信息复制到本地（更新宏还原）
		for (;;)//死循环
			switch (s->mode)//基于当前状态的过程输入
			{
			case inflate_block_mode::IBM_TYPE:
				NEEDBITS(3)
					t = (uInt)b & 7;
				s->last = t & 1;
				switch (t >> 1)
				{
				case 0://已存储
					LuTracev((stderr, "内部信息：存储的块 %s\n",
						s->last ? " (last)" : ""));
					DUMPBITS(3)
						t = k & 7;//转到字节边界
					DUMPBITS(t)
						s->mode = inflate_block_mode::IBM_LENS;//获取存储块的长度
					break;
				case 1://已修复
					LuTracev((stderr, "内部信息：已修复编码块 %s\n",
						s->last ? " (last)" : ""));
					{
						uInt bl, bd;
						const inflate_huft* tl, * td;
						InflateTreesFixed(&bl, &bd, &tl, &td, z);
						s->sub.decode.codes = inflate_codes_new(bl, bd, tl, td, z);
						if (s->sub.decode.codes == Z_NULL)
						{
							r = Z_MEM_ERROR;
							LEAVE
						}
					}
					DUMPBITS(3)
						s->mode = inflate_block_mode::IBM_CODES;
					break;
				case 2://动态
					LuTracev((stderr, "内部信息：动态编码块 %s\n",
						s->last ? " (last)" : ""));
					DUMPBITS(3)
						s->mode = inflate_block_mode::IBM_TABLE;
					break;
				case 3://非法
					DUMPBITS(3)
						s->mode = inflate_block_mode::IBM_BAD;
					z->ErrorMessages = (char*)"非法块类型";
					r = Z_DATA_ERROR;
					LEAVE
				}
				break;//记得 break
			case inflate_block_mode::IBM_LENS:
				NEEDBITS(32)
					if ((((~b) >> 16) & 0xffff) != (b & 0xffff))
					{
						s->mode = inflate_block_mode::IBM_BAD;
						z->ErrorMessages = (char*)"存储块长度非法";
						r = Z_DATA_ERROR;
						LEAVE
					}
				s->sub.left = (uInt)b & 0xffff;
				k = 0;// dump bits
				b = k;
				LuTracev((stderr, "内部信息：已存储长度 %u\n", s->sub.left));
				s->mode = s->sub.left ? inflate_block_mode::IBM_STORED : (s->last ? inflate_block_mode::IBM_DRY : inflate_block_mode::IBM_TYPE);
				break;
			case inflate_block_mode::IBM_STORED:
				if (n == 0)
					LEAVE
					NEEDOUT
					t = s->sub.left;
				if (t > n)
					t = n;
				if (t > m)
					t = m;
				memcpy(q, p, t);
				p += t;
				n -= t;
				q += t;
				m -= t;
				if ((s->sub.left -= t) != 0)
					break;
				LuTracev((stderr, "内部信息：存储完毕，共有 %lu 输出\n",
					z->total_out + (q >= s->read ? q - s->read :
						(s->end - s->read) + (q - s->window))));
				s->mode = s->last ? inflate_block_mode::IBM_DRY : inflate_block_mode::IBM_TYPE;
				break;
			case inflate_block_mode::IBM_TABLE:
				NEEDBITS(14)
					s->sub.trees.table = t = (uInt)b & 0x3fff;
				if ((t & 0x1f) > 29 || ((t >> 5) & 0x1f) > 29)//删除此部分以解决 pkzip 中的错误
				{
					s->mode = inflate_block_mode::IBM_BAD;
					z->ErrorMessages = (char*)"长度或符号距离太大";
					r = Z_DATA_ERROR;
					LEAVE
				}
				t = 258 + (t & 0x1f) + ((t >> 5) & 0x1f);
				if ((s->sub.trees.blens = (uInt*)ZALLOC(z, t, sizeof(uInt))) == Z_NULL)
				{
					r = Z_MEM_ERROR;
					LEAVE
				}
				DUMPBITS(14)
					s->sub.trees.index = 0;
				LuTracev((stderr, "内部信息：表格大小正常\n"));
				s->mode = inflate_block_mode::IBM_BTREE;
			case inflate_block_mode::IBM_BTREE:
				while (s->sub.trees.index < 4 + (s->sub.trees.table >> 10))
				{
					NEEDBITS(3)
						s->sub.trees.blens[border[s->sub.trees.index++]] = (uInt)b & 7;
					DUMPBITS(3)
				}
				while (s->sub.trees.index < 19)
					s->sub.trees.blens[border[s->sub.trees.index++]] = 0;
				s->sub.trees.bb = 7;
				t = InflateTreesBits(s->sub.trees.blens, &s->sub.trees.bb,
					&s->sub.trees.tb, s->hufts, z);
				if (t != Z_OK)
				{
					r = t;
					if (r == Z_DATA_ERROR)
					{
						ZFREE(z, s->sub.trees.blens);
						s->mode = inflate_block_mode::IBM_BAD;
					}
					LEAVE
				}
				s->sub.trees.index = 0;
				LuTracev((stderr, "内部信息：编码树成功\n"));
				s->mode = inflate_block_mode::IBM_DTREE;
			case inflate_block_mode::IBM_DTREE:
				while (t = s->sub.trees.table,
					s->sub.trees.index < 258 + (t & 0x1f) + ((t >> 5) & 0x1f))
				{
					inflate_huft* h;
					uInt i, j, c;
					t = s->sub.trees.bb;
					NEEDBITS(t)
						h = s->sub.trees.tb + ((uInt)b & inflate_mask[t]);
					t = h->bits;
					c = h->base;
					if (c < 16)
					{
						DUMPBITS(t)
							s->sub.trees.blens[s->sub.trees.index++] = c;
					}
					else// c == 16-18 
					{
						i = c == 18 ? 7 : c - 14;
						j = c == 18 ? 11 : 3;
						NEEDBITS(t + i)
							DUMPBITS(t)
							j += (uInt)b & inflate_mask[i];
						DUMPBITS(i)
							i = s->sub.trees.index;
						t = s->sub.trees.table;
						if (i + j > 258 + (t & 0x1f) + ((t >> 5) & 0x1f) ||
							(c == 16 && i < 1))
						{
							ZFREE(z, s->sub.trees.blens);
							s->mode = inflate_block_mode::IBM_BAD;
							z->ErrorMessages = (char*)"数据循环冗余";
							r = Z_DATA_ERROR;
							LEAVE
						}
						c = c == 16 ? s->sub.trees.blens[i - 1] : 0;
						do {
							s->sub.trees.blens[i++] = c;
						} while (--j);
						s->sub.trees.index = i;
					}
				}
				s->sub.trees.tb = Z_NULL;
				{
					uInt bl, bd;
					inflate_huft* tl, * td;
					inflate_codes_statef* c;
					bl = 9;//前瞻性假设，必须 <= 9
					bd = 6;
					t = s->sub.trees.table;
					t = InflateTreesDynamic(257 + (t & 0x1f), 1 + ((t >> 5) & 0x1f),
						s->sub.trees.blens, &bl, &bd, &tl, &td,
						s->hufts, z);
					if (t != Z_OK)
					{
						if (t == (uInt)Z_DATA_ERROR)
						{
							ZFREE(z, s->sub.trees.blens);
							s->mode = inflate_block_mode::IBM_BAD;
						}
						r = t;
						LEAVE
					}
					LuTracev((stderr, "内部信息：建树成功\n"));
					if ((c = inflate_codes_new(bl, bd, tl, td, z)) == Z_NULL)
					{
						r = Z_MEM_ERROR;
						LEAVE
					}
					s->sub.decode.codes = c;
				}
				ZFREE(z, s->sub.trees.blens);
				s->mode = inflate_block_mode::IBM_CODES;
			case inflate_block_mode::IBM_CODES:
				UPDATE
					if ((r = inflate_codes(s, z, r)) != Z_STREAM_END)
						return InflateTreeFflush(s, z, r);
				r = Z_OK;
				FreeInflateCodes(s->sub.decode.codes, z);
				LOAD
					LuTracev((stderr, "内部信息：编码完毕，共有 %lu 输出\n",
						z->total_out + (q >= s->read ? q - s->read :
							(s->end - s->read) + (q - s->window))));
				if (!s->last)
				{
					s->mode = inflate_block_mode::IBM_TYPE;
					break;
				}
				s->mode = inflate_block_mode::IBM_DRY;
			case inflate_block_mode::IBM_DRY:
				FLUSH
					if (s->read != s->write)
						LEAVE
						s->mode = inflate_block_mode::IBM_DONE;
			case inflate_block_mode::IBM_DONE:
				r = Z_STREAM_END;
				LEAVE
			case inflate_block_mode::IBM_BAD:
				r = Z_DATA_ERROR;
				LEAVE
			default:
				r = Z_STREAM_ERROR;
				LEAVE
			}
}

int inflate_blocks_free(inflate_blocks_statef* s, z_streamp z)
{
	inflate_blocks_reset(s, z, Z_NULL);
	ZFREE(z, s->window);
	ZFREE(z, s->hufts);
	ZFREE(z, s);
	LuTracev((stderr, "内部信息：块已被释放\n"));
	return Z_OK;
}

int huft_build(
	uInt* b,//编码长度
	uInt n,//编码数量
	uInt s,//单值码个数
	const uInt* d,//非简单代码的基值列表
	const uInt* e,//非简单码的额外位列表
	inflate_huft** t,//起始表
	uInt* m,//返回最大查找位的实际值
	inflate_huft* hp,//树
	uInt* hn,//树的大小
	uInt* v//权值
)//给定代码长度和最大表大小的列表进行解码
{
	uInt a;//长度为 k 的代码计数器
	uInt c[BMAX + 1];//位长计数表
	uInt f;//在表中每 f 个条目重复一次
	int g;//最大码长
	int h;//权值
	register uInt i, j;//两个计数器
	register int k;//当前代码中的位数
	int l;//以 MB 为单位返回表的位数
	uInt mask;//（1<<w）-1
	register uInt* p;//活动表指针
	inflate_huft* q;//当前表的指针
	struct inflate_huft_s r;//结构分配的表条目
	inflate_huft* u[BMAX];//表堆栈
	register int w;//此表前的位 ==（l*h）
	uInt x[BMAX + 1];//位偏移和代码堆栈
	uInt* xp;//数组 x 的指针
	int y;//添加的伪代码数
	uInt z;//添加到当前表中的条目数的伪代码数
	p = c;//接下来为每个位长度生成计数
#ifndef C0
#define C0 *p++ = 0;
#endif
#define C2 C0 C0 C0 C0
#define C4 C2 C2 C2 C2
	C4;//使用安全的宏
	p;//清除 c[]
	p = b;
	i = n;
	do
	{
		c[*p++]++;
	} while (--i);
	if (c[0] == n)//处理空输入
	{
		*t = (inflate_huft*)Z_NULL;
		*m = 0;
		return Z_OK;
	}
	l = *m;//找到最小和最大长度，用这些值限制 *m
	for (j = 1; j <= BMAX; ++j)
		if (c[j])
			break;
	k = j;//最小码长
	if ((uInt)l < j)
		l = j;
	for (i = BMAX; i; --i)
		if (c[i])
			break;
	g = i;//最大码长
	if ((uInt)l > i)
		l = i;
	*m = l;
	for (y = 1 << j; j < i; j++, y <<= 1)//调整最后长度计数
		if ((y -= c[j]) < 0)
			return Z_DATA_ERROR;
	if ((y -= c[i]) < 0)
		return Z_DATA_ERROR;
	c[i] += y;
	j = 0;//为每个长度在值表中生成起始偏移
	x[1] = j;
	p = c + 1;
	xp = x + 2;
	while (--i)//注意上述的 i == g
		*xp++ = (j += *p++);
	p = b;//按位长顺序制作值表
	i = 0;
	do
	{
		if ((j = *p++) != 0)
			v[x[j]++] = i;
	} while (++i < n);
	n = x[g];//将 n 设置为 v 的长度
	i = 0;//初始值为 0
	x[0] = i;//生成Huffman代码，并为每个生成表条目
	p = v;//按位顺序获取值
	h = -1;
	w = -l;//解码位 == (l * h)
	u[0] = (inflate_huft*)Z_NULL;//消除 C4703 错误
	q = (inflate_huft*)Z_NULL;//消除 C4703 错误
	z = 0;//初始化 z
	for (; k <= g; ++k)//遍历位长度
	{
		a = c[k];
		while (a--)
		{
			while (k > w + l)//这里是长度为 k 位的哈夫曼码，值为 *p
			{
				h++;
				w += l;//上一个表总是 l 位
				z = g - w;//计算 <= l 位的最小大小表
				z = (z > (uInt)l ? l : z);//表大小上限
				if ((f = 1 << (j = k - w)) > a + 1)
				{
					f -= a + 1;
					xp = c + k;
					if (j < z)
						while (++j < z)//尝试 z 位以下的子表
						{
							if ((f <<= 1) <= *++xp)
								break;
							f -= *xp;
						}
				}
				z = 1 << j;
				if (*hn + z > MANY)//建立新表
					return Z_DATA_ERROR;//溢出
				u[h] = q = hp + *hn;
				*hn += z;
				if (h)//尝试连接最后一个表
				{
					x[h] = i;//保存备份模式
					r.bits = (Byte)l;//在此表之前转储的位
					r.exop = (Byte)j;//此表中的位
					j = i >> (w - l);
					r.base = (uInt)(q - u[h - 1] - j);//偏移到此表
					u[h - 1][j] = r;//连接到最后一个表
				}
				else
					*t = q;//保存第一个表的返回结果
			}
			r.bits = (Byte)(k - w);//在 r 中设置表条目
			if (p >= v + n)
				r.exop = 192;//2 的 7 次方与 2 的 6 次方的和
			else if (*p < s)
			{
				r.exop = (Byte)(*p < 256 ? 0 : 32 + 64);
				r.base = *p++;
			}
			else
			{
				r.exop = (Byte)(e[*p - s] + 16 + 64);
				r.base = d[*p++ - s];
			}
			f = 1 << (k - w);//填充条目
			for (j = i >> w; j < z; j += f)
				q[j] = r;
			for (j = 1 << (k - 1); i & j; j >>= 1)//向后递增k位代码i
				i ^= j;
			i ^= j;
			mask = (1 << w) - 1;//备份已完成的表
			while ((i & mask) != x[h])
			{
				h--;//注意：不需要再更新表 q
				w -= l;
				mask = (1 << w) - 1;
			}
		}
	}
	return y != 0 && g != 1 ? Z_BUF_ERROR : Z_OK;
}

int InflateTreesBits(
	uInt* c,// 19 个编码单位
	uInt* bb,//所需位树 / 实际深度
	inflate_huft** tb,//位树结果
	inflate_huft* hp,//哈夫曼树占用的空间
	z_streamp z//其它消息
)
{
	int r;
	uInt hn = 0;//哈夫曼树占用的空间
	uInt* v;//建筑工作区
	if ((v = (uInt*)ZALLOC(z, 19, sizeof(uInt))) == Z_NULL)
		return Z_MEM_ERROR;
	r = huft_build(c, 19, 19, (uInt*)Z_NULL, (uInt*)Z_NULL,
		tb, bb, hp, &hn, v);
	if (r == Z_DATA_ERROR)
		z->ErrorMessages = (char*)"超额请求动态位长树";
	else if (r == Z_BUF_ERROR || *bb == 0)
	{
		z->ErrorMessages = (char*)"不完全动态位长树";
		r = Z_DATA_ERROR;
	}
	ZFREE(z, v);
	return r;
}

int InflateTreesDynamic(
	uInt nl,//文字 / 长度代码数
	uInt nd,//距离码数
	uInt* c,//代码总长
	uInt* bl,//树的广度 / 树实际位深度
	uInt* bd,//所需距离 / 实际位深
	inflate_huft** tl,//文字 / 长度树结果
	inflate_huft** td,//距离树结果
	inflate_huft* hp,//哈夫曼树占用的空间
	z_streamp z//其它信息
)
{
	int r;
	uInt hn = 0;//哈夫曼树占用的空间
	uInt* v;
	if ((v = (uInt*)ZALLOC(z, 288, sizeof(uInt))) == Z_NULL)
		return Z_MEM_ERROR;
	r = huft_build(c, nl, 257, cplens, cplext, tl, bl, hp, &hn, v);
	if (r != Z_OK || *bl == 0)
	{
		if (r == Z_DATA_ERROR)
			z->ErrorMessages = (char*)"计算溢出";
		else if (r != Z_MEM_ERROR)
		{
			z->ErrorMessages = (char*)"无法计算";
			r = Z_DATA_ERROR;
		}
		ZFREE(z, v);
		return r;
	}
	r = huft_build(c + nl, nd, 0, cpdist, cpdext, td, bd, hp, &hn, v);
	if (r != Z_OK || (*bd == 0 && nl > 257))
	{
		if (r == Z_DATA_ERROR)
			z->ErrorMessages = (char*)"位树请求失败";
		else if (r == Z_BUF_ERROR)
		{
			z->ErrorMessages = (char*)"无法计算位树";
			r = Z_DATA_ERROR;
		}
		else if (r != Z_MEM_ERROR)
		{
			z->ErrorMessages = (char*)"清空位树";
			r = Z_DATA_ERROR;
		}
		ZFREE(z, v);
		return r;
	}
	ZFREE(z, v);
	return Z_OK;
}

int InflateTreesFixed(
	uInt* bl,//所需文字 / 实际位深度
	uInt* bd,//所需距离 / 实际位深
	const inflate_huft** tl,//文字 / 长度树结果
	const inflate_huft** td,//距离树结果
	z_streamp//用于内存分配
)
{
	*bl = fixed_bl;
	*bd = fixed_bd;
	*tl = fixed_tl;
	*td = fixed_td;
	return Z_OK;
}

int InflateTreesFast(
	uInt bl, uInt bd,
	const inflate_huft* tl,
	const inflate_huft* td,
	inflate_blocks_statef* s,
	z_streamp z
)//类似注释不再重复
{
	const inflate_huft* t;
	uInt c, d, e, k, m, n, ml, md;
	uLong b;
	Byte* p, * q, * r;
	LOAD
		ml = inflate_mask[bl];
	md = inflate_mask[bd];
	do//直到没有足够的输入或输出空间，用于快速循环
	{
		GRABBITS(20)
			if ((e = (t = tl + ((uInt)b & ml))->exop) == 0)
			{
				DUMPBITS(t->bits)
					LuTracevv((stderr, t->base >= 0x20 && t->base < 0x7f ?
						"内部信息：* literal = '%c'\n" :
						"内部信息：* literal = 0x%02x\n", t->base
						)
					);
				*q++ = (Byte)t->base;
				m--;
				continue;
			}
		for (;;)//死循环
		{
			DUMPBITS(t->bits)
				if (e & 16)
				{
					e &= 15;
					c = t->base + ((uInt)b & inflate_mask[e]);
					DUMPBITS(e)
						LuTracevv((stderr, "内部信息：* length = %u\n", c));
					GRABBITS(15);
					e = (t = td + ((uInt)b & md))->exop;
					for (;;)
					{
						DUMPBITS(t->bits)
							if (e & 16)
							{
								e &= 15;
								GRABBITS(e)
									d = t->base + ((uInt)b & inflate_mask[e]);
								DUMPBITS(e)
									LuTracevv((stderr, "内部信息：* distance = %u\n", d));
								m -= c;//复制
								r = q - d;
								if (r < s->window)//如果需要，则执行交换
								{
									do
									{
										r += s->end - s->window;//强制让指针指向 window
									} while (r < s->window);//覆盖无效距离
									e = (uInt)(s->end - r);
									if (c > e)//交换复制
									{
										c -= e;
										do
										{
											*q++ = *r++;
										} while (--e);
										r = s->window;
										do
										{
											*q++ = *r++;
										} while (--c);
									}
									else//正常复制
									{
										*q++ = *r++;
										c--;
										*q++ = *r++;
										c--;
										do
										{
											*q++ = *r++;
										} while (--c);
									}
								}
								else//正常复制
								{
									*q++ = *r++;
									c--;
									*q++ = *r++;
									c--;
									do
									{
										*q++ = *r++;
									} while (--c);
								}
								break;
							}
							else if ((e & 64) == 0)
							{
								t += t->base;
								e = (t += ((uInt)b & inflate_mask[e]))->exop;
							}
							else
							{
								z->ErrorMessages = (char*)"非法距离编码";
								UNGRAB
									UPDATE
									return Z_DATA_ERROR;
							}
					};
					break;
				}
			if ((e & 64) == 0)
			{
				t += t->base;
				if ((e = (t += ((uInt)b & inflate_mask[e]))->exop) == 0)
				{
					DUMPBITS(t->bits)
						LuTracevv((stderr, t->base >= 0x20 && t->base < 0x7f ?
							"内部信息：* literal = '%c'\n" :
							"内部信息：* literal = 0x%02x\n", t->base));
					*q++ = (Byte)t->base;
					m--;
					break;
				}
			}
			else if (e & 32)
			{
				LuTracevv((stderr, "内部信息：已到达块的尾部\n"));
				UNGRAB
					UPDATE
					return Z_STREAM_END;
			}
			else
			{
				z->ErrorMessages = (char*)"树的广度或深度非法";
				UNGRAB
					UPDATE
					return Z_DATA_ERROR;
			}
		};
	} while (m >= 258 && n >= 10);//假设调用 m >= 258 且 n>=10
	UNGRAB
		UPDATE
		return Z_OK;
}

const uLong* get_crc_table()
{
	return (const uLong*)crc_table;
}

uLong ucrc32(uLong crc, const Byte* buf, uInt len)
{
	if (buf == Z_NULL)
		return 0L;
	crc ^= 0xffffffffL;
	while (len >= 8)
	{
		CRC_DO8(buf);
		len -= 8;
	}
	if (len)
	{
		do
		{
			CRC_DO1(buf);
		} while (--len);
	}
	return crc ^ 0xffffffffL;
}

void UUpdateKeys(unsigned long* keys, char c)
{
	keys[0] = CRC32(keys[0], c);
	keys[1] += keys[0] & 0xFF;
	keys[1] = keys[1] * 134775813L + 1;
	keys[2] = CRC32(keys[2], keys[1] >> 24);
}

char UDecryptBytes(unsigned long* keys)
{
	unsigned temp = ((unsigned)keys[2] & 0xffff) | 2;
	return (char)(((temp * (temp ^ 1)) >> 8) & 0xff);
}

char zdecode(unsigned long* keys, char c)
{
	c ^= UDecryptBytes(keys);
	UUpdateKeys(keys, c);
	return c;
}

uLong adler32(uLong adler, const Byte* buf, uInt len)
{
	unsigned long s1 = adler & 0xffff, s2 = (adler >> 16) & 0xffff;
	int k;
	if (buf == Z_NULL)
		return 1L;
	while (len > 0)
	{
		k = (len < NMAX ? len : NMAX);
		len -= k;
		while (k >= 16)
		{
			AD_DO16(buf);
			buf += 16;
			k -= 16;
		}
		if (k != 0)
		{
			do
			{
				s1 += *buf++;
				s2 += s1;
			} while (--k);
		}//加个括号防止识别出错
		s1 %= BASE;
		s2 %= BASE;
	}
	return (s2 << 16) | s1;
}

const char* zlibVersion()//显示可解压压缩包的版本（该版本值可能不通用）
{
	return ZLIB_VERSION;
}

const char* zError(int err)//显示错误信息
{
	return ERR_MSG(err);
}

voidpf zcalloc(voidpf opaque, unsigned items, unsigned size)
{
	if (opaque)//消除可能的编译器警告
		items += size - size;
	return (voidpf)calloc(items, size);
}

void zcfree(voidpf opaque, voidpf ptr)
{
	zfree(ptr);
	if (opaque)//消除可能的编译器警告
		return;
	return;
}

int inflateReset(z_streamp z)
{
	if (z == Z_NULL || z->state == Z_NULL)
		return Z_STREAM_ERROR;
	z->total_in = z->total_out = 0;
	z->ErrorMessages = Z_NULL;
	z->state->mode = z->state->nowrap ? inflate_mode::IM_BLOCKS : inflate_mode::IM_METHOD;
	inflate_blocks_reset(z->state->blocks, z, Z_NULL);
	LuTracev((stderr, "内部信息：重置\n"));
	return Z_OK;
}

int inflateEnd(z_streamp z)
{
	if (z == Z_NULL || z->state == Z_NULL || z->zfree == Z_NULL)
		return Z_STREAM_ERROR;
	if (z->state->blocks != Z_NULL)
		inflate_blocks_free(z->state->blocks, z);
	ZFREE(z, z->state);
	z->state = Z_NULL;
	LuTracev((stderr, "内部信息：结束\n"));
	return Z_OK;
}

int inflateInit2(z_streamp z)
{
	const char* version = ZLIB_VERSION; int stream_size = sizeof(z_stream);
	if (version == Z_NULL || version[0] != ZLIB_VERSION[0] || stream_size != sizeof(z_stream))
		return Z_VERSION_ERROR;
	int w = -15;
	if (z == Z_NULL)
		return Z_STREAM_ERROR;
	z->ErrorMessages = Z_NULL;
	if (z->zalloc == Z_NULL)
	{
		z->zalloc = zcalloc;
		z->opaque = (voidpf)0;
	}
	if (z->zfree == Z_NULL)
		z->zfree = zcfree;
	if ((z->state = (struct internal_state*)
		ZALLOC(z, 1, sizeof(struct internal_state))) == Z_NULL)
		return Z_MEM_ERROR;
	z->state->blocks = Z_NULL;
	z->state->nowrap = 0;//处理未记录或没有 zlib头 或未经过检查的 nowrap 选项
	if (w < 0)
	{
		w = -w;
		z->state->nowrap = 1;
	}
	if (w < 8 || w > 15)//设置大小
	{
		inflateEnd(z);
		return Z_STREAM_ERROR;
	}
	z->state->wbits = (uInt)w;
	if ((z->state->blocks =
		inflate_blocks_new(z, z->state->nowrap ? Z_NULL : adler32, (uInt)1 << w))
		== Z_NULL)
	{
		inflateEnd(z);
		return Z_MEM_ERROR;
	}
	LuTracev((stderr, "内部信息：已分配\n"));
	inflateReset(z);//重置 zip 专用状态
	return Z_OK;
}

int inflate(z_streamp z, int f)
{
	int r;
	uInt b;
	if (z == Z_NULL || z->state == Z_NULL || z->next_in == Z_NULL)
		return Z_STREAM_ERROR;
	f = f == Z_FINISH ? Z_BUF_ERROR : Z_OK;
	r = Z_BUF_ERROR;
	for (;;)//执行死循环
		switch (z->state->mode)
		{
		case inflate_mode::IM_METHOD:
			IM_NEEDBYTE
				if (((z->state->sub.method = IM_NEXTBYTE) & 0xf) != Z_DEFLATED)
				{
					z->state->mode = inflate_mode::IM_BAD;
					z->ErrorMessages = (char*)"未知的方法";
					z->state->sub.marker = 5;
					break;
				}
			if ((z->state->sub.method >> 4) + 8 > z->state->wbits)
			{
				z->state->mode = inflate_mode::IM_BAD;
				z->ErrorMessages = (char*)"窗体大小非法";
				z->state->sub.marker = 5;
				break;
			}
			z->state->mode = inflate_mode::IM_FLAG;
		case inflate_mode::IM_FLAG:
			IM_NEEDBYTE
				b = IM_NEXTBYTE;
			if (((z->state->sub.method << 8) + b) % 31)
			{
				z->state->mode = inflate_mode::IM_BAD;
				z->ErrorMessages = (char*)"压缩包文件头错误";
				z->state->sub.marker = 5;
				break;
			}
			LuTracev((stderr, "内部信息：压缩包文件头正常\n"));
			if (!(b & PRESET_DICT))
			{
				z->state->mode = inflate_mode::IM_BLOCKS;
				break;
			}
			z->state->mode = inflate_mode::IM_DICT4;
		case inflate_mode::IM_DICT4:
			IM_NEEDBYTE
				z->state->sub.check.need = (uLong)IM_NEXTBYTE << 24;
			z->state->mode = inflate_mode::IM_DICT3;
		case inflate_mode::IM_DICT3:
			IM_NEEDBYTE
				z->state->sub.check.need += (uLong)IM_NEXTBYTE << 16;
			z->state->mode = inflate_mode::IM_DICT2;
		case inflate_mode::IM_DICT2:
			IM_NEEDBYTE
				z->state->sub.check.need += (uLong)IM_NEXTBYTE << 8;
			z->state->mode = inflate_mode::IM_DICT1;
		case inflate_mode::IM_DICT1:
			IM_NEEDBYTE; r;
			z->state->sub.check.need += (uLong)IM_NEXTBYTE;
			z->adler = z->state->sub.check.need;
			z->state->mode = inflate_mode::IM_DICT0;
			return Z_NEED_DICT;
		case inflate_mode::IM_DICT0:
			z->state->mode = inflate_mode::IM_BAD;
			z->ErrorMessages = (char*)"需要一个目录";
			z->state->sub.marker = 0;
			return Z_STREAM_ERROR;
		case inflate_mode::IM_BLOCKS:
			r = inflate_blocks(z->state->blocks, z, r);
			if (r == Z_DATA_ERROR)
			{
				z->state->mode = inflate_mode::IM_BAD;
				z->state->sub.marker = 0;
				break;
			}
			if (r == Z_OK)
				r = f;
			if (r != Z_STREAM_END)
				return r;
			r = f;
			inflate_blocks_reset(z->state->blocks, z, &z->state->sub.check.was);
			if (z->state->nowrap)
			{
				z->state->mode = inflate_mode::IM_DONE;
				break;
			}
			z->state->mode = inflate_mode::IM_CHECK4;
		case inflate_mode::IM_CHECK4:
			IM_NEEDBYTE
				z->state->sub.check.need = (uLong)IM_NEXTBYTE << 24;
			z->state->mode = inflate_mode::IM_CHECK3;
		case inflate_mode::IM_CHECK3:
			IM_NEEDBYTE
				z->state->sub.check.need += (uLong)IM_NEXTBYTE << 16;
			z->state->mode = inflate_mode::IM_CHECK2;
		case inflate_mode::IM_CHECK2:
			IM_NEEDBYTE
				z->state->sub.check.need += (uLong)IM_NEXTBYTE << 8;
			z->state->mode = inflate_mode::IM_CHECK1;
		case inflate_mode::IM_CHECK1:
			IM_NEEDBYTE
				z->state->sub.check.need += (uLong)IM_NEXTBYTE;
			if (z->state->sub.check.was != z->state->sub.check.need)
			{
				z->state->mode = inflate_mode::IM_BAD;
				z->ErrorMessages = (char*)"数据有误";
				z->state->sub.marker = 5;
				break;
			}
			LuTracev((stderr, "内部信息：数据校对成功\n"));
			z->state->mode = inflate_mode::IM_DONE;
		case inflate_mode::IM_DONE:
			return Z_STREAM_END;
		case inflate_mode::IM_BAD:
			return Z_DATA_ERROR;
		default:
			return Z_STREAM_ERROR;
		}
}

LUFILE* lufopen(void* z, unsigned int len, DWORD flags, ZRESULT* err)
{
	if (flags != ZIP_HANDLE && flags != ZIP_FILENAME && flags != ZIP_MEMORY) { *err = ZR_ARGS; return NULL; }
	HANDLE h = 0;
	bool canseek = false, mustclosehandle = false;
	*err = ZR_OK;
	if (flags == ZIP_HANDLE || flags == ZIP_FILENAME)
	{
		if (flags == ZIP_HANDLE)
		{
			HANDLE hf = z;
			h = hf; mustclosehandle = false;
#ifdef DuplicateHandle
			BOOL res = DuplicateHandle(GetCurrentProcess(), hf, GetCurrentProcess(), &h, 0, FALSE, DUPLICATE_SAME_ACCESS);
			if (!res)
				mustclosehandle = true;
#endif
		}
		else
		{
			h = CreateFile((const TCHAR*)z, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (h == INVALID_HANDLE_VALUE)
			{
				*err = ZR_NOFILE;
				return NULL;
			}
			mustclosehandle = true;
		}
		DWORD res = SetFilePointer(h, 0, 0, FILE_CURRENT);
		canseek = (res != 0xFFFFFFFF);
	}
	LUFILE* lf = new LUFILE;
	if (flags == ZIP_HANDLE || flags == ZIP_FILENAME)
	{
		lf->is_handle = true; lf->mustclosehandle = mustclosehandle;
		lf->canseek = canseek;
		lf->h = h; lf->herr = false;
		lf->initial_offset = 0;
		if (canseek)
			lf->initial_offset = SetFilePointer(h, 0, NULL, FILE_CURRENT);
	}
	else
	{
		lf->is_handle = false;
		lf->canseek = true;
		lf->mustclosehandle = false;
		lf->buf = z; lf->len = len; lf->pos = 0; lf->initial_offset = 0;
	}
	*err = ZR_OK;
	return lf;
}

int lufclose(LUFILE* stream)
{
	if (stream == NULL)
		return EOF;
	if (stream->mustclosehandle)
		CloseHandle(stream->h);
	delete stream;
	return 0;
}

int luferror(LUFILE* stream)
{
	if (stream->is_handle && stream->herr)
		return 1;
	else
		return 0;
}

long int luftell(LUFILE* stream)
{
	if (stream->is_handle && stream->canseek)
		return SetFilePointer(stream->h, 0, NULL, FILE_CURRENT) - stream->initial_offset;
	else if (stream->is_handle)
		return 0;
	else
		return stream->pos;
}

int lufseek(LUFILE* stream, long offset, int whence)
{
	if (stream->is_handle && stream->canseek)
	{
		if (whence == SEEK_SET)
			SetFilePointer(stream->h, stream->initial_offset + offset, 0, FILE_BEGIN);
		else if (whence == SEEK_CUR)
			SetFilePointer(stream->h, offset, NULL, FILE_CURRENT);
		else if (whence == SEEK_END)
			SetFilePointer(stream->h, offset, NULL, FILE_END);
		else
			return 19;
		return 0;
	}
	else if (stream->is_handle)
		return 29;
	else
	{
		if (whence == SEEK_SET)
			stream->pos = offset;
		else if (whence == SEEK_CUR)
			stream->pos += offset;
		else if (whence == SEEK_END)
			stream->pos = stream->len + offset;
		return 0;
	}
}

size_t lufread(void* ptr, size_t size, size_t n, LUFILE* stream)
{
	unsigned int toread = (unsigned int)(size * n);
	if (stream->is_handle)
	{
		DWORD red;
		BOOL res = ReadFile(stream->h, ptr, toread, &red, NULL);
		if (!res)
			stream->herr = true;
		return red / size;
	}
	if (stream->pos + toread > stream->len)
		toread = stream->len - stream->pos;
	memcpy(ptr, (char*)stream->buf + stream->pos, toread);
	DWORD red = toread;
	stream->pos += red;
	return red / size;
}

int unzlocal_getByte(LUFILE* fin, int* pi)
{
	unsigned char c;
	int err = (int)lufread(&c, 1, 1, fin);
	if (err == 1)
	{
		*pi = (int)c;
		return UNZ_OK;
	}
	else
	{
		if (luferror(fin))
			return UNZ_ERRNO;
		else
			return UNZ_EOF;
	}
}

int unzlocal_getShort(LUFILE* fin, uLong* pX)
{
	int i;
	int err = unzlocal_getByte(fin, &i);
	uLong x = (uLong)i;
	if (err == UNZ_OK)
		err = unzlocal_getByte(fin, &i);
	x += ((uLong)i) << 8;
	if (err == UNZ_OK)
		*pX = x;
	else
		*pX = 0;
	return err;
}

int unzlocal_getLong(LUFILE* fin, uLong* pX)
{
	int i;
	int err = unzlocal_getByte(fin, &i);
	uLong x = (uLong)i;
	if (err == UNZ_OK)
		err = unzlocal_getByte(fin, &i);
	x += ((uLong)i) << 8;//逐个尝试
	if (err == UNZ_OK)
		err = unzlocal_getByte(fin, &i);
	x += ((uLong)i) << 16;
	if (err == UNZ_OK)
		err = unzlocal_getByte(fin, &i);
	x += ((uLong)i) << 24;
	*pX = (err == UNZ_OK ? x : 0);
	return err;
}

int strcmpcasenosensitive_internal(const char* fileName1, const char* fileName2)
{
	for (;;)//这种死循环比 while (true) 节省内存，对寄存器更友好
	{
		char c1 = *(fileName1++), c2 = *(fileName2++);
		if ((c1 >= 'a') && (c1 <= 'z'))
			c1 -= (char)0x20;
		if ((c2 >= 'a') && (c2 <= 'z'))
			c2 -= (char)0x20;
		if (c1 == 0)
			return ((c2 == 0) ? 0 : -1);
		if (c2 == 0)
			return 1;
		if (c1 < c2)
			return -1;
		if (c1 > c2)
			return 1;
	}
}

int unzStringFileNameCompare(const char* fileName1, const char* fileName2, int iCaseSensitivity)
{
	if (iCaseSensitivity == 1)
		return strcmp(fileName1, fileName2);
	else
		return strcmpcasenosensitive_internal(fileName1, fileName2);
}

uLong unzlocal_SearchCentralDir(LUFILE* fin)
{
	if (lufseek(fin, 0, SEEK_END) != 0)
		return 0xFFFFFFFF;
	uLong uSizeFile = luftell(fin);
	uLong uMaxBack = 0xffff;//全局注释的最大大小
	if (uMaxBack > uSizeFile)
		uMaxBack = uSizeFile;
	unsigned char* buf = (unsigned char*)zmalloc(BUFREADCOMMENT + 4);
	if (buf == NULL)
		return 0xFFFFFFFF;
	uLong uPosFound = 0xFFFFFFFF, uBackRead = 4;
	while (uBackRead < uMaxBack)
	{
		uLong uReadSize, uReadPos;
		int i;
		if (uBackRead + BUFREADCOMMENT > uMaxBack)
			uBackRead = uMaxBack;
		else
			uBackRead += BUFREADCOMMENT;
		uReadPos = uSizeFile - uBackRead;
		uReadSize = ((BUFREADCOMMENT + 4) < (uSizeFile - uReadPos)) ? (BUFREADCOMMENT + 4) : (uSizeFile - uReadPos);
		if (lufseek(fin, uReadPos, SEEK_SET) != 0)
			break;
		if (lufread(buf, (uInt)uReadSize, 1, fin) != 1)
			break;
		for (i = (int)uReadSize - 3; (i--) >= 0;)
		{
			if (((*(buf + i)) == 0x50) && ((*(buf + i + 1)) == 0x4b) && ((*(buf + i + 2)) == 0x05) && ((*(buf + i + 3)) == 0x06))
			{
				uPosFound = uReadPos + i;
				break;
			}
		}
		if (uPosFound != 0)
			break;
	}
	if (buf)
		zfree(buf);
	return
		uPosFound;
}

int unzGoToFirstFile(unzFile file);//提前声明函数
int unzCloseCurrentFile(unzFile file);
unzFile unzOpenInternal(LUFILE* fin)
{
	if (fin == NULL)
		return NULL;
	int err = UNZ_OK;
	unz_s us;
	uLong central_pos, uL;
	central_pos = unzlocal_SearchCentralDir(fin);
	if (central_pos == 0xFFFFFFFF)
		err = UNZ_ERRNO;
	if (lufseek(fin, central_pos, SEEK_SET) != 0)
		err = UNZ_ERRNO;
	if (unzlocal_getLong(fin, &uL) != UNZ_OK)//已检查过签名
		err = UNZ_ERRNO;
	uLong number_disk;//当前磁盘的数量，用于跨磁盘访问 zip，若在当前计算机上不受支持，则该值始终为0
	if (unzlocal_getShort(fin, &number_disk) != UNZ_OK)
		err = UNZ_ERRNO;
	uLong number_disk_with_CD;//以中心目录开头的磁盘数，用于扫描 zip，若在当前计算机上不受支持，则该值始终为0
	if (unzlocal_getShort(fin, &number_disk_with_CD) != UNZ_OK)
		err = UNZ_ERRNO;
	if (unzlocal_getShort(fin, &us.gi.number_entry) != UNZ_OK)//此磁盘上中心目录中的条目总数
		err = UNZ_ERRNO;
	uLong number_entry_CD;//中央目录中的条目总数
	if (unzlocal_getShort(fin, &number_entry_CD) != UNZ_OK)
		err = UNZ_ERRNO;
	if ((number_entry_CD != us.gi.number_entry) || (number_disk_with_CD != 0) || (number_disk != 0))
		err = UNZ_BADZIPFILE;
	if (unzlocal_getLong(fin, &us.size_central_dir) != UNZ_OK)//中心目录的大小
		err = UNZ_ERRNO;
	if (unzlocal_getLong(fin, &us.offset_central_dir) != UNZ_OK)//中心目录开始相对于起始磁盘号的偏移量
		err = UNZ_ERRNO;
	if (unzlocal_getShort(fin, &us.gi.size_comment) != UNZ_OK)
		err = UNZ_ERRNO;
	if ((central_pos + fin->initial_offset < us.offset_central_dir + us.size_central_dir) && (err == UNZ_OK))
		err = UNZ_BADZIPFILE;
	if (err != UNZ_OK)
	{
		lufclose(fin);
		return NULL;
	}
	us.file = fin;
	us.byte_before_the_zipfile = central_pos + fin->initial_offset - (us.offset_central_dir + us.size_central_dir);
	us.central_pos = central_pos;
	us.pfile_in_zip_read = NULL;
	fin->initial_offset = 0;
	unz_s* s = (unz_s*)zmalloc(sizeof(unz_s));// zip 文件本身需要做的处理
	if (!s)//消除编译器警告
	{
		state = 30;
		return NULL;
	}
	*s = us;
	unzGoToFirstFile((unzFile)s);
	return (unzFile)s;
}

int unzClose(unzFile file)//关闭 zip 文件
{
	unz_s* s;
	if (file == NULL)
		return UNZ_PARAMERROR;
	s = (unz_s*)file;
	if (s->pfile_in_zip_read != NULL)
		unzCloseCurrentFile(file);
	lufclose(s->file);
	if (s)
		zfree(s);//若未使用，s = NULL = 0;
	return
		UNZ_OK;
}

int unzGetGlobalInfo(unzFile file, unz_global_info* pglobal_info)
{
	unz_s* s;
	if (file == NULL)
		return UNZ_PARAMERROR;
	s = (unz_s*)file;
	*pglobal_info = s->gi;
	return UNZ_OK;
}

void unzlocal_DosDateToTmuDate(uLong ulDosDate, tm_unz* ptm)//格式转换
{
	uLong uDate;
	uDate = (uLong)(ulDosDate >> 16);
	ptm->tm_mday = (uInt)(uDate & 0x1f);
	ptm->tm_mon = (uInt)((((uDate) & 0x1E0) / 0x20) - 1);
	ptm->tm_year = (uInt)(((uDate & 0x0FE00) / 0x0200) + 1980);
	ptm->tm_hour = (uInt)((ulDosDate & 0xF800) / 0x800);
	ptm->tm_min = (uInt)((ulDosDate & 0x7E0) / 0x20);
	ptm->tm_sec = (uInt)(2 * (ulDosDate & 0x1f));
}

int unzlocal_GetCurrentFileInfoInternal(
	unzFile file, unz_file_info* pfile_info,
	unz_file_info_internal* pfile_info_internal,
	char* szFileName,
	uLong fileNameBufferSize,
	void* extraField,
	uLong extraFieldBufferSize,
	char* szComment,
	uLong commentBufferSize
)
{
	unz_s* s;
	unz_file_info file_info;
	unz_file_info_internal file_info_internal;
	int err = UNZ_OK;
	uLong uMagic;
	long lSeek = 0;
	if (file == NULL)
		return UNZ_PARAMERROR;
	s = (unz_s*)file;
	if (lufseek(s->file, s->pos_in_central_dir + s->byte_before_the_zipfile, SEEK_SET) != 0)
		err = UNZ_ERRNO;
	if (err == UNZ_OK)
	{
		if (unzlocal_getLong(s->file, &uMagic) != UNZ_OK)
			err = UNZ_ERRNO;
		else if (uMagic != 0x02014b50)
			err = UNZ_BADZIPFILE;
	}
	if (unzlocal_getShort(s->file, &file_info.version) != UNZ_OK)
		err = UNZ_ERRNO;
	if (unzlocal_getShort(s->file, &file_info.version_needed) != UNZ_OK)
		err = UNZ_ERRNO;
	if (unzlocal_getShort(s->file, &file_info.flag) != UNZ_OK)
		err = UNZ_ERRNO;
	if (unzlocal_getShort(s->file, &file_info.compression_method) != UNZ_OK)
		err = UNZ_ERRNO;
	if (unzlocal_getLong(s->file, &file_info.dosDate) != UNZ_OK)
		err = UNZ_ERRNO;
	unzlocal_DosDateToTmuDate(file_info.dosDate, &file_info.tmu_date);
	if (unzlocal_getLong(s->file, &file_info.crc) != UNZ_OK)
		err = UNZ_ERRNO;
	if (unzlocal_getLong(s->file, &file_info.compressed_size) != UNZ_OK)
		err = UNZ_ERRNO;
	if (unzlocal_getLong(s->file, &file_info.uncompressed_size) != UNZ_OK)
		err = UNZ_ERRNO;
	if (unzlocal_getShort(s->file, &file_info.size_filename) != UNZ_OK)
		err = UNZ_ERRNO;
	if (unzlocal_getShort(s->file, &file_info.size_file_extra) != UNZ_OK)
		err = UNZ_ERRNO;
	if (unzlocal_getShort(s->file, &file_info.size_file_comment) != UNZ_OK)
		err = UNZ_ERRNO;
	if (unzlocal_getShort(s->file, &file_info.disk_num_start) != UNZ_OK)
		err = UNZ_ERRNO;
	if (unzlocal_getShort(s->file, &file_info.internal_fa) != UNZ_OK)
		err = UNZ_ERRNO;
	if (unzlocal_getLong(s->file, &file_info.external_fa) != UNZ_OK)
		err = UNZ_ERRNO;
	if (unzlocal_getLong(s->file, &file_info_internal.offset_curfile) != UNZ_OK)
		err = UNZ_ERRNO;
	lSeek += file_info.size_filename;
	if ((err == UNZ_OK) && (szFileName != NULL))
	{
		uLong uSizeRead;
		if (file_info.size_filename < fileNameBufferSize)
		{
			*(szFileName + file_info.size_filename) = '\0';
			uSizeRead = file_info.size_filename;
		}
		else
			uSizeRead = fileNameBufferSize;
		if ((file_info.size_filename > 0) && (fileNameBufferSize > 0))
			if (lufread(szFileName, (uInt)uSizeRead, 1, s->file) != 1)
				err = UNZ_ERRNO;
		lSeek -= uSizeRead;
	}
	if ((err == UNZ_OK) && (extraField != NULL))
	{
		uLong uSizeRead;
		if (file_info.size_file_extra < extraFieldBufferSize)
			uSizeRead = file_info.size_file_extra;
		else
			uSizeRead = extraFieldBufferSize;
		if (lSeek != 0)
			if (lufseek(s->file, lSeek, SEEK_CUR) == 0)
				lSeek = 0;
			else
				err = UNZ_ERRNO;
		if ((file_info.size_file_extra > 0) && (extraFieldBufferSize > 0))
			if (lufread(extraField, (uInt)uSizeRead, 1, s->file) != 1)
				err = UNZ_ERRNO;
		lSeek += file_info.size_file_extra - uSizeRead;
	}
	else
		lSeek += file_info.size_file_extra;
	if ((err == UNZ_OK) && (szComment != NULL))
	{
		uLong uSizeRead;
		if (file_info.size_file_comment < commentBufferSize)
		{
			*(szComment + file_info.size_file_comment) = '\0';
			uSizeRead = file_info.size_file_comment;
		}
		else
			uSizeRead = commentBufferSize;
		if (lSeek != 0)
			if (lufseek(s->file, lSeek, SEEK_CUR) == 0)
			{
				// lSeek = 0;//如果需要用到
			}
			else
				err = UNZ_ERRNO;
		if ((file_info.size_file_comment > 0) && (commentBufferSize > 0))
			if (lufread(szComment, (uInt)uSizeRead, 1, s->file) != 1)
				err = UNZ_ERRNO;
		//lSeek += file_info.size_file_comment - uSizeRead;//如果需要用到
	}
	else
	{
		//lSeek += file_info.size_file_comment;//如果需要用到
	}
	if ((err == UNZ_OK) && (pfile_info != NULL))
		*pfile_info = file_info;
	if ((err == UNZ_OK) && (pfile_info_internal != NULL))
		*pfile_info_internal = file_info_internal;
	return err;
}

int unzGetCurrentFileInfo(
	unzFile file,
	unz_file_info* pfile_info,
	char* szFileName,
	uLong fileNameBufferSize,
	void* extraField,
	uLong extraFieldBufferSize,
	char* szComment,
	uLong commentBufferSize
)
{
	return unzlocal_GetCurrentFileInfoInternal(
		file,
		pfile_info,
		NULL,
		szFileName,
		fileNameBufferSize,
		extraField,
		extraFieldBufferSize,
		szComment,
		commentBufferSize
	);
}

int unzGoToFirstFile(unzFile file)
{
	int err;
	unz_s* s;
	if (file == NULL)
		return UNZ_PARAMERROR;
	s = (unz_s*)file;
	s->pos_in_central_dir = s->offset_central_dir;
	s->num_file = 0;
	err = unzlocal_GetCurrentFileInfoInternal(file, &s->cur_file_info, &s->cur_file_info_internal, NULL, 0, NULL, 0, NULL, 0);
	s->current_file_ok = (err == UNZ_OK);
	return err;
}

int unzGoToNextFile(unzFile file)
{
	unz_s* s;
	int err;
	if (file == NULL)
		return UNZ_PARAMERROR;
	s = (unz_s*)file;
	if (!s->current_file_ok)
		return UNZ_END_OF_LIST_OF_FILE;
	if (s->num_file + 1 == s->gi.number_entry)
		return UNZ_END_OF_LIST_OF_FILE;
	s->pos_in_central_dir += SIZECENTRALDIRITEM + s->cur_file_info.size_filename +
		s->cur_file_info.size_file_extra + s->cur_file_info.size_file_comment;
	s->num_file++;
	err = unzlocal_GetCurrentFileInfoInternal(file, &s->cur_file_info,
		&s->cur_file_info_internal,
		NULL, 0, NULL, 0, NULL, 0);
	s->current_file_ok = (err == UNZ_OK);
	return err;
}

int unzLocateFile(unzFile file, const char* szFileName, int iCaseSensitivity)
{
	unz_s* s;
	int err;
	uLong num_fileSaved, pos_in_central_dirSaved;
	if (file == NULL)
		return UNZ_PARAMERROR;
	if (strlen(szFileName) >= UNZ_MAXFILENAMEINZIP)
		return UNZ_PARAMERROR;
	s = (unz_s*)file;
	if (!s->current_file_ok)
		return UNZ_END_OF_LIST_OF_FILE;
	num_fileSaved = s->num_file;
	pos_in_central_dirSaved = s->pos_in_central_dir;
	err = unzGoToFirstFile(file);
	while (err == UNZ_OK)
	{
		char szCurrentFileName[UNZ_MAXFILENAMEINZIP + 1];
		unzGetCurrentFileInfo(file, NULL, szCurrentFileName, sizeof(szCurrentFileName) - 1, NULL, 0, NULL, 0);
		if (unzStringFileNameCompare(szCurrentFileName, szFileName, iCaseSensitivity) == 0)
			return UNZ_OK;
		err = unzGoToNextFile(file);
	}
	s->num_file = num_fileSaved;
	s->pos_in_central_dir = pos_in_central_dirSaved;
	return err;
}

int unzlocal_CheckCurrentFileCoherencyHeader(
	unz_s* s,
	uInt* piSizeVar,
	uLong* poffset_local_extrafield,
	uInt* psize_local_extrafield
)
{
	uLong uMagic, uData, uFlags, size_filename, size_extra_field;
	int err = UNZ_OK;
	*piSizeVar = 0;
	*poffset_local_extrafield = 0;
	*psize_local_extrafield = 0;
	if (lufseek(s->file, s->cur_file_info_internal.offset_curfile + s->byte_before_the_zipfile, SEEK_SET) != 0)
		return UNZ_ERRNO;
	if (err == UNZ_OK)
	{
		if (unzlocal_getLong(s->file, &uMagic) != UNZ_OK)
			err = UNZ_ERRNO;
		else if (uMagic != 0x04034b50)
			err = UNZ_BADZIPFILE;
	}
	if (unzlocal_getShort(s->file, &uData) != UNZ_OK)
		err = UNZ_ERRNO;
	/*else if ((err == UNZ_OK) && (!EQ(uData, (uLong)s->cur_file_info.version)))
		err = UNZ_BADZIPFILE;//如果需要用到*/
	if (unzlocal_getShort(s->file, &uFlags) != UNZ_OK)
		err = UNZ_ERRNO;
	if (unzlocal_getShort(s->file, &uData) != UNZ_OK)
		err = UNZ_ERRNO;
	else if ((err == UNZ_OK) && (uData != s->cur_file_info.compression_method))
		err = UNZ_BADZIPFILE;
	if ((err == UNZ_OK) && (s->cur_file_info.compression_method != 0) && (s->cur_file_info.compression_method != Z_DEFLATED))
		err = UNZ_BADZIPFILE;
	if (unzlocal_getLong(s->file, &uData) != UNZ_OK)// date/time
		err = UNZ_ERRNO;
	if (unzlocal_getLong(s->file, &uData) != UNZ_OK)// crc
		err = UNZ_ERRNO;
	else if ((err == UNZ_OK) && (uData != s->cur_file_info.crc) &&
		((uFlags & 8) == 0))
		err = UNZ_BADZIPFILE;
	if (unzlocal_getLong(s->file, &uData) != UNZ_OK)// size compr
		err = UNZ_ERRNO;
	else if ((err == UNZ_OK) && (uData != s->cur_file_info.compressed_size) &&
		((uFlags & 8) == 0))
		err = UNZ_BADZIPFILE;
	if (unzlocal_getLong(s->file, &uData) != UNZ_OK)// size uncompr
		err = UNZ_ERRNO;
	else if ((err == UNZ_OK) && (uData != s->cur_file_info.uncompressed_size) && ((uFlags & 8) == 0))
		err = UNZ_BADZIPFILE;
	if (unzlocal_getShort(s->file, &size_filename) != UNZ_OK)
		err = UNZ_ERRNO;
	else if ((err == UNZ_OK) && (size_filename != s->cur_file_info.size_filename))
		err = UNZ_BADZIPFILE;
	*piSizeVar += (uInt)size_filename;
	if (unzlocal_getShort(s->file, &size_extra_field) != UNZ_OK)
		err = UNZ_ERRNO;
	*poffset_local_extrafield = s->cur_file_info_internal.offset_curfile +
		SIZEZIPLOCALHEADER + size_filename;
	*psize_local_extrafield = (uInt)size_extra_field;
	*piSizeVar += (uInt)size_extra_field;
	return err;
}

int unzOpenCurrentFile(unzFile file, const char* password)
{
	int err;
	int Store;
	uInt iSizeVar;
	unz_s* s;
	file_in_zip_read_info_s* pfile_in_zip_read_info;
	uLong offset_local_extrafield;//本地额外字段的偏移量
	uInt  size_local_extrafield;
	if (file == NULL)
		return UNZ_PARAMERROR;
	s = (unz_s*)file;
	if (!s->current_file_ok)
		return UNZ_PARAMERROR;
	if (s->pfile_in_zip_read != NULL)
		unzCloseCurrentFile(file);
	if (unzlocal_CheckCurrentFileCoherencyHeader(s, &iSizeVar,
		&offset_local_extrafield, &size_local_extrafield) != UNZ_OK)
		return UNZ_BADZIPFILE;
	pfile_in_zip_read_info = (file_in_zip_read_info_s*)zmalloc(sizeof(file_in_zip_read_info_s));
	if (pfile_in_zip_read_info == NULL)
		return UNZ_INTERNALERROR;
	pfile_in_zip_read_info->read_buffer = (char*)zmalloc(UNZ_BUFSIZE);
	pfile_in_zip_read_info->offset_local_extrafield = offset_local_extrafield;
	pfile_in_zip_read_info->size_local_extrafield = size_local_extrafield;
	pfile_in_zip_read_info->pos_local_extrafield = 0;
	if (pfile_in_zip_read_info->read_buffer == NULL)
	{
		if (pfile_in_zip_read_info != 0)
			zfree(pfile_in_zip_read_info);
		//pfile_in_zip_read_info = 0;//如果需要用到
		return UNZ_INTERNALERROR;
	}
	pfile_in_zip_read_info->stream_initialised = 0;
	if ((s->cur_file_info.compression_method != 0) && (s->cur_file_info.compression_method != Z_DEFLATED))
	{
		//err = UNZ_BADZIPFILE;//如果需要用到
	}
	Store = s->cur_file_info.compression_method == 0;
	pfile_in_zip_read_info->crc32_wait = s->cur_file_info.crc;
	pfile_in_zip_read_info->crc32 = 0;
	pfile_in_zip_read_info->compression_method = s->cur_file_info.compression_method;
	pfile_in_zip_read_info->file = s->file;
	pfile_in_zip_read_info->byte_before_the_zipfile = s->byte_before_the_zipfile;
	pfile_in_zip_read_info->stream.total_out = 0;
	if (!Store)
	{
		pfile_in_zip_read_info->stream.zalloc = (alloc_func)0;
		pfile_in_zip_read_info->stream.zfree = (free_func)0;
		pfile_in_zip_read_info->stream.opaque = (voidpf)0;
		err = inflateInit2(&pfile_in_zip_read_info->stream);
		if (err == Z_OK)
			pfile_in_zip_read_info->stream_initialised = 1;
	}
	pfile_in_zip_read_info->rest_read_compressed = s->cur_file_info.compressed_size;
	pfile_in_zip_read_info->rest_read_uncompressed = s->cur_file_info.uncompressed_size;
	pfile_in_zip_read_info->encrypted = (s->cur_file_info.flag & 1) != 0;
	bool extlochead = (s->cur_file_info.flag & 8) != 0;
	if (extlochead)
		pfile_in_zip_read_info->crcenctest = (char)((s->cur_file_info.dosDate >> 8) & 0xff);
	else
		pfile_in_zip_read_info->crcenctest = (char)(s->cur_file_info.crc >> 24);
	pfile_in_zip_read_info->encheadleft = (pfile_in_zip_read_info->encrypted ? 12 : 0);
	pfile_in_zip_read_info->keys[0] = 305419896L;
	pfile_in_zip_read_info->keys[1] = 591751049L;
	pfile_in_zip_read_info->keys[2] = 878082192L;
	for (const char* cp = password; cp != 0 && *cp != 0; ++cp)
		UUpdateKeys(pfile_in_zip_read_info->keys, *cp);
	pfile_in_zip_read_info->pos_in_zipfile =
		s->cur_file_info_internal.offset_curfile + SIZEZIPLOCALHEADER +
		iSizeVar;
	pfile_in_zip_read_info->stream.avail_in = (uInt)0;
	s->pfile_in_zip_read = pfile_in_zip_read_info;
	return UNZ_OK;
}

int unzReadCurrentFile(unzFile file, voidp buf, unsigned len, bool* reached_eof)
{
	int err = UNZ_OK;
	uInt iRead = 0;
	if (reached_eof != 0) *reached_eof = false;
	unz_s* s = (unz_s*)file;
	if (s == NULL)
		return UNZ_PARAMERROR;
	file_in_zip_read_info_s* pfile_in_zip_read_info = s->pfile_in_zip_read;
	if (pfile_in_zip_read_info == NULL)
		return UNZ_PARAMERROR;
	if ((pfile_in_zip_read_info->read_buffer == NULL))
		return UNZ_END_OF_LIST_OF_FILE;
	if (len == 0)
		return 0;
	pfile_in_zip_read_info->stream.next_out = (Byte*)buf;
	pfile_in_zip_read_info->stream.avail_out = (uInt)len;
	if (len > pfile_in_zip_read_info->rest_read_uncompressed)
		pfile_in_zip_read_info->stream.avail_out = (uInt)pfile_in_zip_read_info->rest_read_uncompressed;
	while (pfile_in_zip_read_info->stream.avail_out > 0)
	{
		if ((pfile_in_zip_read_info->stream.avail_in == 0) && (pfile_in_zip_read_info->rest_read_compressed > 0))
		{
			uInt uReadThis = UNZ_BUFSIZE;
			if (pfile_in_zip_read_info->rest_read_compressed < uReadThis)
				uReadThis = (uInt)pfile_in_zip_read_info->rest_read_compressed;
			if (uReadThis == 0)
			{
				if (reached_eof != 0)
					*reached_eof = true;
				return UNZ_EOF;
			}
			if (lufseek(pfile_in_zip_read_info->file, pfile_in_zip_read_info->pos_in_zipfile + pfile_in_zip_read_info->byte_before_the_zipfile, SEEK_SET) != 0)
				return UNZ_ERRNO;
			if (lufread(pfile_in_zip_read_info->read_buffer, uReadThis, 1, pfile_in_zip_read_info->file) != 1)
				return UNZ_ERRNO;
			pfile_in_zip_read_info->pos_in_zipfile += uReadThis;
			pfile_in_zip_read_info->rest_read_compressed -= uReadThis;
			pfile_in_zip_read_info->stream.next_in = (Byte*)pfile_in_zip_read_info->read_buffer;
			pfile_in_zip_read_info->stream.avail_in = (uInt)uReadThis;
			if (pfile_in_zip_read_info->encrypted)
			{
				char* buf = (char*)pfile_in_zip_read_info->stream.next_in;
				for (unsigned int i = 0; i < uReadThis; ++i)
					buf[i] = zdecode(pfile_in_zip_read_info->keys, buf[i]);
			}
		}
		unsigned int uDoEncHead = pfile_in_zip_read_info->encheadleft;
		if (uDoEncHead > pfile_in_zip_read_info->stream.avail_in)
			uDoEncHead = pfile_in_zip_read_info->stream.avail_in;
		if (uDoEncHead > 0)
		{
			char bufcrc = pfile_in_zip_read_info->stream.next_in[uDoEncHead - 1];
			pfile_in_zip_read_info->rest_read_uncompressed -= uDoEncHead;
			pfile_in_zip_read_info->stream.avail_in -= uDoEncHead;
			pfile_in_zip_read_info->stream.next_in += uDoEncHead;
			pfile_in_zip_read_info->encheadleft -= uDoEncHead;
			if (pfile_in_zip_read_info->encheadleft == 0)
			{
				if (bufcrc != pfile_in_zip_read_info->crcenctest)
					return UNZ_PASSWORD;
			}
		}
		if (pfile_in_zip_read_info->compression_method == 0)
		{
			uInt uDoCopy, i;
			if (pfile_in_zip_read_info->stream.avail_out < pfile_in_zip_read_info->stream.avail_in)
				uDoCopy = pfile_in_zip_read_info->stream.avail_out;
			else
				uDoCopy = pfile_in_zip_read_info->stream.avail_in;
			for (i = 0; i < uDoCopy; ++i)//运行时 ++i 要比 i++ 快一点点
				*(pfile_in_zip_read_info->stream.next_out + i) = *(pfile_in_zip_read_info->stream.next_in + i);
			pfile_in_zip_read_info->crc32 = ucrc32(pfile_in_zip_read_info->crc32, pfile_in_zip_read_info->stream.next_out, uDoCopy);
			pfile_in_zip_read_info->rest_read_uncompressed -= uDoCopy;
			pfile_in_zip_read_info->stream.avail_in -= uDoCopy;
			pfile_in_zip_read_info->stream.avail_out -= uDoCopy;
			pfile_in_zip_read_info->stream.next_out += uDoCopy;
			pfile_in_zip_read_info->stream.next_in += uDoCopy;
			pfile_in_zip_read_info->stream.total_out += uDoCopy;
			iRead += uDoCopy;
			if (pfile_in_zip_read_info->rest_read_uncompressed == 0)
			{
				if (reached_eof != 0)
					*reached_eof = true;
			}
		}
		else
		{
			uLong uTotalOutBefore, uTotalOutAfter, uOutThis;
			const Byte* bufBefore;
			int flush = Z_SYNC_FLUSH;
			uTotalOutBefore = pfile_in_zip_read_info->stream.total_out;
			bufBefore = pfile_in_zip_read_info->stream.next_out;
			err = inflate(&pfile_in_zip_read_info->stream, flush);
			uTotalOutAfter = pfile_in_zip_read_info->stream.total_out;
			uOutThis = uTotalOutAfter - uTotalOutBefore;
			pfile_in_zip_read_info->crc32 = ucrc32(pfile_in_zip_read_info->crc32, bufBefore, (uInt)(uOutThis));
			pfile_in_zip_read_info->rest_read_uncompressed -= uOutThis;
			iRead += (uInt)(uTotalOutAfter - uTotalOutBefore);
			if (err == Z_STREAM_END || pfile_in_zip_read_info->rest_read_uncompressed == 0)
			{
				if (reached_eof != 0)
					*reached_eof = true;
				return iRead;
			}
			if (err != Z_OK)
				break;
		}
	}
	if (err == Z_OK)
		return iRead;
	return err;
}

z_off_t unztell(unzFile file)
{
	unz_s* s;
	file_in_zip_read_info_s* pfile_in_zip_read_info;
	if (file == NULL)
		return UNZ_PARAMERROR;
	s = (unz_s*)file;
	pfile_in_zip_read_info = s->pfile_in_zip_read;
	if (pfile_in_zip_read_info == NULL)
		return UNZ_PARAMERROR;
	return (z_off_t)pfile_in_zip_read_info->stream.total_out;
}

int unzeof(unzFile file)
{
	unz_s* s;
	file_in_zip_read_info_s* pfile_in_zip_read_info;
	if (file == NULL)
		return UNZ_PARAMERROR;
	s = (unz_s*)file;
	pfile_in_zip_read_info = s->pfile_in_zip_read;
	if (pfile_in_zip_read_info == NULL)
		return UNZ_PARAMERROR;
	if (pfile_in_zip_read_info->rest_read_uncompressed == 0)
		return 1;
	else
		return 0;
}

int unzGetLocalExtrafield(unzFile file, voidp buf, unsigned len)
{
	unz_s* s;
	file_in_zip_read_info_s* pfile_in_zip_read_info;
	uInt read_now;
	uLong size_to_read;
	if (file == NULL)
		return UNZ_PARAMERROR;
	s = (unz_s*)file;
	pfile_in_zip_read_info = s->pfile_in_zip_read;
	if (pfile_in_zip_read_info == NULL)
		return UNZ_PARAMERROR;
	size_to_read = (pfile_in_zip_read_info->size_local_extrafield -
		pfile_in_zip_read_info->pos_local_extrafield);
	if (buf == NULL)
		return (int)size_to_read;
	if (len > size_to_read)
		read_now = (uInt)size_to_read;
	else
		read_now = (uInt)len;
	if (read_now == 0)
		return 0;
	if (lufseek(pfile_in_zip_read_info->file,
		pfile_in_zip_read_info->offset_local_extrafield + pfile_in_zip_read_info->pos_local_extrafield, SEEK_SET) != 0)
		return UNZ_ERRNO;
	if (lufread(buf, (uInt)size_to_read, 1, pfile_in_zip_read_info->file) != 1)
		return UNZ_ERRNO;
	return (int)read_now;
}

int unzCloseCurrentFile(unzFile file)//关闭当前已解压文件
{
	int err = UNZ_OK;
	unz_s* s;
	file_in_zip_read_info_s* pfile_in_zip_read_info;
	if (file == NULL)
		return UNZ_PARAMERROR;
	s = (unz_s*)file;
	pfile_in_zip_read_info = s->pfile_in_zip_read;
	if (pfile_in_zip_read_info == NULL)
		return UNZ_PARAMERROR;
	if (pfile_in_zip_read_info->rest_read_uncompressed == 0)
	{
		if (pfile_in_zip_read_info->crc32 != pfile_in_zip_read_info->crc32_wait)
			err = UNZ_CRCERROR;
	}
	if (pfile_in_zip_read_info->read_buffer != 0)
	{
		void* buf = pfile_in_zip_read_info->read_buffer;
		zfree(buf);
		pfile_in_zip_read_info->read_buffer = 0;
	}
	pfile_in_zip_read_info->read_buffer = NULL;
	if (pfile_in_zip_read_info->stream_initialised)
		inflateEnd(&pfile_in_zip_read_info->stream);
	pfile_in_zip_read_info->stream_initialised = 0;
	if (pfile_in_zip_read_info != 0)
		zfree(pfile_in_zip_read_info);
	//pfile_in_zip_read_info = 0;//如果需要用到
	s->pfile_in_zip_read = NULL;
	return err;
}

int unzGetGlobalComment(unzFile file, char* szComment, uLong uSizeBuf)
{
	//int err = UNZ_OK;//如果需要抛出用户提示
	unz_s* s;
	uLong uReadThis;
	if (file == NULL)
		return UNZ_PARAMERROR;
	s = (unz_s*)file;
	uReadThis = uSizeBuf;
	if (uReadThis > s->gi.size_comment)
		uReadThis = s->gi.size_comment;
	if (lufseek(s->file, s->central_pos + 22, SEEK_SET) != 0)
		return UNZ_ERRNO;
	if (uReadThis > 0)
	{
		*szComment = '\0';
		if (lufread(szComment, (uInt)uReadThis, 1, s->file) != 1)
			return UNZ_ERRNO;
	}
	if ((szComment != NULL) && (uSizeBuf > s->gi.size_comment))
		*(szComment + s->gi.size_comment) = '\0';
	return (int)uReadThis;
}

FILETIME time_t2Filetime(const lutime_t t)
{
	LONGLONG i = Int32x32To64(t, 10000000) + 116444736000000000;
	FILETIME ft;
	ft.dwLowDateTime = (DWORD)i;
	ft.dwHighDateTime = (DWORD)(i >> 32);
	return ft;
}

FILETIME DOSDateTime2Filetime(WORD dosdate, WORD dostime)
{
	SYSTEMTIME st;
	st.wYear = (WORD)(((dosdate >> 9) & 0x7f) + 1980);
	st.wMonth = (WORD)((dosdate >> 5) & 0xf);
	st.wDay = (WORD)(dosdate & 0x1f);
	st.wHour = (WORD)((dostime >> 11) & 0x1f);
	st.wMinute = (WORD)((dostime >> 5) & 0x3f);
	st.wSecond = (WORD)((dostime & 0x1f) * 2);
	st.wMilliseconds = 0;
	FILETIME ft; SystemTimeToFileTime(&st, &ft);
	return ft;
}

void EnsureDirectory(const TCHAR* rootdir, const TCHAR* dir)//确认目录
{
	if (rootdir != 0 && GetFileAttributes(rootdir) == 0xFFFFFFFF)
		CreateDirectory(rootdir, 0);
	if (*dir == 0)
		return;
	const TCHAR* lastslash = dir, * c = lastslash;
	while (*c != 0)
	{
		if (*c == '/' || *c == '\\')
			lastslash = c;
		c++;
	}
	const TCHAR* name = lastslash;
	if (lastslash != dir)
	{
		TCHAR tmp[Small];
		memcpy(tmp, dir, sizeof(TCHAR) * (lastslash - dir));
		tmp[lastslash - dir] = 0;
		EnsureDirectory(rootdir, tmp);
		name++;
	}
	TCHAR cd[Small];
	*cd = 0;
	if (rootdir != 0)
		wcscpy_s(cd, rootdir);
	wcscat_s(cd, dir);
	if (GetFileAttributes(cd) == 0xFFFFFFFF)
		CreateDirectory(cd, NULL);
}


/* 解压 zip 类函数 */
class TUnzip
{
public:
	TUnzip(const char* pwd) : uf(0), unzbuf(0), currentfile(-1), czei(-1), password(NULL)
	{
		if (pwd != 0)
		{
			password = new char[strlen(pwd) + 4];
			strcpy_s(password, strlen(pwd) + 4, pwd);//复制长度以规避 _WIN64 警告
		}
	}
	~TUnzip()
	{
		if (password != 0)
			delete[] password;
		password = 0;
		if (unzbuf != 0)
			delete[] unzbuf;
		unzbuf = 0;
	}
	char* password, * unzbuf;
	int currentfile, czei;
	unzFile uf;
	ZIPENTRY cze;
	TCHAR rootdir[Small] = { 0 };
	ZRESULT Open(void* z, unsigned int len, DWORD flags);//打开
	ZRESULT Get(int index, ZIPENTRY* ze);//获取
	ZRESULT Find(const TCHAR* name, bool ic, int* index, ZIPENTRY* ze);//查找
	ZRESULT Unzip(int index, void* dst, unsigned int len, DWORD flags);//解压 zip
	ZRESULT SetUnzipBaseDir(const TCHAR* dir);//设置解压目录
	ZRESULT Close();//关闭 zip 文件
};

typedef struct
{
	DWORD flag;
	TUnzip* unz;
} TUnzipHandleData;

ZRESULT TUnzip::Open(void* z, unsigned int len, DWORD flags)
{
	if (uf != 0 || currentfile != -1)
		return ZR_NOTINITED;
	if (lstrlen(_0) > 1)
	{
		lstrcpy(rootdir, _0);
		for (int i = lstrlen(rootdir) - 1; rootdir[i] != '\\'; --i)
			rootdir[i] = 0;
	}
	else
	{
#ifdef GetCurrentDirectory//尝试获取绝对路径
		GetCurrentDirectory(Small, rootdir);
#else//获取失败则使用相对路径
		_tcscpy(rootdir, _T("\\"));
#endif
	}
	if (rootdir[_tcslen(rootdir) - 1] != '\\' && rootdir[_tcslen(rootdir) - 1] != '/')//判断路径是否为目录形式
		wcscat_s(rootdir, _T("\\"));
	wcscat_s(rootdir, _T("PC Hunter Standard\\"));
	if (flags == ZIP_HANDLE)
	{
		DWORD res = SetFilePointer(z, 0, 0, FILE_CURRENT);
		bool canseek = (res != 0xFFFFFFFF);
		if (!canseek)
			return ZR_SEEK;
	}
	ZRESULT e;
	LUFILE* f = lufopen(z, len, flags, &e);
	if (f == NULL)
		return e;
	uf = unzOpenInternal(f);
	if (uf == 0)
		return ZR_NOFILE;
	return ZR_OK;
}

ZRESULT TUnzip::SetUnzipBaseDir(const TCHAR* dir)
{
	wcscpy_s(rootdir, dir);
	TCHAR lastchar = rootdir[_tcslen(rootdir) - 1];
	if (lastchar != '\\' && lastchar != '/')
		wcscat_s(rootdir, _T("\\"));
	return ZR_OK;
}

ZRESULT TUnzip::Get(int index, ZIPENTRY* ze)
{
	if (index < -1 || index >= (int)uf->gi.number_entry)
		return ZR_ARGS;
	if (currentfile != -1)
		unzCloseCurrentFile(uf);
	currentfile = -1;
	if (index == czei && index != -1)
	{
		memcpy(ze, &cze, sizeof(ZIPENTRY));
		return ZR_OK;
	}
	if (index == -1)
	{
		ze->index = uf->gi.number_entry;
		ze->name[0] = 0;
		ze->attr = 0;
		ze->atime.dwLowDateTime = 0; ze->atime.dwHighDateTime = 0;
		ze->ctime.dwLowDateTime = 0; ze->ctime.dwHighDateTime = 0;
		ze->mtime.dwLowDateTime = 0; ze->mtime.dwHighDateTime = 0;
		ze->comp_size = 0;
		ze->unc_size = 0;
		return ZR_OK;
	}
	if (index < (int)uf->num_file)
		unzGoToFirstFile(uf);
	while ((int)uf->num_file < index)
		unzGoToNextFile(uf);
	unz_file_info ufi;
	char fn[Small];
	unzGetCurrentFileInfo(uf, &ufi, fn, Small, NULL, 0, NULL, 0);
	unsigned int extralen, iSizeVar;
	unsigned long offset;
	int res = unzlocal_CheckCurrentFileCoherencyHeader(uf, &iSizeVar, &offset, &extralen);
	if (res != UNZ_OK)
		return ZR_CORRUPT;
	if (lufseek(uf->file, offset, SEEK_SET) != 0)
		return ZR_READ;
	unsigned char* extra = new unsigned char[extralen];
	if (lufread(extra, 1, (uInt)extralen, uf->file) != extralen)
	{
		delete[] extra;
		return ZR_READ;
	}
	ze->index = uf->num_file;
	TCHAR tfn[Small] = { 0 };
#ifdef UNICODE
	MultiByteToWideChar(CP_UTF8, 0, fn, -1, tfn, Small);
#else
	lstrcpy(tfn, CharToLpcwstr(fn));
#endif
	const TCHAR* sfn = tfn;
	for (;;)//排除不规范路径造成的恶性干扰
	{
		if (sfn[0] != 0 && sfn[1] == ':')
		{
			sfn += 2;
			continue;
		}
		if (sfn[0] == '\\')
		{
			sfn++;
			continue;
		}
		if (sfn[0] == '/')
		{
			sfn++;
			continue;
		}
		const TCHAR* c;
		c = _tcsstr(sfn, _T("\\..\\"));
		if (c != 0)
		{
			sfn = c + 4;
			continue;
		}
		c = _tcsstr(sfn, _T("\\../"));
		if (c != 0)
		{
			sfn = c + 4;
			continue;
		}
		c = _tcsstr(sfn, _T("/../"));
		if (c != 0)
		{
			sfn = c + 4;
			continue;
		}
		c = _tcsstr(sfn, _T("/..\\"));
		if (c != 0)
		{
			sfn = c + 4;
			continue;
		}
		break;
	}
	wcscpy_s(ze->name, sfn);
	unsigned long a = ufi.external_fa;
	bool isdir = (a & 0x40000000) != 0;
	bool readonly = (a & 0x00800000) == 0;
	//bool readable = (a & 0x01000000) != 0;//如果需要用到
	//bool executable = (a & 0x00400000) != 0;//如果需要用到
	bool hidden = false, system = false, archive = true;
	int host = ufi.version >> 8;
	if (host == 0 || host == 7 || host == 11 || host == 14)
	{
		readonly = (a & 0x00000001) != 0;
		hidden = (a & 0x00000002) != 0;
		system = (a & 0x00000004) != 0;
		isdir = (a & 0x00000010) != 0;
		archive = (a & 0x00000020) != 0;
	}
	ze->attr = 0;
	if (isdir)
		ze->attr |= FILE_ATTRIBUTE_DIRECTORY;
	if (archive)
		ze->attr |= FILE_ATTRIBUTE_ARCHIVE;
	if (hidden)
		ze->attr |= FILE_ATTRIBUTE_HIDDEN;
	if (readonly)
		ze->attr |= FILE_ATTRIBUTE_READONLY;
	if (system)
		ze->attr |= FILE_ATTRIBUTE_SYSTEM;
	ze->comp_size = ufi.compressed_size;
	ze->unc_size = ufi.uncompressed_size;
	WORD dostime = (WORD)(ufi.dosDate & 0xFFFF);
	WORD dosdate = (WORD)((ufi.dosDate >> 16) & 0xFFFF);
	FILETIME ftd = DOSDateTime2Filetime(dosdate, dostime);
	FILETIME ft;
	LocalFileTimeToFileTime(&ftd, &ft);
	ze->atime = ft;
	ze->ctime = ft;
	ze->mtime = ft;
	unsigned int epos = 0;
	while (epos + 4 < extralen)
	{
		char etype[3];
		etype[0] = extra[epos + 0];
		etype[1] = extra[epos + 1];
		etype[2] = 0;
		int size = extra[epos + 2];
		if (strcmp(etype, "UT") != 0)
		{
			epos += 4 + size;
			continue;
		}
		int flags = extra[epos + 4];
		bool hasmtime = (flags & 1) != 0;
		bool hasatime = (flags & 2) != 0;
		bool hasctime = (flags & 4) != 0;
		epos += 5;
		if (hasmtime)
		{
			lutime_t mtime = ((extra[epos + 0]) << 0) | ((extra[epos + 1]) << 8) | ((extra[epos + 2]) << 16) | ((extra[epos + 3]) << 24);
			epos += 4;
			ze->mtime = time_t2Filetime(mtime);
		}
		if (hasatime)
		{
			lutime_t atime = ((extra[epos + 0]) << 0) | ((extra[epos + 1]) << 8) | ((extra[epos + 2]) << 16) | ((extra[epos + 3]) << 24);
			epos += 4;
			ze->atime = time_t2Filetime(atime);
		}
		if (hasctime)
		{
			lutime_t ctime = ((extra[epos + 0]) << 0) | ((extra[epos + 1]) << 8) | ((extra[epos + 2]) << 16) | ((extra[epos + 3]) << 24);
			epos += 4;
			ze->ctime = time_t2Filetime(ctime);
		}
		break;
	}
	if (extra != 0)
		delete[] extra;
	memcpy(&cze, ze, sizeof(ZIPENTRY));
	czei = index;
	return ZR_OK;
}

ZRESULT TUnzip::Find(const TCHAR* tname, bool ic, int* index, ZIPENTRY* ze)
{
	char name[Small];
#ifdef UNICODE
	WideCharToMultiByte(CP_UTF8, 0, tname, -1, name, Small, 0, 0);
#else
	TCHAR ttname[Small] = { 0 };
	lstrcpy(ttname, tname);
	TcharToChar(ttname, name);
#endif
	int res = unzLocateFile(uf, name, ic ? CASE_INSENSITIVE : CASE_SENSITIVE);
	if (res != UNZ_OK)
	{
		if (index != 0)
			*index = -1;
		if (ze != NULL)
		{
			ZeroMemory(ze, sizeof(ZIPENTRY));
			ze->index = -1;
		}
		return ZR_NOTFOUND;
	}
	if (currentfile != -1)
		unzCloseCurrentFile(uf);
	currentfile = -1;
	int i = (int)uf->num_file;
	if (index != NULL)
		*index = i;
	if (ze != NULL)
	{
		ZRESULT zres = Get(i, ze);
		if (zres != ZR_OK)
			return zres;
	}
	return ZR_OK;
}

ZRESULT TUnzip::Unzip(int index, void* dst, unsigned int len, DWORD flags)
{
	if (flags != ZIP_MEMORY && flags != ZIP_FILENAME && flags != ZIP_HANDLE)
		return ZR_ARGS;
	if (flags == ZIP_MEMORY)
	{
		if (index != currentfile)
		{
			if (currentfile != -1)
				unzCloseCurrentFile(uf);
			currentfile = -1;
			if (index >= (int)uf->gi.number_entry)
				return ZR_ARGS;
			if (index < (int)uf->num_file)
				unzGoToFirstFile(uf);
			while ((int)uf->num_file < index)
				unzGoToNextFile(uf);
			unzOpenCurrentFile(uf, password);
			currentfile = index;
		}
		bool reached_eof;
		int res = unzReadCurrentFile(uf, dst, len, &reached_eof);
		if (res <= 0)
		{
			unzCloseCurrentFile(uf);
			currentfile = -1;
		}
		if (reached_eof)
			return ZR_OK;
		if (res > 0)
			return ZR_MORE;
		if (res == UNZ_PASSWORD)
			return ZR_PASSWORD;
		return ZR_FLATE;
	}
	if (currentfile != -1)//否则我们会写一个句柄或文件
		unzCloseCurrentFile(uf);
	currentfile = -1;
	if (index >= (int)uf->gi.number_entry)
		return ZR_ARGS;
	if (index < (int)uf->num_file)
		unzGoToFirstFile(uf);
	while ((int)uf->num_file < index)
		unzGoToNextFile(uf);
	ZIPENTRY ze;
	Get(index, &ze);
	if ((ze.attr & FILE_ATTRIBUTE_DIRECTORY) != 0)
	{
		if (flags == ZIP_HANDLE)
			return ZR_OK;
		const TCHAR* dir = (const TCHAR*)dst;
		bool isabsolute = (dir[0] == '/' || dir[0] == '\\' || (dir[0] != 0 && dir[1] == ':'));
		if (isabsolute)
			EnsureDirectory(0, dir);
		else
			EnsureDirectory(rootdir, dir);
		return ZR_OK;
	}
	HANDLE h;
	if (flags == ZIP_HANDLE)
		h = dst;
	else
	{
		const TCHAR* ufn = (const TCHAR*)dst;//处理不规范的路径
		const TCHAR* name = ufn;
		const TCHAR* c = name;
		while (*c != 0)
		{
			if (*c == '/' || *c == '\\')
				name = c + 1;
			c++;
		}
		TCHAR dir[Small];
		wcscpy_s(dir, ufn);
		if (name == ufn)
			*dir = 0;
		else
			dir[name - ufn] = 0;
		TCHAR fn[Small];
		bool isabsolute = (dir[0] == '/' || dir[0] == '\\' || (dir[0] != 0 && dir[1] == ':'));
		if (isabsolute)
		{
			wsprintf(fn, _T("%s%s"), dir, name);
			EnsureDirectory(0, dir);
		}
		else
		{
			wsprintf(fn, _T("%s%s%s"), rootdir, dir, name);
			EnsureDirectory(rootdir, dir);
		}
		h = CreateFile(fn, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, ze.attr, NULL);
	}
	if (h == INVALID_HANDLE_VALUE)
		return ZR_NOFILE;
	unzOpenCurrentFile(uf, password);
	if (unzbuf == 0)
		unzbuf = new char[16384];
	DWORD haderr = 0;
	while (haderr == 0)
	{
		bool reached_eof;
		int res = unzReadCurrentFile(uf, unzbuf, 16384, &reached_eof);
		if (res == UNZ_PASSWORD)
		{
			haderr = ZR_PASSWORD;
			break;
		}
		if (res < 0)
		{
			haderr = ZR_FLATE;
			break;
		}
		if (res > 0)
		{
			DWORD writ;
			BOOL bres = WriteFile(h, unzbuf, res, &writ, NULL);
			if (!bres)
			{
				haderr = ZR_WRITE;
				break;
			}
		}
		if (reached_eof)
			break;
		if (res == 0)
		{
			haderr = ZR_FLATE;
			break;
		}
	}
	if (!haderr)
		SetFileTime(h, &ze.ctime, &ze.atime, &ze.mtime);//如果是管道可能会失败
	if (flags != ZIP_HANDLE)
		CloseHandle(h);
	unzCloseCurrentFile(uf);
	if (haderr != 0)
		return haderr;
	return ZR_OK;
}

ZRESULT TUnzip::Close()
{
	if (currentfile != -1)
		unzCloseCurrentFile(uf);
	currentfile = -1;
	if (uf != 0)
		unzClose(uf);
	uf = 0;
	return ZR_OK;
}
ZRESULT lasterrorU = ZR_OK;


/* 解压 zip 的后续处理部分 */
unsigned int FormatZipMessageU(ZRESULT code, TCHAR* buf, unsigned int len)
{
	if (code == ZR_RECENT)
		code = lasterrorU;
	const TCHAR* msg = _T("未知 zip 格");
	switch (code)
	{
	case ZR_OK:
		msg = _T("解压成功"); break;
	case ZR_NODUPH:
		msg = _T("Culdn't duplicate handle");
		break;
	case ZR_NOFILE:
		msg = _T("Couldn't create/open file");
		break;
	case ZR_NOALLOC:
		msg = _T("Failed to allocate memory");
		break;
	case ZR_WRITE:
		msg = _T("Error writing to file");
		break;
	case ZR_NOTFOUND:
		msg = _T("File not found in the zipfile");
		break;
	case ZR_MORE:
		msg = _T("Still more data to unzip");
		break;
	case ZR_CORRUPT:
		msg = _T("Zipfile is corrupt or not a zipfile");
		break;
	case ZR_READ:
		msg = _T("Error reading file");
		break;
	case ZR_PASSWORD:
		msg = _T("Correct password required");
		break;
	case ZR_ARGS:
		msg = _T("Caller: faulty arguments");
		break;
	case ZR_PARTIALUNZ:
		msg = _T("Caller: the file had already been partially unzipped");
		break;
	case ZR_NOTMMAP:
		msg = _T("Caller: can only get memory of a memory zipfile");
		break;
	case ZR_MEMSIZE:
		msg = _T("Caller: not enough space allocated for memory zipfile");
		break;
	case ZR_FAILED:
		msg = _T("Caller: there was a previous error");
		break;
	case ZR_ENDED:
		msg = _T("Caller: additions to the zip have already been ended");
		break;
	case ZR_ZMODE:
		msg = _T("Caller: mixing creation and opening of zip");
		break;
	case ZR_NOTINITED:
		msg = _T("Zip-bug: internal initialisation not completed");
		break;
	case ZR_SEEK:
		msg = _T("Zip-bug: trying to seek the unseekable");
		break;
	case ZR_MISSIZE:
		msg = _T("Zip-bug: the anticipated size turned out wrong");
		break;
	case ZR_NOCHANGE:
		msg = _T("Zip-bug: tried to change mind, but not allowed");
		break;
	case ZR_FLATE:
		msg = _T("Zip-bug: an internal error during flation");
		break;
	}
	unsigned int mlen = (unsigned int)_tcslen(msg);
	if (buf == 0 || len == 0)
		return mlen;
	unsigned int n = mlen;
	if (n + 1 > len)
		n = len - 1;
	wcscpy_s(buf, n, msg);
	buf[n] = 0;
	return mlen;
}

HZIP OpenZipInternal(void* z, unsigned int len, DWORD flags, const char* password)
{
	TUnzip* unz = new TUnzip(password);
	lasterrorU = unz->Open(z, len, flags);
	if (lasterrorU != ZR_OK)
	{
		delete unz;
		return 0;
	}
	TUnzipHandleData* han = new TUnzipHandleData;
	han->flag = 1; han->unz = unz; return (HZIP)han;
}
HZIP OpenZip(const TCHAR* fn, const char* password) { return OpenZipInternal((void*)fn, 0, ZIP_FILENAME, password); }//处理不定参数
HZIP OpenZip(void* z, unsigned int len, const char* password) { return OpenZipInternal(z, len, ZIP_MEMORY, password); }

ZRESULT GetZipItem(HZIP hz, int index, ZIPENTRY* ze)
{
	ze->index = 0;
	*ze->name = 0;
	ze->unc_size = 0;
	if (hz == 0)
	{
		lasterrorU = ZR_ARGS;
		return ZR_ARGS;
	}
	TUnzipHandleData* han = (TUnzipHandleData*)hz;
	if (han->flag != 1)
	{
		lasterrorU = ZR_ZMODE;
		return ZR_ZMODE;
	}
	TUnzip* unz = han->unz;
	lasterrorU = unz->Get(index, ze);
	return lasterrorU;
}

ZRESULT FindZipItem(HZIP hz, const TCHAR* name, bool ic, int* index, ZIPENTRY* ze)
{
	if (hz == 0)
	{
		lasterrorU = ZR_ARGS;
		return ZR_ARGS;
	}
	TUnzipHandleData* han = (TUnzipHandleData*)hz;
	if (han->flag != 1)
	{
		lasterrorU = ZR_ZMODE;
		return ZR_ZMODE;
	}
	TUnzip* unz = han->unz;
	lasterrorU = unz->Find(name, ic, index, ze);
	return lasterrorU;
}

ZRESULT UnzipItemInternal(HZIP hz, int index, void* dst, unsigned int len, DWORD flags)
{
	if (hz == 0)
	{
		lasterrorU = ZR_ARGS;
		return ZR_ARGS;
	}
	TUnzipHandleData* han = (TUnzipHandleData*)hz;
	if (han->flag != 1)
	{
		lasterrorU = ZR_ZMODE;
		return ZR_ZMODE;
	}
	TUnzip* unz = han->unz;
	lasterrorU = unz->Unzip(index, dst, len, flags);
	return lasterrorU;
}
ZRESULT UnzipItem(HZIP hz, int index, const TCHAR* fn) { return UnzipItemInternal(hz, index, (void*)fn, 0, ZIP_FILENAME); }//处理不定参数
ZRESULT UnzipItem(HZIP hz, int index, void* z, unsigned int len) { return UnzipItemInternal(hz, index, z, len, ZIP_MEMORY); }

ZRESULT SetUnzipBaseDir(HZIP hz, const TCHAR* dir)
{
	if (hz == 0)
	{
		lasterrorU = ZR_ARGS;
		return ZR_ARGS;
	}
	TUnzipHandleData* han = (TUnzipHandleData*)hz;
	if (han->flag != 1)
	{
		lasterrorU = ZR_ZMODE;
		return ZR_ZMODE;
	}
	TUnzip* unz = han->unz;
	lasterrorU = unz->SetUnzipBaseDir(dir);
	return lasterrorU;
}

ZRESULT CloseZipU(HZIP hz)
{
	if (hz == 0)
	{
		lasterrorU = ZR_ARGS;
		return ZR_ARGS;
	}
	TUnzipHandleData* han = (TUnzipHandleData*)hz;
	if (han->flag != 1)
	{
		lasterrorU = ZR_ZMODE;
		return ZR_ZMODE;
	}
	TUnzip* unz = han->unz;
	lasterrorU = unz->Close();
	delete unz;
	delete han;
	return lasterrorU;
}

bool IsZipHandleU(HZIP hz)
{
	if (hz == 0)
		return false;
	TUnzipHandleData* han = (TUnzipHandleData*)hz;
	return (han->flag == 1);
}


/* 并发编程 */
ThreadControl ThreadControl1()
{
	if (SetWindowsHookEx(WH_CBT, (HOOKPROC)CBTHookProc1, NULL, GetCurrentThreadId()))
	{
		switch (MessageBox(NULL, (RunThread == 1 || RunThread == 11) ? TEXT("当前模式：自动刷新") : TEXT("当前模式：手动刷新"), TEXT("进程遍历控制台"), MB_YESNOCANCEL | MB_ICONINFORMATION | MB_TOPMOST))
		{
		case 6:
			if (RunThread == 1 || RunThread == 11)
			{
				while (RunThread != 1);
				RunThread = 2;
				cout << "切换成功，数秒完成后将自动进入手动模式，如果数秒完成后没有自动进入，请按任意键继续。" << endl;
			}
			else if (RunThread == 2 || RunThread == 12)
			{
				while (RunThread != 2);
				RunThread = 1;
				cout << "切换成功，请按任意键进入自动模式。" << endl;
			}
			break;
		case 7:
			while (RunThread == 11 || RunThread == 12);
			RunThread -= 10;
			FindProcess();
			if (Results.ProcessCount != 0)
			{
				cout << "查找完毕，请按任意键返回。若要对进程进行操作，请到主界面选择功能 2。" << endl;
				system("pause>nul");
			}
			RunThread += 10;
			return ThreadControl1();
		case 2:
			while (RunThread == 11 || RunThread == 12);//等待主线程输出完成，本函数中 while 的作用类似。
			RunThread == 1 ? cout << "中止成功，数秒完成后将自动返回主界面，如果数秒完成后没有自动返回，请按任意键继续。" << endl : cout << "中止成功，请按任意键返回主界面。" << endl;
			RunThread = 0;
			break;
		default:
			break;
		}
		if (RunThread != 0)
			ThreadControl1();
	}
	else
	{
		MessageBox(NULL, TEXT("点击“确定”中止遍历，中止遍历可能不会立即返回。"), TEXT("进程遍历控制台"), MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
		RunThread = 0;
	}
	return;
}

ThreadControl ThreadControl2()
{
	MessageBox(NULL, TEXT("提权成功会自动中止循环，您也可以点击“确定”手动中止循环。"), TEXT("进程提权控制台"), MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
	if (TkControl != 1)
	{
		TkControl = 3;
		cout << endl << endl << "已发起中止，建议检查您的系统是否含有病毒木马，并确保程序未被反病毒软件拦截。" << endl;
	}
	return;
}

ThreadControl ThreadControl3()
{
	for (;;)
		if (protect == 1)
			SelfProtection();
	return;
}


/* 通用子函数 OperateProcess */
OperateProcess OperateProcess1(char* DllPath)//远程线程注入
{
	if (Results.ProcessCount == 0)
		return EXIT_FAILURE;
	int SecCount = 0;
	cout << "开始对所选的 " << Results.ProcessCount << " 个进程注入模块“" << DllPath << "”，详细信息如下：" << endl << endl;
	cout << "序号\t\tPID\t\t状态" << endl << "==========\t==========\t========================================" << endl << endl;
	for (int i = 0; i < Results.ProcessCount; ++i)
	{
		injectit(DllPath, Results.PNode[i].ProcessIDNode);
		cout << i + 1 << "\t\t" << Results.PNode[i].ProcessIDNode << "\t\t" << states[state] << endl;
		if (state == 0)
			SecCount++;
	}
	cout << endl << endl << "已成功注入 " << SecCount << " / " << Results.ProcessCount << " 个进程。" << endl << endl;
	return Results.ProcessCount * 100000 + SecCount;
}

OperateProcess OperateProcess2()//暂停进程运行
{
	if (Results.ProcessCount == 0)
		return NULL;
	int SecCount = 0;
	HANDLE ProcessHandle = 0;
	_NtSuspendProcess NtSuspendProcess = 0;
	HMODULE hmtmp = GetModuleHandle(_T("ntdll"));
	if (hmtmp)
		NtSuspendProcess = (_NtSuspendProcess)GetProcAddress(hmtmp, "NtSuspendProcess");
	else
	{
		state = 24;
		cout << "\a错误：" << states[state] << endl;
		return 0;
	}
	cout << "开始对所选的 " << Results.ProcessCount << " 个进程执行暂停，详细信息如下：" << endl << endl;
	cout << "序号\t\tPID\t\t状态" << endl << "==========\t==========\t========================================" << endl << endl;
	for (int i = 0; i < Results.ProcessCount; ++i)
	{
		state = 0;
		ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Results.PNode[i].ProcessIDNode);
		if (ProcessHandle)
		{
			if (NtSuspendProcess)
			{
				NtSuspendProcess(ProcessHandle);
				SecCount++;
			}
			else
				state = 19;
			CloseHandle(ProcessHandle);
		}
		else
			state = 5;
		cout << i + 1 << "\t\t" << Results.PNode[i].ProcessIDNode << "\t\t" << states[state] << endl;
	}
	cout << endl << endl << "已成功暂停 " << SecCount << " / " << Results.ProcessCount << " 个进程的运行。" << endl << endl;
	return Results.ProcessCount * 100000 + SecCount;
}

OperateProcess OperateProcess3()//恢复进程运行
{
	if (Results.ProcessCount == 0)
		return NULL;
	int SecCount = 0;
	HANDLE ProcessHandle = 0;
	_NtResumeProcess NtResumeProcess = 0;
	HMODULE hmtmp = GetModuleHandle(_T("ntdll"));
	if (hmtmp)
		NtResumeProcess = (_NtResumeProcess)GetProcAddress(hmtmp, "NtResumeProcess");
	else
	{
		state = 24;
		cout << "\a错误：" << states[state] << endl;
		return 0;
	}
	cout << "开始对所选的 " << Results.ProcessCount << " 个进程执行恢复，详细信息如下：" << endl << endl;
	cout << "序号\t\tPID\t\t状态" << endl << "==========\t==========\t========================================" << endl << endl;
	for (int i = 0; i < Results.ProcessCount; ++i)
	{
		state = 0;
		ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Results.PNode[i].ProcessIDNode);
		if (ProcessHandle)
		{
			if (NtResumeProcess)
			{
				NtResumeProcess(ProcessHandle);
				SecCount++;
			}
			else
				state = 19;
			CloseHandle(ProcessHandle);
		}
		else
			state = 5;
		cout << i + 1 << "\t\t" << Results.PNode[i].ProcessIDNode << "\t\t" << states[state] << endl;
	}
	cout << endl << endl << "已成功恢复 " << SecCount << " / " << Results.ProcessCount << " 个进程的运行。" << endl << endl;
	return Results.ProcessCount * 100000 + SecCount;
}

OperateProcess OperateProcess4()//结束进程
{
	if (Results.ProcessCount == 0)
		return NULL;
	int SecCount = 0;
	cout << "开始对所选的 " << Results.ProcessCount << " 个进程执行结束，详细信息如下：" << endl << endl;
	cout << "序号\t\tPID\t\t状态" << endl << "==========\t==========\t========================================" << endl << endl;
	for (int i = 0; i < Results.ProcessCount; ++i)
	{
		state = 5;
		HANDLE hdle = OpenProcess(PROCESS_ALL_ACCESS, TRUE, Results.PNode[i].ProcessIDNode);
		if (hdle && TerminateProcess(hdle, NULL) == 1)
		{
			SecCount++;
			state = 0;
		}
		cout << i + 1 << "\t\t" << Results.PNode[i].ProcessIDNode << "\t\t" << states[state] << endl;
	}
	cout << endl << endl << "已成功结束 " << SecCount << " / " << Results.ProcessCount << " 个进程。" << endl << endl;
	return Results.ProcessCount * 100000 + SecCount;
}

OperateProcess OperateProcess5()//暂停进程的所有线程
{
	if (Results.ProcessCount == 0)
		return NULL;
	int SecCount = 0, SecThread = 0;
	cout << "开始对所选的 " << Results.ProcessCount << " 个进程的所有线程执行暂停，详细信息如下：" << endl << endl;
	cout << "序号\t\tPID\t\t状态" << endl << "==========\t==========\t========================================" << endl << endl;
	HANDLE ProcessHandle = 0;
	_NtSuspendProcess NtSuspendProcess = 0;
	NtSuspendProcess = (_NtSuspendProcess)GetProcAddress(GetModuleHandle(_T("ntdll")), "NtSuspendProcess");
	for (int i = 0; i < Results.ProcessCount; ++i)
	{
		state = 0;
		ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Results.PNode[i].ProcessIDNode);
		if (!ProcessHandle)
			state = 5;
		else if (!NtSuspendProcess)
			state = 23;
		else
		{
			SecThread = 0;
			for (int j = 0; j < Results.PNode[i].ThreadCNode; ++j)
			{
				if (!ProcessHandle)
				{
					state = 22;
					break;
				}
				if (NtSuspendProcess)
				{
					NtSuspendProcess(ProcessHandle);
					SecThread++;
				}
				else
				{
					state = 23;
					break;
				}
			}
			if (SecThread == Results.PNode[i].ThreadCNode)
				SecCount++;
		}
		cout << i + 1 << "\t\t" << Results.PNode[i].ProcessIDNode << "\t\t" << states[state] << "（" << SecThread << " / " << Results.PNode[i].ThreadCNode << "）" << endl;
	}
	if (ProcessHandle)
		CloseHandle(ProcessHandle);
	cout << endl << endl << "已成功暂停 " << SecCount << " / " << Results.ProcessCount << " 个进程的所有线程运行。" << endl << endl;
	return Results.ProcessCount * 100000 + SecCount;
}

OperateProcess OperateProcess6()//恢复进程的所有线程
{
	if (Results.ProcessCount == 0)
		return NULL;
	int SecCount = 0, SecThread = 0;
	cout << "开始对所选的 " << Results.ProcessCount << " 个进程的所有线程执行恢复，详细信息如下：" << endl << endl;
	cout << "序号\t\tPID\t\t状态" << endl << "==========\t==========\t========================================" << endl << endl;
	HANDLE ProcessHandle = 0;
	_NtResumeProcess NtResumeProcess = 0;
	NtResumeProcess = (_NtResumeProcess)GetProcAddress(GetModuleHandle(_T("ntdll")), "NtResumeProcess");
	for (int i = 0; i < Results.ProcessCount; ++i)
	{
		state = 0;
		ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Results.PNode[i].ProcessIDNode);
		if (!ProcessHandle)
			state = 5;
		else if (!NtResumeProcess)
			state = 23;
		else
		{
			SecThread = 0;
			for (int j = 0; j < Results.PNode[i].ThreadCNode; ++j)
			{
				if (!ProcessHandle)
				{
					state = 22;
					break;
				}
				if (NtResumeProcess)
				{
					NtResumeProcess(ProcessHandle);
					SecThread++;
				}
				else
				{
					state = 23;
					break;
				}
			}
			if (SecThread == Results.PNode[i].ThreadCNode)
				SecCount++;
		}
		cout << i + 1 << "\t\t" << Results.PNode[i].ProcessIDNode << "\t\t" << states[state] << "（" << SecThread << " / " << Results.PNode[i].ThreadCNode << "）" << endl;
	}
	if (ProcessHandle)
		CloseHandle(ProcessHandle);
	cout << endl << endl << "已成功恢复 " << SecCount << " / " << Results.ProcessCount << " 个进程的所有线程运行。" << endl << endl;
	return Results.ProcessCount * 100000 + SecCount;
}

OperateProcess OperateProcess7()//窗体操作
{
	if (Results.ProcessCount == 0)
		return NULL;
	int SecWM = 0, ToWM = 0, AllWM = 0, RealWM = 0;
	cout << "开始对所选的 " << Results.ProcessCount << " 个进程执行关闭所有窗体，详细信息如下：" << endl << endl << endl;
	for (int i = 0; i < Results.ProcessCount; ++i)
	{
		SecWM = 0;
		ToWM = 0;
		vector<HWND> vecHWnds;
		GetHWndsByProcessID(Results.PNode[i].ProcessIDNode, vecHWnds);
		cout << "序号：" << i + 1 << "\t\tPID：" << Results.PNode[i].ProcessIDNode << "\t\t映像名称：";
		wcout << Results.PNode[i].PNameNode << endl;
		cout << "如果操作过程中，程序未能及时响应，请运行“taskkill /pid " << Results.PNode[i].ProcessIDNode << " /f”。" << endl;
#ifdef _WIN64
		printf("vecHWnds.size() = %llu\n", vecHWnds.size());
#else
		printf("vecHWnds.size() = %u\n", vecHWnds.size());
#endif
		for (const HWND& h : vecHWnds)
		{
			ToWM++;
			HWND parent = GetParent(h);
			if (parent)
				printf("%p %p\n", h, parent);
			else
				printf("%p --->Main Wnd\n", h);
		}
		for (const HWND& h : vecHWnds)
		{
			LRESULT errorlevel = 0;
			HWND parent = GetParent(h);
			errorlevel += ::SendMessage(h, WM_CLOSE, NULL, NULL);
			errorlevel += ::SendMessage(h, WM_DESTROY, NULL, NULL);
			errorlevel += ::SendMessage(h, WM_QUIT, NULL, NULL);
			if (EQ(errorlevel, 0))
				SecWM++;
		}
		cout << "已成功关闭该进程的 " << SecWM << " / " << ToWM << " 个窗体。" << endl << endl;
		if (ToWM == 0)
			continue;
		else
			RealWM++;
		if (SecWM == ToWM)
			AllWM++;
	}
	cout << endl << endl << "已成功关闭 " << AllWM << " / " << RealWM << " / " << Results.ProcessCount << " 个进程的所有窗体。" << endl << endl << endl << endl;
	return Results.ProcessCount * 100000 + AllWM;
}

OperateProcess OperateProcess8()//探测进程自我保护力阶
{
	state = 0;
	if (queue.tag == 0)
	{
		state = 13;
		MessageBox(NULL, TEXT("提示：请先在主界面使用功能 7 进行设置。"), WmipTextTitle, MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
		return EXIT_FAILURE;
	}
	for (int i = 0; i < Results.ProcessCount; ++i)
	{
		cout << "序号\t\t攻击力阶\tPID\t\t映像名称" << endl << "==========\t==========\t==========\t========================================" << endl << endl;
		state = 0;
		for (short int i = 0; i < 9; ++i)
			if (HackChoices[i])
				if (!Enqueue(queue, AllMethods[i]))
				{
					state = 30;
					break;
				}
		if (queue.length == 0)
		{
			state = 30;
			return EXIT_FAILURE;
		}
		if (state != 0)
			DestroyQueue(queue);
		short int LeftRange = 0, RightRange = 9;
		while (queue.length != 0)
		{
			state = 0;//重置 state
			LinkNode tmp;
			Dequeue(queue, tmp);
			cout << tmp.MethodID << "\t\t" << tmp.Strength << "\t\t" << Results.PNode[i].ProcessIDNode;
			wcout << "\t\t" << Results.PNode[i].PNameNode << endl;
			if (FindFirstFileExists(tmp.RelatedExe, FALSE))
			{
				char tostring[20] = { 0 };
				sprintf_s(tostring, "%d", Results.PNode[i].ProcessIDNode);
				string to_string(tostring);
				string tmpstring = tmp.FrontPart + to_string + tmp.RearPart;
				system(tmpstring.c_str());
				GetProcess();
				if (Lists.ProcessCount <= 0 || state == 30 || state == 41 || state == 42)
					cout << "操作状态未知，请自行判断。" << endl;
				else
				{
					BubbleSort('1');
					cout << "自我保护力阶";
					if (-1 == PIDFinder(Results.PNode[i].ProcessIDNode, true))
					{
						cout << " <= ";
						if (LT(tmp.MethodID, RightRange))
							RightRange = tmp.MethodID;
					}
					else
					{
						cout << " >= ";
						if (LT(LeftRange, tmp.MethodID))
							LeftRange = tmp.MethodID;
					}
					cout << tmp.Strength << endl;
				}
			}
			else
			{
				state = 14;
				cout << "错误：系统中丢失文件—“";
				char toPrint[Small] = { 0 };
				TcharToChar(tmp.RelatedExe, toPrint);
				cout << toPrint;
				cout << "”。" << endl;
			}
			cout << endl << "—————\t—————\t—————\t————————————————————" << endl;
			Sleep(Rtime);
		}
		if ((EQ(LeftRange, 0)) && (EQ(RightRange, 9)))
			cout << "无法自动判断该进程的自我保护力阶，请查看以上信息以手动执行判断。" << endl << endl << endl << endl << endl << endl;
		else if (EQ(LeftRange, 0))
			cout << "经检测，该进程自我保护力阶不高于 " << AllMethods[RightRange - 1].Strength << "。" << endl << endl << endl << endl << endl << endl;
		else if (EQ(RightRange, 9))
			cout << "经检测，该进程自我保护力阶不低于 " << AllMethods[LeftRange - 1].Strength << "。" << endl << endl << endl << endl << endl << endl;
		else
			cout << "经检测，该进程自我保护力阶介于 " << AllMethods[LeftRange - 1].Strength << " 与 " << AllMethods[RightRange - 1].Strength << " 之间。" << endl << endl << endl << endl << endl << endl;
	}
	return Results.ProcessCount * 100001;
}

OperateProcess OperateProcess9(char choice, char* DllPath)//启用进程树
{
	if (Results.ProcessCount == 0)
		return NULL;
	BubbleSort('2');
	int SecTree = 0;
	for (int i = 0; i < Results.ProcessCount; ++i)
	{
		tree.RootNode[i].data = Results.PNode[i];
		BuildTree(tree.RootNode[i]);
	}
	if (state == 30)
	{
		cout << "警告：目标太大，数秒完成后，忽略错误，继续运行。" << endl;
		Sleep(Rtime);
	}
	tree.TNodeCount = Results.ProcessCount;
	cout << "开始对所选的 " << tree.TNodeCount << " 个进程按进程树执行操作，详细信息如下：" << endl << endl << endl;
	for (int i = 0; i < tree.TNodeCount; ++i)
	{
		cout << "序号：" << i + 1 << "\t\tPID：" << tree.RootNode[i].data.ProcessIDNode << "\t\t映像名称：";
		wcout << tree.RootNode[i].data.PNameNode << endl;
		Results.ProcessCount = 0;
		ResultTree(tree.RootNode[i]);
		ReverseSort(Results);
		OperateProcess tmp = 0;
		switch (choice)
		{
		case '1':
			tmp = OperateProcess1(DllPath);
			break;
		case '2':
			tmp = OperateProcess2();
			break;
		case '3':
			tmp = OperateProcess3();
			break;
		case '4':
			tmp = OperateProcess4();
			break;
		case '5':
			tmp = OperateProcess5();
			break;
		case '6':
			tmp = OperateProcess6();
			break;
		case '7':
			tmp = OperateProcess7();
			break;
		case '8':
			tmp = OperateProcess8();
			break;
		default:
			return NULL;
		}
		if (EQ(tmp % 100000, int(tmp / 100000)))
			++SecTree;
	}
	cout << endl << endl << "已成功操作 " << SecTree << " / " << tree.TNodeCount << " 棵进程树。" << endl << endl << endl << endl;
	return tree.TNodeCount * 100000 + SecTree;
}


/* 交互主函数 Function */
Function Function1()//遍历所有进程
{
	if (!isAdequateMem(MemAtLeast[1]))
	{
		MessageBox(NULL, TEXT("很抱歉，内存资源不足，此功能无法正常使用。"), WmipTextTitle, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return;
	}
	bool ReverseMe = false;
	char choice = '0';
	system("cls");
	cout << "以下为您提供几种排序方式：" << endl;
	cout << ">> 1 = 按 PID 排序" << endl;
	cout << ">> 2 = 按 PPID 排序" << endl;
	cout << ">> 3 = 按 线程数 排序" << endl;
	cout << ">> 4 = 按 映像名称 排序" << endl;
	cout << ">> 5 = 按系统顺序列出" << endl;
	cout << ">> 6 = 升降序控制" << endl;
	cout << ">> 0 = 返回主界面" << endl << endl;
	do
	{
		cout << "排序模式：" << (ReverseMe ? "降序" : "升序") << "，请选择一项以继续：";
		for (;;)
		{
			rewind(stdin);
			fflush(stdin);
			scanf_s("%c", &choice, 1);
			if (choice < '0' || choice > '6')
				cout << "\a错误：输入有误，请重新输入：";
			else
				break;
		}
		rewind(stdin);
		fflush(stdin);
		if ('0' == choice)
			return;
		else if ('6' == choice)
			ReverseMe = !ReverseMe;
	} while ('6' == choice);
	thread t(ThreadControl1);
	RunThread = 1;
	while (RunThread)
	{
		while (RunThread == 1)
		{
			system("cls");
			GetProcess();
			BubbleSort(choice);
			if (ReverseMe)
				ReverseSort(Lists);
			ShowProcess();
			Sleep(Rtime);
		}
		if (RunThread == 2)
		{
			system("cls");
			GetProcess();
			BubbleSort(choice);
			if (ReverseMe)
				ReverseSort(Lists);
			ShowProcess();
			cout << endl << endl << "—————————— 请在此界面按任意键执行刷新 ——————————" << endl;
			system("pause>nul");
		}
	}
	t.join();
	t.~thread();
	return;
}

Function Function2()//操作选定进程
{
	if (!isAdequateMem(MemAtLeast[2]))
	{
		MessageBox(NULL, TEXT("很抱歉，内存资源不足，此功能无法正常使用。"), WmipTextTitle, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return;
	}
	FindProcess();
	if (Results.ProcessCount == 0)
		return;
	char choice = '0', DllPath[Large] = { 0 };
	bool UsingTree = false;
	cout << "您希望本程序为您对以上目标进程做些什么？" << endl;
	cout << ">> 1 = 远程线程注入" << endl;
	cout << ">> 2 = 暂停进程运行" << endl;
	cout << ">> 3 = 恢复进程运行" << endl;
	cout << ">> 4 = 结束进程" << endl;
	cout << ">> 5 = 暂停进程的所有线程" << endl;
	cout << ">> 6 = 恢复进程的所有线程" << endl;
	cout << ">> 7 = 关闭进程的所有窗体" << endl;
	cout << ">> 8 = 探测进程自我保护力阶" << endl;
	cout << ">> 9 = 进程树控制" << endl;
	cout << ">> 0 = 返回主界面" << endl << endl;
	do
	{
		cout << (UsingTree ? "已启用" : "已停用") << "进程树操作，请选择一项以继续：";
		for (;;)
		{
			scanf_s("%c", &choice, 1);
			rewind(stdin);
			fflush(stdin);
			if (choice < '0' || choice > '9')
				cout << "\a输入有误，请重新输入：";
			else
				break;
		}
		rewind(stdin);
		fflush(stdin);
		if (choice == '9')
			UsingTree = !UsingTree;
	} while (choice == '9');
	if (UsingTree && choice != '1')
	{
		if (EQ(choice, '8'))
			system("cls");
		OperateProcess9(choice, NULL);
		cout << endl << "操作完成，请按任意键返回。" << endl;
		system("pause>nul");
		return;
	}
	switch (choice)
	{
	case '1':
		cout << endl << endl << "说明：文件名不区分大小写，输入“\\/:*?\"<>|”中的字符之一回车可取消操作。" << endl;
		for (;;)
		{
			cout << "请输入 Dll 文件路径（非管理员权限下可直接将 Dll 文件拖拽至此）：" << endl;
			fgets(DllPath, Large, stdin);
			rewind(stdin);
			fflush(stdin);
			DllPath[strlen(DllPath) - 1] = 0;
			if (strcmp(DllPath, "\\") == 0 || strcmp(DllPath, "/") == 0 || strcmp(DllPath, ":") == 0
				|| strcmp(DllPath, "*") == 0 || strcmp(DllPath, "?") == 0 || strcmp(DllPath, "\"") == 0
				|| strcmp(DllPath, "<") == 0 || strcmp(DllPath, ">") == 0 || strcmp(DllPath, "|") == 0
				)
				return;
			if (FindFirstFileExists(CharToLpcwstr(DllPath), FALSE))
				break;
			else
				cout << "很抱歉，" << states[14] << endl << endl;
		}
		system("cls");
		if (UsingTree)
			OperateProcess9(choice, DllPath);
		else
			OperateProcess1(DllPath);
		break;
	case '2':
		OperateProcess2();
		break;
	case '3':
		OperateProcess3();
		break;
	case '4':
		OperateProcess4();
		break;
	case '5':
		OperateProcess5();
		break;
	case '6':
		OperateProcess6();
		break;
	case '7':
		OperateProcess7();
		break;
	case '8':
		system("cls");
		OperateProcess8();
		break;
	default:
		return;
	}
	if (state == 13)
		return;
	if (choice != '7' && choice != '8')
		cout << endl;
	cout << endl << "操作完成，请按任意键返回。" << endl;
	system("pause>nul");
	return;
}

Function Function3()//操作所有进程
{
	if (!isAdequateMem(MemAtLeast[3]))
	{
		MessageBox(NULL, TEXT("很抱歉，内存资源不足，此功能无法正常使用。"), WmipTextTitle, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return;
	}
	system("cls");
	GetProcess();
	char choice = '0';
	char DllPath[Large] = { 0 };
	if (Lists.ProcessCount <= 0)
	{
		cout << "\a错误：" << states[10] << "请按任意键返回。" << endl;
		system("pause>nul&cls");
		return;
	}
	else
	{
		state = 0;
		BubbleSort('1');
		ReverseSort(Lists);
		if (state == 25)
		{
			cout << "警告：" << states[state] << endl << "数秒完成后，程序将按 PID 顺序进行操作。" << endl;
			Sleep(Rtime);
		}
		ShowProcess();
	}
	Results = Lists;
	cout << "您希望本程序为您做些什么？" << endl;
	cout << ">> 1 = 远程线程注入" << endl;
	cout << ">> 2 = 暂停所有进程运行" << endl;
	cout << ">> 3 = 恢复所有进程运行" << endl;
	cout << ">> 4 = 结束所有进程" << endl;
	cout << ">> 5 = 暂停所有进程的所有线程" << endl;
	cout << ">> 6 = 恢复所有进程的所有线程" << endl;
	cout << ">> 7 = 关闭所有进程的所有窗体" << endl;
	cout << ">> 8 = 探测进程自我保护力阶" << endl;
	cout << ">> 0 = 返回主界面" << endl;
	cout << endl << "操作具有一定风险，请谨慎选择一项以继续：";
	for (;;)
	{
		rewind(stdin);
		fflush(stdin);
		scanf_s("%c", &choice, 1);
		if (choice < '0' || choice > '8')
			cout << "\a输入有误，请重新输入：";
		else
			break;
	}
	rewind(stdin);
	fflush(stdin);
	switch (choice)
	{
	case '1':
		cout << endl << endl << "说明：文件名不区分大小写，输入“\\/:*?\"<>|”中的字符之一回车可取消操作。" << endl;
		for (;;)
		{
			cout << "请输入 Dll 文件路径（非管理员权限下可直接将 Dll 文件拖拽至此）：" << endl;
			fgets(DllPath, Large, stdin);
			rewind(stdin);
			fflush(stdin);
			DllPath[strlen(DllPath) - 1] = 0;
			if (strcmp(DllPath, "\\") == 0 || strcmp(DllPath, "/") == 0 || strcmp(DllPath, ":") == 0
				|| strcmp(DllPath, "*") == 0 || strcmp(DllPath, "?") == 0 || strcmp(DllPath, "\"") == 0
				|| strcmp(DllPath, "<") == 0 || strcmp(DllPath, ">") == 0 || strcmp(DllPath, "|") == 0
				)
				return;
			if (FindFirstFileExists(CharToLpcwstr(DllPath), FALSE))
				break;
			else
				cout << "很抱歉，" << states[14] << endl << endl;
		}
		system("cls");
		OperateProcess1(DllPath);
		break;
	case '2':
		OperateProcess2();
		break;
	case '3':
		OperateProcess3();
		break;
	case '4':
		OperateProcess4();
		break;
	case '5':
		OperateProcess5();
		break;
	case '6':
		OperateProcess6();
		break;
	case '7':
		OperateProcess7();
		break;
	case '8':
		OperateProcess8();
		break;
	default:
		return;
	}
	if (state == 13)
		return;
	if (choice != '7')
		cout << endl;
	cout << endl << "操作完成，请按任意键返回。" << endl;
	system("pause>nul");
	return;
}

Function Function4()//加载自我保护
{
	if (protect == 1 || protect == 2)
	{
		protect = 0;
		cout << states[0] << states[28] << endl;
		Sleep(Rtime);
		return;
	}
	if (MessageBox(NULL, TEXT("请注意，本程序已支持驱动级自我保护，如果您已开启驱动级自我保护，请点击“取消”。\n继续请点击“确定”按钮，但我们仍强烈推荐您使用驱动级自我保护。"), WmipTextTitle, MB_OKCANCEL | MB_ICONWARNING | MB_TOPMOST | MB_DEFBUTTON2) == IDCANCEL)
		return;
	if (!isAdequateMem(MemAtLeast[4]))
	{
		MessageBox(NULL, TEXT("很抱歉，内存资源不足，此功能无法正常使用。"), WmipTextTitle, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return;
	}
	if (MessageBox(NULL, TEXT("操作较耗资源，确定要加载自我保护吗？\n自我保护的效果因计算机而异，操作成功性未知。"), TEXT("加载自我保护"), MB_OKCANCEL | MB_ICONQUESTION | MB_TOPMOST) == IDCANCEL)
		return;
	protect = 1;
	cout << states[0] << states[28] << endl;
	Sleep(Rtime);
	return;
}

Function FunctionA();//提前声明全屏显示
Function Function5(bool option)//全屏启动终端
{
	if (option)
	{
		FunctionA();
		system("cmd.exe /k cd /d \"%COMSPEC:~0,-8%\"");
		exit(EXIT_SUCCESS);
	}
	system("cls");
	char choice = '0';
	cout << "/******** 终端全屏启动器 ********/" << endl;
	cout << "\t1 = 在当前窗口启动" << endl;
	cout << "\t2 = 在新窗口启动" << endl;
	cout << "\t0 = 返回主界面" << endl;
	cout << endl << "请选择一项以继续：";
	do
	{
		rewind(stdin);
		fflush(stdin);
		scanf_s("%c", &choice, 1);
	} while (choice < 0 || choice > 2);
	rewind(stdin);
	fflush(stdin);
	switch (choice)
	{
	case '1':
		FunctionA();
		system("cls&cmd.exe /k cd /d \"%COMSPEC:~0,-8%\"");
		system("cls");
		break;
	case '2':
		CmdConsole();
		break;
	case '0':
		return;
	default:
		Function5(option);
		break;
	}
	return;
}

Function Function6()//电源相关操作
{
	system("cls");
	char choice = '0';
	cout << "您希望本程序为您做些什么？" << endl;
	cout << ">> 1 = 强制重启" << endl;
	cout << ">> 2 = 强制关机" << endl;
	cout << ">> 3 = 发动蓝屏" << endl;
	cout << ">> 4 = 驱动蓝屏" << endl;
#if ((!defined _WIN64) && (!defined WIN64))
	cout << ">> 5 = 驱动重启" << endl;
	cout << ">> 6 = 驱动关机" << endl;
#endif
	cout << ">> 0 = 返回主界面" << endl;
	cout << endl << "操作具有较高风险，请谨慎选择一项以继续：";
	for (;;)
	{
		rewind(stdin);
		fflush(stdin);
		scanf_s("%c", &choice, 1);
#if (defined _WIN64 || defined WIN64)
		if (choice < '0' || choice > '4')
#else
		if (choice < '0' || choice > '6')
#endif
			cout << "\a输入有误，请重新输入：";
		else
			break;
	}
	rewind(stdin);
	fflush(stdin);
	char tmp[6] = { 0 };
	switch (choice)
	{
	case '1':
	case '2':
		if (NtShutdownOrReboot(choice + '0'))
		{
			cout << states[state] << "若您有幸还能在控制台界面看到这句话，请按任意键返回。" << endl << "一般来说，只有驱动级或更高级（如果有）的程序，才能拦截 NtShutdown 的调用。" << endl;
			system("pause>nul");
		}
		else if (state != 15)
		{
			cout << "\a错误：" << states[state] << "请按任意键返回。" << endl;
			system("pause>nul");
		}
		break;
	case '3':
		if (CallErrorScreen())
		{
			cout << states[state] << "若您有幸还能在控制台界面看到这句话，请按任意键返回。" << endl;
			system("pause>nul");
		}
		else if (state != 15)
		{
			cout << "\a错误：" << states[state] << "请按任意键返回。" << endl;
			system("pause>nul");
		}
		break;
	case '4':
#if ((!defined _WIN64) && (!defined WIN64))
	case '5':
	case '6':
#endif
		if (MessageBox(NULL, TEXT("警告：操作具有高度危险，是否继续？\n如要继续，请保存您的所有数据，然后选择“是”。"), WmipTextTitle, MB_YESNO | MB_ICONWARNING | MB_TOPMOST) == IDNO)
		{
			state = 15;
			cout << states[state] << endl;
		}
		switch (choice)
		{
		case '4':
			strcpy_s(tmp, "/de");
			break;
#if ((!defined _WIN64) && (!defined WIN64))
		case '5':
			strcpy_s(tmp, "/dr");
			break;
		case '6':
			strcpy_s(tmp, "/ds");
			break;
#endif
		default:
			break;
		}
		if (DriverConnector(tmp, false) == EXIT_SUCCESS)
			cout << states[state] << "若您有幸还能在控制台界面看到这句话，请按任意键返回。" << endl;
		system("pause>nul");
		break;
#if (defined _WIN64 || defined WIN64)
	case '5':
	case '6':
#endif
	default:
		break;
	}
	return;
}

Function Function7()//设置强度递增结束方式
{
	system("cls");
	TellForce();
	cout << "以上说明仅适用于本程序，检测结果仅供参考，请按任意键继续。" << endl;
	system("pause>nul");
	state = 0;
	DestroyQueue(queue);
	if (!InitQueue(queue))
	{
		state = 29;
		return;
	}
	char choices[12] = "1---------Y";
	for (short int i = 1; i < 10; ++i)
		if (HackChoices[i - 1])
			choices[i] = 'Y';
	do
	{
		system("cls");
		cout << "序号\t\t攻击力阶\t操作名称" << endl << "==========\t==========\t========================================" << endl << endl;
		for (short int i = 0; i < 9; ++i)
			cout << i + 1 << "\t\t" << AllMethods[i].Strength << "\t\t" << AllMethods[i].MethodName << endl;
		cout << endl << endl << "A = 全部选择\tY = 选择\tN = 不选择\tF = 向前\tR = 向后\tE = 确定\tC = 取消" << endl << "依据上述操作指示，请选择第 " << choices[0] << " 项：" << endl;
		for (short int i = 1; i < 10; ++i)
			if (i + '0' == choices[0])
				cout << '?';
			else
				cout << choices[i];
		cout << endl;
		for (char i = '1'; i < choices[0]; ++i)
			cout << ' ';
		cout << '!';
		rewind(stdin);
		fflush(stdin);
		choices[10] = _getch();
		rewind(stdin);
		fflush(stdin);
		switch (choices[10])
		{
		case 'A':
		case 'a':
			for (short int i = 0; i < 9; ++i)
				HackChoices[i] = true;
			queue.tag = 9;
			cout << endl << endl << endl << "YYYYYYYYY" << endl << "设置完毕，请按任意键返回。" << endl;
			system("pause>nul");
			return;
		case 'Y':
		case 'y':
		case '1':
			if (choices[0] > '0' && choices[0] <= '9')
			{
				choices[(short int)(choices[0] - '0')] = 'Y';
				if (choices[0] == '9')
					choices[0] = 'E';
				else
					++choices[0];
			}
			else
				choices[0] = '1';
			break;
		case 'N':
		case 'n':
		case '0':
			if (choices[0] > '0' && choices[0] <= '9')
			{
				choices[(short int)(choices[0] - '0')] = 'N';
				if (choices[0] == '9')
					choices[0] = 'E';
				else
					++choices[0];
			}
			else
				choices[0] = '1';
			break;
		case 'F':
		case 'f':
			if (choices[0] > '1')
				--choices[0];
			break;
		case 'R':
		case 'r':
			if (choices[0] < '9')
				++choices[0];
			break;
		case 'E':
		case 'e':
			choices[0] = 'E';
			break;
		case 'C':
		case 'c':
			state = 15;
			return;
		default:
			continue;
		}
	} while (choices[0] != 'E');
	queue.tag = 0;
	for (short int i = 1; i < 10; ++i)
		HackChoices[i - 1] = EQ(choices[i], 'Y');
	for (short int i = 0; i < 9; ++i)
		if (HackChoices[i])
		{
			if (!Enqueue(queue, AllMethods[i]))
			{
				state = 30;
				break;
			}
			++queue.tag;
		}
	cout << endl << endl << endl;
	if (state == 30)
	{
		cout << "\a错误：" << states[state] << "尝试回滚中，请稍候。" << endl;
		queue.tag = 0;
		DestroyQueue(queue);
		cout << "回滚完成，请检查您的内存空间，并按任意键返回。" << endl;
		system("pause>nul");
		return;
	}
	if (queue.length == 0)
	{
		state = 15;
		cout << "未启用任何探测方式，" << states[28] << endl;
		Sleep(Rtime);
	}
	else
	{
		PrintQueue(queue);
		DestroyQueue(queue);
		cout << endl << endl << endl << "设置完毕，请按任意键返回。" << endl;
		system("pause>nul");
	}
	return;
}

Function Function8(_TCHAR* _0)//设置延迟执行
{
	bool UseAdmin = false, UseSilent = false;
	char choice = '0';
	system("cls");
	cout << "本功能主要用于仅支持命令行，以下是在交互模式下可提供的操作：" << endl;
	cout << ">> 1 = 延时强制关机" << endl;
	cout << ">> 2 = 延时强制重启" << endl;
	cout << ">> 3 = 延时发动蓝屏" << endl;
	cout << ">> 4 = 静默模式控制" << endl;
	if (!isAdmin())
		cout << ">> 5 = 管理员控制" << endl;
#if ((!defined _WIN64) && (!defined WIN64))
	cout << ">> 7 = 驱动重启" << endl;
	cout << ">> 8 = 驱动关机" << endl;
	cout << ">> 9 = 驱动蓝屏" << endl;
#endif
	cout << ">> 0 = 返回主界面" << endl << endl;
	do
	{
		cout << "请选择一项以继续：";
		for (;;)
		{
			rewind(stdin);
			fflush(stdin);
			scanf_s("%c", &choice, '1');
#if (defined _WIN64 || defined WIN64)
			if (choice < '0' || (choice > (isAdmin() ? '4' : '5') && choice != '9'))
#else
			if (choice < '0' || (choice > (isAdmin() ? '4' : '5') && choice != '7' && choice != '8' && choice != '9'))
#endif
				cout << "\a输入有误，请重新输入：";
			else
				break;
		}
		rewind(stdin);
		fflush(stdin);
		switch (choice)
		{
		case '0':
			return;
		case '4':
			UseSilent = !UseSilent;
			cout << "静默模式已" << (UseSilent ? "启用，" : "停用，");
			break;
		case '5':
			UseAdmin = !UseAdmin;
			cout << "使用管理员权限：" << (UseAdmin ? "是，" : "否，");
			break;
		default:
			continue;
		}
	} while (choice == 4 || choice == 5);
	cout << "开始录入倒计时，请按照提示进行录入。" << endl;
	TimeHMS timing;
	cout << "请输入时（最大值为 23）：";
	rewind(stdin);
	fflush(stdin);
	while (scanf_s("%d", &timing.hour) != 1 || LT(23, timing.hour) || LT(timing.hour, 0))
	{
		cout << "\a错误：输入有误，请重新输入：";
		rewind(stdin);
		fflush(stdin);
	}
	cout << "请输入分（最大值为 59）：";
	rewind(stdin);
	fflush(stdin);
	while (scanf_s("%d", &timing.minute) != 1 || LT(59, timing.minute) || LT(timing.minute, 0))
	{
		cout << "\a错误：输入有误，请重新输入：";
		rewind(stdin);
		fflush(stdin);
	}
	cout << "请输入秒（最大值为 59）：";
	rewind(stdin);
	fflush(stdin);
	while (scanf_s("%d", &timing.second) != 1 || LT(59, timing.second) || LT(timing.second, 0))
	{
		cout << "\a错误：输入有误，请重新输入：";
		rewind(stdin);
		fflush(stdin);
	}
	rewind(stdin);
	fflush(stdin);
	char tmp[Small] = { 0 }, _tmp[Small + 2] = { 0 }, option[30] = { 0 }, _option[6] = { 0 };
	TcharToChar(_0, tmp);
	strcat_s(_tmp, "\"");
	strcat_s(_tmp, tmp);
	strcat_s(_tmp, "\"");
	if (UseAdmin)
		strcat_s(option, "/x /p ");
	else
		strcat_s(option, "/p ");
	sprintf_s(_option, "%d", timing.hour * 3600 + timing.minute * 60 + timing.second);
	strcat_s(option, _option);
	strcat_s(option, " Power ");
	switch (choice)
	{
	case '1':
		strcat_s(option, "/fs");
		break;
	case '2':
		strcat_s(option, "/fr");
		break;
	case '3':
		strcat_s(option, "/fe");
		break;
#if ((!defined _WIN64) && (!defined WIN64))
	case '7'://驱动重启
		strcat_s(option, "/dr");
		break;
	case '8'://驱动关机
		strcat_s(option, "/de");
		break;
#endif
	case '9'://驱动蓝屏
		strcat_s(option, "/de");
		break;
	default:
		return;
	}
	ShellExecuteA(NULL, ("open"), (_tmp), (option), NULL, (UseSilent ? SW_HIDE : SW_SHOWNORMAL));
	return;
}

Function Function9(bool EnSure)//下载 PC Hunter Standard
{
	state = 0;
	if (LQ(Small, lstrlen(_0) + (lstrlen(L"\\PC Hunter Standard ??.exe") << 1)))
	{
		state = 32;
		return;
	}
	_TCHAR loadPath[Small] = { 0 }, runPath[Small] = { 0 };
	lstrcat(loadPath, _0);
	for (int i = lstrlen(loadPath) - 1; loadPath[i] != L'\\'; --i)
		loadPath[i] = 0;
	lstrcpy(runPath, loadPath);
	lstrcat(runPath, L"\\PC Hunter Standard\\PC Hunter Standard ");
	lstrcat(runPath, (is64BitOS() ? L"64.exe" : L"32.exe"));
	lstrcat(loadPath, L"PC Hunter Standard.zip");
	if (FindFirstFileExists(runPath, FALSE))
	{
		ShellExecute(NULL, _T("open"), runPath, NULL, NULL, SW_SHOW);
		return;//已有文件直接运行返回
	}
	if (EnSure || (MessageBox(NULL, TEXT("即将发起下载或运行，是否继续？\n下载过程中，请您请耐心等待。"), TEXT("下载 PC Hunter Standard"), MB_YESNO | MB_ICONQUESTION | MB_TOPMOST) == IDYES))
	{
		char _loadPath[Small] = { 0 };
		TcharToChar(loadPath, _loadPath);
		SetFileAttributes(loadPath, FILE_ATTRIBUTE_NORMAL);
		RemoveFile(_loadPath);//程序正常运行会自动解压，如果压缩包存在但解压文件丢失，则为了安全，重新下载
		if (!FileDownload(PCHS, _loadPath))
			return;//文件下载失败
		HZIP hz = OpenZip(loadPath, 0);
		ZIPENTRY ze;
		GetZipItem(hz, -1, &ze);
		int numitems = ze.index;
		for (int i = 0; i < numitems; ++i)
		{
			GetZipItem(hz, i, &ze);
			UnzipItem(hz, i, ze.name);
		}
		CloseZip(hz);//解压文件完成
		SetFileAttributes(loadPath, FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_ARCHIVE);
		if (FindFirstFileExists(runPath, FALSE))
		{
#ifdef _WIN64
			if (LQ(long long int(ShellExecute(NULL, _T("open"), runPath, NULL, NULL, SW_SHOW)), 32))
				state = 35;
#else
			if (LQ(int(ShellExecute(NULL, _T("open"), runPath, NULL, NULL, SW_SHOW)), 32))
				state = 35;
#endif
		}
		else
			state = 14;
	}
	else
	{
		state = 15;
		return;
	}
	return;
}


Function FunctionA()//全屏显示
{
	system("mode con: cols=150");
	HWND hwnd = GetForegroundWindow();
	int cx = GetSystemMetrics(SM_CXSCREEN);//屏幕宽度
	int cy = GetSystemMetrics(SM_CYSCREEN);//屏幕高度
	LONG l_WinStyle = GetWindowLong(hwnd, GWL_STYLE);//获取窗口信息
	DeleteMenu(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE, MF_BYCOMMAND);//禁止窗口被关
	SetWindowLong(hwnd, GWL_STYLE, (l_WinStyle | WS_POPUP | WS_MAXIMIZE) & ~WS_CAPTION & ~WS_THICKFRAME & ~WS_BORDER & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX & ~WS_SIZEBOX & ~WS_SYSMENU);//禁用菜单
	SetWindowPos(hwnd, HWND_TOP, 0, 0, cx, cy, 0);
	if (isAdmin() && FindFirstFileExists(L"DriverLoader.exe", FALSE))
		system("DriverLoader.exe /start ZwTerminateProcess.sys");
	return;
}

Function FunctionB()//打开程序所在目录
{
	char _char[Large] = { 0 }, path[Large] = "explorer.exe \"";
	TcharToChar(_0, _char);
	size_t i = strlen(_char) - 1;
	while ((_char[i]) != '\\')
		i--;
	for (size_t j = 0; j < i; ++j)
		path[j + strlen("explorer.exe \"")] = _char[j];
	system(path);
	Sleep(Rtime);
	return;
}

Function FunctionC()//获取本地进程 token 权限
{
	system("cls");
	if (EnableDebugPriv("SeDebugPrivilege"))
	{
		cout << "获取本地进程 token 权限成功，" << states[28] << endl;
		TkControl = 1;
	}
	else
	{
		if (SetWindowsHookEx(WH_CBT, (HOOKPROC)CBTHookProc2, NULL, GetCurrentThreadId()))
		{
			int errorcontrol = MessageBox(NULL, CCharToLpcwstr(states[state].c_str()), CCharToLpcwstr(states[4].c_str()), MB_ABORTRETRYIGNORE | MB_ICONERROR | MB_TOPMOST);
			if (errorcontrol == 3)
			{
				TkControl = 2;
				const char Weekname[7][3] = { "日", "一", "二", "三", "四", "五", "六" };
				thread t(ThreadControl2);
				time_t timep;
				tm _tm;
				time(&timep);
				localtime_s(&_tm, &timep);
				cout << _tm.tm_year + 1900 << "年" << _tm.tm_mon + 1 << "月" << _tm.tm_mday << "日星期" << Weekname[_tm.tm_wday] << (_tm.tm_hour < 10 ? " " : "") << _tm.tm_hour << ':' << (_tm.tm_min < 10 ? "0" : "") << _tm.tm_min << ':' << (_tm.tm_sec < 10 ? "0" : "") << _tm.tm_sec << "\t事件：提权发起循环，如果在输出信息满屏后提权仍未成功，建议中止循环。" << endl << endl;
				while (TkControl == 2)
				{
					time(&timep);
					localtime_s(&_tm, &timep);
					if (EnableDebugPriv("SeDebugPrivilege"))
					{
						cout << endl << _tm.tm_year + 1900 << "年" << _tm.tm_mon + 1 << "月" << _tm.tm_mday << "日星期" << Weekname[_tm.tm_wday] << (_tm.tm_hour < 10 ? " " : "") << _tm.tm_hour << ':' << (_tm.tm_min < 10 ? "0" : "") << _tm.tm_min << ':' << (_tm.tm_sec < 10 ? "0" : "") << _tm.tm_sec << "\t成功：" << "获取本地进程 token 权限成功，请点击对话框中的“确定”按钮返回。" << endl;
						TkControl = 1;
						t.join();
						t.~thread();
						return;
					}
					else
						cout << _tm.tm_year + 1900 << "年" << _tm.tm_mon + 1 << "月" << _tm.tm_mday << "日星期" << Weekname[_tm.tm_wday] << (_tm.tm_hour < 10 ? " " : "") << _tm.tm_hour << ':' << (_tm.tm_min < 10 ? "0" : "") << _tm.tm_min << ':' << (_tm.tm_sec < 10 ? "0" : "") << _tm.tm_sec << "\t错误：" << states[4] << "详细信息：" << states[state] << endl;
					Sleep(Rtime);
				}
				TkControl = 0;
				t.join();
				t.~thread();
				return;
			}
			else if (errorcontrol == 4)
				FunctionC();
			else
				return;
		}
		else
			cout << '\a' << states[4] << "详细信息：" << states[state] << endl;
	}
	Sleep(Rtime);
	return;
}

Function FunctionD()//修改倒计时
{
	system("cls");
	cout << "说明：倒计时将作用于短暂性停留以及自动刷新进程列表间隔等功能，默认值为 5000 毫秒。" << endl;
	cout << "当前倒计时：" << Rtime << "，如不需要更改请输入该值。" << endl;
	cout << "单位换算：1 分钟 = 60 秒\t1 秒 = 1000 毫秒" << endl;
	cout << "有效范围：" << MinTime << " - " << MaxTime << "，请输入倒计时（单位：毫秒）：";
	rewind(stdin);
	fflush(stdin);
	while (scanf_s("%d", &Rtime) != 1 || Rtime < MinTime || Rtime > MaxTime)
	{
		Rtime = 5000;
		cout << "\a很抱歉，您输入的倒计时有误，请重新输入：";
		rewind(stdin);
		fflush(stdin);
	}
	rewind(stdin);
	fflush(stdin);
	cout << endl << endl;
	if (LT(Rtime, DefaultTime))
	{
		cout << "修改成功！正在数秒，完成后将自动返回。" << endl;
		Sleep(Rtime);
	}
	else
	{
		cout << "修改成功，请按任意键返回。" << endl;
		system("pause>nul");
	}
	return;
}

Function FunctionE()//以管理员权限启动
{
	cout << endl << endl << endl;
	if (!IsAboveVistaVersion(_WIN32_WINNT_WIN7))
	{
		state = 27;
		cout << states[state] << "请按任意键返回。" << endl;
		system("pause>nul");
		return;
	}
	if (GetAdminPermission(_0, NULL))
	{
		cout << "已发起运行，程序将自动退出。" << endl;
		Sleep(Rtime);
		system("cls");
		exit(EXIT_SUCCESS);
	}
	else
	{
		if (state == 15)
			cout << states[state] << "请按任意键返回。" << endl;
		else
			cout << "\a错误：" << states[state] << "请按任意键返回。" << endl;
		system("pause>nul");
	}
	return;
}

Function FunctionF()//颜色选择面板
{
	char commandline[10] = "color ", colorcode[3] = { 0 };
	strcpy_s(colorcode, color_code);
	bool flag = true;
	while (flag)
	{
		for (size_t i = strlen("color "); i < 10; ++i)
			commandline[i] = 0;
		system("cls");
		cout << "可供选择的颜色有：" << endl;
		cout << "\t0 = 黑色\t8 = 灰色" << endl;
		cout << "\t1 = 蓝色\t9 = 淡蓝色" << endl;
		cout << "\t2 = 绿色\tA = 淡绿色" << endl;
		cout << "\t3 = 浅绿色\tB = 淡浅绿色" << endl;
		cout << "\t4 = 红色\tC = 淡红色" << endl;
		cout << "\t5 = 紫色\tD = 淡紫色" << endl;
		cout << "\t6 = 黄色\tE = 淡黄色" << endl;
		cout << "\t7 = 白色\tF = 亮白色" << endl;
		cout << endl << "请输入代号（如：FC 在亮白色上产生淡红色）：" << endl;
		rewind(stdin);
		fflush(stdin);
#ifdef _WIN64
		scanf_s("%s", &colorcode, int(sizeof(colorcode)));
#else
		scanf_s("%s", &colorcode, sizeof(colorcode));
#endif
		rewind(stdin);
		fflush(stdin);
		if (strlen(colorcode) == 1)
		{
			if ((colorcode[0] >= '0' && colorcode[0] <= '9') || (colorcode[0] >= 'A' && colorcode[0] <= 'F') || (colorcode[0] >= 'a' && colorcode[0] <= 'f'))
			{
				strcat_s(commandline, colorcode);
				system(commandline);
			}
			else
			{
				cout << '\a';
				continue;
			}
		}
		else if (strlen(colorcode) == 2)
		{
			if (((colorcode[0] >= '0' && colorcode[0] <= '9') || (colorcode[0] >= 'A' && colorcode[0] <= 'F') || (colorcode[0] >= 'a' && colorcode[0] <= 'f')) && ((colorcode[1] >= '0' && colorcode[1] <= '9') || (colorcode[1] >= 'A' && colorcode[1] <= 'F') || (colorcode[1] >= 'a' && colorcode[1] <= 'f')) && (colorcode[0] != colorcode[1]))
			{
				strcat_s(commandline, colorcode);
				system(commandline);
			}
			else
			{
				cout << '\a';
				continue;
			}
		}
		else
		{
			cout << '\a';
			continue;
		}
		switch (MessageBox(NULL, TEXT("颜色预览如图，是否保留？"), TEXT("颜色设置"), MB_YESNOCANCEL | MB_ICONQUESTION | MB_TOPMOST))
		{
		case 6:
			strcpy_s(color_code, colorcode);
			system("cls");
			flag = false;
			break;
		case 7:
			for (size_t i = strlen("color "); i < 10; ++i)
				commandline[i] = 0;
			strcat_s(commandline, color_code);
			system(commandline);
			system("cls");
			break;
		case 2:
			for (size_t i = strlen("color "); i < 10; ++i)
				commandline[i] = 0;
			strcat_s(commandline, color_code);
			system(commandline);
			flag = false;
			break;
		default:
			continue;
		}
	}
	return;
}

Function FunctionG()//优化程序运行
{
	GetModuleFileName(NULL, _0, Small);
	setlocale(LC_CTYPE, "");
	UpdateMethods();
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);//更改优先级
	SetProcessPriorityBoost(GetCurrentProcess(), FALSE);//锁定优先级
	if (::IsWow64())//关闭重定向
		::WOW64FsDir(TRUE);
	return;
}

Function ArgHelp(_TCHAR* _tchar)//命令行帮助函数
{
	if (_wcsicmp(_tchar, L"/?") == 0 || _wcsicmp(_tchar, L"-?") == 0)
	{
		cout << endl << "描述：综合进程管理器。" << endl << endl;
		cout << "可用首参数列表：" << endl;
		cout << "\tAddPath\t\t将程序添加到 Path" << endl;
		cout << "\tCheck\t\t探测进程自我保护力阶" << endl;
		cout << "\tcmd\t\t将当前终端全屏" << endl;
		cout << "\tDllHook\t\t远程线程注入" << endl;
		cout << "\tDriver\t\t驱动结束进程" << endl;
		cout << "\tdump\t\t搜索并转存指定进程信息" << endl;
		cout << "\tgetPCH\t\t下载并运行 PC Hunter Standard" << endl;
		cout << "\tgoose\t\t启动进程监控（请在反病毒界面启动）" << endl;
		cout << "\tHelp\t\t显示交互模式下的帮助" << endl;
		cout << "\tList\t\t升序枚举当前所有可读进程" << endl;
		cout << "\tNoTopmost\t取消置顶当前窗口" << endl;
		cout << "\tOpenPath\t打开程序所在目录" << endl;
		cout << "\tPermission\t粗略检测当前运行权限" << endl;
		cout << "\tPower\t\t电源相关操作" << endl;
		cout << "\tReverse\t\t倒序枚举当前所有可读进程" << endl;
		cout << "\tRuncmd\t\t启动一个新的全屏终端" << endl;
		cout << "\tStatus\t\t更改进程运行状态" << endl;
		cout << "\tTerminate\t结束进程" << endl;
		cout << "\tTopmost\t\t置顶当前窗口" << endl;
		cout << "\tTreeCheck\t按进程树探测进程自我保护力阶" << endl;
		cout << "\tTreeHook\t按进程树执行远程线程注入" << endl;
		cout << "\tTreeStatus\t按进程树更改进程运行状态" << endl;
		cout << "\t/p\t\t使用 system() 函数延时启动" << endl;
		cout << "\t/ph\t\t隐藏窗口延时启动" << endl;
		cout << "\t/ps\t\t显示窗口延时启动" << endl;
		cout << "\t/x\t\t以管理员权限启动（Windows 7及以上可用）" << endl;
		cout << "\t（无参数）\t进入交互模式（注意：退出后该终端窗口大小不会被还原）" << endl;
		cout << "\t/? 或 -?\t显示此帮助" << endl << endl;
		cout << "返回值说明：" << endl;
		cout << "\t" << "一个十位负数" << "\t操作被终端中止" << endl;
		cout << "\t" << EXIT_DRIVER_ERROR << "\t\t加载驱动失败或与驱动通讯时发生异常" << endl;
		cout << "\t" << EXIT_MEMERROR << "\t\t内存资源难以支持程序运行" << endl;
		cout << "\t" << EXIT_UNSAFELY << "\t\t程序被恶意间谍或运行平台不正确" << endl;
		cout << "\t" << EXIT_OPTIONS_TOO_LESS << "\t\t命令行参数太少" << endl;
		cout << "\t" << EXIT_GRAMMAR_ERROR << "\t\t命令行参数不正确" << endl;
		cout << "\t" << EXIT_SUCCESS << "\t\t枚举操作成功结束" << endl;
		cout << "\t" << EXIT_FAILURE << "\t\t调整自身进程权限过程中发生错误" << endl;
		cout << "\t" << EXIT_WITH_NONE_SELECTED << "\t\t目标对象为空" << endl;
		cout << "\t" << EXIT_FILE_UNEXISTED << "\t\t系统找不到指定文件" << endl;
		cout << "\t" << EXIT_OUT_OF_SCHEDULE << "\t\t进程意外中止" << endl;
		cout << "\t[yyyy 0 xxxx]\t以 0 为界限分割目标数 yyyy 和成功数 xxxx" << endl;
		cout << endl << endl << "注意事项：" << endl;
#ifdef _WIN64
		cout << "\t本程序仅支持 64 位 Windows 操作系统；" << endl;
#else
		cout << "\t本程序仅支持 Windows 操作系统；" << endl;
#endif
		cout << "\t若您希望静默执行，请在命令行后添加“ 1 > nul”；" << endl;
		cout << "\t若您希望分屏查看该命令行帮助，请键入“wmip /?|more”；" << endl;
		cout << "\t若本程序文件名不为“wmip.exe”，请自行更替命令行，这不会影响程序的运行；" << endl;
		cout << "\t本程序支持通配符，通配符说明：“?”代表一个字符，“*”代表不定数目（含 0 ）个字符；" << endl;
		cout << "\t本程序在执行树操作时，不认为进程 System（PID = 4）是进程 System Idle Process（PID = 0）的父进程；" << endl;
		cout << "\t若命令行包含特殊字符，请依照命令行规则进行转义，同时，请参阅“/x”参数和 /p 系列参数的命令行使用帮助；" << endl;
		cout << "\t依照 Microsoft Windows 命令行有关规则，命令行参数中的“/”可替换成“-”，但可能会产生可忽略的微秒级延迟；" << endl;
		cout << "\t依照 Microsoft Windows 有关规则，命令行不区分大小写，但若不严格依照首参数列表，可能会产生可忽略的微秒级延迟；" << endl;
		cout << "\t程序可能会被特征值扫描技术误识别为木马，但程序运行完全依赖于用户，且程序不收集用户的任何隐私数据，请放心放行。" << endl;
		cout << endl << "如需详细信息，请在命令行首参数列表后加“/?”或“-?”，例如：“wmip DllHook /?”将显示首参数 DllHook 的详细用法。" << endl << endl;
		return;
	}
	else if (_wcsicmp(_tchar, L"AddPath") == 0)
		cout << "将程序添加到 Path。" << endl;
	else if (_wcsicmp(_tchar, L"Check") == 0)
	{
		cout << endl << "描述：探测进程自我保护力阶。" << endl << endl;
		cout << "二级参数：" << endl << "\t[Options]\t启用参数" << endl;
		cout << "三级参数：" << endl << "\t/all\t\t指定所有进程（此参数必须放在二级参数后且不能与其它三级参数连用）" << endl << "\t/im\t\t指定映像名称" << endl << "\t[imagename]\t映像名称" << endl << "\t/pid\t\t指定 PID" << endl << "\t[PID]\t\t进程标识符" << endl << endl;
		cout << "用法：" << endl << "\twmip Check [Options] /im [imagename] /pid [PID] ..." << endl << "\twmip Check [Options] /im [imagename] /all" << endl << endl;
		cout << "示例：" << endl << "\twmip Check /11110000 /im cmd.exe" << endl << "\twmip Check /all /im notepad.exe /pid 648" << endl << "\twmip Check Y------- /all" << endl << endl;
		if (MessageBox(NULL, TEXT("显示详细信息吗？"), WmipTextTitle, MB_YESNO | MB_ICONQUESTION | MB_TOPMOST) == IDNO)
			return;
		cout << "序号\t\t攻击力阶\t操作名称" << endl << "==========\t==========\t========================================" << endl << endl;
		for (short int i = 0; i < 8; ++i)
			cout << i + 1 << "\t\t" << AllMethods[i].Strength << "\t\t" << AllMethods[i].MethodName << endl;
		cout << endl;
		if (MessageBox(NULL, TEXT("显示说明吗？"), WmipTextTitle, MB_YESNO | MB_ICONQUESTION | MB_TOPMOST) == IDYES)
		{
			TellForce();
			cout << "以上说明仅适用于本程序，如要探测高自我保护力阶的进程，请键入“wmip getPCH”。" << endl << endl;
		}
	}
	else if (_wcsicmp(_tchar, L"cmd") == 0)
		cout << "将当前终端全屏。" << endl;
	else if (_wcsicmp(_tchar, L"DllHook") == 0)
	{
		cout << endl << "描述：远程线程注入 Dll 模块。" << endl << endl;
		cout << "二级参数：" << endl << "\t[DllPath]\tDll 文件所在路径" << endl;
		cout << "三级参数：" << endl << "\t/all\t\t指定所有进程（此参数必须放在二级参数后且不能与其它三级参数连用）" << endl << "\t/im\t\t指定映像名称" << endl << "\t[imagename]\t映像名称" << endl << "\t/pid\t\t指定 PID" << endl << "\t[PID]\t\t进程标识符" << endl << endl;
		cout << "用法：" << endl << "\twmip DllHook [DllPath] /im [imagename] /pid [PID] ..." << endl << "\twmip DllHook [DllPath] /all" << endl << endl;
		cout << "示例：" << endl << "\twmip DllHook win32u.dll /im \"PC Hunter Standard 64.exe\"" << endl << "\twmip DllHook ntdll.dll /im notepad.exe /pid 648" << endl << "\twmip DllHook user32.dll /all" << endl << endl;
	}
	else if (_wcsicmp(_tchar, L"Driver") == 0)
	{
		cout << endl << "描述：驱动结束进程。" << endl << endl;
		cout << "二级参数：" << endl << "\tTerminate\t通知驱动结束进程" << endl << "\tTest\t\t尝试连接驱动" << endl << endl;
		cout << "三级参数：" << endl << "\t/all\t\t指定所有进程（此参数必须放在二级参数后且不能与其它三级参数连用）" << endl << "\t/im\t\t指定映像名称" << endl << "\t[imagename]\t映像名称" << endl << "\t/pid\t\t指定 PID" << endl << "\t[PID]\t\t进程标识符" << endl << endl;
		cout << "用法：" << endl << "\twmip Driver [Terminate|Test] /im [imagename] /pid [PID] ..." << endl << "\twmip Driver [Terminate|Test] /all" << endl << endl;
		cout << "示例：" << endl << "\twmip Driver Test /all" << endl << "\twmip Driver Terminate /im \"PC Hunter Standard 32.exe\"" << endl << "\twmip Driver Terminate /im notepad.exe /pid 648" << endl << "\twmip Driver Terminate /all" << endl << endl;
	}
	else if (_wcsicmp(_tchar, L"dump") == 0)
	{
		cout << endl << "描述：搜索并转存指定进程信息。" << endl << endl;
		cout << "二级参数：" << endl << "\t[FilePath]\t文件路径" << endl << "\tNULL\t在当前终端列举搜索结果而不转存为文件" << endl;
		cout << "三级参数：" << endl << "\t/all\t\t指定所有进程（此参数必须放在二级参数后且不能与其它三级参数连用）" << endl << "\t/im\t\t指定映像名称" << endl << "\t[imagename]\t映像名称" << endl << "\t/pid\t\t指定 PID" << endl << "\t[PID]\t\t进程标识符" << endl << endl;
		cout << "用法：" << endl << "\twmip dump [[FilePath]|NULL] /im [imagename] /pid [PID] ..." << endl << "\twmip dump [[FilePath]|NULL] /all" << endl << endl;
		cout << "示例：" << endl << "\twmip dump D:\\1.txt /im \"PC Hunter*\" /pid 648" << endl << "\twmip dump NULL /all" << endl << endl;
		cout << "注意事项：" << endl << "\t所有存储将直接覆盖原文件，存储会保存计算机时间；" << endl << "\t存储前，程序将去除属性保护，存储成功后，程序将会对目标文件执行属性保护。" << endl << endl;
	}
	else if (_wcsicmp(_tchar, L"getPCH") == 0)
	{
		cout << "下载并运行 PC Hunter Standard。请在程序所在目录查找下载的 zip 文件。" << endl;
		cout << "若要打开程序所在目录，请键入“wmip OpenPath”。" << endl;
	}
	else if (_wcsicmp(_tchar, L"goose") == 0)
		cout << "启动进程监控，一般地，请在反病毒界面启动。" << endl;
	else if (_wcsicmp(_tchar, L"Help") == 0)
	{
		cout << "这是一款利用数据结构操作 Windows 进程的工具，支持命令行启动。" << endl;
		cout << "联系作者：微信：DazzlingUniverse，QQ：1306561600。" << endl;
	}
	else if (_wcsicmp(_tchar, L"List") == 0)
	{
		cout << endl << "描述：升序枚举当前所有可读进程。" << endl << endl;
		cout << "二级参数：" << endl << "\t/pid\t\t按 PID 升序" << endl << "\t/ppid\t\t按 PPID 升序" << endl << "\t/thread\t\t按 线程数 升序" << endl << "\t/im\t\t按 映像名称 升序" << endl << "\t/system\t\t按 系统输出 升序" << endl << endl;
		cout << "用法：" << endl << "\twmip List [/pid|/ppid|/thread|/im|/system]" << endl << endl;
		cout << "示例：" << endl << "\twmip List /pid" << endl << "\twmip List /ppid" << endl << endl;
		cout << "若要使用 PC Hunter Standard 查看检纵进程，请键入“wmip getPCH”。" << endl << endl;
	}
	else if (_wcsicmp(_tchar, L"NoTopmost") == 0)
		cout << "取消置顶当前窗口。" << endl;
	else if (_wcsicmp(_tchar, L"OpenPath") == 0)
		cout << "打开程序所在目录。" << endl;
	else if (_wcsicmp(_tchar, L"Permission") == 0)
		cout << "用于检测当前终端是否具备管理员权限，仅针对 Windows 7及以上。" << endl;
	else if (_wcsicmp(_tchar, L"Power") == 0)
	{
		cout << endl << "描述：电源相关操作（请谨慎使用）。" << endl << endl;
#if (defined _WIN64 || defined WIN64)
		cout << "二级参数：" << endl << "\t/fs\t强制关机" << endl << "\t/fr\t强制重启" << endl << "\t/fe\t发动蓝屏" << endl << endl;
		cout << "用法：" << endl << "\twmip Power [/fs|/fr|/fe]" << endl << endl;
		cout << "示例：" << endl << "\twmip Power /fs" << endl << "\twmip Power /fr" << endl << "\twmip Power /fe" << endl << endl;
#else
		cout << "二级参数：" << endl << "\t/fs\t强制关机" << endl << "\t/fr\t强制重启" << endl << "\t/fe\t发动蓝屏" << endl << "\t/dr\t驱动重启" << endl << "\t/ds\t驱动关机" << endl << "\t/de\t驱动蓝屏" << endl << endl;
		cout << "用法：" << endl << "\twmip Power [/fs|/fr|/fe|/dr|/ds|/de]" << endl << endl;
		cout << "示例：" << endl << "\twmip Power /fs" << endl << "\twmip Power /fr" << endl << "\twmip Power /fe" << endl << "\twmip Power /dr" << endl << "\twmip Power /ds" << endl << "\twmip Power /de" << endl << endl;
#endif
	}
	else if (_wcsicmp(_tchar, L"Reverse") == 0)
	{
		cout << endl << "描述：降序枚举当前所有可读进程。" << endl << endl;
		cout << "二级参数：" << endl << "\t/pid\t\t按 PID 降序" << endl << "\t/ppid\t\t按 PPID 降序" << endl << "\t/thread\t\t按 线程数 降序" << endl << "\t/im\t\t按 映像名称 降序" << endl << "\t/system\t\t按 系统输出 降序" << endl << endl;
		cout << "用法：" << endl << "\twmip Reverse [/pid|/ppid|/thread|/im|/system]" << endl << endl;
		cout << "示例：" << endl << "\twmip Reverse /pid" << endl << "\twmip Reverse /ppid" << endl << endl;
		cout << "若要使用 PC Hunter Standard 查看检纵进程，请键入“wmip getPCH”。" << endl << endl;
	}
	else if (_wcsicmp(_tchar, L"Runcmd") == 0)
		cout << "全屏启动新的终端。" << endl;
	else if (_wcsicmp(_tchar, L"Status") == 0)
	{
		cout << endl << "描述：更改进程运行状态。" << endl << endl;
		cout << "二级参数：" << endl << "\tSuspend\t\t暂停进程运行" << endl << "\tResume\t\t恢复进程运行" << endl << "\tSuspendAll\t暂停进程的所有线程运行" << endl << "\tResumeAll\t恢复进程的所有线程运行" << endl;
		cout << "三级参数：" << endl << "\t/all\t\t指定所有进程（此参数必须放在二级参数后且不能与其它三级参数连用）" << endl << "\t/im\t\t指定映像名称" << endl << "\t[imagename]\t映像名称" << endl << "\t/pid\t\t指定 PID" << endl << "\t[PID]\t\t进程标识符" << endl << endl;
		cout << "用法：" << endl << "\twmip Status [Suspend|Resume|SuspendAll|Resumeall] /im [imagename] /pid [PID] ..." << endl << "\twmip Status [Suspend|Resume|SuspendAll|Resumeall] /all" << endl << endl;
		cout << "示例：" << endl << "\twmip Status Suspend /pid 4" << endl << "\twmip Status Resume /im cmd.exe" << endl << "\twmip Status SuspendAll /im QQ.exe" << endl << "\twmip Status ResumeAll /pid 768" << endl << endl;
	}
	else if (_wcsicmp(_tchar, L"Terminate") == 0)
	{
		cout << endl << "描述：结束进程。" << endl << endl;
		cout << "二级参数：" << endl << "\t/t\t\t结束进程树" << endl << "\t/n\t\t仅结束选定进程" << endl << "\t/w\t\t作关闭窗口处理而非结束进程" << endl;
		cout << "三级参数：" << endl << "\t/all\t\t指定所有进程（此参数必须放在二级参数后且不能与其它三级参数连用）" << endl << "\t/im\t\t指定映像名称" << endl << "\t[imagename]\t映像名称" << endl << "\t/pid\t\t指定 PID" << endl << "\t[PID]\t\t进程标识符" << endl << endl;
		cout << "用法：" << endl << "\twmip Terminate [/t|/n|/w] /im [imagename] /pid [PID] ..." << endl << "\twmip Terminate [/t|/n|/w] /all" << endl << endl;
		cout << "示例：" << endl << "\twmip Terminate /n /im \"PC Hunter*\" /pid 648" << endl << "\twmip Terminate /t /im cmd.exe" << endl << "\twmip Terminate /n /all" << endl << "\twmip Terminate /w /im notepad.exe /pid 768" << endl << endl;
	}
	else if (_wcsicmp(_tchar, L"Topmost") == 0)
		cout << "置顶当前窗口。" << endl;
	else if (_wcsicmp(_tchar, L"TreeCheck") == 0)
	{
		cout << "描述：按进程树探测进程自我保护力阶。" << endl;
		cout << "命令语法与首参数“Check”相似，请键入“wmip Check /?”了解相关参数。" << endl;
	}
	else if (_wcsicmp(_tchar, L"TreeHook") == 0)
	{
		cout << "描述：按进程树注入 Dll 模块。" << endl;
		cout << "命令语法与首参数“DllHook”相似，请键入“wmip DllHook /?”了解相关参数。" << endl;
	}
	else if (_wcsicmp(_tchar, L"TreeStatus") == 0)
	{
		cout << "描述：按进程树更改进程运行状态。" << endl;
		cout << "命令语法与首参数“Status”相似，请键入“wmip Status /?”了解相关参数。" << endl;
	}
	else if ((_wcsicmp(_tchar, L"/p") == 0 || _wcsicmp(_tchar, L"-p") == 0) || (_wcsicmp(_tchar, L"/ph") == 0 || _wcsicmp(_tchar, L"-ph") == 0) || (_wcsicmp(_tchar, L"/ps") == 0 || _wcsicmp(_tchar, L"-ps") == 0))
	{
		cout << endl << "描述：延时执行，但忽略返回信息。" << endl << endl;
		cout << "执行参数：" << endl << "\t[timing]\t倒计时" << endl << "\t[options]\t其它命令行参数" << endl << endl;
		cout << "用法：" << endl << "\twmip [/p|/ph|/ps] [timing] [option1, option2, option3, ...]" << endl << endl;
		cout << "示例：" << endl << "\twmip /p 10 List" << endl << "\twmip /ph 30 Terminate /w /im cmd.exe" << endl << "\twmip /ps /x Status SuspendAll /all" << endl << endl;
		cout << "注意事项：" << endl << "\t命令行中，每包含一个 /p 系列参数，请依照命令行规则对所有特殊字符转义一次。" << endl << "\t若您不希望在当前窗口运行倒计时，可使用类似“wmip /ph 0 /p 10 Power /fs”的搭配。" << endl << endl;
	}
	else if (_wcsicmp(_tchar, L"/x") == 0 || _wcsicmp(_tchar, L"-x") == 0)
	{
		cout << endl << "描述：在新的终端提权启动，仅针对 Windows 7及以上。" << endl << endl;
		cout << "执行参数：" << endl << "\t[options]\t其它命令行参数" << endl << endl;
		cout << "用法：" << endl << "\twmip /x [option1, option2, option3, ...]" << endl << endl;
		cout << "示例：" << endl << "\twmip /x list /system" << endl << "\twmip /x DllHook ntdll.dll /im \"PC Hunter Standard 32.exe\"" << endl << "\twmip /x" << endl << endl;
		cout << "注意事项：" << endl << "\t命令行中，每包含一个“/x”参数，请依照命令行规则对所有特殊字符转义一次。" << endl << endl;
	}
	else
	{
		cout << "\a无效查询—“";
		wcout << _tchar;
		cout << "”。" << endl << "有关用法，请键入“wmip /?”或“wmip -?”。" << endl;
	}
	return;
}

Function FunctionI(bool EnSure)//添加程序到环境变量中
{
	state = 0;
	char mywmip[Small] = { 0 }, CommandLine[Small << 1] = "call echo F|xcopy \"";
	TcharToChar(_0, mywmip);
	strcat_s(CommandLine, mywmip);
	strcat_s(CommandLine, "\" \"");
	for (size_t i = strlen(mywmip) - 1; mywmip[i] != '\\'; --i)//保留目录形式
		mywmip[i] = 0;
	strcat_s(CommandLine, mywmip);
	strcat_s(CommandLine, "wmip.exe\" /C /H /K /V /Y");
	if (_0[lstrlen(_0) - 9] == '\\' && (_0[lstrlen(_0) - 8] == 'W' || _0[lstrlen(_0) - 8] == 'w') && (_0[lstrlen(_0) - 7] == 'M' || _0[lstrlen(_0) - 7] == 'm')
		&& (_0[lstrlen(_0) - 6] == 'I' || _0[lstrlen(_0) - 6] == 'i') && (_0[lstrlen(_0) - 5] == 'P' || _0[lstrlen(_0) - 5] == 'p')
		)
		cout << "文件名已是 wmip.exe，程序继续运行。";
	else
		system(CommandLine);
	if (!isAdmin())
	{
		state = 40;
		MessageBox(NULL, CCharToLpcwstr(states[state].c_str()), WmipTextTitle, MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
		cout << states[state] << endl;
		return;
	}
	HKEY hKey, _hKey;
	const _TCHAR* key = L"System\\CurrentControlSet\\Control\\Session Manager\\Environment";
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, key, 0, KEY_READ | KEY_WRITE, &hKey) != ERROR_SUCCESS)
	{
		state = 36;
		MessageBox(NULL, CCharToLpcwstr(states[state].c_str()), WmipTextTitle, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return;
	}
	char data[Large] = { 0 };
	DWORD dwType = REG_SZ, dwSize = sizeof(data), _dwType = REG_SZ, _dwSize = sizeof(data);
	if (RegQueryValueExA(hKey, "Path", NULL, &dwType, (LPBYTE)data, &dwSize) != ERROR_SUCCESS)
	{
		state = 37;
		MessageBox(NULL, CCharToLpcwstr(states[state].c_str()), WmipTextTitle, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return;
	}
	if (LT(strlen(data), strlen("C:\\Windows\\System32\\") << 1))//至少包含系统变量
		for (int i = 1; i < Large >> 1; ++i)//处理可能存在的数据异常
			if (data[i << 1] != 0)
				data[i] = data[i << 1];
	if (LT(strlen(data), strlen("C:\\Windows\\System32\\") << 1))
	{
		state = 37;
		MessageBox(NULL, CCharToLpcwstr(states[state].c_str()), WmipTextTitle, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return;
	}
	cout << endl << endl << "当前环境变量：" << endl << data << endl << endl << endl;
	string str1(mywmip);
	string str2(data);
	if (str2.find(str1) != string::npos)
	{
		state = 39;
		MessageBox(NULL, CCharToLpcwstr(states[state].c_str()), WmipTextTitle, MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
		return;
	}
	if (!(EnSure || (MessageBox(NULL, TEXT("将会向环境中添加本程序所在路径，是否继续？\n由于环境变量复杂，程序不提供撤回操作。"), TEXT("下载 PC Hunter Standard"), MB_YESNO | MB_ICONQUESTION | MB_TOPMOST) == IDYES)))
	{
		state = 15;
		return;
	}
	if (strlen(data) >= Large)
	{
		state = 38;
		MessageBox(NULL, CCharToLpcwstr(states[state].c_str()), WmipTextTitle, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return;
	}
	if (data[strlen(data) - 1] != ';')//如果最后一个字符不是分号
		strcat_s(data, ";");
	strcat_s(data, mywmip);
	strcat_s(data, ";");
	if (RegSetValueExA(hKey, "Path", NULL, REG_SZ, (const unsigned char*)data, (DWORD)strlen(data)) != ERROR_SUCCESS)
	{
		state = 38;
		MessageBox(NULL, CCharToLpcwstr(states[state].c_str()), WmipTextTitle, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return;
	}
	RegCloseKey(hKey);
	cout << "正在尝试读取新的环境变量，请求注册表中，请稍候。" << endl << endl;
	for (int i = 0; i < Large; ++i)
		data[i] = 0;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, key, 0, KEY_READ | KEY_WRITE, &_hKey) != ERROR_SUCCESS)//以下部分不需要设置 state
	{
		MessageBox(NULL, CCharToLpcwstr(states[36].c_str()), WmipTextTitle, MB_OK | MB_ICONWARNING | MB_TOPMOST);
		return;
	}
	if (RegQueryValueExA(_hKey, "Path", NULL, &_dwType, (LPBYTE)data, &_dwSize) != ERROR_SUCCESS)
	{
		MessageBox(NULL, CCharToLpcwstr(states[37].c_str()), WmipTextTitle, MB_OK | MB_ICONWARNING | MB_TOPMOST);
		return;
	}
	if (LT(strlen(data), strlen("C:\\Windows\\System32\\") << 1))//至少包含两个系统变量
		for (int i = 1; i < Large >> 1; ++i)//处理可能存在的数据异常
			if (data[i << 1] != 0)
				data[i] = data[i << 1];
	if (LT(strlen(data), strlen("C:\\Windows\\System32\\") << 1))
	{
		MessageBox(NULL, CCharToLpcwstr(states[37].c_str()), WmipTextTitle, MB_OK | MB_ICONWARNING | MB_TOPMOST);
		return;
	}
	cout << "当前环境变量：" << endl << data << endl << endl << endl;
	RegCloseKey(_hKey);
	if (state >= 36 && state <= 38)
		cout << "错误：";
	cout << states[state];
	return;
}


/* 反病毒专用函数 */
Goose DriverNoProcess()//驱动级禁止创建进程
{
	if (Reminder == 0)
		return;
	hHook = SetWindowsHookEx(WH_CBT, (HOOKPROC)CBTHookProc3, NULL, GetCurrentThreadId());
	if (MessageBox(NULL, (hHook ?
		TEXT("如果您开启了驱动级进程自我保护，请忽略此消息。\n在程序运行后，一个强大的驱动级程序配置了禁止创建进程或线程。\n此时，界面文字将无法正常显示，部分功能将无法正常使用。\n如果您使用了 PC Hunter Standard 禁止创建进程，请您取消配置。\n如果这不是您的主动操作，您的计算机可能已经中了 rookit 木马。") :
		TEXT("如果您开启了驱动级进程自我保护，请忽略此消息。\n在程序运行后，一个强大的驱动级程序配置了禁止创建进程或线程。\n此时，界面文字将无法正常显示，部分功能将无法正常使用。\n如果您使用了 PC Hunter Standard 禁止创建进程，请您取消配置。\n如果这不是您的主动操作，您的计算机可能已经中了 rookit 木马。\n提示：点击“取消”可不再显示此消息。")),
		WmipTextTitle, MB_OKCANCEL | MB_ICONWARNING | MB_TOPMOST) == IDCANCEL)
		Reminder = 0;
	return;
}

int CountTreeNode(TreeNode isVirus, int EdCount)//节点计数
{
	int isV_count = EdCount;
	if (isVirus.FirstChild)
	{
		isV_count++;
		CountTreeNode(*isVirus.FirstChild, isV_count);
	}
	if (isVirus.NextSibling)
	{
		isV_count++;
		CountTreeNode(*isVirus.NextSibling, isV_count);
	}
	return isV_count;
}

Goose Goose1()//进程树监控
{
	if (!isAdequateMem(MemAtLeast[0]))
	{
		MessageBox(NULL, TEXT("很抱歉，内存资源不足，进程防御开启失败！"), WmipTextTitle, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return;
	}
	for (;;)
	{
		GetProcess();
		for (int i = 0; i < Lists.ProcessCount; ++i)
		{
			tree.RootNode[i].data = Lists.PNode[i];
			BuildTree(tree.RootNode[i]);
			if (CountTreeNode(tree.RootNode[i], NULL) > 10)
			{
				HANDLE hdle = OpenProcess(PROCESS_ALL_ACCESS, TRUE, tree.RootNode[i].data.ProcessIDNode);
				if (hdle)
					TerminateProcess(hdle, NULL);
			}
		}
	}
	return;
}

Goose Goose2()//进程时监控
{
	/* 初始化进程监控 */
	if (!InitNode())
	{
		MessageBox(NULL, TEXT("很抱歉，内存资源不足，进程防御开启失败！"), WmipTextTitle, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return;
	}
	FILE* fpp;
	char LogP[MAX_PATH] = { 0 };
	TcharToChar(_0, LogP);
	for (size_t i = strlen(LogP) - 1; i > 0; --i)
		if (LogP[i] == '\\')
			break;
		else
			LogP[i] = 0;
	strcat_s(LogP, LogName);
	fopen_s(&fpp, LogP, "at+");
	if (fpp)
	{
		fprintf_s(fpp, "进程防御已启动！（02）\n");
		fclose(fpp);
	}
	TimeNodes* i = TimeNode, * j = TimeNode, * k = TimeNode;
	if (!(i && j))
	{
		MessageBox(NULL, TEXT("程序运行异常，进程防御未完全生效！"), WmipTextTitle, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return;
	}
	
	/* 核心代码 */
	for (;;)
	{
		/* 实时监控 */
		TimeNodes* pCur = TimeNode;
		while (pCur)
		{
			bool isExit = true;
			HANDLE hProceessnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			PROCESSENTRY32 pe32;
			pe32.dwSize = sizeof(pe32);
			BOOL hProcess = Process32First(hProceessnap, &pe32);
			while (hProcess)
			{
				if (pe32.th32ProcessID != 0 && !isInList(pe32.th32ProcessID))
				{
					AddNode(pe32.th32ProcessID, pe32.th32ParentProcessID, CrowdTime, pe32.szExeFile);
					fopen_s(&fpp, LogP, "at+");
					if (fpp)
					{
						fwprintf_s(fpp, L"进程启动\t\tTime = %s\t\tPID = %d\t\tPPID = %d\t\tName = \"%s\"\n", getLocalTime(), pe32.th32ProcessID, pe32.th32ParentProcessID, pe32.szExeFile);
						fclose(fpp);
					}
				}
				if (isExit && pe32.th32ProcessID == pCur->data.ProcessID)
					isExit = false;
				hProcess = Process32Next(hProceessnap, &pe32);
			}
			if (isExit && pCur != TimeNode)
			{
				fopen_s(&fpp, LogP, "at+");
				if (fpp)
				{
					fwprintf_s(fpp, L"进程退出\t\tTime = %s\t\tPID = %d\t\tPPID = %d\t\tName = \"%s\"\n", getLocalTime(), pe32.th32ProcessID, pe32.th32ParentProcessID, pe32.szExeFile);
					fclose(fpp);
				}
				TimeNodes* tmpCur = pCur->next;
				if (j == pCur)//防止最新进程退出造成 0xDDDDDDDD 类错误
					j = TimeNode;
				if (i == pCur)
					i = TimeNode;
				DeleteNode(pCur);
				pCur = tmpCur;
			}
			else
				pCur = pCur->next;
		}

		/* 处理恶意程序 */
		if (!j)
			j = TimeNode;
		if (!i)
			i = TimeNode;
		while (j && j->next)
			j = j->next;
		if (j->data.Time <= 3000)//没有新进程
			continue;
		while ((i->data.Time <= 1000 || CrowdTime - i->data.Time > 1000) && (i && i->next) && j->data.Time > i->data.Time)
			i = i->next;
		int tmpDisc = 1;//两个指针在一起时有一个进程
		k = i;
		while (k && k->next && k != j)
		{
			k = k->next;
			tmpDisc++;
		}
		if (tmpDisc >= 10)
		{
			int tmpPPID[Small] = { i->data.PPID };//父进程
			int countPPID[Small] = { 1 };//共同父进程数
			int qq = 0;//可疑进程数的索引值
			for (k = i; k && k != j; k = k->next)
			{
				bool flag = true;//是否新的父进程
				for (int pp = 0; tmpPPID[pp] != 0; ++pp)
					if (tmpPPID[pp] == k->data.PPID)
					{
						flag = false;
						countPPID[pp]++;
					}
				if (flag)
				{
					tmpPPID[++qq] = k->data.PPID;
					countPPID[qq] = 1;
				}
				fopen_s(&fpp, LogP, "at+");
				HANDLE hdle = OpenProcess(PROCESS_ALL_ACCESS, TRUE, k->data.ProcessID);
				if (hdle)
				{
					TerminateProcess(hdle, NULL);
					if (fpp)
					{
						fwprintf_s(fpp, L"结束进程\t\tTime = %s\t\tPID = %d\t\tPPID = %d\t\tName = \"%s\"\n", getLocalTime(), k->data.ProcessID, k->data.PPID, k->data.ImgNmae);
						fclose(fpp);
					}
				}
				else
				{
					if (fpp)
					{
						fwprintf_s(fpp, L"错误：结束进程失败！\t\tTime = %s\t\tPID = %d\t\tPPID = %d\t\tName = \"%s\"\n", getLocalTime(), k->data.ProcessID, k->data.PPID, k->data.ImgNmae);
						fclose(fpp);
					}
				}
			}
			for (int pp = 0; tmpPPID[pp] != 0; ++pp)
				if (countPPID[pp] >= 10)
				{
					fopen_s(&fpp, LogP, "at+");
					HANDLE hdle = OpenProcess(PROCESS_ALL_ACCESS, TRUE, tmpPPID[pp]);
					if (hdle)
					{
						TerminateProcess(hdle, NULL);
						if (fpp)
						{
							fwprintf_s(fpp, L"结束进程树\t\tTime = %llu\t\tPID = %d\t\t可疑父进程（创建子进程 %d 个）\n", CrowdTime, tmpPPID[pp], countPPID[pp]);
							fclose(fpp);
						}
						HANDLE hProceessnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
						PROCESSENTRY32 pe32;
						pe32.dwSize = sizeof(pe32);
						BOOL hProcess = Process32First(hProceessnap, &pe32);
						while (hProcess)
						{
							if (tmpPPID[pp] && pe32.th32ParentProcessID == tmpPPID[pp])
							{
								HANDLE chdle = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pe32.th32ParentProcessID);
								if (chdle)
								{
									TerminateProcess(chdle, NULL);
									if (fpp)
										fwprintf_s(fpp, L"\t结束子进程\t\tTime = %llu\t\tPID = %d\t\tPPID = %d\t\tName = \"%s\"\n", CrowdTime, pe32.th32ProcessID, pe32.th32ParentProcessID, pe32.szExeFile);
								}
								else
								{
									if (fpp)
										fwprintf_s(fpp, L"\t子错误：结束子进程失败！\t\tTime = %llu\t\tPID = %d\t\tPPID = %d\t\tName = \"%s\"\n", CrowdTime, pe32.th32ProcessID, pe32.th32ParentProcessID, pe32.szExeFile);
								}
							}
							hProcess = Process32Next(hProceessnap, &pe32);
						}
					}
					else
					{
						if (fpp)
						{
							fwprintf_s(fpp, L"错误：结束进程树失败！\t\tTime = %llu\t\tPID = %d\t\t可疑父进程（创建子进程 %d 个）\n", CrowdTime, tmpPPID[pp], countPPID[pp]);
							fclose(fpp);
						}
					}
				}
		}
	}
	return;
}

Goose ThreadTiming()
{
	while (RunTimeThread)
	{
		Sleep(10);
		CrowdTime += 10;
	}
	return;
}

Goose Goose0()
{
	Sleep(gapTime);//开机启动延时
	RunTimeThread = true;
	thread m0(ThreadTiming);//计时器
	thread m1(Goose1);//进程树监控
	thread m2(Goose2);//进程时监控
	Sleep(Rtime);//延迟启动
	while (!(m1.joinable() && m2.joinable()))
		Sleep(Rtime);
	//RunTimeThread = false;
	m1.join();
	m2.join();
	while (!m0.joinable());
	m0.join();
	return;
}



/* 遵循 Microsoft Windows 命令行规则的 main 函数 */
int MainActivity(_TCHAR* argg)
{
	if (system("chcp 936&title wmip&color e&cls") != 0)
		DriverNoProcess();
	FunctionA();
	thread protecting(ThreadControl3);
	int temp = GetLastError();
	protecting.detach();
	if (EnableDebugPriv("SeDebugPrivilege"))
		TkControl = 1;
	else if (MessageBox(NULL, CCharToLpcwstr((states[4] + string("\n详细信息：") + states[state]).c_str()), TEXT("是否继续？"), MB_YESNO | MB_ICONERROR | MB_TOPMOST) == IDNO)
		return EXIT_FAILURE;
	char choice = 'A';
	_TCHAR THtmp[5] = _T("/?");
	do
	{
		if (system("cls") != 0)
			DriverNoProcess();
		state = 0;//归还 state 状态
		cout << "/******************** wmip ********************/" << endl << endl;
		cout << "主要功能：" << endl;
		cout << "\t1 = 遍历所有进程" << endl;
		cout << "\t2 = 操作选定进程" << endl;
		cout << "\t3 = 操作所有进程（风险）" << endl;
		cout << (protect == 0 ? "\t4 = 加载 API层 自我保护（耗能）" : "\t4 = 关闭 API层 自我保护") << endl;
		cout << "\t5 = 全屏开启终端" << endl;
		cout << "\t6 = 电源相关操作（高风险）" << endl;
		cout << "\t7 = 设置强度递增结束方式" << endl;
		cout << "\t8 = 设置延迟执行" << endl;
		cout << "\t9 = PC Hunter Standard" << endl;
		cout << "\t0 = 退出程序" << endl << endl;
		cout << "辅助功能：" << endl;
		cout << "\tA = 刷新界面" << endl;
		cout << "\tB = 打开程序所在目录" << endl;
		cout << "\tC = 获取本地进程 token 权限" << endl;
		cout << "\tD = 修改倒计时" << endl;
		cout << "\tE = 以管理员权限启动" << endl;
		cout << "\tF = 更改颜色" << endl;
		cout << "\tG = 优化程序运行" << endl;
		cout << "\tH = 显示关于" << endl;
		cout << "\tI = 将程序添加到 Path" << endl;
		cout << "\t? = 显示命令行帮助" << endl;
		cout << endl << endl;
		cout << "请选择一项以继续：";
		rewind(stdin);
		fflush(stdin);
		scanf_s("%c", &choice, 1);
		rewind(stdin);
		fflush(stdin);
		if (!isAdequateMem(MemAtLeast[0]))
		{
			MessageBox(NULL, TEXT("内存资源不足，程序无法正常运行。"), WmipTextTitle, MB_OK | MB_ICONERROR | MB_TOPMOST);
			return EXIT_MEMERROR;
		}
		switch (choice)
		{
		case '0':
			if (MessageBox(NULL, TEXT("确定要退出吗？欢迎再次使用哦！"), WmipTextTitle, MB_OKCANCEL | MB_ICONQUESTION | MB_TOPMOST) == IDOK)
				choice = 0;
			break;
		case '1':
			Function1();
			break;
		case '2':
			Function2();
			break;
		case '3':
			Function3();
			break;
		case '4':
			Function4();
			break;
		case '5':
			Function5(false);
			break;
		case '6':
			Function6();
			break;
		case '7':
			Function7();
			break;
		case '8':
			Function8(argg);
			break;
		case '9':
			Function9(false);
			if (state != 0 && state != 15 && state != 35)
				MessageBox(NULL, CCharToLpcwstr(states[state].c_str()), WmipTextTitle, MB_OK | MB_ICONWARNING | MB_TOPMOST);
			break;
		case 'A':
		case 'a':
			FunctionA();
			break;
		case 'B':
		case 'b':
			FunctionB();
			break;
		case 'C':
		case 'c':
			FunctionC();
			break;
		case 'D':
		case 'd':
			FunctionD();
			break;
		case 'E':
		case 'e':
			FunctionE();
			break;
		case 'F':
		case 'f':
			FunctionF();
			break;
		case 'G':
		case 'g':
			system("cls");
			cout << "优化仅对本程序本次运行期间有效，开始执行优化，请稍候。" << endl;
			FunctionG();
			cout << "优化完成，请按任意键返回。" << endl;
			system("pause>nul");
			break;
		case 'H':
		case 'h':
			system("cls");
			lstrcpy(THtmp, L"Help");
			ArgHelp(THtmp);
			cout << endl << endl << "显示关于完成，请按任意键返回。" << endl;
			system("pause>nul");
			break;
		case 'I':
		case 'i':
			system("cls");
			FunctionI(false);
			cout << "请按任意键返回。" << endl;
			system("pause>nul");
			break;
		case '?':
			system("cls&set /p=%COMSPEC:~0,-8%<nul");
			cout << ">\"";
			wcout << argg;
			cout << "\" /?" << endl;
			lstrcpy(THtmp, L"/?");
			ArgHelp(THtmp);
			cout << endl << endl << "显示帮助完成，请按任意键返回。" << endl;
			system("pause>nul");
			break;
		default:
			continue;
		}
	} while (choice != 0);
	system("cls");
	protecting.~thread();
	return EXIT_SUCCESS;
}

int _tmain(int argc, _TCHAR* argv[])//主函数
{
	FunctionG();
	if (argc == 1)
		return MainActivity(argv[0]);
	else if (argc > 1)
	{
		int i, result = 0;
		for (i = 1; i < argc; ++i)
		{
			if (_wcsicmp(argv[i], L"/?") == 0 || _wcsicmp(argv[i], L"-?") == 0)
			{
				ArgHelp(argv[1]);
				return EXIT_SUCCESS;
			}
		}
		if (_wcsicmp(argv[1], L"Goose") == 0)
		{
			if (IsAboveVistaVersion(_WIN32_WINNT_WIN7) && !isAdmin())
			{
				state = 0;
				GetAdminPermission(_0, argv[1]);
				switch (state)
				{
				case 0:
					cout << "提示：提权操作成功结束。" << endl;
					return EXIT_SUCCESS;
				case 15:
					cout << "提示：用户取消操作。" << endl;
					return EXIT_SUCCESS;
				case 14:
					cout << "\a错误：系统找不到指定文件。" << endl;
					return EXIT_FILE_UNEXISTED;
				default:
					cout << "\a错误：无法完成操作，拒绝访问。" << endl;
					return EXIT_FAILURE;
				}
			}
			Goose0();
			return EXIT_SUCCESS;
		}
		if (_wcsicmp(argv[1], L"cmd") == 0)
		{
			Function5(true);
			return EXIT_FAILURE;
		}
		else if (_wcsicmp(argv[1], L"Runcmd") == 0)
		{
			if (CmdConsole())
			{
				cout << states[0] << endl;
				return EXIT_SUCCESS;
			}
			else
			{
				cout << "\a错误：" << states[26] << endl;
				return EXIT_FAILURE;
			}
		}
		else if (_wcsicmp(argv[1], L"Topmost") == 0)
		{
			if (SetWindowPos(GetForegroundWindow(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE) == 1)
			{
				cout << states[0] << endl;
				return EXIT_SUCCESS;
			}
			else
			{
				cout << "\a错误：" << states[23] << endl;
				return EXIT_FAILURE;
			}
		}
		else if (_wcsicmp(argv[1], L"NoTopmost") == 0)
		{
			if (SetWindowPos(GetForegroundWindow(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE) == 1)
			{
				cout << states[0] << endl;
				return EXIT_SUCCESS;
			}
			else
			{
				cout << "\a错误：" << states[23] << endl;
				return EXIT_FAILURE;
			}
		}
		else if (_wcsicmp(argv[1], L"/x") == 0 || _wcsicmp(argv[1], L"-x") == 0)
		{
			if (!IsAboveVistaVersion(_WIN32_WINNT_WIN7))
			{
				state = 27;
				cout << states[state] << endl;
				return EXIT_SUCCESS;
			}
			TCHAR Commandline[Large] = L"/k call \"";
			if (argc > 2)
			{
				result = argc + lstrlen(Commandline);
				for (i = 0; i < argc && result < Large - 1; ++i)
					result += lstrlen(argv[i]) + (isIncludeSpace(argv[i]) ? 2 : 0);
				if (LQ(Large - 1, result))
				{
					state = 26;
					cout << "\a错误：" << states[state] << endl;
					return EXIT_FAILURE;
				}
				lstrcat(Commandline, argv[0]);
				lstrcat(Commandline, L"\"");
				for (i = 2; i < argc; ++i)
				{
					if (isIncludeSpace(argv[i]))
					{
						lstrcat(Commandline, L" \"");
						lstrcat(Commandline, argv[i]);
						lstrcat(Commandline, L"\"");
					}
					else
					{
						lstrcat(Commandline, L" ");
						lstrcat(Commandline, argv[i]);
					}
				}
			}
			TCHAR Terminal[8] = L"cmd.exe";
			if (argc > 2 ? GetAdminPermission(Terminal, Commandline) : GetAdminPermission(argv[0], NULL))
			{
				cout << states[state] << endl;
				return EXIT_SUCCESS;
			}
			else
			{
				if (state == 15)
					cout << states[state] << endl;
				else
					cout << "\a错误：" << states[state] << endl;
				return EXIT_FAILURE;
			}
		}
		else if ((_wcsicmp(argv[1], L"/p") == 0 || _wcsicmp(argv[1], L"-p") == 0) || (_wcsicmp(argv[1], L"/ph") == 0 || _wcsicmp(argv[1], L"-ph") == 0) || (_wcsicmp(argv[1], L"/ps") == 0 || _wcsicmp(argv[1], L"-ps") == 0))
			return DelayRun(argc, argv);
		else if (_wcsicmp(argv[1], L"Help") == 0)
		{
			ArgHelp(argv[1]);
			return EXIT_SUCCESS;
		}
		else if (_wcsicmp(argv[1], L"Permission") == 0)
		{
			if (IsAboveVistaVersion(_WIN32_WINNT_WIN7))
			{
				cout << (isAdmin() ? "当前运行权限：管理员权限。" : "当前运行权限：标准用户权限。") << endl;
				return EXIT_SUCCESS;
			}
			else
			{
				state = 27;
				cout << states[state] << endl;
				return EXIT_FAILURE;
			}
		}
		else if (_wcsicmp(argv[1], L"getPCH") == 0)
		{
			Function9(true);
			if (state == 15 || state == 35)
			{
				cout << states[state] << endl;
				return EXIT_SUCCESS;
			}
			else if (state == 0)
			{
				cout << states[state] << endl;
				return EXIT_SUCCESS;
			}
			else
			{
				cout << "\a错误：" << states[state] << endl;
				return EXIT_FAILURE;
			}
		}
		else if (_wcsicmp(argv[1], L"OpenPath") == 0)
		{
			FunctionB();
			cout << states[0] << endl;
			return EXIT_SUCCESS;
		}
		else if (_wcsicmp(argv[1], L"AddPath") == 0)
		{
			cout << endl;
			FunctionI(true);
			cout << endl << endl;
			if (state >= 36 && state <= 38)
				return EXIT_FAILURE;
			else
				return EXIT_SUCCESS;
		}
		else if (argc > 2 && (_wcsicmp(argv[1], L"Power") == 0) &&
			((_wcsicmp(argv[2], L"/fs") == 0 || _wcsicmp(argv[2], L"-fs") == 0)
			|| (_wcsicmp(argv[2], L"/fr") == 0) || (_wcsicmp(argv[2], L"-fr") == 0)
			|| (_wcsicmp(argv[2], L"/fe") == 0 || _wcsicmp(argv[2], L"-fe") == 0)
			|| (_wcsicmp(argv[2], L"/de") == 0 || _wcsicmp(argv[2], L"-de") == 0)
#if ((!defined _WIN64) && (!defined WIN64))
			|| (_wcsicmp(argv[2], L"/dr") == 0 || _wcsicmp(argv[2], L"-dr") == 0)
			|| (_wcsicmp(argv[2], L"/ds") == 0 || _wcsicmp(argv[2], L"-ds") == 0)
#endif
			))
		{
			if (_wcsicmp(argv[2], L"/fs") == 0 || _wcsicmp(argv[2], L"-fs") == 0)
				NtShutdownOrReboot(2);
			else if (_wcsicmp(argv[2], L"/fr") == 0 || _wcsicmp(argv[2], L"-fr") == 0)
				NtShutdownOrReboot(1);
			else if (_wcsicmp(argv[2], L"/fe") == 0 || _wcsicmp(argv[2], L"-fe") == 0)
				CallErrorScreen();
#if ((!defined _WIN64) && (!defined WIN64))
			else if (_wcsicmp(argv[2], L"/dr") == 0 || _wcsicmp(argv[2], L"-dr") == 0)
			{
				if (MessageBox(NULL, TEXT("警告：操作具有高度危险，是否继续？\n如要继续，请保存您的所有数据，然后选择“是”。"), WmipTextTitle, MB_YESNO | MB_ICONWARNING | MB_TOPMOST) == IDNO)
				{
					cout << states[15] << endl;
					return EXIT_SUCCESS;
				}
				strcpy_s(Name_IN[0], "/dr");
				return DriverConnector(Name_IN[0], false);
			}
			else if (_wcsicmp(argv[2], L"/ds") == 0 || _wcsicmp(argv[2], L"-ds") == 0)
			{
				if (MessageBox(NULL, TEXT("警告：操作具有高度危险，是否继续？\n如要继续，请保存您的所有数据，然后选择“是”。"), WmipTextTitle, MB_YESNO | MB_ICONWARNING | MB_TOPMOST) == IDNO)
				{
					cout << states[15] << endl;
					return EXIT_SUCCESS;
				}
				strcpy_s(Name_IN[0], "/ds");
				return DriverConnector(Name_IN[0], false);
			}
#endif
			else if (_wcsicmp(argv[2], L"/de") == 0 || _wcsicmp(argv[2], L"-de") == 0)
			{
				if (MessageBox(NULL, TEXT("警告：操作具有高度危险，是否继续？\n如要继续，请保存您的所有数据，然后选择“是”。"), WmipTextTitle, MB_YESNO | MB_ICONWARNING | MB_TOPMOST) == IDNO)
				{
					cout << states[15] << endl;
					return EXIT_SUCCESS;
				}
				strcpy_s(Name_IN[0], "/de");
				return DriverConnector(Name_IN[0], false);
			}
			if (state == 0 || state == 15)
			{
				cout << states[state] << endl;
				return EXIT_SUCCESS;
			}
			else
			{
				cout << '\a' << states[state] << endl;
				return EXIT_FAILURE;
			}
		}
		else if (argc >= 3 && (_wcsicmp(argv[1], L"List") == 0) || (_wcsicmp(argv[1], L"Reverse") == 0))
		{
			GetProcess();
			if (Lists.ProcessCount <= 0)
			{
				state = 10;
				cout << "\a错误：" << states[state] << endl;
				return EXIT_FAILURE;
			}
			if (_wcsicmp(argv[2], L"/pid") == 0 || _wcsicmp(argv[2], L"-pid") == 0)
				BubbleSort('1');
			else if (_wcsicmp(argv[2], L"/ppid") == 0 || _wcsicmp(argv[2], L"-ppid") == 0)
				BubbleSort('2');
			else if (_wcsicmp(argv[2], L"/thread") == 0 || _wcsicmp(argv[2], L"-thread") == 0)
				BubbleSort('3');
			else if (_wcsicmp(argv[2], L"/im") == 0 || _wcsicmp(argv[2], L"-im") == 0)
				BubbleSort('4');
			else if (_wcsicmp(argv[2], L"/system") == 0 || _wcsicmp(argv[2], L"-system") == 0)
				BubbleSort('5');
			if (_wcsicmp(argv[1], L"Reverse") == 0)
				ReverseSort(Lists);
			cout << endl;
			ShowProcess();
			cout << endl;
			return EXIT_SUCCESS;
		}
		if (argc == 2)
		{
			cout << states[12] << endl;
			return EXIT_GRAMMAR_ERROR;
		}
		else if (argc > 3 && (_wcsicmp(argv[3], L"/all") == 0 || _wcsicmp(argv[3], L"-all") == 0))
		{
			if (!(_wcsicmp(argv[1], L"dump") == 0 || (_wcsicmp(argv[1], L"Driver") == 0 && _wcsicmp(argv[2], L"Test") == 0)))//排除两种模式
			{
				if (MessageBox(NULL, TEXT("操作具有一定风险，确定要全选所有进程吗？"), WmipTextTitle, MB_OKCANCEL | MB_ICONQUESTION | MB_TOPMOST) == IDCANCEL)
				{
					cout << states[15] << endl;
					return EXIT_SUCCESS;
				}
			}
		}
		else if (argc < 5)
		{
			cout << states[11] << endl;
			return EXIT_OPTIONS_TOO_LESS;
		}
		if (!EnableDebugPriv("SeDebugPrivilege"))
		{
			cout << '\a' << states[4] << "详细信息：" << states[state] << endl;
			return EXIT_FAILURE;
		}
		GetCommandResult(argc, argv);
		if (Results.ProcessCount == 0 && _wcsicmp(argv[2], L"Terminate") != 0)//排除驱动结束
		{
			cout << states[13] << endl;
			return EXIT_WITH_NONE_SELECTED;
		}
		if (_wcsicmp(argv[1], L"DllHook") == 0 || _wcsicmp(argv[1], L"TreeHook") == 0)
		{
			if (!FindFirstFileExists(argv[2], FALSE))
			{
				cout << states[14] << endl;
				return EXIT_FILE_UNEXISTED;
			}
			TcharToChar(argv[2], Name_IN[0]);
			cout << endl;
			if (_wcsicmp(argv[1], L"DllHook") == 0)
				return OperateProcess1(Name_IN[0]);
			else if (_wcsicmp(argv[1], L"TreeHook") == 0)
				return OperateProcess9(1, Name_IN[0]);
		}
		else if (_wcsicmp(argv[1], L"Driver") == 0)
		{
			if (_wcsicmp(argv[2], L"Test") == 0)
			{
				char testMsg[] = "测试模式";
				return DriverConnector(testMsg, true);
			}
			else if (_wcsicmp(argv[2], L"Terminate") == 0)
			{
				if (_wcsicmp(argv[3], L"/all") == 0)
				{
					strcpy_s(Name_IN[1], "/all");
					return DriverConnector(Name_IN[1], false);
				}
				bool success = true;
				int i = 3;
			im:
				if (_wcsicmp(argv[i], L"/im") == 0 || _wcsicmp(argv[i], L"-im") == 0)
				{
					while (_wcsicmp(argv[i + 1], L"/pid") && _wcsicmp(argv[i + 1], L"-pid"))
					{
						i++;
						TcharToChar(argv[i], Name_IN[0]);
						strcpy_s(Name_IN[1], "/im ");
						strcat_s(Name_IN[1], Name_IN[0]);
						if (DriverConnector(Name_IN[1], false) != EXIT_SUCCESS)
							success = false;
						if (i + 1 >= argc)
							goto over;
					}
					i++;
					goto pid;
				}
			pid:
				if (_wcsicmp(argv[i], L"/pid") == 0 || _wcsicmp(argv[i], L"-pid") == 0)
				{
					while (_wcsicmp(argv[i + 1], L"/im") && _wcsicmp(argv[i + 1], L"-im"))
					{
						i++;
						TcharToChar(argv[i], Name_IN[0]);
						strcpy_s(Name_IN[1], "/pid ");
						strcat_s(Name_IN[1], Name_IN[0]);
						if (DriverConnector(Name_IN[1], false) != EXIT_SUCCESS)
							success = false;
						if (i + 1 >= argc)
							goto over;
					}
					i++;
					goto im;
				}
			over:
				return success ? EXIT_SUCCESS : EXIT_DRIVER_ERROR;
			}
			else
			{
				cout << states[12] << endl;
				return EXIT_OPTIONS_TOO_LESS;
			}
		}
		else if (_wcsicmp(argv[1], L"dump") == 0)
		{
			if (_wcsicmp(argv[2], L"NULL") == 0)
			{
				cout << endl;
				ShowResults();
				cout << endl;
				return EXIT_SUCCESS;
			}
			TcharToChar(argv[2], Name_IN[0]);
			SetFileAttributes(argv[2], FILE_ATTRIBUTE_NORMAL);//去除属性保护
			ofstream fp;//文件流
			fp.open(Name_IN[0]);
			if (!fp)//利用重载判断文件是否正常使用
			{
				state = 31;
				cout << "\a错误：" << states[state] << endl;
				return EXIT_FAILURE;
			}
			if (fp)
				fp << "PID\t\tPPID\t\t线程数\t\t进程名" << endl << "==========\t==========\t==========\t========================================" << endl << endl;
			else
			{
				state = 23;
				cout << "\a错误：" << states[state] << endl;
				return EXIT_FAILURE;
			}
			for (int i = 0; i < Results.ProcessCount; ++i)
			{
				if (fp)
					fp << Results.PNode[i].ProcessIDNode << "\t\t" << Results.PNode[i].ParentIDNode << "\t\t" << Results.PNode[i].ThreadCNode << "\t\t";
				else
				{
					state = 23;
					cout << "\a错误：" << states[state] << endl;
					return EXIT_FAILURE;
				}
				TcharToChar(Results.PNode[i].PNameNode, Name_IN[i]);
				if (fp)
					fp << Name_IN[i] << endl;
				else
				{
					state = 23;
					cout << "\a错误：" << states[state] << endl;
					return EXIT_FAILURE;
				}
			}
			if (fp)
				fp << endl << endl << "当前共有 " << Lists.ProcessCount << " 个进程，共有 " << Lists.ThreadCount << " 条线程。" << endl << "其中，查找到 " << Results.ProcessCount << " 个进程，共包含有 " << Results.ThreadCount << " 条线程。";
			else
			{
				state = 23;
				cout << "\a错误：" << states[state] << endl;
				return EXIT_FAILURE;
			}
			if (fp)
				fp.close();
			else
			{
				state = 23;
				cout << "\a错误：" << states[state] << "已尽可能保存所有数据。" << endl;
				return EXIT_FAILURE;
			}
			SetFileAttributes(argv[2], FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_ARCHIVE);//增加属性保护
			cout << states[0] << endl;
			return EXIT_SUCCESS;
		}
		else if (_wcsicmp(argv[1], L"Check") == 0)
		{
			if (!InitQueue(queue))
			{
				cout << "\a错误：" << states[29] << endl;
				return EXIT_FAILURE;
			}
			if (_wcsicmp(argv[2], L"/all") == 0 || _wcsicmp(argv[2], L"-all") == 0)
			{
				for (i = 0; i < 9; ++i)
					HackChoices[i] = true;
				queue.tag = 9;
			}
			else
			{
				queue.tag = 0;
				for (i = 0; i < (min(lstrlen(argv[2]), 9)); ++i)
					if (argv[2][i] == 'Y' || argv[2][i] == 'y' || argv[2][i] == '1')
					{
						HackChoices[i] = true;
						queue.tag++;
					}
				if (queue.tag == 0)
				{
					cout << states[13] << endl;
					return EXIT_WITH_NONE_SELECTED;
				}
			}
			cout << endl;
			return OperateProcess8();
		}
		else if (_wcsicmp(argv[1], L"TreeCheck") == 0)
		{
			if (!InitQueue(queue))
			{
				cout << "\a错误：" << states[29] << endl;
				return EXIT_FAILURE;
			}
			if (_wcsicmp(argv[2], L"/all") == 0 || _wcsicmp(argv[2], L"-all") == 0)
			{
				for (i = 0; i < 8; ++i)
					HackChoices[i] = true;
				queue.tag = 8;
			}
			else
			{
				queue.tag = 0;
				for (i = 0; i < (min(lstrlen(argv[2]), 8)); ++i)
					if (argv[2][i] == 'Y' || argv[2][i] == 'y' || argv[2][i] == '1')
					{
						HackChoices[i] = true;
						queue.tag++;
					}
				if (queue.tag == 0)
				{
					cout << states[13] << endl;
					return EXIT_WITH_NONE_SELECTED;
				}
			}
			cout << endl;
			return OperateProcess9(8, NULL);
		}
		else if (_wcsicmp(argv[1], L"Terminate") == 0)
		{
			cout << endl;
			if (_wcsicmp(argv[2], L"/T") == 0 || _wcsicmp(argv[2], L"-T") == 0)
				return OperateProcess9(4, NULL);
			else if (_wcsicmp(argv[2], L"/W") == 0 || _wcsicmp(argv[2], L"-W") == 0)
				return OperateProcess7();
			else
				return OperateProcess4();
		}
		else if (_wcsicmp(argv[1], L"Status") == 0)
		{
			if (_wcsicmp(argv[2], L"Suspend") == 0)
			{
				cout << endl;
				return OperateProcess2();
			}
			else if (_wcsicmp(argv[2], L"Resume") == 0)
			{
				cout << endl;
				return OperateProcess3();
			}
			else if (_wcsicmp(argv[2], L"SuspendAll") == 0)
			{
				cout << endl;
				return OperateProcess5();
			}
			else if (_wcsicmp(argv[2], L"ResumeAll") == 0)
			{
				cout << endl;
				return OperateProcess6();
			}
		}
		else if (_wcsicmp(argv[1], L"TreeStatus") == 0)
		{
			if (_wcsicmp(argv[2], L"Suspend") == 0)
			{
				cout << endl;
				return OperateProcess9(2, NULL);
			}
			else if (_wcsicmp(argv[2], L"Resume") == 0)
			{
				cout << endl;
				return OperateProcess9(3, NULL);
			}
			else if (_wcsicmp(argv[2], L"SuspendAll") == 0)
			{
				cout << endl;
				return OperateProcess9(5, NULL);
			}
			else if (_wcsicmp(argv[2], L"ResumeAll") == 0)
			{
				cout << endl;
				return OperateProcess9(6, NULL);
			}
		}
		else
		{
			cout << states[12] << endl;
			return EXIT_GRAMMAR_ERROR;
		}
	}
	else
	{
		cout << "\a严重错误：程序被间谍，命令行被非法更改！" << endl;
		cout << "请按任意键退出，或直接关闭本窗口。" << endl;
		system("pause>nul");
		return EXIT_UNSAFELY;
	}
	return EXIT_OUT_OF_SCHEDULE;
}