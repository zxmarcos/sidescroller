
#include "spritemap.h"
#include "system.h"
#include "videosystem.h"
#include "spritemanager.h"

SpriteMap::SpriteMap() {
    isloaded  = false;
    spr = NULL;
    pal = NULL;
}

SpriteMap::~SpriteMap()
{
}


int SpriteMap::open(const char *filename) {
#if USE_SPRMGR
    loader = g_SpriteManager->open(filename);
	if (!loader)
		Log("Falha com os sprites!\n");
#else
    loader.open(filename);
#endif
	isloaded = true;
	return 1;
}
int SpriteMap::close() {
	return 1;
}
int SpriteMap::parse(int group, int index) {
#if USE_SPRMGR
    spr = loader->get(group, index);
#else
	spr = loader.get(group, index);
#endif
	if (!spr) {
		Log("Não foi possível encontrar o sprite (%d:%d)\n", group, index);
		return 0;
	}
	return 1;
}
int SpriteMap::x() {
	if (spr)
		return spr->xaxis;
	return 0;
}
int SpriteMap::y() {
	if (spr)
		return spr->yaxis;
	return 0;
}
int SpriteMap::width() {
	if (spr)
		return spr->surface->w;
	return 0;
}
int SpriteMap::height() {
	if (spr)
		return spr->surface->h;
	return 0;
}
void SpriteMap::drawAt(int dx, int dy, bool v, bool h) {
    SDL_Surface *surf = whatsSurface(v, h);

#if USE_SPRMGR
    pal = loader->getPal(spr->palno);
#else
    pal = loader.getPal(spr->palno);
#endif
    if (pal)
        surf->format->palette = &pal->pal;

    SDL_Rect r;
    r.x = dx - spr->xaxis;
    r.y = dy - spr->yaxis;
    r.w = surf->w;
    r.h = surf->h;
    // usar o a transparência do sprite
    SDL_SetColorKey(surf, SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
    g_Video->Blit(surf, NULL, &r);
}


SDL_Surface *SpriteMap::whatsSurface(bool v, bool h) {

    // surface sem transformações
    if (v == false && h == false) {
        if (spr)
            return spr->surface;
        else
            return NULL;
    }

    SDL_Surface *surf = NULL;

    if (h == true && v == false) {
        // não criamos uma imagem invertida ainda
        if (!spr->hsurface) {
            // cria uma cópia da surface principal
            surf = SDL_ConvertSurface(spr->surface,
                                      spr->surface->format,
                                      spr->surface->flags);

            for (int y = 0; y < surf->h; y++) {
                Uint8 *src = (Uint8*)spr->surface->pixels + (spr->surface->w - 1 +
                                                           (y * spr->surface->pitch));
                Uint8 *dst = (Uint8*)surf->pixels + (y * surf->pitch);
                for (int x = 0; x < surf->w; x++)
                    *dst++ = *src--;
            }
            spr->hsurface = surf;
        } else
            surf = spr->hsurface;
    }

    if (h == false && v == true)
    {
        // não criamos uma imagem invertida ainda
        if (!spr->vsurface) {
            // cria uma cópia da surface principal
            surf = SDL_ConvertSurface(spr->surface,
                                      spr->surface->format,
                                      spr->surface->flags);

            for (int y = 0, k = surf->h - 1; y < surf->h; y++, k--) {
                Uint8 *src = (Uint8*)spr->surface->pixels + (k * spr->surface->pitch);
                Uint8 *dst = (Uint8*)surf->pixels + (y * surf->pitch);
                for (int x = 0; x < surf->w; x++)
                    *dst++ = *src++;
            }
            spr->vsurface = surf;
        } else
            surf = spr->vsurface;
    }

    if (h == true && v == true)
    {
        // não criamos uma imagem invertida ainda
        if (!spr->vhsurface) {
            // pegamos a surface já invertida horizontalmente
            SDL_Surface *srcSurface = whatsSurface(false, true);

            // cria uma cópia da surface principal
            surf = SDL_ConvertSurface(srcSurface,
                                      srcSurface->format,
                                      srcSurface->flags);

            for (int y = 0, k = srcSurface->h - 1; y < surf->h; y++, k--) {
                Uint8 *src = (Uint8*)srcSurface->pixels + (k * srcSurface->pitch);
                Uint8 *dst = (Uint8*)surf->pixels + (y * surf->pitch);
                for (int x = 0; x < surf->w; x++)
                    *dst++ = *src++;
            }
            spr->vhsurface = surf;
        } else
            surf = spr->vhsurface;
    }
    return surf;
}
