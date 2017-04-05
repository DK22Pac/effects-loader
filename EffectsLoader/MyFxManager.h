#pragma once
#include <vector>
#include <string>
#include "game_sa\FxManager_c.h"

#define PROJECT_PATH "models\\effects\\"

/*  FXP - FX Project
	FXS - FX System
    Load custom effects (fxs files) > Load effects.fxp file (ignore effect if it was already loaded as custom) */

class MyFxManager : public FxManager_c {
	static std::vector<unsigned int> customParticlesKeys;
	static std::vector<std::string> customTexturesNames;
	static char tempSystemName[256];
    static void LoadSystemTexturesCB(char *path, void *dictionary);
    static bool TextureAlreadyLoaded(char *name);
    static void LoadPNGTextureCB(char *path, void *dictionary);
    static void LoadDDSTextureCB(char *path, void *dictionary);
    static unsigned int GetSystemNameKey(int file);
    static bool IsThisParticleLoaded(unsigned int key);
    static void LoadFxSystemFileCB(char *path, void *data);
    static void LoadFxSystemCB(char *path, void *data);
public:
    bool LoadProject(char *fxFileName);
};