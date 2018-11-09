#include "Base.h"
#include "CmdArgument.h"
#include "gflags/gflags.h"

DEFINE_int32(worldid, -1, "world id, >=0");
#if defined(__WINDOWS__)
DEFINE_bool(debugbreak, false, "debugbreak is bool");
#endif

void InitCmdArgument( int *argc, char*** argv )
{
	__ENTER_FUNCTION

	::google::ParseCommandLineFlags(argc, argv, true);

	AssertEx(FLAGS_worldid >= 0, "worldid is not >= 0!");

#if defined(__WINDOWS__)
	g_Command_Auto_DebugBreak = AutoDebugBreak();
#endif

	__LEAVE_FUNCTION
}

tint32 WorldID( void )
{
	return FLAGS_worldid;
}

#if defined(__WINDOWS__)
bool AutoDebugBreak( void )
{
	return FLAGS_debugbreak;
}
#endif