#include <ncurses.h>

#include "Files.h"
#include "TableFiles.h"
#include "Displayable.h"
#include "View.h"


TableFiles::TableFiles() {}

TableFiles::~TableFiles() {}

void TableFiles::display(Displayable *d)
{
    setTextMenuBar("n:new File   D:delete   [enter]:open file    q:quit");
    Files *f = static_cast<Files *>(d);
wclear(mainWin);
    unsigned int row = 0;
 //   drawHLine(row++);
    for (int i = 0; i < f->size(); i++)
    {
        if (i == currentFile)
        {
            wattron(mainWin, A_REVERSE);
            wattron(mainWin, A_BOLD);
        }
        mvwprintw(mainWin, row++, 0, (f->at(i)).c_str());
        wattroff(mainWin, A_REVERSE);
        wattroff(mainWin, A_BOLD);
    }
//    drawHLine(row++);
    prefresh(mainWin, 0, 0, 3, 0, lines - 1, cols);
}

void TableFiles::navigate(Displayable *d)
{
    Files *f = static_cast<Files *>(d);
    display(f);
    int ch = 0;
    while ((ch = wgetch(mainWin)) != 'q')
    {
        switch (ch)
        {
        case 'n':
            view->addFile(f);
            break;

        case 'D':
            if (f->size() != 0)
                view->deleteFile(f, currentFile);
            else
                view->infoBox(5, 50, "There are no files to cancel.", "Error");
            break;
            break;

        case 'e':
            // view->editTask(p->getTask(currentFile));
            break;
        case KEY_UP:
            highlight(f, -1);
            break;
        case KEY_DOWN:
            highlight(f, 1);
            break;
        case KEY_RESIZE:
        {
            getmaxyx(stdscr, lines, cols);
            wresize(mainWin, 200, cols);
            display(f);
            break;
        }

        case '\n':
        case '\r':
        case KEY_ENTER:
        {
            view->openFile(f, currentFile);
        }
        }
        display(f);
    }
    currentFile = 0;
}

void TableFiles::highlight(Displayable *d, int i)
{
    Files *f = static_cast<Files *>(d);
    currentFile += i;
    if (currentFile < 0)
        currentFile = f->size() - 1;
    if (currentFile == f->size())
        currentFile = 0;
}