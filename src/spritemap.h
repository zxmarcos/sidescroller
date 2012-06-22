#ifndef SPRITEMAP_H
#define SPRITEMAP_H

#include "sffv2loader.h"
#include "spritemanager.h"

#define USE_SPRMGR  1

class SpriteMap
{
    public:
        SpriteMap();
        ~SpriteMap();

		int open(const char *filename);
		int close();

		int parse(int group, int index);
		int x();
		int y();
		int width();
		int height();

		void drawAt(int dx, int dy, bool v, bool h);
    private:
        /**
		 * Função chamada para receber qual surface desenhar, dependendo
		 * da inversão (H,V,HV,), se não existir ela irá criar as surfaces;
		 * @arg v	Inversão vertical
		 * @arg h	Inversão horizontal
		 */
		SDL_Surface *whatsSurface(bool v, bool h);

		bool isloaded;
#if USE_SPRMGR
		SFFV2LoaderPtr loader;
#else
		SFFV2Loader loader;
#endif
		SFFv2::SpriteDef *spr;
		SFFv2::PaletteDef *pal;
};

#endif // SPRITEMAP_H
