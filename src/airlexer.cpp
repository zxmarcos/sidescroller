/*
 *
 * Depois de muitos problemas com flex e bison,
 * decide escrever o parser na mão mesmo =P
 */
#include "AirLexer.h"
#include <cctype>
#include <cstdlib>
#include <string.h>
#include "System.h"

#define Char  *textPos
#define Next    ++textPos
#define Prev    --textPos
#define Peek    *(textPos+1)

AirLexer::AirLexer()
{
    //ctor
}

AirLexer::~AirLexer()
{
    //dtor
}

void AirLexer::SetBuffer(char *str)
{
    textBase = str;
    textPos = str;
    nLine = 1;
}



int AirLexer::GetToken()
{
tryagain:
    while ((isspace(Char)) && (Char != '\0'))
        Next;

    if (Char == ';')
    {
        while ((Char != '\0') && (Char != '\n'))
            Next;
    }
    if (!Char) {
        tokType = EOS;
        return tokType;
    }
    if (Char == '\n')
    {
        nLine++;
        Next;
        goto tryagain;
    }


    if (IsDelim(Char))
    {
        switch (Char)
        {
            case '[': tokType = RBRK; break;
            case ']': tokType = LBRK; break;
            case ',': tokType = SEMICOLON; break;
            case ':': tokType = COLON; break;
            case '=': tokType = EQUAL; break;
        }
        Next;
        return tokType;
    }
    else
    if (isalpha(Char))
    {
        int strsize = 0;
        char *savepos = textPos;
        char *ptr = tokBuffer;
        while (isalpha(Char) && Char && strsize < 256)
        {
            *ptr++ = tolower(Char);
            ++strsize;
            Next;
        }
        *ptr = '\0';

        if (!strcmp(tokBuffer, "begin"))
            tokType = BEGIN;
        else
        if (!strcmp(tokBuffer, "action"))
            tokType = ACTION;
        else { textPos = savepos; goto error; }
            //tokType = LITERAL;
        return tokType;
    }
    else
    if (isdigit(Char) || (strchr("+-", Char)))
    {
        int strsize = 0;
        char *ptr = tokBuffer;

        if (strchr("+-", Char))
        {
            if (!isdigit(Peek))
                goto error;
            strsize++;
            *ptr++ = Char;
            Next;
        }

        while (isdigit(Char) && Char && strsize < 256)
        {
            *ptr++ = Char;
            ++strsize;
            Next;
        }
        *ptr = '\0';
        tokType = NUMBER;

        iValue = atoi(tokBuffer);
        return tokType;
    }
error:
    Log("Lex> Line %d, %s\n", nLine, textPos);



    return 0;
}

int AirLexer::IsDelim(char c)
{
    if (strchr("[]=,:", c))
        return 1;
    return 0;
}
