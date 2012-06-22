#ifndef PROPERTY_H
#define PROPERTY_H
#include <list>
#include "TinyXML/tinyxml2.h"
using namespace tinyxml2;

class Property
{
    public:
        /** Default constructor */
        Property();
        /** Default destructor */
        ~Property();

        int load(XMLElement *root);
    private:
        struct Entry {
            const char *name;
            const char *value;
        };
        std::list<Entry*> entrys;
};

#endif // PROPERTY_H
