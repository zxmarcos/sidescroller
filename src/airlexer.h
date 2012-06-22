#ifndef AIRLEXER_H
#define AIRLEXER_H


class AirLexer
{
    public:
        enum {
            BEGIN = 256,
            ACTION,
            NUMBER,
            LITERAL,
            CLSN1,
            CLSN2,
            CLSN1DEF,
            CLSN2DEF,
            LOOPSTART,
            RBRK = '[',
            LBRK = ']',
            EQUAL = '=',
            COLON = ':',
            SEMICOLON = ',',
            EOS = 0,
        };

        AirLexer();
        virtual ~AirLexer();

        int GetToken();
        void SetBuffer(char *str);

    private:
        int nLine;
        int IsDelim(char c);
        int iValue;
        int tokType;
        char tokBuffer[256];
        char *textBase;
        char *textPos;

    public:
        int IValue() const { return iValue; }

};

#endif // AIRLEXER_H
