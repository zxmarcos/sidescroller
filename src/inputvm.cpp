#include "inputvm.h"
#include "system.h"
#include "inputsystem.h"
#include "inputbackend.h"
#include <string>

using namespace std;

#define F_LNK   0x08000000
#define F_RLS   0x10000000
#define F_HLD   0x20000000
#define F_BUT   0x40000000
#define F_DIR   0x80000000

enum {
    dU = (1 << 0),
    dD = (1 << 1),
    dB = (1 << 2),
    dF = (1 << 3),
    ba = (1 << 4),
    bb = (1 << 5),
    bc = (1 << 6),
    bx = (1 << 7),
    by = (1 << 8),
    bz = (1 << 9),
};


InputStream *buildcmd(const char *str);


InputStream *qcf;

void showcmd(InputStream *x) {
    string *s = new string();
    for (size_t i = 0; i < x->stream.size(); i++) {
        int thiscmd = 0;
        XInputstate *st = x->stream[i];
        if (!st)
            continue;
        for (int n = 0; n < __INPUT_MAX__; n++) {
            if (st->sym[n].state == KS_UP)
                continue;
            else
            if (st->sym[n].state == KS_PRESSED) {
                if (thiscmd) s->append("+"); ++thiscmd;
                s->append(InputSymbolName[n]);
            }
            else
            if (st->sym[n].state == KS_RELEASE) {
                if (thiscmd) s->append("+"); ++thiscmd;
                s->append("~");
                s->append(InputSymbolName[n]);
            }
            else
            if (st->sym[n].state == KS_HOLDING) {
                if (thiscmd) s->append("+"); ++thiscmd;
                s->append("/");
                s->append(InputSymbolName[n]);
            }
        }
        if (thiscmd) s->append(",");
        thiscmd = 0;
    }
    if (s->size() > 1)
        s->append("\n");
    const char *str = s->c_str();
    if (s->size() > 1)
        Log(str);

    delete str;
    delete s;
}

InputVM::InputVM(){
    bufpos = 0;
    last = 0;
    blankcount = 0;

    //qcf = buildcmd("D,DF,F,a");
   // qcf = buildcmd("a,/F,~D+/F,/D+/F,/D+F,/D,D");
    qcf = buildcmd("D,/D,/D+F,/D+/F,~D+/F,/F,a");
    //Log("testando comando:");
//    showcmd(qcf);
}

InputVM::~InputVM() {
}


void InputVM::checkForCommands() {
    bool asserted = true;
    if (!bufpos)
        return;
    int i_buf = bufpos - 1;
    int s_pos = (int)qcf->stream.size() - 1;
    if (s_pos < 0)
        return;

    while (s_pos >= 0) {
        XInputstate *p = qcf->stream[s_pos];
        for (int n = 1; n < __INPUT_MAX__; n++) {
            //D,/D,/D+F,/D+/F,~D+/F,/F,a
            switch (p->sym[n].state) {
                case KS_UP: continue;
                case KS_PRESSED:
                    if ((kbuffer[i_buf].sym[n].state == KS_PRESSED) ||
                        (kbuffer[i_buf].sym[n].state == KS_HOLDING))
                        continue;
                    else asserted = false;
                        break;
                case KS_HOLDING:
                    if (kbuffer[i_buf].sym[n].state == KS_HOLDING)
                        continue;
                    else asserted = false;
                        break;
                case KS_RELEASE:
                    if (kbuffer[i_buf].sym[n].state == KS_RELEASE)
                        continue;
                    else asserted = false;
                        break;
            }
        }
        //if (!asserted)
        //    Log("Não aceito em buf[%d]:str[%d]\n",i_buf,s_pos);
        s_pos--;
        i_buf--;
    }
    if (asserted)
        Log("\nComando aceito!\n");
}

void InputVM::update() {
    updateBuffer();
    checkForCommands();
}


void InputVM::shiftBuffer() {
    // desloca todos elementos uma posição para trás
    // o primeiro elemento é descartado
    for (int i = 1; i < KS_BUF_MAX; i++)
        for (int n = 1; n < __INPUT_MAX__; n++)
            kbuffer[i - 1].sym[n] = kbuffer[i].sym[n];
    // coloca no último elemento
    bufpos = KS_BUF_MAX - 1;
}

void InputVM::push(XInputstate& st) {
    if (bufpos < KS_BUF_MAX) {
        for (int n = 1; n < __INPUT_MAX__; n++) {
            kbuffer[bufpos].sym[n] = st.sym[n];
        }
    } else {
        shiftBuffer();
        for (int n = 1; n < __INPUT_MAX__; n++)
            kbuffer[bufpos].sym[n] = st.sym[n];
    }
    bufpos++;
}
bool InputVM::isLast(XInputstate& st) {
    if (!bufpos)
        return false;

    bool yes = true;
    // posição zero, não há nada no buffer
    for (int n = 1; n < __INPUT_MAX__; n++) {
        if (kbuffer[bufpos-1].sym[n].state != st.sym[n].state) {
            yes = false;
            break;
        }
    }
    return yes;
}

bool InputVM::isNeutral(XInputstate& st) {
    bool yes = true;
    // posição zero, não há nada no buffer
    for (int n = 1; n < __INPUT_MAX__; n++) {
        if (st.sym[n].state != KS_UP) {
            yes = false;
            break;
        }
    }
    return yes;
}
// atualiza o buffer de simbolos de entrada
void InputVM::updateBuffer() {
    XInputstate thisTime;
    memset(&thisTime,0,sizeof(thisTime));
    for (int n = 1; n < __INPUT_MAX__; n++) {
        thisTime.sym[n] = g_Input->getstate(n);
    }
    if (isNeutral(thisTime))
        return;

    if (!isLast(thisTime))
        push(thisTime);
}

#define STATE   ((flag & F_HLD) ? KS_HOLDING : ((flag & F_RLS) ? KS_RELEASE : KS_PRESSED))

InputStream *buildcmd(const char *str) {
    int ninputs = 0;

    InputStream *is = new InputStream;
    XInputstate *inp = new XInputstate;
    is->stream.clear();

    unsigned flag = 0;

    while (*str) {
        char c = *str;
        switch (c) {
            // novo input no stream
            case ',': {
                is->push(inp);
                inp = new XInputstate;
                ninputs++;
                flag = 0;
                break;
            }
            case '+':
                flag = 0;
                break;

            case '~': {
                if (flag & F_HLD);
                else
                    flag |= F_RLS;
                break;
            }
            case '/': {
                if (flag & F_RLS);
                else
                    flag |= F_HLD;
                break;
            }
            case 'D': {
                inp->sym[IBS_DOWN].state = STATE;
                break;
            }
            case 'F': {
                inp->sym[IBS_RIGHT].state = STATE;
                break;
            }
            case 'U': {
                inp->sym[IBS_UP].state = STATE;
                break;
            }
            case 'B': {
                inp->sym[IBS_LEFT].state = STATE;
                break;
            }
            case 'a': {
                inp->sym[IBS_BUTTON_A].state = STATE;
                break;
            }
        }
        ++str;
        if (!*str) {
            ninputs++;
            is->push(inp);
        }

    }

    return is;
}
