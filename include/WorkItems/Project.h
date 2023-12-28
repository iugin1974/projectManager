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
	bool done = false;
	std::vector<std::string> listFiles;
	std::vector<Task *> taskList;
	static bool sortVector(Task* t1, Task* t2);

public:
	Project();
	~Project();
	void setProjectLibrary(ProjectLibrary *pl);
	void addTask(Task *t);
	Task *getNewTask(Project* p);
	Task *getTask(unsigned int t);
	std::vector<Task *> getTaskList();
	void addFile(std::string path);
	Files* getFileList();
	bool hasFiles();
	bool isDone() override;
	void setDone(bool done) override;
	unsigned int getPercent();	
	unsigned int size();
	void update();
	void removeTask(unsigned int i);
	Table* getTable() override;
  std::string getFormattedInfo() override;
  void checkAndUpdateDoneStatus();
	};	

#endif
