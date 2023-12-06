#include <curses.h>
#include <string>

#include "Table.h"
#include "Project.h"
#include "ProjectLibrary.h"

Table::Table() {}

void Table::setWindows(View *v, WINDOW *win, WINDOW *mb, WINDOW *hw) {
view = v;
    mainWin = win;
    menuBar = mb;
    headerWin = hw;
}

Table::~Table() {}

void Table::initTable(int l, int c)
{
    cols = c;
    lines = l;
    tableH = lines - 5;
}

void Table::setTextMenuBar(std::string txt)
{
    wclear(menuBar);
    wresize(menuBar, 1, cols);
    init_pair(3, COLOR_WHITE, COLOR_BLUE);
    wbkgd(menuBar, COLOR_PAIR(3));
    wattron(menuBar, A_BOLD);
    mvwprintw(menuBar, 0, 0, txt.c_str());
    wattroff(menuBar, A_BOLD);
    wrefresh(menuBar);
}

void Table::drawHLine(WINDOW *win, int line)
{
    int x = 0;
    int y = 0;
    getmaxyx(win, y, x);
    for (int i = 0; i < x; i++)
    {
        mvwaddch(win, line, i, ACS_HLINE);
    }
}

void Table::drawCol(WINDOW *win, int colW[], int arrayLength, int line, chtype ch)
{
    int xPos = 0;
    for (int i = 0; i < arrayLength; i++)
    {
        xPos += colW[i];
        mvwaddch(win, line, xPos, ch);
    }
}

void Table::printHeader(std::string text[], int colW[], int arrayLength, int alignement, int line)
{
    wclear(headerWin);
    int left = 0;
    for (int i = 0; i < arrayLength; i++)
    {
        int textLength = text[i].length();
        int pos = 0;
        if (alignement == CENTER_ALIGN)
        {
            pos = left + (colW[i] - textLength) / 2;
        }
        else if (alignement == LEFT_ALIGN)
        {
            pos = left + 2;
        }
        mvwprintw(headerWin, line, pos, text[i].c_str());
        left += colW[i]; // Avanza alla posizione successiva, considerando la larghezza totale della colonna
    }
    wrefresh(headerWin);
}

void Table::setText(WINDOW *win, int colW[], int line, int col, std::string text)
{
    int pos = 0;
    for (int i = 0; i < col; i++)
        pos = pos + colW[i];
    pos += 2;
    mvwprintw(win, line, pos, text.c_str());
}
/**
 * Ritorna il numero di linea del progetto evidenziato.
 */
int Table::getCurrentLine(ProjectLibrary *pl, unsigned int currentProject)
{
    unsigned int currentLine = 1;
    for (unsigned int i = 0; i < currentProject; i++)
    {
        Project *p = pl->getProject(i);
        // aggiunge il numero delle task, perché quando sono
        // scritte, ognuna occupa una riga
        currentLine += p->size();
        // aggiunge 1 perché anche il nome del progetto viene scritto
        // e 1 per la linea di delimitazione della tabella
        currentLine += 2;
    }
    return currentLine;
}