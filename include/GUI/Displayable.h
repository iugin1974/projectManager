#ifndef DISPLAYABLE_H
#define DISPLAYABLE_H
class Table;

class Displayable {
public:
virtual Table* getTable() = 0;
virtual ~Displayable() = default;  // Distruttore virtuale
};

#endif