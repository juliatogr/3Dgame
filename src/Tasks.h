#pragma once
#include <string>

#ifndef TASK_H
#define TASK_H

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
	Code(int id, std::string  o);
};
#endif