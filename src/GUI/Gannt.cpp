#include <ncurses.h>

#include "Displayable.h"
#include "WorkItem.h"
#include "Gannt.h"
#include "Table.h"
#include "View.h"
#include "Date.h"
#include "Task.h"

Gannt::Gannt()
{
}

Gannt::~Gannt() {}

// TODO
// Puoi mettere la data di creazione delle task, tipo gannt
// cambia creation in start

long int Gannt::getFactor(Project *p, int cols)
{
    tm startProject = p->getDate(Project::START_DATE);
    tm endProject = p->getDate(Project::END_DATE);
    long int sp = Date::toLong(&startProject);
    long int ep = Date::toLong(&endProject);
    // Poiché la data iniziale del progetto è il punto zero,
    // devo idealmente spostare il tutto a sinistra.
    // Quindi sottraggo alla data finale la data iniziale.
    ep = ep - sp;
    return ep / cols;
}

long int Gannt::getPos(Project *p, tm *date, long int factor)
{
    tm startProject = p->getDate(WorkItem::START_DATE);
    long int sp = Date::toLong(&startProject);
    long int dl = Date::toLong(date);
    dl = dl - sp;
    return dl / factor;
}

void Gannt::display(Displayable *d)
{
wclear(mainWin);
    setTextMenuBar("q:quit");
    int wNameWidth = 0;
    Project *p = static_cast<Project *>(d);
    long int factor = getFactor(p, cols - wNameWidth);
    // calcola la posizione della data auttuale
    tm now;
    tm start = p->getDate(WorkItem::START_DATE);
    Date::now(&now);
    int long nowL = Date::toLong(&now);
    int long startL = Date::toLong(&start);
    nowL = nowL - startL;
    int nowI = nowL / factor;

    for (unsigned int i = 0; i < p->size(); i++)
    {
        Task *t = p->getTask(i);
        if (t->isDone())
        {
            wattron(mainWin, COLOR_PAIR(1));
        }
        else if (t->getPriority() == "h")
        {
            wattron(mainWin, COLOR_PAIR(2));
        }
        // disegna prima la barra e poi scrive il nome della task
        // per poterlo scrivere sulla barra
        tm st = t->getDate(WorkItem::START_DATE);
        tm et = t->getDate(WorkItem::END_DATE);
        int startPos = getPos(p, &st, factor);
        int endPos = getPos(p, &et, factor);
        for (int j = startPos; j <= endPos; j++)
        {
            mvwaddch(mainWin, i+1, j, ACS_CKBOARD);
        }

        std::string text = t->getText()+" ";
        mvwprintw(mainWin, i+1, 0, text.c_str());

        wattroff(mainWin, COLOR_PAIR(1));
        wattroff(mainWin, COLOR_PAIR(2));
    }
    // segna la linea della data attuale sopra e sotto
        mvwaddch(mainWin, 0, nowI, chtype ACS_VLINE);
        for (int i = p->size()+1; i < lines; i++)
        mvwaddch(mainWin, i, nowI, chtype ACS_VLINE);
    prefresh(mainWin, 0, 0, 3, 0, lines - 1, cols);
}

void Gannt::navigate(Displayable *d)
{
    Project *p = static_cast<Project *>(d);
    display(p);
    int ch = 0;
    while ((ch = wgetch(mainWin)) != 'q')
    {
        switch (ch)
        {
        case 'q':
            break;

        default:
            break;
        }
        display(d);
    }
}

void Gannt::highlight(Displayable *d, int i) {}