#ifndef SYSTEM_H_INCLUDED
#define SYSTEM_H_INCLUDED

#include <cstdio>
#include <cstdlib>
#include <cstdarg>

#define LOG_LEVEL   3

#define ENGINE_NAME "Sidescroller"
#define SAFE_DELETE(p)  { if (p) delete p; p = NULL; }
#define foreach BOOST_FOREACH

// funções para o linux
#define stricmp strcasecmp
#define strtoint(s) ((s)?atoi(s):0)

int LogInit();
void Log(const char *fmt, ...);
void LogErr(int error, const char *fmt, ...);
void LogFatal(const char *fmt, ...);
void LogClose();
void LogEnter();
void LogBack();
void LogInnerSize(int n);
void LogLoadState();
void LogSaveState();

#endif // SYSTEM_H_INCLUDED
