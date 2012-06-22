/*
 * SFFV2Loader.cpp
 * Elecbyte Sprite File Format loader
 *
 * Suporte inicial para as compressões RAW,RLE8,RLE5
 * Somente imagens de 8bits
 *
 * 03/05/2012:
 *     Código refatorado, essa classe só é encarregada de carregar o arquivo
 */
#include <cstdio>
#include <cstdlib>
#include <SDL/SDL.h>
#include <list>
#include <map>
#include <utility>
#include "sffv2loader.h"
#include "system.h"
#include "videosystem.h"

using namespace std;

namespace SFFv2
{

SFFV2Loader::SFFV2Loader() {
    fileCache = NULL;
    file = NULL;
    memset(&header, 0, sizeof(struct SpriteFileHeader));
    listSprites.clear();
    listPalettes.clear();
    mapSprites.clear();
}

SFFV2Loader::~SFFV2Loader() {
    Log("Destruindo sff %s...\n", filename_);
    for (list<PaletteDef*>::iterator i = listPalettes.begin(); i != listPalettes.end(); i++)
        SAFE_DELETE(*i);
    for (list<SpriteDef*>::iterator i = listSprites.begin(); i != listSprites.end(); i++)
        SAFE_DELETE(*i);
}

SpriteDef *SFFV2Loader::get(short group, short index) {
#if 1
    for (list<SpriteDef*>::iterator i = listSprites.begin(); i != listSprites.end(); i++) {
        SpriteDef *spr = *i;
        if (spr->group == group && spr->index == index)
            return spr;
    }
#else
    SpriteDef *spr = mapSprites[std::make_pair(group, index)];
    if (spr)
        return spr;
#endif
    return NULL;
}

PaletteDef *SFFV2Loader::getPal(int no) {
    for (list<PaletteDef*>::iterator i = listPalettes.begin(); i != listPalettes.end(); i++) {
        PaletteDef *pal = *i;
        if (pal->palno == no)
            return pal;
    }
    return NULL;
}

int SFFV2Loader::open(const char *filename) {
    FILE *fp = NULL;
    filename_ = filename;

    Log("Carregando o arquivo de sprites %s...\n", filename);
    fp = fopen(filename, "rb");
    if (!fp) {
        Log("Não foi possível carregar o arquivo %s\n", filename);
        return 0;
    }
    // Calculamos o tamanho do arquivo
    fseek(fp, 0, SEEK_END);
    fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    // alocamos um buffer na memória para o arquivo
    fileCache = new unsigned char[fileSize];
    // copiamos o arquivo para a memória para criamos um SDL_RWops
    fread(fileCache, fileSize, 1, fp);
    fclose(fp);
    fp = NULL;

    file = SDL_RWFromConstMem(fileCache, fileSize);

    LogEnter();
    loadHeader();
    loadPalettes();
    loadSprites();
    LogBack();

    SDL_RWclose(file);
    SAFE_DELETE(fileCache);
    return 1;
}

void SFFV2Loader::loadHeader() {
    SDL_RWseek(file, 0, SEEK_SET);
    SDL_RWread(file, &header, sizeof(struct SpriteFileHeader), 1);
#if 0
    Log("sprOffset: 0x%08x\n", header.sprOffset);
    Log("nSprites: %d\n", header.nSprites);

    Log("palOffset: 0x%08x\n", header.palOffset);
    Log("nPalettes: %d\n", header.nPalettes);

    Log("ldataOffset: 0x%08x\n", header.ldataOffset);
    Log("ldataSize: %d\n", header.ldataSize);
    Log("tdataOffset: 0x%08x\n", header.tdataOffset);
    Log("tdataSize: %d\n", header.tdataSize);
#endif
}

void SFFV2Loader::loadPalettes() {
//    Log("Carregando paletas de cores...\n");
    LogEnter();

    for (unsigned i = 0; i < header.nPalettes; i++) {
        SDL_RWseek(file, header.palOffset + (i * sizeof(struct PaletteNode)), SEEK_SET);
        PaletteDef *pal = new PaletteDef();
//        Log("Paleta %d\n", i);
        LogEnter();
        struct PaletteNode palhead;
        SDL_RWread(file, &palhead, sizeof(struct PaletteNode), 1);

#if 0
        // LogInfo
        Log("Group: %d\n", palhead.group);
        Log("Index: %d\n", palhead.index);
        Log("Colors: %d\n", palhead.colors);
        Log("Linked: %d\n", palhead.linked);
        Log("Offset: 0x%08x\n", palhead.offset);
        Log("Size: %d\n", palhead.size);
#endif

        pal->group = palhead.group;
        pal->index = palhead.index;
        pal->palno = i;
        loadPaletteData(pal, &palhead, i);

        listPalettes.push_back(pal);
        LogBack();
    }
    LogBack();
}


void SFFV2Loader::loadPaletteData(PaletteDef *pal, struct PaletteNode *palnode, int thisIndex) {
    SDL_RWseek(file, header.ldataOffset + palnode->offset, SEEK_SET);

    int colors = 0;
    if (palnode->size)
        colors = palnode->size / 4;
    else
        colors = palnode->colors;
    pal->pal.ncolors = colors;
    pal->pal.colors = new SDL_Color[colors];
    SDL_Color *pcol = pal->pal.colors;

    for (int i = 0; i < colors; i++) {
        SDL_RWread(file, &pcol[i].r, 1, 1);
        SDL_RWread(file, &pcol[i].g, 1, 1);
        SDL_RWread(file, &pcol[i].b, 1, 1);
        SDL_RWread(file, &pcol[i].unused, 1, 1);
    }
#if 0 // faz o dump das paletas
    char _filename[256];
    sprintf(_filename, "pal%d.bin", thisIndex);
    FILE *out = fopen(_filename, "wb");
    fwrite(pcol, sizeof(SDL_Color) * colors, 1, out);
    fclose(out);
#endif

}

void SFFV2Loader::loadSprites() {
    Log("Carregando sprites...\n");

    int last_time = SDL_GetTicks();
    SDL_RWseek(file, header.sprOffset, SEEK_SET);

    LogEnter();

    for (unsigned i = 0; i < header.nSprites; i++) {
        SDL_RWseek(file, header.sprOffset + (i * sizeof(struct SpriteNode)), SEEK_SET);
        SpriteDef *spr = new SpriteDef();
        if (!spr) {
            LogFatal("Falta de memória em %s:%d\n", __FILE__,__LINE__);
        }
        spr->thisIndex = i;

        //Log("Sprite %d\n", i);
        LogEnter();
        struct SpriteNode sprhead;
        SDL_RWread(file, &sprhead, sizeof(struct SpriteNode), 1);

        spr->group = sprhead.group;
        spr->index = sprhead.index;
        spr->palno = sprhead.palno;
        spr->xaxis = sprhead.xaxis;
        spr->yaxis = sprhead.yaxis;
#if 0
        Log("group %d, index %d\n", sprhead.group, sprhead.index);
        Log("img %dx%dx%d, palno %d\n", sprhead.width, sprhead.height, sprhead.depth, sprhead.palno);
        Log("axis %d,%d\n", sprhead.xaxis, sprhead.yaxis);
        Log("fmt %d, flags %x\n", sprhead.format, sprhead.flags);
        Log("offset: 0x%08x\n", sprhead.offset);
        Log("size: %d, linked %d\n", sprhead.size, sprhead.linked);

#endif
        loadSpriteData(spr, &sprhead, i);
		//SDL_SetColorKey(spr->surface, SDL_SRCCOLORKEY|SDL_RLEACCEL, 0);
        LogBack();

        listSprites.push_back(spr);
        mapSprites[std::make_pair(spr->group, spr->index)] = spr;
    }
    int time_to_load = SDL_GetTicks() - last_time;
    Log("Sprites (%d) carregados em %fs\n", header.nSprites, (float)time_to_load / 1000.0);
    LogBack();
}

void SFFV2Loader::loadSpriteData(SpriteDef *spr, struct SpriteNode *sprnode, int thisIndex) {
    if (sprnode->linked) {
        for (list<SpriteDef*>::iterator i = listSprites.begin(); i != listSprites.end(); i++) {
            SpriteDef *link = *i;
            if (link->thisIndex == sprnode->linked) {
                spr->surface = SDL_ConvertSurface(link->surface,
                                      link->surface->format,
                                      link->surface->flags);
            }
        }
        return;
    }
    unsigned int loffset = 0;
    if (sprnode->flags & SPR_FLAG_TDATA)
        loffset = header.tdataOffset + sprnode->offset + 4;
    else
        loffset = header.ldataOffset + sprnode->offset + 4;

    SDL_RWseek(file, loffset, SEEK_SET);

    int depth = (sprnode->depth == 5 ? 8 : sprnode->depth);

    spr->surface = g_Video->Surface(sprnode->width, sprnode->height, depth);


    switch (sprnode->format) {
        case SPR_FMT_RAW: decodeRAW(spr, sprnode); break;
        case SPR_FMT_RLE8: decodeRLE8(spr, sprnode); break;
        case SPR_FMT_RLE5: decodeRLE5(spr, sprnode); break;
     //   case SPR_FMT_LZ5: decodeLZ5(spr, sprnode); break;
        default:
            Log("[%d] %d Formato não suportado (%d,%d)!\n", thisIndex, (unsigned)sprnode->format,
                sprnode->group, sprnode->index);
            break;
    }


}

void SFFV2Loader::decodeRAW(SpriteDef *spr, struct SpriteNode *sprnode) {
    for (int y = 0; y < spr->surface->h; y++) {
        char *ptr = ((char*)spr->surface->pixels + (y * spr->surface->pitch));
        SDL_RWread(file, ptr, sprnode->width, 1);
    }

}

void SFFV2Loader::decodeRLE8(SpriteDef *spr, struct SpriteNode *sprnode) {
    char *buffer = new char[sprnode->width * sprnode->height];
    char *ptr = buffer;

    char packet = 0;
    char data = 0;
    int runsize = 0;
    unsigned int count = 0;
    int write = 0;

    const int max_buf_size = (sprnode->width * sprnode->height);

    if (!sprnode->size)
        return;

    while (count < sprnode->size) {
        SDL_RWread(file, &packet, 1, 1);

        if ((packet & 0xC0) == 0x40) {
            runsize = packet & 0x3F;
            SDL_RWread(file, &data, 1, 1);
            count++;
        }
        else {
            runsize = 1;
            data = packet;
        }
        while (runsize > 0) {
            if (write < max_buf_size) {
                *ptr++ = data;
                write++;
            }
            runsize--;

        }
        count++;
    }

    for (int y = 0; y < spr->surface->h; y++) {
        char *xptr = ((char*)spr->surface->pixels + (y * spr->surface->pitch));
        memcpy(xptr, &buffer[y * sprnode->width], sprnode->width);
    }
    SAFE_DELETE(buffer);
}


void SFFV2Loader::decodeRLE5(SpriteDef *spr, struct SpriteNode *sprnode) {
    char *buffer = new char[sprnode->width * sprnode->height];
    char *ptr = buffer;
    unsigned int count = 0;
    int write = 0;

	// Para evitarmos buffer overflow
    const int max_buf_size = (sprnode->width * sprnode->height);

    if (!sprnode->size)
        return;

    while (count < sprnode->size) {
        unsigned char run_lenght = 0, color_bit = 0, data_lenght = 0, color = 0;
        SDL_RWread(file, &run_lenght, 1, 1);
        SDL_RWread(file, &color_bit, 1, 1);

        data_lenght = color_bit & 0x7F;
        count += 2;

        if (color_bit & 0x80) {
            SDL_RWread(file, &color, 1, 1);
            count++;
        } else
            color = 0;

        for (unsigned char i = 0; i <= run_lenght; i++) {
            if (write < max_buf_size) {
                *ptr++ = color;
                ++write;
            }
        }
        for (unsigned char i = 0; i < data_lenght; i++) {
            SDL_RWread(file, &color, 1, 1);
            count++;
            run_lenght = color >> 5;

			// não sei porque, mas só assim esse código funcionou '-'
            run_lenght++;

            color &= 0x1F;
            while (run_lenght > 0) {
                if (write < max_buf_size) {
                    *ptr++ = color;
                    ++write;
                }
                run_lenght--;
            }
        }
    }
    for (int y = 0; y < spr->surface->h; y++) {
        char *xptr = ((char*)spr->surface->pixels + (y * spr->surface->pitch));
        memcpy(xptr, &buffer[y * sprnode->width], sprnode->width);
    }
    SAFE_DELETE(buffer);
}

void SFFV2Loader::decodeLZ5(SpriteDef *spr, struct SpriteNode *sprnode) {
    char *buffer = new char[sprnode->width * sprnode->height];
    char *ptr = buffer;
    unsigned int count = 0;
    int write = 0;

	// Para evitarmos buffer overflow
    const int max_buf_size = (sprnode->width * sprnode->height);

    if (!sprnode->size)
        return;

    while (count < sprnode->size) {
        unsigned char ctrl_packet = 0;
        unsigned char packet_types[8];

        SDL_RWread(file, &ctrl_packet, 1, 1);
        SDL_RWread(file, packet_types, 8, 1);
        count += 9;

        for (int packet = 0; packet < 8; packet++) {
            unsigned char data = packet_types[packet];
            // lz5
            if (ctrl_packet & (1 << packet)) {
                if (!(data & 0x1F)) {
                    unsigned char copy_lenght = data >> 6;
                    Log("cl%d",copy_lenght);

                } else {

                    Log("short lz5,");

                }
            } else {
                // rle short
                if (data & 0xE0) {
                    int run_lenght = (data & 0xE0) >> 5;
                    unsigned char color = data & 0x1F;
                    while (run_lenght > 0) {
                        if (write < max_buf_size) {
                            *ptr++ = color;
                            ++write;
                        }
                        run_lenght--;
                    }
                } else {
                    // rle long
                    unsigned char run_lenght = 0;
                    unsigned char color = data & 0x1F;
                    SDL_RWread(file, &run_lenght, 1, 1);
                    count++;

                     while (run_lenght > 0) {
                        if (write < max_buf_size) {
                            *ptr++ = color;
                            ++write;
                        }
                        run_lenght--;
                    }
                }
            }
        }
    }

    for (int y = 0; y < spr->surface->h; y++) {
        char *xptr = ((char*)spr->surface->pixels + (y * spr->surface->pitch));
        memcpy(xptr, &buffer[y * sprnode->width], sprnode->width);
    }
    SAFE_DELETE(buffer);
}


 SpriteDef::SpriteDef() {
    surface = NULL;
    hsurface = NULL;
    vsurface = NULL;
    vhsurface = NULL;
};

SpriteDef::~SpriteDef() {
// imagem destruida pelo shared_ptr
//    if (surface)
//        SDL_FreeSurface(surface);
    if (hsurface)
        SDL_FreeSurface(hsurface);
    if (vsurface)
        SDL_FreeSurface(vsurface);
    if (vhsurface)
        SDL_FreeSurface(vhsurface);
};

PaletteDef::PaletteDef() {
};
PaletteDef::~PaletteDef() {
    SAFE_DELETE(pal.colors);
}


}

