#ifndef ANIMMAPPER_H
#define ANIMMAPPER_H

#include <list>
#include <map>
#include <vector>
#include "animmanager.h"
#include "colobjectgroup.h"
#include "airloader.h"


class AnimMap;
class ActionDef;
class FrameDef;
class AIRLoader;
typedef boost::shared_ptr<AIRLoader> AIRLoaderPtr;

#define USE_ANIMGR  1

class AnimMap
{
    public:
		AnimMap();
		~AnimMap();
		/**
		 * Carrega um arquivo de animações.
		 * @arg filename Nome do arquivo
		 */
		void open(const char *filename);
		/**
		 * Escolhe a animação para exibir
		 * @arg no  Número do action
		 */
		void set(int no);
		/**
		 * Atualiza os contadores da animação com 1 tick de tempo.
		 */
		void update();
		/**
		 * Retorna o número do action.
		 */
		int number();
		/**
		 * Retorna o número do grupo do sprite atual da animação.
		 */
		int group();
		/**
		 * Retorna o número do indice do sprite atual da animação.
		 */
		int index();
		/**
		 * Retorna o valor de x do sprite atual da animação.
		 */
		int x();
		/**
		 * Retorna o valor de y do sprite atual da animação.
		 */
		int y();
		/**
		 * Retorna se o sprite atual é invertido horizontalmente.
		 */
		bool fliph();
		/**
		 * Retorna se o sprite atual é invertido verticalmente.
		 */
		bool flipv();
		ColObjectGroup *clsn1();
		ColObjectGroup *clsn2();
		ColObjectGroup *clsn1default();
		ColObjectGroup *clsn2default();

    private:
		FrameDef *frame;
		ActionDef *action;
#if USE_ANIMGR
		AIRLoaderPtr loader;
#else
		AIRLoader *loader;
#endif

		// player
		int time;
		int elemTime;
		int elem;
};


#endif // ANIMMAP_H
