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

unsigned int ProjectLibrary::size() {
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
    for (Project* p : listProject) {
        std::cout << "Project:" << p->getText() << "\n";
        for (Task* t : p ->getTaskList()) {
            std::cout << "\t" << t->getText() << "\n";
        }
    }
}

#include "TableProject.h"
Table* ProjectLibrary::getTable() {
    return new TableProject();
}

