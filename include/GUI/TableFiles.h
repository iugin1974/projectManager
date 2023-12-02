#ifndef TABLEFILES_H
#define TABLEFILES_H

#include "Displayable.h"
#include "Table.h"

class TableFiles : public Table
{
private:
  int currentFile = 0;

public:
  TableFiles();
  void display(Displayable *d) override;
  void navigate(Displayable *d) override;
  void highlight(Displayable *d, int i) override;
};

#endif
