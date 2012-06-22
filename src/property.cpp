#include "property.h"
#include "system.h"

Property::Property() {
    entrys.clear();
}

Property::~Property() {
}

int Property::load(XMLElement *root) {
    if (!root)
        return 0;
    XMLElement *propts = root->FirstChildElement("properties");
    if (!propts)
        return 0;
    XMLElement *current = propts->FirstChildElement("property");
    while (current) {
        const char *name = strdup(current->Attribute("name"));
        if (name) {
            const char *value = strdup(current->Attribute("value"));
            Entry *entry = new Entry;
            entry->name = name;
            entry->value = value;
            entrys.push_back(entry);
            Log("Propriedade encontrada %s = %s\n", name, value);
        }
        current = current->NextSiblingElement("property");
    }
    return 1;
}
