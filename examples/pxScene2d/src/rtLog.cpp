// rtLog.cpp CopyRight 2005-2015 John Robinson
//#include "stdafx.h"

#include <stdio.h>
//#include "windows.h"
#include <stdarg.h>
#include <string.h>
#include "rtLog.h"

void rtLog(const char* format, ...)
{
  // JRXXX
#if 0
    char buffer[1024];
    buffer[0] = 0;
    va_list ptr; va_start(ptr, format);
    strcpy(buffer, RTLOGPREFIX);
    if (_vsnprintf(buffer+strlen(RTLOGPREFIX), sizeof(buffer)-strlen(buffer)-1, format, ptr) < 1)
    {
        strcpy(buffer, "rtLog buffer overflow\n");
    }
    OutputDebugStringA(buffer);
    va_end(ptr);
#else
    char buffer[1024];
    buffer[0] = 0;
    va_list ptr; va_start(ptr, format);
    strcpy(buffer, RTLOGPREFIX);
    if (vsnprintf(buffer+strlen(RTLOGPREFIX), sizeof(buffer)-strlen(buffer)-1, format, ptr) < 1)
    {
        strcpy(buffer, "rtLog buffer overflow\n");
    }
    //    OutputDebugStringA(buffer);
    printf("%s", buffer);
    va_end(ptr);
#endif
}

