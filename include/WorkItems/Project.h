#ifndef PROJECT_H
#define PROJECT_H
#include <string>
#include <vector>
#include <ctime>
#include "Displayable.h"
#include "WorkItem.h"
class Task;
class Table;
class ProjectLibrary;
class Files;

class Project : public Displayable, public WorkItem
{
private:
	ProjectLibrary *pl;
	Files* files = NULL;
	std::vector<std::string> listFiles;
	std::vector<Task *> taskList;
	unsigned int percent = 0;
	void calculatePercent();
	static bool sortVector(Task* t1, Task* t2);

public:
	Project(WorkItem* parent);
	~Project();
	void setProjectLibrary(ProjectLibrary *pl);
	void addTask(Task *t);
	Task *getNewTask();
	Task *getTask(unsigned int t);
	std::vector<Task *> getTaskList();
	void addFile(std::string path);
	Files* getFileList();
	bool hasFiles();
	bool isDone();
	unsigned int getPercent();	
	unsigned int size();
	void update();
	void removeTask(unsigned int i);
	Table* getTable() override;
	};	

#endif
