// Todo.cpp
#include "Todo.h"

Todo::Todo() : WorkItem()
{
}

void Todo::setDone(bool d)
{
    done = d;
}

bool Todo::isDone()
{
    return done;
}

std::string Todo::getFormattedInfo()
{
    return std::string(isDone() ? "d" : "-") +
           std::string(hasComment() ? "c" : "-");
}
