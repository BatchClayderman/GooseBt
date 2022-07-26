#pragma once
#include <ntifs.h>
VOID listenerUnload();
NTSTATUS listenerEntry();
#if ((!defined _WIN64) && (!defined WIN64))
VOID DrivenReboot();
VOID DrivenShutdown();
#endif
VOID DrivenError();
BOOLEAN StringToULONG(char* targetPID, DWORD* pid);
BOOLEAN PIDCallTerminate(DWORD targetPID);
VOID ZwKillImage(char* dwProcessName);