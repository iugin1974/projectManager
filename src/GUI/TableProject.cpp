#include <curses.h>

#include "ProjectLibrary.h"
#include "Project.h"
#include "TableProject.h"
#include "Displayable.h"
#include "View.h"
#include "Task.h"

TableProject::TableProject() {}

void TableProject::display(Displayable *d)
{
    setTextMenuBar("[enter]:edit   n:new Project   t:new task   f:files   s:save   e:edit   C:comment   c:view comment   g:Gannt diagram   2:add end date   S:save on ftp   D:delete   r:set root   q:quit");
    wclear(mainWin);
    ProjectLibrary *pl = static_cast<ProjectLibrary *>(d);
    int colWidth[] = {14, 14, 10};
    std::string h1[4] = {"Date", "Rem. Days", "Done", "Project name"};
    printHeader(h1, colWidth, 4, LEFT_ALIGN, 0);
    unsigned int row = 0;
    drawHLine(mainWin, row);
    drawCol(mainWin, colWidth, 3, row++, ACS_TTEE);
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

        if (p->hasComment())
        {
            setText(mainWin, colWidth, row, 3, "*" + p->getText());
        }
        else
        {
            setText(mainWin, colWidth, row, 3, p->getText());
        }

        wattroff(mainWin, A_REVERSE);
        wattroff(mainWin, A_BOLD);
        drawCol(mainWin, colWidth, 3, row++, ACS_VLINE);

        // Task
        for (unsigned int j = 0; j < p->size(); j++)
        {

            Task *t = p->getTask(j);
            if (t->isDone())
            {
                wattron(mainWin, COLOR_PAIR(1));
            }
            else if (t->getPriority() == "h")
            {
                wattron(mainWin, A_BOLD);
                wattron(mainWin, COLOR_PAIR(2));
            }
            std::string name = t->getText();
            if (t->dateIsValid())
                name += " (" + t->getDateAsString(Project::END_DATE) + ")";
            setText(mainWin, colWidth, row, 3, name);
            wattroff(mainWin, COLOR_PAIR(1));
            wattroff(mainWin, COLOR_PAIR(2));
            wattroff(mainWin, A_BOLD);
            drawCol(mainWin, colWidth, 3, row++, ACS_VLINE);
        }
        drawHLine(mainWin, row);
        drawCol(mainWin, colWidth, 3, row++, ACS_PLUS);
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
            if (project == nullptr)
                break;
            view->deleteProject(pl, currentProject);
            break;
        case 'r':
            view->setRoot();
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
