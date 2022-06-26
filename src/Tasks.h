#pragma once
#include <string>
#include "entity.h"


enum TASK_ID {
	CODE = 0
}; 
class Task
{
public:
	int ID;
	bool isCompleted;
	bool isActive;
	TASK_ID type;
};

class Code : public Task {
public: 
	std::string obj;
	std::string test;
	Door* door1;
	Door* door2;
	int nD;

	Code(int id, std::string o, Door* d1, Door* d2);
	Code(int id, std::string  o, Door* d);
	void OpenDoors();
};
