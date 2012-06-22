#include "entity.h"
#include "system.h"
#include "inputsystem.h"
#include "videosystem.h"
#include <list>
#include <lua.hpp>

using namespace std;

const char *Entity::lua_global_name = "__ENTITY_OBJECT__";

Entity::Entity() : XYObject() {
    faced = false;
    grounded = false;
    headed = false;
    isloaded = false;
    collided = false;
    anim = new AnimMap();
    sprite = new SpriteMap();
    xpos = 100;
    ypos = 100;
}

Entity::~Entity() {
    delete anim;
    delete sprite;
}

int Entity::open(const char *filename) {
    L = luaL_newstate();
    luaopen_math(L);
    // salvamos nossa entidade na pilha da vm
    lua_pushlightuserdata(L, this);
    lua_setglobal(L, lua_global_name);

    // registra nossas funções
    LUA_REGISTER(L, Entity, ChangeAnim);
    LUA_REGISTER(L, Entity, GetAnim);
    LUA_REGISTER(L, Entity, Input);
    // posição e velocidade
    LUA_REGISTER(L, Entity, SetX);
    LUA_REGISTER(L, Entity, SetY);
    LUA_REGISTER(L, Entity, AddX);
    LUA_REGISTER(L, Entity, AddY);
    LUA_REGISTER(L, Entity, GetX);
    LUA_REGISTER(L, Entity, GetY);
    LUA_REGISTER(L, Entity, SetVelX);
    LUA_REGISTER(L, Entity, SetVelY);
    LUA_REGISTER(L, Entity, AddVelX);
    LUA_REGISTER(L, Entity, AddVelY);
    LUA_REGISTER(L, Entity, GetVelX);
    LUA_REGISTER(L, Entity, GetVelY);
    // estados de colisão
    LUA_REGISTER(L, Entity, IsGrounded);
    LUA_REGISTER(L, Entity, IsFaced);
    LUA_REGISTER(L, Entity, IsHeaded);
    LUA_REGISTER(L, Entity, IsHited);
    // ...
    LUA_REGISTER(L, Entity, SetFacing);
    LUA_REGISTER(L, Entity, IsFacing);

    Log("Carregando o script...\n");
    // carrega o script
    if (luaL_dofile(L, filename) == LUA_OK) {
        lua_pcall(L, 0, 0, 0);
        // tenta carregar o arquivo de sprites
        lua_getglobal(L, "SPRITE_FILE");
        const char *str = lua_tostring(L, -1);
        if (str)
            sprite->open(str);
        // tenta carregar o arquivo de animações
        lua_getglobal(L, "ANIM_FILE");
        str = lua_tostring(L, -1);
        if (str)
            anim->open(str);
        // tenta carregar o nome
        lua_getglobal(L, "NAME");
        name = lua_tostring(L, -1);
        isloaded = true;
    } else {
        Log("Erro ao carregar o arquivo %d\n::%s\n", filename, lua_tostring(L, -1));
        isloaded = false;
        return 0;
    }
    return 1;
}


void Entity::close() {
    lua_close(L);
}

const char *Entity::myname() {
    return name;
}

void Entity::update() {
    if (isloaded) {
        lua_getglobal(L, "update");
        if (lua_pcall(L, 0, 0, 0) != LUA_OK)
            Log("Erro: %s\n", lua_tostring(L, -1));
        anim->update();
        sprite->parse(anim->group(), anim->index());
    }
}

void Entity::drawAt(int dx, int dy)
{
    if (isloaded) {
        sprite->parse(anim->group(), anim->index());
        xdraw = dx + anim->x();
        ydraw = dy + anim->y();

        sprite->drawAt(xdraw, ydraw, false, facing ? true : false);
        drawBox(anim->clsn2());
    }
}

inline void transbox(ColObject *obj, int x, int y)
{
    int x1 = x + obj->GetX();
    int y1 = y + obj->GetY();
    int x2 = x + obj->GetW() - x1;
    int y2 = y + obj->GetH() - y1;
    obj->SetRect(x1, y1, x2, y2);
}

