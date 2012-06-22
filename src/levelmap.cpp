/*
 *  Carregador de levels no formato TMX (Tiled)
 *
 *
 **/
#include <cstdio>
#include <cstdlib>
#include <zlib.h>
#include <list>

#include <boost/foreach.hpp>

#include "levelmap.h"
#include "system.h"
#include "tileset.h"
#include "util.h"
#include "videosystem.h"

#define LOG_INFO    0

const char *TileTypeName[TILE_MAX_NAME] = {
    "normal",
    "solid",
    "slope_flag",
    "slope_r",
    "slope_l",
    "v_slope_r",
    "v_slope_l",
    "groundable",
    "slope_link"
};

using namespace std;

LevelMap::LevelMap()
{
    renderX = -9999;
    renderY = -9999;
}

LevelMap::~LevelMap()
{
    foreach (TileSet *tileset, listTileSet)
        SAFE_DELETE(tileset);
    foreach (Layer *layer, listLayers)
        SAFE_DELETE(layer);
    foreach (ColObjectGroup *group, listColGroups)
        SAFE_DELETE(group);
}

int LevelMap::open(const char *name)
{
    doc = new XMLDocument();

    Log("Carregando o mapa %s...", name);
    // Carrega o nosso mapa
    if (doc->LoadFile(name) != XML_NO_ERROR) {
        Log("Erro ao carregar o mapa %s\n", name);
        return 0;
    }
    else
        Log("OK\n");

    // Fazemos o parsing das informações gerais do mapa
    LogEnter();
    XMLElement *map = doc->FirstChildElement("map");
    mapWidth = strtoint(map->Attribute("width"));
    mapHeight = strtoint(map->Attribute("height"));
    tileWidth = strtoint(map->Attribute("tilewidth"));
    tileHeight = strtoint(map->Attribute("tileheight"));
#if LOG_INFO
    Log("Version: %s\n", map->Attribute("version"));
    Log("Orientation: %s\n", map->Attribute("orientation"));
    Log("Size: %dx%d\n", mapWidth, mapHeight);
    Log("TileSize: %dx%d\n", tileWidth, tileHeight);
#endif
    zplane = new int [mapWidth * mapHeight];
    memset(zplane, 0, mapWidth*mapHeight*sizeof(int));


    loadTilesets(map);
    loadLayers(map);
    loadColObjectGroups(map);
    LogBack();


    // Criaremos nosso buffer para renderização
    int scrXTiles = (g_Video->GetW() / tileWidth) + 2;
    int scrYTiles = (g_Video->GetH() / tileHeight) + 2;

    renderBuffer = g_Video->Surface(scrXTiles * tileWidth,
                                    scrYTiles * tileHeight,
                                    16);

    // carregamos a informação sobre o plano Z do cenário
    // entre os layers
    //foreach (Layer *layer, listLayers) {
    for (list<Layer*>::iterator i=listLayers.begin(); i != listLayers.end(); i++) {
            Layer *layer = *i;
        for (int y = 0; y < mapHeight; y++) {
            for (int x = 0; x < mapWidth; x++) {
                if (layer->nTileArray[x + y * mapWidth] != 0)
                    zplane[x + y * mapWidth] = layer->nid;
            }
        }
    }

#if 0
    // Fazemos o dump da tabela de tipos
    Log("TileTable\n");
    LogSaveState();
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            Log("%d,", typeAt(x*16,y*16));
        }
        Log("\n");
    }
    LogLoadState();
#endif
    SAFE_DELETE(doc);

    return 1;
}

int LevelMap::h()
{
    return mapHeight;
}
int LevelMap::w()
{
    return mapWidth;
}

int LevelMap::tileh()
{
    return tileHeight;
}
int LevelMap::tilew()
{
    return tileWidth;
}

int LevelMap::xmax() {
    return mapWidth*tileWidth;
}
int LevelMap::ymax() {
    return mapHeight*tileHeight;
}

int LevelMap::typeAt(int x, int y) {
    const int tilex = x / tileWidth;
    const int tiley = y / tileHeight;
    const int pos = tilex + (tiley * w());

	for (list<Layer*>::iterator i=listLayers.begin(); i != listLayers.end(); i++) {
            Layer *layer = *i;
        // temos o nosso layer
        if (layer->nid == zplane[pos]) {
            const int tileid = layer->nTileArray[pos];
            return tileType[tileid];
        }
    }
    return 0;
}

