#include "Inventory.h"

Inventory::Inventory() {

}

void Inventory::addEntity(Entity* ent) {
	this->savedEntities.push_back(ent);
	this->numSavedEntities++;
}
