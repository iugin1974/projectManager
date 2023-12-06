#ifndef GANNT_H
#define GANNT_H

#include <ncurses.h>

#include "Displayable.h"
#include "Table.h"

class Gannt : public Table
{
private:
int long getFactor(Project* p, int cols);
long int getPos(Project* p, tm* date, long int factor);

public:
Gannt();
~Gannt();
    void display(Displayable *d) override;
    void navigate(Displayable *d) override;
    void highlight(Displayable *d, int i) override;
};
#endif