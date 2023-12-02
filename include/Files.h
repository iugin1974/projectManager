#ifndef FILES_H
#define FILES_H

#include <vector>
#include <string>

#include "Displayable.h"
class Table;

class Files : public Displayable {

private:
std::vector<std::string> files;
static std::string root;

public:
Files();
void add(std::string file);
void remove(int i);
int size();
std::string at(int i);
static void setRoot(std::string root);
static std::string getRoot();
Table* getTable();
};
#endif