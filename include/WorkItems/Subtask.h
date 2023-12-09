#ifndef SUBTASK_H
#define SUBTASK_H
#include <string>

#include "Task.h"
class Subtask : public Task {

public:
Subtask(WorkItem* parent);
~Subtask();
std::string getFormattedInfo() override;
void setDone(bool done) override;
};

#endif