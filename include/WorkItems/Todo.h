// Todo.h
#pragma once
#include <string>
#include "WorkItem.h"

class Todo : public WorkItem
{
public:
	Todo();
	~Todo() override = default;

	void setDone(bool d) override;
	bool isDone() override;

	std::string getFormattedInfo() override;

private:
	bool done = false;
};
