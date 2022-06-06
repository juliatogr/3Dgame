enum TASK_ID {
	CODE = 0
}; class Task
{
public:
	int ID;
	TASK_ID type;
};

class Code : public Task {
	int obj;
	Code(int id, int o);
	bool InputCodeIsCorrect(int inp);
};