void LevelMap::loadTilesets(XMLElement *map)
{
    Log("Carregando os tilesets...\n");

    nTileSets = 0;
    XMLElement *tileset = map->FirstChildElement("tileset");
    while (tileset) {
        TileSet *tset = new TileSet();

        tset->name = strdup(tileset->Attribute("name"));
        tset->firstGID = strtoint(tileset->Attribute("firstgid"));
        tset->tileWidth = strtoint(tileset->Attribute("tilewidth"));
        tset->tileHeight = strtoint(tileset->Attribute("tileheight"));
        tset->spacing = strtoint(tileset->Attribute("spacing"));
        tset->margin = strtoint(tileset->Attribute("margin"));

        tset->props.load(tileset);

        LogEnter();
#if LOG_INFO
        tset->LogInfo();
#endif
        XMLElement *image = tileset->FirstChildElement("image");
        if (image) {
            TileImage *timg = new TileImage(tset);

            timg->filename = strdup(image->Attribute("source"));
            timg->trans = strdup(image->Attribute("trans"));
            timg->width = strtoint(image->Attribute("width"));
            timg->height = strtoint(image->Attribute("height"));

            timg->surface = IMG_Load(timg->filename);
            if (!timg->surface) {
                Log("Não foi possível carregar a imagem %s\n", timg->filename);
                Log("IMG_Error: %s\n", IMG_GetError());
            }
            SDL_Surface *xsurf = timg->surface;
            timg->surface =  SDL_DisplayFormat(xsurf);

            // transparência
            if (timg->trans && timg->surface) {
                int r = 0, g = 0, b = 0;
                ParseRGB(timg->trans, &r, &g, &b);
                SDL_SetColorKey(timg->surface,
                                SDL_SRCCOLORKEY|SDL_RLEACCEL,
                                SDL_MapRGB(timg->surface->format, r, g, b));
            }


            tset->image = timg;
            tset->nXTiles = (timg->width / tset->tileWidth);
            tset->nYTiles = (timg->height / tset->tileWidth);
            tset->nTiles = tset->nXTiles * tset->nYTiles;

            // isso é realmente necessário???
			tset->typeArray = new int[tset->nTiles];
#if LOG_INFO
            LogEnter();
            timg->LogInfo();
            LogBack();
#endif
			loadTiletypes(tileset, tset);
			tileType[0] = TILE_NORMAL;
        }
        LogBack();
        listTileSet.push_back(tset);

        // incrementa o número de tilesets
        nTileSets++;
        tileset = tileset->NextSiblingElement("tileset");
    }
    Log("Encontrados %d tilesets\n", nTileSets);
}

void LevelMap::loadTiletypes(XMLElement *root, TileSet *ts)
{
	Log("Carregando os tipos...\n");
	XMLElement *tile = root->FirstChildElement("tile");
	while (tile) {
		// carrega o id do tile
		int id = strtoint(tile->Attribute("id"));
		// procura pelo nó de propriedades
		XMLElement *propts = tile->FirstChildElement("properties");
		// valor padrão do tipo do tile
		ts->typeArray[id] = TILE_NORMAL;
		const int gid = ts->firstGID + id;

		if (propts) {
			// carrega o nó property
			XMLElement *prop = propts->FirstChildElement("property");
			if (prop) {
				// verifica se o tipo desse nó é do tipo "type"
				if (!stricmp(prop->Attribute("name"), "type")) {
					const char *typestr = prop->Attribute("value");

					if (!stricmp(typestr, "normal"))
						ts->typeArray[id] = TILE_NORMAL;
					else
					if (!stricmp(typestr, "solid"))
						ts->typeArray[id] = TILE_SOLID;
					else
					if (!stricmp(typestr, "sloper"))
						ts->typeArray[id] = TILE_SLOPER;
					else
					if (!stricmp(typestr, "slopel"))
						ts->typeArray[id] = TILE_SLOPEL;
                    else
					if (!stricmp(typestr, "vslopel"))
						ts->typeArray[id] = TILE_VSLOPEL;
                    else
					if (!stricmp(typestr, "vsloper"))
						ts->typeArray[id] = TILE_VSLOPER;
					else
					if (!stricmp(typestr, "groundable"))
						ts->typeArray[id] = TILE_GROUNDABLE;
					else
					if (!stricmp(typestr, "slopeflag"))
						ts->typeArray[id] = TILE_SLOPEFLAG;
					else
					if (!stricmp(typestr, "slopelink"))
						ts->typeArray[id] = TILE_SLOPELINK;
					else
						Log("Tipo inválido %s\n", typestr);

                    tileType[gid] = ts->typeArray[id];
#if LOG_INFO
					Log("Tile %d, tipo %d, %s\n", id, ts->typeArray[id], typestr);
#endif
				}
			}
		}
		tile = tile->NextSiblingElement("tile");;
	}
}

