#include "MyFxManager.h"
#include "Search.h"
#include "LogFile.h"
#include <stdio.h>
#include <Windows.h>
#include "game_sa\FxManager_c.h"
#include "game_sa\CTxdStore.h"
#include "game_sa\CFileMgr.h"
#include "game_sa\CKeyGen.h"

std::vector<unsigned int> MyFxManager::customParticlesKeys;
char MyFxManager::tempSystemName[256];
std::vector<std::string> MyFxManager::customTexturesNames;

void MyFxManager::LoadSystemTexturesCB(char *path, void *dictionary) {
    char folder_path[MAX_PATH];
    strcpy(folder_path, PROJECT_PATH);
    strcat(folder_path, path);
    Search::ForAllFiles(folder_path, "dds", LoadDDSTextureCB, dictionary);
    Search::ForAllFiles(folder_path, "png", LoadPNGTextureCB, dictionary);
}

bool MyFxManager::TextureAlreadyLoaded(char *name) {
    for (auto &i : customTexturesNames) {
        if (!_stricmp(i.c_str(), name))
            return true;
    }
    return false;
}

void MyFxManager::LoadPNGTextureCB(char *path, void *dictionary) {
    char texName[MAX_PATH];
    _splitpath(path, NULL, NULL, texName, NULL);
    texName[31] = '\0';
    if (TextureAlreadyLoaded(texName)) {
        LogFile::WriteFormattedLine("Loading PNG texture \"%s\" - texture was already loaded.", path);
        return;
    }
    LogFile::WriteFormattedLine("Loading PNG texture \"%s\"", path);
    int width, height, depth, flags;
    RwImage *image = RtPNGImageRead(path);
    if (!image) {
        LogFile::WriteFormattedLine("\"%s\" - FAILED to load texture", path);
        return;
    }
    customTexturesNames.push_back(texName);
    RwImageFindRasterFormat(image, 4, &width, &height, &depth, &flags);
    RwRaster *raster = RwRasterCreate(width, height, depth, flags);
    RwRasterSetFromImage(raster, image);
    RwImageDestroy(image);
    RwTexture *tex = RwTextureCreate(raster);
    RwTextureSetName(tex, texName);
    if ((tex->raster->cFormat & 0x80) == 0)
        RwTextureSetFilterMode(tex, rwFILTERLINEAR);
    else
        RwTextureSetFilterMode(tex, rwFILTERLINEARMIPLINEAR);
    RwTextureSetAddressing(tex, rwTEXTUREADDRESSWRAP);
    RwTexture *oldTex = RwTexDictionaryFindNamedTexture(reinterpret_cast<TxdDef *>(dictionary)->m_pRwDictionary, texName);
    if (oldTex) {
        LogFile::WriteFormattedLine("Removing old texture \"%s\"", texName);
        RwTexDictionaryRemoveTexture(oldTex);
        RwTextureDestroy(oldTex);
    }
    RwTexDictionaryAddTexture(reinterpret_cast<TxdDef *>(dictionary)->m_pRwDictionary, tex);
}

void MyFxManager::LoadDDSTextureCB(char *path, void *dictionary) {
    char texName[MAX_PATH];
    _splitpath(path, NULL, NULL, texName, NULL);
    texName[31] = '\0';
    if (TextureAlreadyLoaded(texName)) {
        LogFile::WriteFormattedLine("Loading DDS texture \"%s\" - texture was already loaded.", path);
        return;
    }
    LogFile::WriteFormattedLine("Loading DDS texture \"%s\"", path);
    char ddsPath[MAX_PATH];
    strcpy(ddsPath, path);
    ddsPath[strlen(ddsPath) - 4] = '\0';
    RwTexture *tex = RwD3D9DDSTextureRead(ddsPath, NULL);
    if (!tex) {
        LogFile::WriteFormattedLine("\"%s\" - FAILED to load texture");
        return;
    }
    RwTextureSetAddressing(tex, rwTEXTUREADDRESSWRAP);
    customTexturesNames.push_back(texName);
    RwTexture *oldTex = RwTexDictionaryFindNamedTexture(reinterpret_cast<TxdDef *>(dictionary)->m_pRwDictionary, texName);
    if (oldTex) {
        LogFile::WriteFormattedLine("Removing old texture \"%s\"", texName);
        RwTexDictionaryRemoveTexture(oldTex);
        RwTextureDestroy(oldTex);
    }
    RwTexDictionaryAddTexture(reinterpret_cast<TxdDef *>(dictionary)->m_pRwDictionary, tex);
}

