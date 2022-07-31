/* 不支持符号链接用户相关性 */
#define NT_DEVICE_NAME               L"\\Device\\devProcessProtector"             // Driver Name
#define SYMBOLIC_LINK_NAME           L"\\DosDevices\\ProcessProtector"            // Symbolic Link Name
#define WIN32_LINK_NAME              "\\\\.\\ProcessProtector"                    // Win32 Link Name

/* 支持符号链接用户相关性 */
#define SYMBOLIC_LINK_GLOBAL_NAME    L"\\DosDevices\\Global\\ProcessProtector"    // Symbolic Link Name
#define DATA_TO_APP                  "This string is from driver to APP"

/* Device IO Control Codes */
#define IOCTL_BASE              0x800
#define MY_CTL_CODE(i)          \
    CTL_CODE                    \
    (                           \
        FILE_DEVICE_UNKNOWN,    \
        IOCTL_BASE + i,         \
        METHOD_BUFFERED,        \
        FILE_ANY_ACCESS         \
    )

#define IOCTL_HELLO_WORLD       MY_CTL_CODE(0)
#define IOCTRL_REC_FROM_APP     MY_CTL_CODE(1)
#define IOCTRL_SEND_TO_APP      MY_CTL_CODE(2)