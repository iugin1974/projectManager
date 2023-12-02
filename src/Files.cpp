#include <vector>
#include <string>

#include "Files.h"
#include "TableFiles.h"

std::string Files::root = "";


Files::Files() {
    files = std::vector<std::string>();
}

void Files::add(std::string file) {
    files.push_back(file);
}

void Files::remove(int i) {
    files.erase(files.begin() + i);
}

int Files::size() {
    return files.size();
}

std::string Files::at(int i) {
return files.at(i);
}

/*
Poiché il programma gira su più computer, ma il file xml è nel server ftp,
le path dei file sono senza radice.
In un computer c'è il file /home/user/myfile.txt, in un altro /home/user2/myfile.txt
Allora si salva la radice /home/user in un computer e nell'altro l'altra radice /home/user2
Files memorizza in root appunto questa radice.
*/
void Files::setRoot(std::string r) {
    Files::root = r;
}

std::string Files::getRoot() {
    return Files::root;
}

Table* Files::getTable() {
    return new TableFiles();
}