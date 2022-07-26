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

DEFINE_GUID (GUID_DEVINTERFACE_KMDFSerial,
    0xb47755b5,0x9ec0,0x4553,0xbe,0x85,0x9a,0x6e,0x81,0x46,0x01,0x0a);
// {b47755b5-9ec0-4553-be85-9a6e8146010a}
