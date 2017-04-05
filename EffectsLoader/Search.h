#pragma once

class Search {
public:
    static bool GetFirstFile(char *outPath, char *folderpath, char *extension);
    static void ForAllFolders(char *path, void(*callback)(char *, void *), void *data);
    static void ForAllFiles(char *folderpath, char *extension, void(*callback)(char *, void *), void *data);
};