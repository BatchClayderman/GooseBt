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

DEFINE_GUID (GUID_DEVINTERFACE_ZwOpReg,
    0x8f34ce1b,0xb675,0x4bab,0xbf,0xf3,0x82,0xad,0xa9,0xf8,0x80,0x12);
// {8f34ce1b-b675-4bab-bff3-82ada9f88012}
