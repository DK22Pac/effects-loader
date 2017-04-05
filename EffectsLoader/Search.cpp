#include "Search.h"
#include <Windows.h>
#include <stdio.h>

bool Search::GetFirstFile(char *outPath, char *folderpath, char *extension) {
    WIN32_FIND_DATA fd;
    char search_path[MAX_PATH];
    sprintf(search_path, "%s*.%s", folderpath, extension);
    HANDLE hFind = FindFirstFile(search_path, &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !(fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) && fd.cFileName[0] != '.') {
                if (outPath)
                    strcpy(outPath, fd.cFileName);
                FindClose(hFind);
                return true;
            }
        } while (FindNextFile(hFind, &fd));
        FindClose(hFind);
    }
    return false;
}

void Search::ForAllFolders(char *path, void(*callback)(char *, void *), void *data) {
    char search_path[MAX_PATH];
    sprintf(search_path, "%s*.*", path);
    WIN32_FIND_DATA fd;
    HANDLE hFind = FindFirstFile(search_path, &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && !(fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) && fd.cFileName[0] != '.')
                callback(fd.cFileName, data);
        } while (FindNextFile(hFind, &fd));
        FindClose(hFind);
    }
}

void Search::ForAllFiles(char *folderpath, char *extension, void(*callback)(char *, void *), void *data) {
    char search_path[MAX_PATH];
    sprintf(search_path, "%s\\*.%s", folderpath, extension);
    WIN32_FIND_DATA fd;
    HANDLE hFind = FindFirstFile(search_path, &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !(fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) && fd.cFileName[0] != '.') {
                char path[MAX_PATH];
                sprintf(path, "%s\\%s", folderpath, fd.cFileName);
                callback(path, data);
            }
        } while (FindNextFile(hFind, &fd));
        FindClose(hFind);
    }
}