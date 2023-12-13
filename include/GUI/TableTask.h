#ifndef TABLETASK_H
#define TABLETASK_H

#include "Displayable.h"
#include "Table.h"

class TableTask : public Table
{
private:
  int currentTask = 0;
  void showHelp();
public:
  TableTask();
  ~TableTask();
  void display(Displayable *d) override;
  void navigate(Displayable *d) override;
  void highlight(Displayable *d, int i) override;
};

#endif
