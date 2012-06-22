#ifndef INPUTBACKEND_H
#define INPUTBACKEND_H

#include <SDL/SDL.h>

// definições para codificar o valor do input


typedef enum
{
	IBS_START = 1,
	IBS_SELECT,
	IBS_UP,
	IBS_DOWN,
	IBS_LEFT,
	IBS_RIGHT,
	IBS_BUTTON_A,
	IBS_BUTTON_B,
	IBS_BUTTON_C,
	IBS_BUTTON_X,
	IBS_BUTTON_Y,
	IBS_BUTTON_Z,
	EXIT_CODE,
	__INPUT_MAX__,
} InputSymbol;


extern const char *InputSymbolName[__INPUT_MAX__];
/**
 * Classe responsável por nos dizer se algum botão foi pressionado
 */
class InputBackend
{
    public:
        /** Default constructor */
        InputBackend();
        /** Default destructor */
        ~InputBackend();

        int init();
        void close();
        int poll();
        int getstate(InputSymbol inp);
        int bind(InputSymbol inp, int code);
    private:
        unsigned int bindTable[__INPUT_MAX__];
        Uint8 stateTable[__INPUT_MAX__];
        int find();
        Uint8 *keystate;
        Uint32 lastPoll;
        int numkeys;
};

#endif // INPUTBACKEND_H
