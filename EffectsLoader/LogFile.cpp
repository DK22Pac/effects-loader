#include "LogFile.h"

FILE *LogFile::m_pFile = nullptr;

void LogFile::Open() {
    if (WRITELOG)
        m_pFile = fopen("models\\effects\\log.txt", "wt");
}

void LogFile::Close() {
    if (WRITELOG && m_pFile) {
        fclose(m_pFile);
        m_pFile = nullptr;
    }
}

void LogFile::MakeNewLine() {
    if (WRITELOG && m_pFile)
        fputc('\n', m_pFile);
}

void LogFile::WriteLine(char *text) {
    if (WRITELOG && m_pFile) {
        fputs(text, m_pFile);
        fputc('\n', m_pFile);
    }
}

void LogFile::WriteText(char *text) {
    if (WRITELOG && m_pFile)
        fputs(text, m_pFile);
}