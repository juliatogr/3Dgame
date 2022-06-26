#include "Inventory.h"

Inventory::Inventory() {

}

void Inventory::addNote(Entity* ent) {
	Note* n = new Note(ent);
	this->Notes.push_back(n);
}