void Entity::drawBox(ColObjectGroup *col)
{
    if (!col)
        return;

    for (std::list<ColObject*>::iterator i = col->listObjs.begin();
         i != col->listObjs.end(); i++) {
        ColObject *obj = *i;

        int x1, y1, x2, y2;
        x1 = xdraw + obj->GetX();
        y1 = ydraw + obj->GetY();
        x2 = xdraw + obj->GetW() - x1;
        y2 = ydraw + obj->GetH() - y1;

        SDL_Rect r;
        r.x = x1;
        r.y = y1;
        r.w = x2;
        r.h = y2;
        g_Video->DrawRect(&r, 0xFFC0);

    }
}


//
bool Entity::checkBoxes(ColObjectGroup *a, ColObjectGroup *b, int bx, int by)
{
    ColObject src;
    if (!a || !b)
        return false;
    if (a->listObjs.empty() || b->listObjs.empty())
        return false;
    for (list<ColObject*>::iterator i = a->listObjs.begin(); i != a->listObjs.end(); i++) {
        ColObject *p_a = *i;
        ColObject o_a(p_a);
        transbox(&o_a, x(), y());
        for (list<ColObject*>::iterator k = b->listObjs.begin(); k != b->listObjs.end(); k++) {
            ColObject *p_b = *i;
            ColObject o_b(p_b);
            transbox(&o_b, bx, by);
            if (o_a == o_b)
                return true;
        }
    }
    return false;
}

bool Entity::checkCol(Entity *right) {
   // if (checkBoxes(anim->clsn2(), right->anim->clsn2(), right->x(), right->y()))
        return true;
    return false;
}


int Entity::width() const {
    if (sprite)
        return sprite->width();
    return 0;
}

int Entity::height() const {
    if (sprite)
        return sprite->height();
    return 0;
}

void Entity::setFaced(bool v) {
    faced = v;
}
void Entity::setGrounded(bool v) {
    grounded = v;
}
void Entity::setHeaded(bool v) {
    headed = v;
}
void Entity::setCollided(bool v) {
    collided = v;
}

bool Entity::isFacing() const {
    return facing;
}

//=============================================================================
// LUA CALLBACKS
//=============================================================================

LUA_IMPLEMENT(Entity, ChangeAnim)
{
    LUA_OBJECT(Entity);

    int no = lua_tonumber(L, -2);
    //Log("ChangeAnim para %d\n", no);
    if (_this->anim)
        _this->anim->set(no);
    return 0;
}

LUA_IMPLEMENT(Entity, GetAnim)
{
    LUA_OBJECT(Entity);
    if (_this->anim) {
        lua_pushnumber(L, _this->anim->number());
    } else
        lua_pushnumber(L, -1);
    return 1;
}

// implementando os controles
LUA_IMPLEMENT(Entity, Input)
{
    LUA_OBJECT(Entity);

    const char *name = lua_tostring(L, -2);
    int ret = 0;

    if (!name || !_this)
        return 0;

    if (!stricmp(name, "foward")) {
        if (_this->facing)
            ret = g_Input->getstate(IBS_LEFT).state == KS_HOLDING;
        else
            ret = g_Input->getstate(IBS_RIGHT).state == KS_HOLDING;
    } else
    if (!stricmp(name, "^foward")) {
        if (_this->facing)
            ret = g_Input->getstate(IBS_LEFT).state == KS_RELEASE;
        else
            ret = g_Input->getstate(IBS_RIGHT).state == KS_RELEASE;
    } else
    if (!stricmp(name, "back")) {
        if (_this->facing)
            ret = g_Input->getstate(IBS_RIGHT).state == KS_HOLDING;
        else
            ret = g_Input->getstate(IBS_LEFT).state == KS_HOLDING;
    } else
    if (!stricmp(name, "^back")) {
        if (_this->facing)
            ret = g_Input->getstate(IBS_RIGHT).state == KS_RELEASE;
        else
            ret = g_Input->getstate(IBS_LEFT).state == KS_RELEASE;
    } else
    if (!stricmp(name, "up"))
        ret = g_Input->getstate(IBS_UP).state == KS_HOLDING;
    else if (!stricmp(name, "down"))
        ret = g_Input->getstate(IBS_DOWN).state == KS_HOLDING;
    else if (!stricmp(name, "a"))
        ret = g_Input->getstate(IBS_BUTTON_A).state == KS_HOLDING;
    else if (!stricmp(name, "b"))
        ret = g_Input->getstate(IBS_BUTTON_B).state == KS_HOLDING;

    lua_pushinteger(L, ret);
    return 1;
}