void LevelMap::loadLayers(XMLElement *map)
{
    Log("Carregando os Layers...\n");

    nLayers = 0;
    XMLElement *layer = map->FirstChildElement("layer");
    while (layer) {
        Layer *lyer = new Layer();
        // incrementa o número de tilesets
        nLayers++;

        lyer->name = strdup(layer->Attribute("name"));
        lyer->opacity = strtoint(layer->Attribute("opacity"));
        lyer->width = strtoint(layer->Attribute("width"));
        lyer->height = strtoint(layer->Attribute("height"));
        const char *visible = layer->Attribute("visible");
        if (visible)
            lyer->visible = strtoint(visible);
        else
            lyer->visible = 1;
        lyer->nid = nLayers;

        lyer->props.load(layer);
#if LOG_INFO
        LogEnter();
        lyer->logInfo();
#endif
        loadLayerData(lyer, layer);
#if LOG_INFO
        LogBack();
#endif
        listLayers.push_back(lyer);


        layer = layer->NextSiblingElement("layer");
    }


    Log("Encontrados %d layers, %d\n", nLayers, listLayers.size());
}

void LevelMap::loadLayerData(Layer *layer, XMLElement *li)
{
    Log("Carregando mapa de tiles do layer...\n");
    LogEnter();

    // encontramos a seção data dentro de <layer></layer>
    XMLElement *data = li->FirstChildElement("data");

    // Aloca nossa matriz de tiles
    layer->nTileArray = new unsigned int[layer->width * layer->height];

    if (data) {
        // verificamos a codificação do layer
        // NOTA: só é suportado base64
        char *encoding = strdup(data->Attribute("encoding"));
        // verificamos a compressão utilizada
        // NOTA: só é suportado zlib
        char *compression = strdup(data->Attribute("compression"));
#if LOG_INFO
        Log("Encoding: %s\n", encoding);
        Log("Compression: %s\n", compression);
#endif
        if (!strcmp(encoding, "base64")) {
            if (!strcmp(compression, "zlib"))
                loadBase64zlib(layer, data);
        }
    }
    LogBack();
}

void LevelMap::loadBase64zlib(Layer *layer, XMLElement *data)
{
//    Log("Decodificando a base64...\n");

    layer->nTileArray = new unsigned int [mapWidth * mapHeight];

    int decode_size = 0;
    char *decode_b64 = Base64::Decode((char*)data->GetText(), &decode_size);

    int zerr = 0;
    unsigned long dstlen = mapWidth * mapHeight * sizeof(unsigned int);
    zerr = uncompress((Bytef*) layer->nTileArray, &dstlen,
                      (Bytef*) decode_b64, decode_size);
#if LOG_INFO
    if (zerr != Z_OK)
        Log("Zlib decompress OK\n");
    else
        Log("Zlib decompress Err %d\n", zerr);
#endif
    SAFE_DELETE(decode_b64);

// Dumpa a tabela de tiles do layer
#if 0
    Log("TileTable %s\n", layer->name);
    LogSaveState();
    for (int y = 0; y < mapHeight; y++)
        for (int x = 0; x < mapWidth; x++)
            Log("%d,", (unsigned)layer->nTileArray[y*mapWidth+x]);
    Log("\n");
    LogLoadState();
#endif
}


void LevelMap::loadColObjectGroups(XMLElement *map)
{
    Log("Carregando os objetos de colisão...\n");
    XMLElement *objgrp = map->FirstChildElement("objectgroup");

    while (objgrp) {
        ColObjectGroup *group = new ColObjectGroup();
        group->SetName(strdup(objgrp->Attribute("name")));

        loadColObjetcs(objgrp, group);
        listColGroups.push_back(group);

        objgrp = objgrp->NextSiblingElement("objectgroup");
    }
}