unsigned int MyFxManager::GetSystemNameKey(int file) {
    unsigned int posn = CFileMgr::Tell(file);
    char line[256];
    CFileMgr::ReadLine(file, line, 256);
    unsigned int version;
    sscanf(line, "%d", &version);
    CFileMgr::ReadLine(file, line, 256);
    if (version > 100)
        CFileMgr::ReadLine(file, line, 256);
    CFileMgr::ReadLine(file, line, 256);
    sscanf(line, "%*s %s", tempSystemName);
    CFileMgr::Seek(file, posn, SEEK_SET);
    return CKeyGen::GetUppercaseKey(tempSystemName);
}

bool MyFxManager::IsThisParticleLoaded(unsigned int key) {
    for (auto i : customParticlesKeys) {
        if (i == key)
            return true;
    }
    return false;
}

void MyFxManager::LoadFxSystemFileCB(char *path, void *data) {
    char linebuf[256];
    char header[32];
    int file = CFileMgr::OpenFile(path, "r");
    if (file > 0) {
        CFileMgr::ReadLine(file, linebuf, 256); // FX_SYSTEM_DATA:
        sscanf(linebuf, "%s", header);
        if (!strncmp(header, "FX_SYSTEM_DATA:", 15)) {
            unsigned int key = GetSystemNameKey(file);
            if (!IsThisParticleLoaded(key)) {
                customParticlesKeys.push_back(key);
                LogFile::WriteFormattedLine("Loading custom system \"%s\" (\"%s\")", path, tempSystemName);
                reinterpret_cast<MyFxManager *>(data)->LoadFxSystemBP(path, file);
            }
            else
                LogFile::WriteFormattedLine("Loading custom system \"%s\" (\"%s\") - this system was already loaded.", path, tempSystemName);
        }
        CFileMgr::CloseFile(file);
    }
}

void MyFxManager::LoadFxSystemCB(char *path, void *data) {
    char folder_path[MAX_PATH];
    strcpy(folder_path, PROJECT_PATH);
    strcat(folder_path, path);
    Search::ForAllFiles(folder_path, "fxs", LoadFxSystemFileCB, data);
}

bool MyFxManager::LoadProject(char *fxFileName) {
    char txdFileName[MAX_PATH];
    strcpy(txdFileName, fxFileName);
    strcpy(&txdFileName[strlen(txdFileName) - 4], "PC.txd");
    this->m_nFxTxdIndex = CTxdStore::AddTxdSlot("fx");
    CTxdStore::LoadTxd(this->m_nFxTxdIndex, txdFileName);
    // Load custom effects
    Search::ForAllFolders(PROJECT_PATH, LoadSystemTexturesCB, CTxdStore::ms_pTxdPool->GetAt(this->m_nFxTxdIndex));
    CTxdStore::AddRef(this->m_nFxTxdIndex);
    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(this->m_nFxTxdIndex);
    Search::ForAllFolders(PROJECT_PATH, LoadFxSystemCB, this);
    // Load default effects
    int file = CFileMgr::OpenFile(fxFileName, "r");
    if (file > 0) {
        char linebuf[256];
        char header[32];
        CFileMgr::ReadLine(file, linebuf, 256); // FX_PROJECT_DATA:
        CFileMgr::ReadLine(file, linebuf, 256); // 
        CFileMgr::ReadLine(file, linebuf, 256); // FX_SYSTEM_DATA:
        sscanf(linebuf, "%s", header);
        if (!strncmp(header, "FX_SYSTEM_DATA:", 15)) {
            do {
                unsigned int key = GetSystemNameKey(file);
                if (!IsThisParticleLoaded(key)) {
                    LogFile::WriteFormattedLine("Loading default system \"%s\"", tempSystemName);
                    this->LoadFxSystemBP(fxFileName, file);
                    CFileMgr::ReadLine(file, linebuf, 256); // 
                    CFileMgr::ReadLine(file, linebuf, 256); // FX_SYSTEM_DATA:
                    sscanf(linebuf, "%s", header);
                }
                else {
                    LogFile::WriteFormattedLine("Loading default system \"%s\" - this system was already loaded.", tempSystemName);
                    while (CFileMgr::ReadLine(file, linebuf, 256)) {
                        sscanf(linebuf, "%s", header);
                        if (!strncmp(header, "FX_SYSTEM_DATA:", 15))
                            break;
                    }
                }
            } while (!strncmp(header, "FX_SYSTEM_DATA:", 15));
        }
        CFileMgr::CloseFile(file);
    }
    CTxdStore::PopCurrentTxd();
    this->m_pool.Optimise();
    customParticlesKeys.clear();
    customTexturesNames.clear();
    return true;
}