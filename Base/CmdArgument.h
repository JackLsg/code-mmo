#ifndef _CMDARGUMENT_H_
#define _CMDARGUMENT_H_

#include "Base.h"

//命令行格式为：-参数名1 参数值1 -参数名2 参数值2...

void	InitCmdArgument(int *argc, char*** argv);

//世界ID
tint32	WorldID(void);

#if defined(__WINDOWS__)
bool AutoDebugBreak(void);
#endif

#endif