#ifndef _CMDARGUMENT_H_
#define _CMDARGUMENT_H_

#include "Base.h"

//�����и�ʽΪ��-������1 ����ֵ1 -������2 ����ֵ2...

void	InitCmdArgument(int *argc, char*** argv);

//����ID
tint32	WorldID(void);

#if defined(__WINDOWS__)
bool AutoDebugBreak(void);
#endif

#endif