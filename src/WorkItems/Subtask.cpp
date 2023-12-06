#include "Task.h"
#include "Subtask.h"

Subtask::Subtask(WorkItem *parent) : WorkItem(parent)
{
}

Subtask::~Subtask() {}

void Subtask::setDone(bool d)
{
	done = d;
}

bool Subtask::isDone()
{
	return done;
}
/*
 * Setta la priorità della task.
 * l = bassa
 * h = alta
 */
void Subtask::setPriority(std::string p)
{
	priority = p;
}

/*
 * Ritorna la priorità della task.
 * 0 = bassa
 * 1 = alta
 */
std::string Subtask::getPriority()
{
	return priority;
}

std::string Subtask::getFormattedInfo()
{

    return "";
}

// non viene usato in subtask
Table *Subtask::getTable()
{
    return nullptr;
}