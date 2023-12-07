#include "Task.h"
#include "Subtask.h"

Subtask::Subtask(WorkItem* parent) : Task(parent) {
}

Subtask::~Subtask(){};

std::string Subtask::getFormattedInfo() {
	return std::string((priority == Task::HIGH) ? "h" : "-") +
		   std::string((isDone() == true) ? "d" : "-") +
		   std::string((hasComment()) ? "c" : "-");
}