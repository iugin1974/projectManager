// Workspace.cpp
#include "Workspace.h"
#include "TableProject.h"

Table *Workspace::getTable()
{
	return new TableProject();
}
