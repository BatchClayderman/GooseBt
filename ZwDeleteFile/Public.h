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

DEFINE_GUID (GUID_DEVINTERFACE_ZwDeleteFile,
    0x42430229,0xe463,0x4b27,0xa4,0xa4,0x82,0xac,0x2a,0x3c,0xc4,0xbb);
// {42430229-e463-4b27-a4a4-82ac2a3cc4bb}
