#include "entity.h"



class Inventory
{
public:

	std::vector<Note*> Notes;

	Inventory();
	void addNote(Entity* n);
};

