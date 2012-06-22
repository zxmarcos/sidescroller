#include "airloader.h"
#include "system.h"

FrameDef::FrameDef(short g, short i, short x, short y, short t) {
	group = g;
	index = i;
	xaxis = x;
	yaxis = y;
	time = t;
}

FrameDef::~FrameDef() {
}

ActionDef::ActionDef(int n) {
	number = n;
}

ActionDef::~ActionDef() {
}

AIRLoader::AIRLoader() {
    action = NULL;
    frame = NULL;
    currClsn1 = NULL;
    currClsn1def = NULL;
    currClsn2 = NULL;
    currClsn2def = NULL;
}

AIRLoader::~AIRLoader() {
}

void AIRLoader::createAction(int n) {
 //   Log("Novo action %d\n", n);
    nFrames = 0;
    currClsn1def = NULL;
    currClsn2def = NULL;
    nLoop = -1;
	action = new ActionDef(n);
	mapActions[n] = action;
}

void AIRLoader::createClsn(int t, int idx, int x1, int y1, int x2, int y2) {
    ColObject *obj = new ColObject(x1, y1, x2, y2);
    if (t == 1) {
        obj->SetColor(0,0,255);
        if (isDefaultClsn)
            currClsn1def->AddObj(obj);
        else
            currClsn1->AddObj(obj);
    }
    else if (t == 2) {
        obj->SetColor(255,0,0);
        if (isDefaultClsn)
            currClsn2def->AddObj(obj);
        else
            currClsn2->AddObj(obj);
    }
}

void AIRLoader::setLoopstart() {
    nLoop = nFrames;
    if (action)
        action->loop = nLoop;
}

void AIRLoader::createClsnList(int t, bool def, int sz) {
    ColObjectGroup *obj = new ColObjectGroup();

    isDefaultClsn = def ? 1 : 0;
    if (isDefaultClsn) {
        if (t == 1)
            currClsn1def = obj;
        else if (t == 2)
            currClsn2def = obj;
    }
    else {
        if (t == 1)
            currClsn1 = obj;
        else if (t == 2)
            currClsn2 = obj;
    }
}

void AIRLoader::createFrame(int g, int i, int x, int y, int t) {
    frame = new FrameDef(g, i, x, y, t);
    action->vFrames.push_back(frame);
    frame->clsn1 = currClsn1;
    frame->clsn2 = currClsn2;
    frame->defClsn1 = currClsn1def;
    frame->defClsn2 = currClsn2def;
}

void AIRLoader::open(const char *filename) {
    Log("Carregando animações %s...\n", filename);
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        Log("Não foi possível abrir o arquivo %s\n", filename);
        return;
    }
    g_animParser = this;
    extern FILE *yyin;
    extern int yyparse();
    yyin = fp;
    yyparse();
    fclose(fp);
}

ActionDef *AIRLoader::get(int no) {
    ActionDef *ac = mapActions[no];
    if (ac)
        return ac;
    return NULL;
}

