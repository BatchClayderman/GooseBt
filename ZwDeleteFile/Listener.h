#pragma once
#include <ntifs.h>
VOID listenerUnload();
NTSTATUS listenerEntry();
NTSTATUS IBinaryNtZwDeleteFile(UNICODE_STRING uDeletePathName);
NTSTATUS IBinaryNtSetInformationFileDeleteFile(UNICODE_STRING uDeletePathName);