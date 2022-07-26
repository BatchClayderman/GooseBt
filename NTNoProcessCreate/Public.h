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

DEFINE_GUID (GUID_DEVINTERFACE_NTNoProcessCreate,
    0x22cdc29d,0xc43c,0x45db,0x98,0x0f,0x68,0xcd,0xc2,0x2b,0xf1,0x58);
// {22cdc29d-c43c-45db-980f-68cdc22bf158}
