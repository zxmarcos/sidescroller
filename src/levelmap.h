#ifndef LevelMap_H
#define LevelMap_H
#include <SDL/SDL.h>
#include <list>
#include "TinyXML/tinyxml2.h"
#include "tileset.h"
#include "layer.h"
#include "colobject.h"
#include "colobjectgroup.h"
#include <map>

using namespace tinyxml2;
class TileSet;
class Layer;
class ColObjectGroup;
class LevelMap;

extern LevelMap *g_Map;

/**
 * Tipos de tile
 */
typedef enum TileTypes
{
	TILE_NORMAL = 0,
	TILE_SOLID,
	TILE_SLOPEFLAG,
	TILE_SLOPER,
	TILE_SLOPEL,
	TILE_VSLOPER,
	TILE_VSLOPEL,
	TILE_GROUNDABLE,
	TILE_SLOPELINK,
	TILE_MAX_NAME
} TileType;

extern const char *TileTypeName[];

class LevelMap
{
    public:
        LevelMap();
        virtual ~LevelMap();
		/**
		 * Carrega um arquivo .TMX
		 * @arg name	Nome do arquivo .TMX
		 *
		 * Somente é suportado arquivos com codificação base64 e comprimidos com zlib
		 */
        int open(const char *name);
		/**
		 * Renderiza o map de acordo com as coordenadas x e y em pixels.
		 * @arg x	Posição x da camera
		 * @arg y	Posição y da camera
		 */
        void render(int x, int y);
		/**
		 * Faz o blit do mapa renderizado na tela.
		 */
        void blit();

		/**
		 * Retorna o tipo do tile nas coordenadas x e y do mapa
		 * @arg x	Posição x do tile
		 * @arg y	Posição y do tile
		 */
		int typeAt(int x, int y);

		/**
		 * Desenha um tile pelo seu id global em uma surface
		 * @arg dst		Surface de destino
		 * @arg id		GID - Identificador global do tile
		 * @arg sx		Posição x para desenhar
		 * @arg sy		Posição y para desenhar
		 */
        void drawTile(SDL_Surface *dst, int id, int sx, int sy);
		/**
		 * Retorna a altura do map em número de tiles
		 */
        int h();
		/**
		 * Retorna a largura do mapa em número de tiles
		 */
        int w();
		/**
		 * Retorna a largura dos tiles em número de pixels
		 */
		int tilew();
		/**
		 * Retorna a altura dos tiles em número de pixels
		 */
        int tileh();

        int xmax();
        int ymax();

        void DrawColObjectGroup();
        int CheckCollision(ColObject *obj);

    private:
        void loadTilesets(XMLElement *map);
		void loadTiletypes(XMLElement *root, TileSet *ts);
        void loadLayers(XMLElement *map);
        void loadLayerData(Layer *layer, XMLElement *li);
        void loadBase64zlib(Layer *layer, XMLElement *data);
        void loadColObjectGroups(XMLElement *map);
        void loadColObjetcs(XMLElement *eg, ColObjectGroup *group);

        SDL_Surface *renderBuffer;
        SDL_Rect renderPos;
		XMLDocument *doc;
		int *zplane;
        int renderX;
        int renderY;
        int renderXscroll;
        int renderYscroll;
        int tileWidth;
        int tileHeight;
        int mapWidth;
        int mapHeight;
        int nTileSets;
        int nLayers;
        std::map<int, int> tileType;
        std::list<TileSet*> listTileSet;
        std::list<Layer*> listLayers;
        std::list<ColObjectGroup*> listColGroups;
};



#endif // LevelMap_H
