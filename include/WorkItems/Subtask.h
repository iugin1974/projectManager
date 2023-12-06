#ifndef SUBTASK_H
#define SUBTASK_H

#include "Task.h"
#include "WorkItem.h"
class Subtask : public Displayable, public WorkItem
{
private:
  std::string priority = "l";
  bool done = false;

public:
  Subtask(WorkItem *parent);
  ~Subtask() override;
  void setPriority(std::string priority);
  bool isDone();
  void setDone(bool done);
  std::string getPriority();
  std::string getFormattedInfo() override;
  Table *getTable() override;
};
#endif