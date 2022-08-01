#pragma once
VOID listenerUnload();
NTSTATUS listenerEntry(PDRIVER_OBJECT pDriver);
BOOLEAN NtOccurFile(UNICODE_STRING uPathName);