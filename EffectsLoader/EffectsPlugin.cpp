#include <Windows.h>
#include <plugin\plugin.h>
#include <patch\CPatch.h>
#include "MyFxManager.h"

using namespace plugin;

bool __fastcall LoadProject(MyFxManager *fxMan, int, char *fileName)
{
#ifdef WRITELOG
	logFile = fopen("models\\effects\\log.txt", "wt");
#endif
	bool result = fxMan->LoadProject(fileName);
#ifdef WRITELOG
	fclose(logFile);
#endif
	return result;
}

void LoadPlugin()
{
	System::RegisterPlugin("EffectLoader", "DK22Pac", "EffectLoader.asi", "1.0", 0x100, GAME_SA_1_0_US, NULL);
	CPatch::RedirectJump(0x5C2420, LoadProject);
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
    if(reason == DLL_PROCESS_ATTACH)
		LoadPlugin();
    return TRUE;
}