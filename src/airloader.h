#ifndef AIRLOADER_H
#define AIRLOADER_H
#include "animmap.h"
#include "colobjectgroup.h"
#include <map>
#include <vector>

class FrameDef
{
	friend class AnimMap;
	friend class ActionDef;
	friend class AIRLoader;
	public:
		FrameDef(short g, short i, short x, short y, short t);
		~FrameDef();
	private:
		ColObjectGroup *defClsn1;
		ColObjectGroup *defClsn2;
		ColObjectGroup *clsn1;
		ColObjectGroup *clsn2;
		short group;
		short index;
		short xaxis;
		short yaxis;
		short time;
		bool vflip;
		bool hflip;
};

class ActionDef
{
    friend class AnimMap;
    friend class AIRLoader;
	public:
		ActionDef(int n);
		~ActionDef();
	private:
		int number;
		int loop;
		std::vector<FrameDef*> vFrames;
};

class AIRLoader
{
    public:
        /** Default constructor */
        AIRLoader();
        /** Default destructor */
        ~AIRLoader();

        /**
		 * Carrega um arquivo de animações.
		 * @arg filename Nome do arquivo
		 */
		void open(const char *filename);

		ActionDef *get(int no);

		// Builder
		void createAction(int n);
		void createClsn(int t, int idx, int x1, int y1, int x2, int y2);
		void setLoopstart();
		void createClsnList(int t, bool def, int sz);
		void createFrame(int g, int i, int x, int y, int t);
    private:
        std::map<int,ActionDef*> mapActions;
        FrameDef *frame;
		ActionDef *action;
		ColObjectGroup *currClsn1;
		ColObjectGroup *currClsn2;
		ColObjectGroup *currClsn1def;
		ColObjectGroup *currClsn2def;
		int nFrames;
		int nLoop;
		int isDefaultClsn;
};

extern AIRLoader *g_animParser;

#endif // AIRLOADER_H