void LevelMap::loadColObjetcs(XMLElement *eg, ColObjectGroup *group)
{
    Log("Carregando objetos do grupo...\n");
    XMLElement *eobj = eg->FirstChildElement("object");
    int nObjects = 0;
    while (eobj)
    {
        ColObject *obj = new ColObject();
        int x, y, w, h;

        x = strtoint(eobj->Attribute("x"));
        y = strtoint(eobj->Attribute("y"));
        w = strtoint(eobj->Attribute("width"));
        h = strtoint(eobj->Attribute("height"));

        obj->SetColor(255,0,255);
        obj->SetRect(x, y, w, h);
        group->AddObj(obj);
        nObjects++;

//        Log("Obj %d = %d,%d,%d,%d\n", nObjects, x, y, w, h);
        eobj = eobj->NextSiblingElement("object");
    }
    Log("Encontrado %d objetos.\n", nObjects);
}


void LevelMap::DrawColObjectGroup()
{
    for (list<ColObjectGroup*>::iterator i = listColGroups.begin(); i != listColGroups.end(); i++)
        (*i)->Draw(renderX, renderY);
}

int LevelMap::CheckCollision(ColObject *obj)
{
    for (list<ColObjectGroup*>::iterator i = listColGroups.begin(); i != listColGroups.end(); i++) {
        ColObjectGroup *group = *i;
        if (*group == *obj)
            return 1;
    }
    return 0;
}


void LevelMap::render(int x, int y)
{
    const int xStartTile = x / tileWidth;
    const int yStartTile = y / tileHeight;
    renderXscroll = x % tileWidth;
    renderYscroll = y % tileHeight;

    // se a imagem estiver na mesma posição, não fazemos nada
    // só atualizamos as informações de rolagem, que já foram redefinidas
    if (renderX == xStartTile && renderY == yStartTile) {
        return;
    } else {
        renderX = xStartTile;
        renderY = yStartTile;
    }

    // limpa o buffer
    //SDL_FillRect(renderBuffer, NULL, g_Video->Color(60, 100, 100));

    int xScroll = 0;
    int yScroll = 0;

    // quantos pixels precisaremos deslizar para termos a imagem na posição x
    if (x % tileWidth)
        xScroll = x % tileWidth;
    // quantas linhas precisaremos pular para termos a imagem na posição y
    if (y % tileHeight)
        yScroll = y % tileHeight;

    // calcula o número máximo de tiles a renderizar em (x,y)
    const int yTilesMax = renderBuffer->h / tileHeight + 2;
    const int xTilesMax = renderBuffer->w / tileWidth + 2;



    // Desenhamos todos os layers
    //foreach (Layer *layer, listLayers) {
    for (list<Layer*>::iterator i=listLayers.begin(); i != listLayers.end(); i++) {
            Layer *layer = *i;

        if (!layer->visible)
            continue;

        int xPos = 0;
        int yPos = 0;

        for (int yTile = yStartTile; yTile < (yStartTile + yTilesMax); yTile++) {
            for (int xTile = xStartTile; xTile < (xStartTile + xTilesMax); xTile++) {
                int pos = xTile + (yTile * mapWidth);
                    // verifica se o tile está dentro do layer
                    if (pos < (layer->width * layer->height)) {

                        // pegamos o indíce (GID) do tile no nosso array
                        int index = layer->nTileArray[pos];
                        drawTile(renderBuffer, index, xPos, yPos);
                    }
                // incrementa a posição de desenho X no comprimento do tile
                xPos += tileWidth;
            }
            xPos = 0;
            // incrementa a posição de desenho Y na altura do tile
            yPos += tileHeight;
        }
    }

    renderPos.x = xScroll;
    renderPos.y = yScroll;
    renderPos.w = g_Video->GetW();
    renderPos.h = g_Video->GetH();
}


void LevelMap::blit()
{
    renderPos.x = renderXscroll;
    renderPos.y = renderYscroll;
    g_Video->Blit(renderBuffer, &renderPos, NULL);
}


void LevelMap::drawTile(SDL_Surface *dst, int id, int sx, int sy)
{
    // id 0 é transparente
    if (!id)
        return;

    for (list<TileSet*>::iterator i=listTileSet.begin(); i != listTileSet.end(); i++) {
            TileSet *tileset = *i;
        if ((id >= tileset->firstGID) && (id <= (tileset->firstGID + tileset->nTiles)))
            tileset->image->DrawTile(dst, id - tileset->firstGID, sx, sy);
    }
}
