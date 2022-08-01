#pragma once
VOID listenerUnload();
NTSTATUS listenerEntry(PDRIVER_OBJECT pDriver);
NTSTATUS ntIBinaryCreateKey(UNICODE_STRING uPath, UNICODE_STRING uSubKey);
NTSTATUS ntIBinaryDeleteKey(UNICODE_STRING uPath, UNICODE_STRING uSubKey, BOOLEAN isSubFolder);
NTSTATUS ntIBinarySetKeyValue(UNICODE_STRING uPath, UNICODE_STRING uSubKey, ULONG uType, PVOID uValue, ULONG uSize);
NTSTATUS ntIBinaryQueryKey(UNICODE_STRING uPath);