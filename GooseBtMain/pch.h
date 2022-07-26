#ifndef PCH_H
#define PCH_H
#include <iostream>
#include <signal.h>
#include <sys/types.h>
#include "framework.h"
#ifndef REALTIME
#define REALTIME 5
#endif
#ifndef SEVERE
#define SEVERE 4
#endif
#ifndef HIGH
#define HIGH 3
#endif
#ifndef MIDDLE
#define MIDDLE 2
#endif
#ifndef LOW
#define LOW 1
#endif
#ifndef IDLE
#define IDLE 0
#endif
std::string GF_GetEXEPath();
BOOL ApplySettings(short int MonLevel);
short int GetRemoteState();
#endif //PCH_H