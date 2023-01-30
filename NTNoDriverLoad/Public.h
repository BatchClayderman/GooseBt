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

DEFINE_GUID (GUID_DEVINTERFACE_NTNoDriverLoad,
    0x2b295bac,0x1e80,0x4edb,0x82,0x77,0x5c,0x7c,0xd9,0x88,0xe5,0x21);
// {2b295bac-1e80-4edb-8277-5c7cd988e521}
