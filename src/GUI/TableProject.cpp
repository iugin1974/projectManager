#include <curses.h>

#include "ProjectLibrary.h"
#include "Project.h"
#include "TableProject.h"
#include "Displayable.h"
#include "View.h"
#include "Task.h"
#include "Subtask.h"

TableProject::TableProject() {}

TableProject::~TableProject() {}

void TableProject::display(Displayable *d)
{
    ProjectLibrary *pl = static_cast<ProjectLibrary *>(d);
    /*if (pl->size() == 0)
    {
        setTextMenuBar("n:new Project");
    }
    else
    {
        setTextMenuBar("[enter]:edit   n:new Project   t:new task   f:files   s:save   e:edit   C:comment   c:view comment   g:Gannt diagram   2:add end date   S:save on ftp   D:delete   r:set root   q:quit");
    }*/

    int nt = 0; // number of Tasks
    int ns = 0; // number of Subtasks
    if (pl->size() > 0)
    {
        nt = pl->getProject(currentProject)->size();
        Project *p = pl->getProject(currentProject);
        for (unsigned int j = 0; j < p->size(); j++)
        {
            Task *t = p->getTask(j);
            ns += t->size();
        }
    }
    std::string text = "Total " + std::to_string(pl->size()) + " projects. The current project has " + std::to_string(nt) + " task and " + std::to_string(ns) + " subtask";
    setTextMenuBar(text);
    wclear(mainWin);
    int colWidth[] = {14, 14, 10, 10};
    std::string h1[5] = {"Ends", "Rem. Days", "Done", "Flags", "Project name"};
    printHeader(h1, colWidth, 4, LEFT_ALIGN, 0);
    unsigned int row = 0;
    drawHLine(mainWin, row);
    drawCol(mainWin, colWidth, 4, row++, ACS_TTEE);
    for (int i = 0; i < pl->size(); i++)
    {
        if (i == currentProject)
        {
            wattron(mainWin, A_REVERSE);
            wattron(mainWin, A_BOLD);
        }
        Project *p = pl->getProject(i);
        if (p->dateIsValid())
        {
            setText(mainWin, colWidth, row, 0, p->getDateAsString(Project::END_DATE).c_str());
            int d = p->getRemainingDays(p->getDate(WorkItem::END_DATE));
            setText(mainWin, colWidth, row, 1, std::to_string(d));
        }
        else
        {
            setText(mainWin, colWidth, row, 0, "--/--/--");
            setText(mainWin, colWidth, row, 1, "---");
        }
        std::string percent = std::to_string(p->getPercent()) + "%%";
        setText(mainWin, colWidth, row, 2, percent);

        setText(mainWin, colWidth, row, 3, p->getFormattedInfo());
        setText(mainWin, colWidth, row, 4, p->getText());

        wattroff(mainWin, A_REVERSE);
        wattroff(mainWin, A_BOLD);
        drawCol(mainWin, colWidth, 4, row++, ACS_VLINE);

        // Task
        for (unsigned int j = 0; j < p->size(); j++)
        {

            Task *t = p->getTask(j);
            if (t->isDone())
            {
                wattron(mainWin, COLOR_PAIR(1));
            }
            else if (t->getPriority() == Task::HIGH)
            {
                wattron(mainWin, A_BOLD);
                wattron(mainWin, COLOR_PAIR(2));
            }
            std::string name = t->getText();
            if (t->dateIsValid())
                name += " (" + t->getDateAsString(Project::END_DATE) + ")";
            setText(mainWin, colWidth, row, 4, name);
            wattroff(mainWin, COLOR_PAIR(1));
            wattroff(mainWin, COLOR_PAIR(2));
            wattroff(mainWin, A_BOLD);
            drawCol(mainWin, colWidth, 4, row++, ACS_VLINE);
            if (showSubtask)
            {
                for (unsigned int x = 0; x < t->size(); x++)
                {
                    Subtask *s = t->getSubtask(x);
                    setText(mainWin, colWidth, row, 4, "\t" + s->getText());
                    drawCol(mainWin, colWidth, 4, row++, ACS_VLINE);
                }
            }
        }
        drawHLine(mainWin, row);
        drawCol(mainWin, colWidth, 4, row++, ACS_PLUS);
    }
    unsigned int currentLine = getCurrentLine(pl, currentProject);
    int l = lines - 5;
    unsigned int gap = (int)(currentLine / l) * l;
    prefresh(mainWin, gap, 0, 3, 0, lines - 2, cols);
}

