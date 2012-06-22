#ifndef SFFV2LOADER_H
#define SFFV2LOADER_H

#include <cstdio>
#include <cstdlib>
#include <SDL/SDL.h>
#include <list>
#include <map>
#include <utility>

class SpriteMap;

// Formato de sprites em SFFv2 da Elecbyte (MUGEN)
namespace SFFv2
{

class PaletteDef;
class SpriteDef;

struct SpriteFileHeader
{
    char sign[12];
    char version[4];
    unsigned : 32; // reservado
    unsigned : 32; // reservado
    char compatver[4];
    unsigned : 32; // reservado
    unsigned : 32; // reservado
    unsigned int sprOffset;
    unsigned int nSprites;
    unsigned int palOffset;
    unsigned int nPalettes;
    unsigned int ldataOffset;
    unsigned int ldataSize;
    unsigned int tdataOffset;
    unsigned int tdataSize;
    unsigned : 32; // reservado
    unsigned : 32; // reservado
    char unused[436];
} __attribute__((packed));

typedef enum {
    SPR_FMT_RAW = 0,
    SPR_FMT_INVALID,
    SPR_FMT_RLE8,
    SPR_FMT_RLE5,
    SPR_FMT_LZ5
} SPRITE_DATA_FORMAT;

typedef enum {
    SPR_FLAG_TDATA = 1,
} SPRITE_FLAGS;

struct SpriteNode
{
    short group;
    short index;
    short width;
    short height;
    short xaxis;
    short yaxis;
    short linked;
    char format;
    char depth;
    unsigned int offset;
    unsigned int size;
    short palno;
    unsigned short flags;
} __attribute__((packed));

struct PaletteNode
{
    short group;
    short index;
    short colors;
    short linked;
    unsigned int offset;
    unsigned int size;
} __attribute__((packed));


class SFFV2Loader
{
    public:
        SFFV2Loader();
        ~SFFV2Loader();
        int open(const char *filename);
        SpriteDef *get(short group, short index);
        PaletteDef *getPal(int no);

    protected:
    private:
        const char *filename_;
		/**
		 * Função refatorada para ler o cabeçalho do arquivo
		 */
        void loadHeader();
		/**
		 * Função refatorada para ler as paletas do arquivo
		 */
        void loadPalettes();
		/**
		 * Função utilizada para ler os dados da paleta no arquivo
		 * @arg pal	Definição da paleta onde os dados serão carregados
		 * @arg palnode	Nó da paleta no arquivo contendo as inforamções de localização
		 * @arg thisIndex Número da paleta
		 */
        void loadPaletteData(PaletteDef *pal, struct PaletteNode *palnode, int thisIndex);
		/**
		 * Função refatorada para ler os sprites do arquivo
		 */
        void loadSprites();
		/**
		 * Função utilizada para ler os dados dos sprites no arquivo
		 * @arg spr	Definição do sprite onde os dados serão carregados
		 * @arg sprnode	Nó dos sprites no arquivo contendo as inforamções de localização
		 * @arg thisIndex Número da sprite
		 */
        void loadSpriteData(SpriteDef *spr, struct SpriteNode *sprnode, int thisIndex);
		/**
		 * Funções utilizadas para decodificar os dados dos sprites no arquivo
		 */
        void decodeRAW(SpriteDef *spr, struct SpriteNode *sprnode);
        void decodeRLE8(SpriteDef *spr, struct SpriteNode *sprnode);
        void decodeRLE5(SpriteDef *spr, struct SpriteNode *sprnode);
        void decodeLZ5(SpriteDef *spr, struct SpriteNode *sprnode);
		SDL_RWops *file;
        unsigned char *fileCache;
        unsigned int fileSize;
        struct SpriteFileHeader header;
        std::list<PaletteDef*> listPalettes;
        std::list<SpriteDef*> listSprites;
        std::map<std::pair<int,int>, SpriteDef *> mapSprites;
};

class PaletteDef
{
    friend class SFFV2Loader;
    friend class ::SpriteMap;
    public:
        PaletteDef();
        ~PaletteDef();
    private:
        short palno;
        short group;
        short index;
        SDL_Palette pal;
};

class SpriteDef
{
    friend class SFFV2Loader;
    friend class ::SpriteMap;
    public:
        SpriteDef();
        ~SpriteDef();

    private:
        int thisIndex;
        short group;
        short index;
        short xaxis;
        short yaxis;
        short palno;
        bool isLinked;
        SDL_Surface *surface;
        SDL_Surface *hsurface;
        SDL_Surface *vsurface;
        SDL_Surface *vhsurface;
};

};

#endif // SFFV2LOADER_H
