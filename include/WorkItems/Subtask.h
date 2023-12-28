#ifndef SUBTASK_H
#define SUBTASK_H
#include <string>

#include "Task.h"
class Subtask : public Task {

public:
Subtask(Task* parent);
~Subtask();
std::string getFormattedInfo() override;
bool isDone() override;
void setDone(bool done) override;
};

#endif