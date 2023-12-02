#ifndef TABLESUBTASK_H
#define TABLESUBTASK_H

#include "Displayable.h"
#include "Table.h"

class TableSubtask : public Table
{
private:
  int currentSubtask = 0;
public:
  TableSubtask();
  void display(Displayable *d) override;
  void navigate(Displayable *d) override;
  void highlight(Displayable *d, int i) override;
};

#endif
