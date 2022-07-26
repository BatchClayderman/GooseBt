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

DEFINE_GUID (GUID_DEVINTERFACE_KMDFCallback,
    0x74423351,0x1b63,0x4c5c,0x82,0xed,0x59,0x27,0x31,0x51,0x10,0x93);
// {74423351-1b63-4c5c-82ed-592731511093}
