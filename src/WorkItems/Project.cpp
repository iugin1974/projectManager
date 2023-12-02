#include <vector>
#include "Task.h"
#include "Project.h"
#include <ctime>
#include <string>
#include "ProjectLibrary.h"
#include <algorithm>
#include <random>
#include "Files.h"
#include "TableTask.h"

Project::Project(WorkItem* parent) : WorkItem(parent) {
files = new Files();
}

Project::~Project() {
	for (unsigned int i = 0; i<taskList.size(); i++) {
		delete taskList.at(i);
	}
	delete files;
}

void Project::setProjectLibrary(ProjectLibrary* _pl) {
	pl = _pl;
}

void Project::addFile(std::string f) {
  listFiles.push_back(f);
}

Files* Project::getFileList() {
return files;
}

bool Project::hasFiles() {
return listFiles.size() > 0;
}

/*
 * Crea una Task vuota e ritorna un puntatore
 * alla Task stessa */
void Project::addTask(Task* t) {
	taskList.push_back(t);
	update();
}

Task* Project::getNewTask() {
	Task* t = new Task(this);
	return t;
}

Task* Project::getTask(unsigned int i) {
	return taskList.at(i);
}

std::vector<Task*> Project::getTaskList() {
	return taskList;
}

bool Project::isDone() {
	/* Se il progetto non ha Task, viene considerato come non finito */
	if (taskList.size() == 0) return false;

	for (unsigned int i = 0; i < taskList.size(); i++) {
		if (!taskList.at(i)->isDone()) return false;
	}
	return true;
}

void Project::calculatePercent() {
	int taskNumber = taskList.size();
	int taskDone = 0;
	for (unsigned int i = 0; i<taskList.size(); i++) {
		if (taskList.at(i)->isDone()) taskDone++;
	}
	percent = taskDone * 100 / taskNumber; 
}

unsigned int Project::getPercent() {
	return percent;
}

unsigned int Project::size() {
	return taskList.size();
}

void Project::removeTask(unsigned int i) {
	Task* t = taskList.at(i);
	delete t;
	taskList.erase(taskList.begin()+i);
	update();
}

void Project::update() {
  sort(taskList.begin(), taskList.end(), WorkItem::sortVector);
	if (taskList.size() > 0)
		calculatePercent();
	pl->stateChanged();
}

Table* Project::getTable() {
	return new TableTask();
}