void TableProject::navigate(Displayable *d)
{
    ProjectLibrary *pl = static_cast<ProjectLibrary *>(d);
    display(pl);
    int ch = 0;
    while ((ch = wgetch(mainWin)) != 'q')
    {
        Project *project = nullptr;
        if (pl->size() > 0)
            project = pl->getProject(currentProject);

        switch (ch)
        {
        case '?':
            showHelp();
            break;
        case 's':
            if (pl->isChanged())
                view->save(pl);
            else
                view->infoBox(5, 50, "Nothing to save.", "Info");
            break;
        case 'S':
            view->saveOnFtp(pl);
            break;
        case 'n':
            view->createNewProject(pl);
            break;
        case 'e':
            if (project == nullptr)
                break;
            view->editProject(project);
            display(pl);
            break;
        case 'C':
            if (project == nullptr)
                break;
            view->addComment(project);
            break;
        case 'c':
            if (project == nullptr)
                break;
            view->showComment(project->getComment());
            break;
        case 'd':
        if (project == nullptr)
        break;
            if (project->size() > 0)
                break;
            project->setDone(!project->isDone());
            break;
        case 'g':
            if (project == nullptr)
                break;
            view->gannt(project);
            break;
        case 't':
            if (project == nullptr)
                break;
            view->createNewTask(project);
            break;
        case 'D':
        {
            if (project == nullptr)
                break;
                // controlla se fosse l'ultimo progetto
                // visualizzato nella tabella
            bool isLast = currentProject == pl->size() - 1;
            view->deleteProject(pl, currentProject);
            // se l'utente ha cancellato l'ultima riga della tabella
            // il progetto corrente passa alla penultima,
            // ma solo se ci sono ancora progetti,
            // altrimenti currentProject darebbe un valore sbagliato
            if (isLast && pl->size() > 0)
                currentProject--;
            break;
        }
        case 'r':
            view->setRoot();
            break;
        case 'k':
            showSubtask = !showSubtask;
            break;
        case 'f':
        {
            if (project == nullptr)
                break;
            Files *f = project->getFileList();
            view->navigateTable(f);
            break;
        }
        case KEY_UP:
            if (project == nullptr)
                break;
            highlight(pl, -1);
            break;
        case KEY_DOWN:
            if (project == nullptr)
                break;
            highlight(pl, 1);
            break;
        case '\n':
        case '\r':
        case KEY_ENTER:
        {
            if (project == nullptr)
                break;
            view->navigateTable(dynamic_cast<Displayable *>(project));
            break;
        }
        case KEY_RESIZE:
        {
            getmaxyx(stdscr, lines, cols);
            wresize(mainWin, 200, cols);
            display(pl);
            break;
        }
        }
        display(pl);
    }
}

void TableProject::highlight(Displayable *d, int i)
{
    ProjectLibrary *pl = static_cast<ProjectLibrary *>(d);
    currentProject += i;
    if (currentProject < 0)
        currentProject = pl->size() - 1;
    if (currentProject == pl->size())
        currentProject = 0;
}

void TableProject::showHelp()
{
    wclear(mainWin);
    wclear(headerWin);
    wprintw(mainWin, "Help\n\n");
    wprintw(mainWin, "[Enter]: Enter Task window\n");
    wprintw(mainWin, "n: new project\n");
    wprintw(mainWin, "e: edit current project\n");
    wprintw(mainWin, "2: set end date\n");
    wprintw(mainWin, "C: add comment\n");
    wprintw(mainWin, "d: set done/undone\n");
    wprintw(mainWin, "c: view comment\n");
    wprintw(mainWin, "g: show Gantt diagramm\n");
    wprintw(mainWin, "k: show Subtasks");
    wprintw(mainWin, "t: add new Task\n");
    wprintw(mainWin, "D: delete current project\n");
    wprintw(mainWin, "f: show File window\n");
    wprintw(mainWin, "s: save\n");
    wprintw(mainWin, "S: save on ftp-Server\n");
    wprintw(mainWin, "?: show this help\n\n\n");
    wprintw(mainWin, "Press any key to continue.");
    wrefresh(mainWin);
    wrefresh(headerWin);
    wgetch(mainWin);
}