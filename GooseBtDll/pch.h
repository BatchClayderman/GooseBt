#ifndef PCH_H
#define PCH_H
#include "framework.h"
#include <iostream>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#ifdef WIN32
#include <Windows.h>
#endif
#ifdef _WIN64
#include <Windows.h>
#ifndef WIN32
#define WIN32 1
#endif
#endif
#endif
#endif //PCH_H

#ifndef EXPORT_DLL
#define EXPORT_DLL extern "C" _declspec(dllexport)
#endif//EXPORT_DLL
EXPORT_DLL int Add(int a, int b);

#ifndef _GooseBtDll_H
#define _GooseBtDll_H
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#endif//_GooseBtDll_H