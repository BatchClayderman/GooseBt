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

DEFINE_GUID (GUID_DEVINTERFACE_KMDFKeyboard,
    0x30712525,0x2370,0x4124,0x8e,0xf0,0xd1,0xbb,0x6d,0xde,0xea,0x96);
// {30712525-2370-4124-8ef0-d1bb6ddeea96}
