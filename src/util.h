#ifndef UTIL_H
#define UTIL_H

#include <string>
namespace Base64
{
    char *Decode(char *str, int *dbufsz);
    char *Encode(char *str);
    char *CleanString(char *str);
};

void ParseRGB(const char *str, int *r, int *g, int *b);


#endif // UTIL_H
