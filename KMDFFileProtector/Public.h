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

DEFINE_GUID (GUID_DEVINTERFACE_KMDFFileProtector,
    0x26b0b527,0x0b8c,0x4f2e,0xa8,0xa4,0x59,0x12,0xed,0x66,0x57,0x14);
// {26b0b527-0b8c-4f2e-a8a4-5912ed665714}
