#ifndef PROJECT_LIBRARY_H
#define PROJECT_LIBRARY_H

#include <vector>
#include "Observable.h"
#include "Project.h"
#include "Displayable.h"
class Table;

class ProjectLibrary : public Observable, public Displayable {
	
	private:
		Observer* observer = NULL;
		std::vector<Project*> listProject;
		static bool sortVector(Project* p1, Project* p2);
		bool changed = false;
	
	public:
		ProjectLibrary();
		~ProjectLibrary();
		void addProject (Project* p);
		Project* getNewProject();
		Project* getProject (unsigned int p);
		void getListProject(std::vector<Project*>*);
		void addObserver(Observer* o) override;
		void removeObserver(Observer* o) override;
		void stateChanged() override;
		bool isChanged();
		void setChanged(bool changed);
		int size();
		void toString();
		void removeProject(unsigned int i);
		Table* getTable() override;
};

#endif
