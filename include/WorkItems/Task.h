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
  std::vector<Subtask *> subtaskList;
  Project *project;
  void notifyProject();

protected:
  bool done = false;
  int priority = LOW;

public:
  static const int LOW = 0;
  static const int HIGH = 1;
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
  unsigned int size();
  bool isDone();
  virtual void setDone(bool done);
  void checkAndUpdateDoneStatus(const Subtask* callerSubtask);
  void setPriority(int priority);
  int getPriority();
  Table *getTable() override;
  std::string getFormattedInfo() override;
};

#endif
