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

DEFINE_GUID (GUID_DEVINTERFACE_KMDFThread,
    0x4ba17245,0xe59d,0x43ca,0x84,0x1a,0xae,0x6d,0xa5,0xdb,0x7b,0xcc);
// {4ba17245-e59d-43ca-841a-ae6da5db7bcc}
