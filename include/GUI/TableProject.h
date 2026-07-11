#ifndef TABLEPROJECT_H
#define TABLEPROJECT_H
#include <vector>
#include "Displayable.h"
#include "Table.h"

class WorkItem;
class ProjectLibrary;
class TodoLibrary;

class TableProject : public Table
{
private:
  int currentRow = 0;
  bool showSubtask = false;
  void showHelp();
  std::vector<WorkItem *> buildTopLevelList(ProjectLibrary *pl, TodoLibrary *tl);

public:
  TableProject();
  ~TableProject();
  void display(Displayable *d) override;
  void navigate(Displayable *d) override;
  void highlight(Displayable *d, int i) override;
};
#endif