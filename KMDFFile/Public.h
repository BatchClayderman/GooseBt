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

DEFINE_GUID (GUID_DEVINTERFACE_KMDFFile,
    0x29b63700,0x1290,0x4fd8,0xbf,0xd7,0x4c,0xf9,0x83,0x79,0x6c,0x03);
// {29b63700-1290-4fd8-bfd7-4cf983796c03}
