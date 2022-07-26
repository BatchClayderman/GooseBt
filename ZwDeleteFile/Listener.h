#pragma once
#include <ntifs.h>
VOID listenerUnload();
NTSTATUS listenerEntry(PDRIVER_OBJECT pDriver);
BOOLEAN GooseBtZwDeleteFile(UNICODE_STRING us);