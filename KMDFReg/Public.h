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

DEFINE_GUID (GUID_DEVINTERFACE_KMDFReg,
    0xdeeb260d,0xcaf9,0x48cd,0x9a,0x5a,0xd5,0xc7,0x05,0xf8,0x29,0xd9);
// {deeb260d-caf9-48cd-9a5a-d5c705f829d9}
