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

DEFINE_GUID (GUID_DEVINTERFACE_KMDFTDI,
    0xad40631a,0xc2c3,0x4441,0x8e,0x04,0x82,0xa1,0x1d,0x9f,0x13,0x34);
// {ad40631a-c2c3-4441-8e04-82a11d9f1334}
