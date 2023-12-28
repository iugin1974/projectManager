#include <ncurses.h>

#include "Project.h"
#include "Displayable.h"
#include "WorkItem.h"
#include "Task.h"
#include "View.h"
#include "TableSubtask.h"
#include "Subtask.h"

TableSubtask::TableSubtask() {}
TableSubtask::~TableSubtask() {}

void TableSubtask::display(Displayable *d)
{
    Task *t = static_cast<Task *>(d);
    wclear(mainWin);
    std::string text = "Total " + std::to_string(t->size()) + " subtasks";
    setTextMenuBar(text);
    unsigned int row = 0;
    int colWidth[] = {14, 14, 10};
    std::string h1[4] = {"Starts", "Ends", "Flags", "Task name"};
    printHeader(h1, colWidth, 4, LEFT_ALIGN, 0);

    drawHLine(mainWin, row++);

    drawCol(mainWin, colWidth, 4, row, ACS_TTEE);

    for (unsigned int i = 0; i < t->size(); i++)
    {
        Subtask *st = t->getSubtask(i);
        if (st->isDone())
        {
            wattron(mainWin, COLOR_PAIR(1));
        }
        else if (st->getPriority() == Task::HIGH)
        {
            wattron(mainWin, COLOR_PAIR(2));
        }
        if (i == currentSubtask)
        {
            wattron(mainWin, A_BOLD);
        }
        setText(mainWin, colWidth, row, 0, st->getDateAsString(WorkItem::START_DATE));
        if (st->dateIsValid())
            setText(mainWin, colWidth, row, 1, st->getDateAsString(WorkItem::END_DATE));
        else
            setText(mainWin, colWidth, row, 1, "--/--/--");

        setText(mainWin, colWidth, row, 2, st->getFormattedInfo());
        setText(mainWin, colWidth, row, 3, st->getText());

        wattroff(mainWin, A_BOLD);
        wattroff(mainWin, COLOR_PAIR(1));
        drawCol(mainWin, colWidth, 3, row++, ACS_VLINE);
    }
    drawHLine(mainWin, row);
    drawCol(mainWin, colWidth, 3, row, ACS_BTEE);
    prefresh(mainWin, 0, 0, 3, 0, lines - 2, cols);
}

void TableSubtask::navigate(Displayable *d)
{
    Task *task = static_cast<Task *>(d);
    display(task);
    int ch = 0;
    while ((ch = wgetch(mainWin)) != 'q')
    {
        Subtask *subtask = nullptr;
        if (task->size() > 0)
            subtask = task->getSubtask(currentSubtask);
        switch (ch)
        {
        case 'n':
            view->createNewSubtask(task);
            break;
        case 'b':
            view->brainstorming(task, this);
            break;
        case 'p':
            if (task->size() > 0)
            {
                view->changeTaskPriority(subtask);
            }
            break;

        case '1':
            if (task->size() > 0)
            {
                view->setDate(subtask, WorkItem::START_DATE);
            }
            break;

        case '2':
            if (task->size() > 0)
            {
                view->setDate(subtask, WorkItem::END_DATE);
            }
            break;
        case 'D':
            if (task->size() != 0)
            {
                bool isLast = currentSubtask == task->size() - 1;
                view->deleteSubtask(task, currentSubtask);
                // se l'utente ha cancellato l'ultima riga della tabella
            // la task corrente passa alla penultima,
            // ma solo se ci sono ancora task,
            // altrimenti currentTask darebbe un valore sbagliato

                if (isLast && task->size() > 0)
                    currentSubtask--;
            }
            else
                view->infoBox(5, 50, "There are no subtasks to cancel.", "Error");
            break;
        case 'd':
        {
            if (subtask->isDone())
                subtask->setDone(false);
            else
                subtask->setDone(true);
        }
        break;
        case 'e':
            view->editTask(subtask);
            break;
        case 'C':
            view->addComment(subtask);
            break;
        case 'c':
            view->showComment(subtask->getComment());
            break;
        case KEY_UP:
            highlight(task, -1);
            break;
        case KEY_DOWN:
            highlight(task, 1);
            break;
        case KEY_RESIZE:
        {
            getmaxyx(stdscr, lines, cols);
            wresize(mainWin, 200, cols);
            display(task);
            break;
        }
        }
        display(task);
    }
    currentSubtask = 0;
}

void TableSubtask::highlight(Displayable *d, int i)
{
    Task *t = static_cast<Task *>(d);
    currentSubtask += i;
    if (currentSubtask < 0)
        currentSubtask = t->size() - 1;
    if (currentSubtask == t->size())
        currentSubtask = 0;
}
