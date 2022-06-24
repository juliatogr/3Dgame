#include "entity.h"
class Inventory
{
public:
	int numSavedEntities = 0;
	std::vector<Entity*> savedEntities;

	Inventory();
	void addEntity(Entity* ent);
};

