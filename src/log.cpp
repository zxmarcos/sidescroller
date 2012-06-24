
#include "system.h"
#include <string.h>
//#include <windows.h>

static FILE *logfp = NULL;
static bool isLogOpen = false;
// nivel de identação
static int innerLevel = 0;
static int innerSize = 1;


int LogInit() {
    logfp = fopen("log.txt", "w");
    if (!logfp)
        logfp = stdout;
    else
        isLogOpen = true;
    LogInnerSize(2);
    return 1;
}

void Log(const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    char buffer[256];
    memset(buffer, 0, 256);
    for (int i = 0; i < (innerLevel * innerSize); i++)
        buffer[i] = ' ';

    fprintf(logfp, "%s", buffer);
    vfprintf(logfp, fmt, va);
    va_end(va);
}


void LogFatal(const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    char buffer[256];
    vsprintf(buffer, fmt, va);
//    MessageBox(NULL, buffer, "Erro Fatal!!!", MB_ICONERROR);
    va_end(va);
    exit(-1);
}

void LogErr(int error, const char *fmt, ...) {
}

void LogClose() {
    if (isLogOpen)
        fclose(logfp);
}

void LogEnter() { innerLevel++; }
void LogBack()
{
    if (innerLevel)
        innerLevel--;
}
void LogInnerSize(int n) {
    if (n > 0)
        innerSize = n;
    else
        innerSize = 1;
}

static int _innerSizeSave = 0;
static int _innerLevelSave = 0;

void LogSaveState()
{
    _innerLevelSave = innerLevel;
    _innerSizeSave = innerSize;
    innerLevel = 0;
    innerSize = 1;
}

void LogLoadState()
{
    innerLevel = _innerLevelSave;
    innerSize = _innerSizeSave;
}
