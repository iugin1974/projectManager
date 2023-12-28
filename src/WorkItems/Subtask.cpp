#include "Task.h"
#include "Subtask.h"

Subtask::Subtask(Task* parent) : Task(parent) {
}

Subtask::~Subtask(){};

std::string Subtask::getFormattedInfo() {
	return std::string((priority == Task::HIGH) ? "h" : "-") +
		   std::string((isDone() == true) ? "d" : "-") +
		   std::string((hasComment()) ? "c" : "-");
}

void Subtask::setDone(bool d) {
    done = d;
		Task* t = static_cast<Task*>(parent);
        t->checkAndUpdateDoneStatus();  
}

bool Subtask::isDone() {
    return done;
}