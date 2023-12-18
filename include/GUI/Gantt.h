#ifndef GANTT_H
#define GANTT_H

#include <ncurses.h>

#include "Displayable.h"
#include "Table.h"

class Gantt : public Table
{
private:
bool showSubtask = false;
int firstShowedRow = 0;
int long getFactor(Project* p, int cols);
long int getPos(Project* p, tm* date, long int factor);

public:
Gantt();
~Gantt();
    void display(Displayable *d) override;
    void navigate(Displayable *d) override;
    void highlight(Displayable *d, int i) override;
};
#endif