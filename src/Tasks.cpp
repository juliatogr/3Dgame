#include "Tasks.h"

Code::Code(int id, int o)
{
	this->ID = id;
	this->obj = o;
	this->type = CODE;
}

bool Code::InputCodeIsCorrect(int inp)
{
	if (inp == this->obj) {
		return true;
	}
	return false;
}
