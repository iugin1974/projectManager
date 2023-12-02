#ifndef TASK_H
#define TASK_H
#include <string>
#include <vector>
#include "Displayable.h"
#include "WorkItem.h"
class Subtask;
class Project;

class Task : public Displayable, public WorkItem
{

private:
  std::vector<Subtask*> subtaskList;
  Project *project;
  void notifyProject();

protected:
  bool done = false;
  std::string priority = "l";


public:
  Task(WorkItem *parent);
  Task();
  ~Task();
  WorkItem getParent();
  Project *getProject();
  Subtask *getNewSubtask();
  Subtask *getSubtask(unsigned int i);
  void addSubtask(Subtask *t);
  void removeSubtask(unsigned int i);
  std::vector<Subtask *> getSubtaskList();
  int size();
  bool isDone();
  void setDone(bool done);
  void setPriority(std::string priority);
  std::string getPriority();
	Table* getTable() override;
};

#endif
