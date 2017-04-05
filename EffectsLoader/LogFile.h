#pragma once
#include <stdio.h>

const bool WRITELOG = true;

class LogFile {
    static FILE *m_pFile;
public:
    static void Open();
    static void Close();
    static void MakeNewLine();
    static void WriteLine(char *text);
    static void WriteText(char *text);

    template<typename... ArgTypes>
    static void WriteFormattedLine(char *format, ArgTypes... args) {
        if (WRITELOG && m_pFile) {
            fprintf(m_pFile, format, args...);
            fputc('\n', m_pFile);
        }
    }

    template<typename... ArgTypes>
    static void WriteFormattedText(char *format, ArgTypes... args) {
        if (WRITELOG && m_pFile)
            fprintf(m_pFile, format, args...);
    }
};