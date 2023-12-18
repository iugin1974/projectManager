#include <ncurses.h>

#include "Displayable.h"
#include "WorkItem.h"
#include "Gantt.h"
#include "Table.h"
#include "View.h"
#include "Date.h"
#include "Task.h"
#include "Subtask.h"

Gantt::Gantt()
{
}

Gantt::~Gantt() {}

// Dato un progetto p e il numero di coloone dello schermo
// la funzione calcola il fattore per cui la fine del
// progetto coincide con la larghezza dello schermo
long int Gantt::getFactor(Project *p, int cols)
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

// calcola la posizione sullo schermo di una data *date in base
// al fattore calcolato dalla funzione precedente
long int Gantt::getPos(Project *p, tm *date, long int factor)
{
    tm startProject = p->getDate(WorkItem::START_DATE);
    long int sp = Date::toLong(&startProject);
    long int dl = Date::toLong(date);
    dl = dl - sp;
    return dl / factor;
}

void Gantt::display(Displayable *d)
{
    wclear(mainWin);
    setTextMenuBar("q:quit");

    Project *p = static_cast<Project *>(d);
    long int factor = getFactor(p, cols);
    // calcola la posizione della data auttuale
    tm start = p->getDate(WorkItem::START_DATE);
    tm now;
    Date::now(&now);
    int long nowL = Date::toLong(&now);
    int long startL = Date::toLong(&start);
    nowL = nowL - startL;
    int nowI = nowL / factor;
    int row = 1;
    for (Task *t : p->getTaskList())
    {
        if (t->isDone())
        {
            wattron(mainWin, COLOR_PAIR(1));
        }
        else if (t->getPriority() == Task::HIGH)
        {
            wattron(mainWin, COLOR_PAIR(2));
        }
        std::string text = t->getText() + " ";
        mvwprintw(mainWin, row++, 0, text.c_str());
        // disegna prima la barra e poi scrive il nome della task
        // per poterlo scrivere sulla barra
        tm st = t->getDate(WorkItem::START_DATE);
        tm et = t->getDate(WorkItem::END_DATE);
        int startPos = getPos(p, &st, factor);
        int endPos = getPos(p, &et, factor);

        for (; startPos <= endPos; startPos++)
        {
            mvwaddch(mainWin, row, startPos, ACS_CKBOARD);
        }

        if (showSubtask)
        {
            for (Subtask *s : t->getSubtaskList())
            {
                row++;
                mvwprintw(mainWin, row++, 0, s->getText().c_str());
                tm ss = s->getDate(WorkItem::START_DATE);
                tm es = s->getDate(WorkItem::END_DATE);
                int startPosS = getPos(p, &ss, factor);
                int endPosS = getPos(p, &es, factor);
                for (; startPosS <= endPosS; startPosS++)
                {
                    mvwaddch(mainWin, row, startPosS, ACS_DIAMOND);
                }
            }
            row++;
        }
        wattroff(mainWin, COLOR_PAIR(1));
        wattroff(mainWin, COLOR_PAIR(2));
        row++;
    }
    // segna la linea della data attuale sopra e sotto
    for (int i = 0; i < 200; i++)
        mvwaddch(mainWin, i, nowI, chtype ACS_VLINE);
    prefresh(mainWin, firstShowedRow, 0, 3, 0, lines - 1, cols);
}

void Gantt::navigate(Displayable *d)
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
        case 'k':
            showSubtask = !showSubtask;
            firstShowedRow = 0;
            break;
        case KEY_DOWN:
            firstShowedRow++;
            break;
        case KEY_UP:
            if (firstShowedRow > 0)
                firstShowedRow--;
            break;
        default:
            break;
        }
        display(d);
    }
}

void Gantt::highlight(Displayable *d, int i) {}