LUA_IMPLEMENT(Entity, SetX)
{
    LUA_OBJECT(Entity);

    float pos = lua_tonumber(L, -2);
    _this->xpos = pos;
    return 0;
}


LUA_IMPLEMENT(Entity, SetY)
{
    LUA_OBJECT(Entity);

    float pos = lua_tonumber(L, -2);
    _this->ypos = pos;
    return 0;
}

LUA_IMPLEMENT(Entity, AddX)
{
    LUA_OBJECT(Entity);

    float pos = lua_tonumber(L, -2);
    _this->xpos += pos;
    return 0;
}


LUA_IMPLEMENT(Entity, AddY)
{
    LUA_OBJECT(Entity);

    float pos = lua_tonumber(L, -2);
    _this->ypos += pos;
    return 0;
}

LUA_IMPLEMENT(Entity, GetX)
{
    LUA_OBJECT(Entity);
    lua_pushnumber(L, _this->xpos);
    return 1;
}



LUA_IMPLEMENT(Entity, GetY)
{
    LUA_OBJECT(Entity);
    lua_pushnumber(L, _this->ypos);
    return 1;
}

LUA_IMPLEMENT(Entity, SetVelX)
{
    LUA_OBJECT(Entity);

    float vel = lua_tonumber(L, -2);
    _this->xvel = vel;
    return 0;
}

LUA_IMPLEMENT(Entity, SetVelY)
{
    LUA_OBJECT(Entity);

    float vel = lua_tonumber(L, -2);
    _this->yvel = vel;
    return 0;
}

LUA_IMPLEMENT(Entity, AddVelX)
{
    LUA_OBJECT(Entity);

    float vel = lua_tonumber(L, -2);
    _this->xvel += vel;
    return 0;
}

LUA_IMPLEMENT(Entity, AddVelY)
{
    LUA_OBJECT(Entity);

    float vel = lua_tonumber(L, -2);
    _this->yvel += vel;
    return 0;
}


LUA_IMPLEMENT(Entity, GetVelX)
{
    LUA_OBJECT(Entity);
    lua_pushnumber(L, _this->xvel);
    return 1;
}

LUA_IMPLEMENT(Entity, GetVelY)
{
    LUA_OBJECT(Entity);
    lua_pushnumber(L, _this->yvel);
    return 1;
}

LUA_IMPLEMENT(Entity, IsGrounded)
{
    LUA_OBJECT(Entity);
    lua_pushnumber(L, _this->grounded ? 1 : 0);
    return 1;
}

LUA_IMPLEMENT(Entity, IsFaced)
{
    LUA_OBJECT(Entity);
    lua_pushnumber(L, _this->faced ? 1 : 0);
    return 1;
}

LUA_IMPLEMENT(Entity, IsHeaded)
{
    LUA_OBJECT(Entity);
    lua_pushnumber(L, _this->headed ? 1 : 0);
    return 1;
}

LUA_IMPLEMENT(Entity, IsHited)
{
    LUA_OBJECT(Entity);
    lua_pushnumber(L, _this->collided ? 1 : 0);
    return 1;
}

LUA_IMPLEMENT(Entity, IsFacing)
{
    LUA_OBJECT(Entity);
    lua_pushnumber(L, _this->facing ? 1 : 0);
    return 1;
}

LUA_IMPLEMENT(Entity, SetFacing)
{
    LUA_OBJECT(Entity);

    int cod = lua_tonumber(L, -2);
    _this->facing = cod ? 1 : 0;
    return 0;
}
