#ifndef INVENTORY_H
#define INVENTORY_H
#include "entity.h"

class Inventory
{
public:

	std::vector<Note*> Notes;

	Inventory();
	void addNote(Entity* n, Texture* img, int id);
};

#endif