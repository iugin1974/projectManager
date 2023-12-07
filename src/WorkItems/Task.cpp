#include "Task.h"
#include "Project.h"
#include <string>
#include "Subtask.h"
#include "TableSubtask.h"

Task::Task(WorkItem *parent) : WorkItem(parent)
{
}

Task::~Task()
{
	for (unsigned int i = 0; i < subtaskList.size(); i++)
	{
		delete subtaskList.at(i);
	}
}

Subtask *Task::getNewSubtask()
{
	Subtask *s = new Subtask(this);
	return s;
}

Subtask *Task::getSubtask(unsigned int i)
{
	return subtaskList.at(i);
}

std::vector<Subtask *> Task::getSubtaskList()
{
	return subtaskList;
}

int Task::size()
{
	return subtaskList.size();
}
void Task::addSubtask(Subtask *s)
{
	subtaskList.push_back(s);
}

void Task::removeSubtask(unsigned int i)
{
	Subtask *s = subtaskList.at(i);
	subtaskList.erase(subtaskList.begin() + i);
	delete s;
}

Project *Task::getProject()
{
	return project;
}

void Task::setDone(bool d)
{
	done = d;
}

bool Task::isDone()
{
	return done;
}

/*
 * Setta la priorità della task.
 * l = bassa
 * h = alta
 */
void Task::setPriority(int p)
{
	priority = p;
}

/*
 * Ritorna la priorità della task.
 * 0 = bassa
 * 1 = alta
 */
int Task::getPriority()
{
	return priority;
}

void Task::notifyProject()
{
	project->update();
}

Table *Task::getTable()
{
	return new TableSubtask();
}

std::string Task::getFormattedInfo()
{
	return std::string((priority == Task::HIGH) ? "h" : "-") +
		   std::string((isDone() == true) ? "d" : "-") +
		   std::string((hasComment()) ? "c" : "-");
}