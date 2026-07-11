// TodoLibrary.h
#ifndef TODOLIBRARY_H
#define TODOLIBRARY_H
#include <vector>
#include "Observable.h"
#include "Todo.h"
#include "Observer.h"

class Table;

class TodoLibrary : public Observable
{
private:
	std::vector<Todo> listTodo;
	Observer *observer = nullptr;
	bool changed = false;

public:
	TodoLibrary();
	void addObserver(Observer *o) override;
	void removeObserver(Observer *o) override;
	void stateChanged() override;
	void addNewTodo(Todo& t);
	void removeTodo(Todo* t);
	Todo &getTodo(unsigned int i);
	const std::vector<Todo>& getTodoList();
	void removeTodo(unsigned int i);
	unsigned int size();

	void setChanged(bool changed);
	bool isChanged();

	void toString() const;
};
#endif
