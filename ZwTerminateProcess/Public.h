/*++

Module Name:

    public.h

Abstract:

    This module contains the common declarations shared by driver
    and user applications.

Environment:

    user and kernel

--*/

//
// Define an Interface Guid so that apps can find the device and talk to it.
//

DEFINE_GUID (GUID_DEVINTERFACE_ZwTerminateProcess,
    0x0de91249,0x827a,0x4793,0xa8,0xf6,0x17,0xee,0x09,0x62,0xcb,0x3b);
// {0de91249-827a-4793-a8f6-17ee0962cb3b}
