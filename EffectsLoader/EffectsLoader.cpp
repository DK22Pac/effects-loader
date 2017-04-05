#include "plugin.h"
#include "MyFxManager.h"
#include "LogFile.h"

using namespace plugin;

class EffectsLoader {
public:
    static bool __fastcall MyLoadProject(MyFxManager *fxMan, int, char *fileName) {
        LogFile::Open();
        bool result = fxMan->LoadProject(fileName);
        LogFile::Close();
        return result;
    }

    EffectsLoader() {
        plugin::patch::RedirectJump(0x5C2420, MyLoadProject);
    }
} plgInst;