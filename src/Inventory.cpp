#include "Inventory.h"

Inventory::Inventory() {

}

void Inventory::addNote(Entity* ent, Texture* img, int id) {
	Note* n = new Note(ent, img, id);
	this->Notes.push_back(n);
}
