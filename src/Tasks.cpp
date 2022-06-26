#include "Tasks.h"

Code::Code(int id, std::string o)
{
	this->ID = id;
	this->obj = o;
	this->test = "";
	this->type = CODE;
	this->isCompleted = false;
	this->isActive = false;
}
