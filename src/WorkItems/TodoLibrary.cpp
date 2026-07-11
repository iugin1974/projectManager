#include "TodoLibrary.h"
#include "Todo.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include "Task.h"

TodoLibrary::TodoLibrary() {
}

void TodoLibrary::addNewTodo(Todo& t) {
    listTodo.push_back(t);
    // TODO -> sort
    // sort(listTodo.begin(), listTodo.end(), WorkItem::sortVector);
    stateChanged();
}

void TodoLibrary::removeTodo(Todo *t)
{
	for (auto it = listTodo.begin(); it != listTodo.end(); ++it)
	{
		if (&(*it) == t)
		{
			listTodo.erase(it);
			stateChanged();
			return;
		}
	}
}

Todo& TodoLibrary::getTodo(unsigned int p) {
    return listTodo.at(p);
}

void TodoLibrary::addObserver(Observer* o) {
    observer = o;
}

void TodoLibrary::removeObserver(Observer *o)
{
	if (observer == o)
		observer = nullptr;
}


const std::vector<Todo>& TodoLibrary::getTodoList() {
return listTodo;
}

void TodoLibrary::stateChanged() {
    if (observer != NULL) {
        observer->updateView(this);
        setChanged(true);
    }
}

void TodoLibrary::setChanged(bool changed) {
    this->changed = changed;
}

bool TodoLibrary::isChanged() {
    return changed;
}

unsigned int TodoLibrary::size() {
    return listTodo.size();
}

void TodoLibrary::removeTodo(unsigned int i) {
    listTodo.erase(listTodo.begin()+i);
  // TODO implementa sort
    //  sort(listTodo.begin(), listTodo.end(), WorkItem::sortVector);
    stateChanged();
}

void TodoLibrary::toString() const {
    for (Todo t : listTodo) {
        std::cout << "Todo:" << t.getText() << "\n";
    }
}
