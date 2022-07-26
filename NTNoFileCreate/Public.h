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

DEFINE_GUID (GUID_DEVINTERFACE_NTNoFileCreate,
    0x8f8eab0a,0x0b24,0x45e9,0xb0,0x23,0x84,0x72,0x80,0xd6,0x30,0x2f);
// {8f8eab0a-0b24-45e9-b023-847280d6302f}
