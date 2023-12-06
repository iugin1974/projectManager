#ifndef TABLEPROJECT_H
#define TABLEPROJECT_H

#include "Displayable.h"
#include "Table.h"

class TableProject : public Table {
  private:
  int currentProject = 0;

  public:
    TableProject();
    ~TableProject();
    void display(Displayable *d) override;
    void navigate(Displayable *d) override;
    void highlight(Displayable *d, int i) override;
};

#endif
