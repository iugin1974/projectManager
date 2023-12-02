#include "ProjectLibrary.h"
#include "Project.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include "Task.h"

ProjectLibrary::ProjectLibrary() {
}

ProjectLibrary::~ProjectLibrary() {
    for (unsigned int i = 0; i< listProject.size(); i++) {
        delete listProject.at(i);
    }
}

void ProjectLibrary::addProject(Project* p) {
    listProject.push_back(p);
    sort(listProject.begin(), listProject.end(), WorkItem::sortVector);
    stateChanged();
}

Project* ProjectLibrary::getNewProject() {
    Project *p = new Project(nullptr);
    p->setProjectLibrary(this);
    return p;
}

Project* ProjectLibrary::getProject(unsigned int p) {
    return listProject.at(p);
}

void ProjectLibrary::addObserver(Observer* o) {
    observer = o;
}

void ProjectLibrary::removeObserver(Observer* o) {
    
}

void ProjectLibrary::getListProject(std::vector<Project*>* v) {
v = &listProject;
}

void ProjectLibrary::stateChanged() {
    if (observer != NULL) {
        observer->updateView(this);
        setChanged(true);
    }
}

void ProjectLibrary::setChanged(bool changed) {
    this->changed = changed;
}

bool ProjectLibrary::isChanged() {
    return changed;
}

int ProjectLibrary::size() {
    return listProject.size();
}

void ProjectLibrary::removeProject(unsigned int i) {
    Project* p = listProject.at(i);
    delete p;
    listProject.erase(listProject.begin()+i);
    sort(listProject.begin(), listProject.end(), WorkItem::sortVector);
    stateChanged();
}

void ProjectLibrary::toString() {
    for (int i = 0; i < size(); i++) {
        Project* p = getProject(i);
        std::cout << "Project:" << p->getText() << "\n";
        for (unsigned int j = 0; j < p->size(); j++) {
            Task* t = p->getTask(j);
            std::cout << "\t" << t->getText() << "\n";
        }
    }
}

#include "TableProject.h"
Table* ProjectLibrary::getTable() {
    return new TableProject();
}

