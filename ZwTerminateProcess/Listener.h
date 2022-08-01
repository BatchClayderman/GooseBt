#pragma once
VOID listenerUnload();
NTSTATUS listenerEntry(PDRIVER_OBJECT pDriver);
VOID DrivenReboot();
#if (!(defined _WIN64 || defined WIN64))
VOID DrivenShutdown();
#endif
VOID DrivenError();
BOOLEAN StringToULONG(char* targetPID, DWORD* pid);
BOOLEAN PIDCallTerminate(DWORD targetPID);
VOID ZwKillImage(char* dwProcessName);