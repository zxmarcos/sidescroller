#include "util.h"
#include <string.h>
#include <cstdio>
#include <string>
#include <cctype>

namespace Base64
{



static const char base64_table[65] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

static const char base64_index[256] = {
   64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,62, 0, 0, 0,63,
   52,53,54,55,56,57,58,59,60,61, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
   15,16,17,18,19,20,21,22,23,24,25, 0, 0, 0, 0, 0,
    0,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
   41,42,43,44,45,46,47,48,49,50,51, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

// Função necessária para retirar todos os espaços de uma string,
// o TinyXML retorna textos com espaços e quebras de linhas
// que modificam a saida do decodificador, então precisamos limpar
// essa string;
char *CleanString(char *str) {
    int size = 0;
    char *ptr = str;
    while (*ptr) {
        if (!isspace(*ptr))
            size++;
        ++ptr;
    }
    char *buffer = new char[size + 1];
    char *cptr = buffer;
    ptr = str;

    while (*ptr)
    {
        if (!isspace(*ptr))
            *cptr++ = *ptr;
        ptr++;
    }
    *cptr = '\0';
    return buffer;
}

char *Decode(char *str2, int *dbufsz) {
    char *str = CleanString(str2);
    int slen = strlen(str);
    int pad = 0;

    if (slen % 4) {
        pad = 4 - slen % 4;
        slen -= pad;
    }

    char *buffer;
    // valor final do buffer decodificado
    // esse valor é o máximo, porém pode ser diminuido com o pad
    int bufsize = (slen / 4) * 3;

    // nosso buffer de entrada
    char *lbuffer = new char[slen];
    memset(lbuffer, '=', slen);
    memcpy(lbuffer, str, strlen(str));

    // buffer de saida
    buffer = new char[bufsize];
    memset(buffer, 0, bufsize);
    char *dptr = buffer;

    for (int i = 0; i < (slen); i += 4)
    {
        char w, x, y, z;
        char a, b, c;
        char iw, ix, iy, iz;
        iw = lbuffer[i + 0];
        ix = lbuffer[i + 1];
        iy = lbuffer[i + 2];
        iz = lbuffer[i + 3];

        w = base64_index[iw & 0xFF];
        x = base64_index[ix & 0xFF];
        y = base64_index[iy & 0xFF];
        z = base64_index[iz & 0xFF];

        a = ((w & 0x3F) << 2) | ((x & 0x30) >> 4);
        b = ((x & 0x0F) << 4) | ((y & 0x3C) >> 2);
        c = ((y & 0x03) << 6) | ((z & 0x3F));

        *dptr++ = a;
        *dptr++ = b;
        *dptr++ = c;
    }
    for (int i = 1; lbuffer[slen-i] == '='; i++)
        bufsize--;

    if (dbufsz)
        *dbufsz = bufsize;
    delete lbuffer;
    delete str;
    return buffer;
}
char *Encode(char *str) {
    char *lbuffer;
    int pad = 0;

    // Normaliza o tamanho da string de entrada
    int slen = strlen(str);
    if (slen % 3) {
        pad = 3 - slen % 3;
        slen += pad;
    }

    // para cada 3 bytes, temos 1 byte a mais, ao final
    // adicionamos 1 como o caracter terminador \0
    int bufsize = slen + (slen / 3) + 1;
    char *buffer = new char[bufsize];
    lbuffer = new char[slen];
    memset(lbuffer, 0, slen);
    memcpy(lbuffer, str, strlen(str));
    memset(buffer, 0, bufsize);

    char *eptr = buffer;
    for (int i = 0; i < slen; i += 3)
    {
        char a, b, c;
        char w, x, y, z;
        a = lbuffer[i + 0];
        b = lbuffer[i + 1];
        c = lbuffer[i + 2];

        // codifica os 4 indices
        w = ( a & 0xFC) >> 2;
        x = ((a & 0x03) << 4) | ((b & 0xF0) >> 4);
        y = ((b & 0x0F) << 2) | ((c & 0xC0) >> 6);
        z = ( c & 0x3F);

        *eptr++ = base64_table[w & 0x3F];
        *eptr++ = base64_table[x & 0x3F];
        *eptr++ = base64_table[y & 0x3F];
        *eptr++ = base64_table[z & 0x3F];
    }

    // resolvemos o problema do "padding"
    while (pad)
    {
        *(--eptr) = '=';
        pad--;
    }

    delete lbuffer;
    return buffer;
}



}
void ToLowerString(char *str)
{
    char *p = str;
    while (*p) {
        *p = tolower(*p);
        ++p;
    }
}
// convertes uma string "00FF00", para os valores de RGB
void ParseRGB(const char *str, int *r, int *g, int *b)
{
    char *str2 = Base64::CleanString((char *)str);
    ToLowerString(str2);
    int value = 0;
    sscanf(str2, "%x", &value);

    *r = (value & 0xFF0000) >> 16;
    *g = (value & 0x00FF00) >> 8;
    *b = (value & 0x0000FF);
    delete str2;
}
