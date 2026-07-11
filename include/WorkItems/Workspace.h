// Workspace.h
#ifndef WORKSPACE_H
#define WORKSPACE_H
#include "Displayable.h"

class ProjectLibrary;
class TodoLibrary;
class Table;

class Workspace : public Displayable
{
private:
	ProjectLibrary *pl;
	TodoLibrary *tl;

public:
	Workspace(ProjectLibrary *pl, TodoLibrary *tl) : pl(pl), tl(tl) {}
	ProjectLibrary *getProjectLibrary() { return pl; }
	TodoLibrary *getTodoLibrary() { return tl; }
	Table *getTable() override;
};
#endif
