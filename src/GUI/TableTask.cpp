#include <ncurses.h>

#include "Project.h"
#include "Table.h"
#include "TableTask.h"
#include "Displayable.h"
#include "WorkItem.h"
#include "Task.h"
#include "TableSubtask.h"
#include "Subtask.h"
#include "View.h"

TableTask::TableTask() {}
TableTask::~TableTask() {}


void TableTask::display(Displayable *d)
{
    wclear(mainWin);
    setTextMenuBar("[Enter]:edit   n:new Task   b:brainstorming   d:done/undone   C:comment   c:view comment   e:edit   D:delete   p:change priority   1:add start date   2:add end date   g:Gannt diagram   q:quit");
    Project *p = static_cast<Project *>(d);
    unsigned int row = 0;
    int colWidth[] = {14, 14, 10};
    std::string h1[4] = {"Starts", "Ends", "Priority", "Task name"};
    printHeader(h1, colWidth, 4, LEFT_ALIGN, 0);

    drawHLine(mainWin, row++);

    drawCol(mainWin, colWidth, 4, row, ACS_TTEE);

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
        if (i == currentTask)
        {
            wattron(mainWin, A_BOLD);
        }
        setText(mainWin, colWidth, row, 0, t->getDateAsString(WorkItem::START_DATE));
        if (t->dateIsValid())
            setText(mainWin, colWidth, row, 1, t->getDateAsString(WorkItem::END_DATE));
        else
            setText(mainWin, colWidth, row, 1, "--/--/--");

        setText(mainWin, colWidth, row, 2, t->getPriority().c_str());
        //if (t->hasComment())
        //{
            setText(mainWin, colWidth, row, 3, t->getFormattedInfo() + " " + t->getText());
        //}
        //else
        //{
        //    setText(mainWin, colWidth, row, 3, t->getText());
        //}

        wattroff(mainWin, A_BOLD);
        wattroff(mainWin, COLOR_PAIR(1));
        drawCol(mainWin, colWidth, 3, row++, ACS_VLINE);
        ///////////////////////////////
        for (unsigned int j = 0; j < t->size(); j++)
        {
            Subtask *sub = t->getSubtask(j);
            setText(mainWin, colWidth, row, 3, "\t" + sub->getText());
            drawCol(mainWin, colWidth, 3, row++, ACS_VLINE);
        }
    }
    drawHLine(mainWin, row);
    drawCol(mainWin, colWidth, 3, row, ACS_BTEE);
    prefresh(mainWin, 0, 0, 3, 0, lines - 2, cols);
}

void TableTask::navigate(Displayable *d)
{
    Project *p = static_cast<Project *>(d);
    display(p);
    int ch = 0;
    while ((ch = wgetch(mainWin)) != 'q')
    {
        Task *task = nullptr;
        if (p->size() > 0)
            task = p->getTask(currentTask);

        switch (ch)
        {
        case 'n':
            view->createNewTask(p);
            break;
        case 's':
            if (task == nullptr)
                break;
            view->createNewSubtask(task);
            break;
        case 'p':
            if (task == nullptr)
                break;
            view->changeTaskPriority(task);
            break;

        case '1':
            if (task == nullptr)
                break;
            view->setDate(task, WorkItem::START_DATE);
            break;

        case '2':
            if (task == nullptr)
                break;
            view->setDate(task, WorkItem::END_DATE);
            break;
        case 'g':
            if (task == nullptr)
                break;
            view->gannt(p);
            break;
        case 'D':
            if (task == nullptr)
                break;
            view->deleteTask(p, currentTask);
            break;
        case 'd':
            if (task == nullptr)
                break;
            if (task->isDone())
                task->setDone(false);
            else
                task->setDone(true);
            break;
        case 'b':
            if (task == nullptr)
                break;
            view->brainstorming(p, this);
            break;
        case 'e':
            if (task == nullptr)
                break;
            view->editTask(task);
            break;
        case 'C':
            if (task == nullptr)
                break;
            view->addComment(task);
            break;
        case 'c':
            if (task == nullptr)
                break;
            view->showComment(task->getComment());
            break;
        case KEY_UP:
            if (task == nullptr)
                break;
            highlight(p, -1);
            break;
        case KEY_DOWN:
            if (task == nullptr)
                break;
            highlight(p, 1);
            break;
        case KEY_RESIZE:
        {
            getmaxyx(stdscr, lines, cols);
            wresize(mainWin, 200, cols);
            display(p);
            break;
        }
        case '\n':
        case '\r':
        case KEY_ENTER:
            if (task == nullptr)
                break;
            view->navigateTable(task);
            break;
        }
        display(p);
    }
    currentTask = 0;
}

void TableTask::highlight(Displayable *d, int i)
{
    Project *p = static_cast<Project *>(d);
    currentTask += i;
    if (currentTask < 0)
        currentTask = p->size() - 1;
    if (currentTask == p->size())
        currentTask = 0;
}
