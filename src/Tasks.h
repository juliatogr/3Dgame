#pragma once
#include <string>
#include "entity.h"


enum TASK_ID {
	CODE,
	READ,
	DEVELOP
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

	Code(int id, std::string o);
	Code(int id, std::string o, Door* d1, Door* d2);
	Code(int id, std::string  o, Door* d);
	void OpenDoors();
};

class ReadNote :public Task {
public:
	int nD;
	Door* door1;
	Door* door2;
	ReadNote(int id);
	ReadNote(int id, Door* d1);
	ReadNote(int id, Door* d1, Door* d2);
	void OpenDoors();
};

class Develop : public Task {
public:
	

	Develop(int id);
	
};