#pragma once
#include <ntifs.h>
VOID listenerUnload();
NTSTATUS listenerEntry();
BOOLEAN NtOccurFile(UNICODE_STRING uPathName);