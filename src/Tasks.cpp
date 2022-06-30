#include "Tasks.h"

Code::Code(int id, std::string o, Door* d1, Door* d2)
{
	this->ID = id;
	this->obj = o;
	this->test = "";
	this->type = CODE;
	this->isCompleted = false;
	this->isActive = false;
	this->door1 = d1;
	this->door2 = d2;
	this->nD = 2;
}

Code::Code(int id, std::string o, Door* d)
{
	this->ID = id;
	this->obj = o;
	this->test = "";
	this->type = CODE;
	this->isCompleted = false;
	this->isActive = false;
	this->door1 = d;
	this->nD = 1;
}

Code::Code(int id, std::string o)
{
	this->ID = id;
	this->obj = o;
	this->test = "";
	this->type = CODE;
	this->isCompleted = false;
	this->isActive = false;
	this->nD = 0;
}

void Code::OpenDoors()
{
	if (this->nD == 2) {
		this->door1->isOpening = true;
		this->door2->isOpening = true;
		std::cout << "doo1 & door 2" << std::endl;
	}
	if (this->nD == 1) {
		this->door1->isOpening = true;
		std::cout << "doo1" << std::endl;

	}
}

ReadNote::ReadNote(int id)
{
	this->ID = id;
	this->type = READ;
	this->isCompleted = false;
	this->isActive = false;
	this->nD = 0;
}

ReadNote::ReadNote(int id, Door* d1)
{
	this->ID = id;
	this->type = READ;
	this->isCompleted = false;
	this->isActive = false;
	this->door1 = d1;
	this->nD = 1;
}

ReadNote::ReadNote(int id, Door* d1, Door* d2)
{
	this->ID = id;
	this->type = READ;
	this->isCompleted = false;
	this->isActive = false;
	this->door1 = d1;
	this->door2 = d2;
	this->nD = 2;
}

void ReadNote::OpenDoors()
{
	if (this->nD == 2) {
		this->door1->isOpening = true;
		this->door2->isOpening = true;
		std::cout << "doo1 & door 2" << std::endl;
	}
	if (this->nD == 1) {
		this->door1->isOpening = true;
		std::cout << "doo1" << std::endl;

	}
}


Develop::Develop(int id)
{
	this->ID = id;
	this->type = DEVELOP;
	this->isCompleted = false;
	this->isActive = false;
}


