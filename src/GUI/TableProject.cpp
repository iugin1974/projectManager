#include <curses.h>

#include "ProjectLibrary.h"
#include "TodoLibrary.h"
#include "Project.h"
#include "Todo.h"
#include "Workspace.h"
#include "TableProject.h"
#include "Displayable.h"
#include "View.h"
#include "Task.h"
#include "Subtask.h"

TableProject::TableProject() {}

TableProject::~TableProject() {}

std::vector<WorkItem *> TableProject::buildTopLevelList(ProjectLibrary *pl, TodoLibrary *tl)
{
	std::vector<WorkItem *> items;
	for (unsigned int i = 0; i < tl->size(); i++)
		items.push_back(&tl->getTodo(i));
	for (unsigned int i = 0; i < pl->size(); i++)
		items.push_back(pl->getProject(i));
	return items;
}

void TableProject::display(Displayable *d)
{
	Workspace *ws = dynamic_cast<Workspace *>(d);
	if (ws == nullptr)
		return;
	ProjectLibrary *pl = ws->getProjectLibrary();
	TodoLibrary *tl = ws->getTodoLibrary();

	std::vector<WorkItem *> items = buildTopLevelList(pl, tl);

	int nt = 0, ns = 0;
	if (items.size() > 0)
	{
		if (Project *cp = dynamic_cast<Project *>(items.at(currentRow)))
		{
			nt = cp->size();
			for (unsigned int j = 0; j < cp->size(); j++)
				ns += cp->getTask(j)->size();
		}
	}
	std::string text = "Total " + std::to_string(pl->size()) + " projects, "
		+ std::to_string(tl->size()) + " todos. Current item has "
		+ std::to_string(nt) + " task and " + std::to_string(ns) + " subtask";
	setTextMenuBar(text);
	wclear(mainWin);
	int colWidth[] = {14, 14, 10, 10};
	std::string h1[5] = {"Ends", "Rem. Days", "Done", "Flags", "Name"};
	printHeader(h1, colWidth, 5, LEFT_ALIGN, 0);
	unsigned int row = 0;

	// --- Blocco Todo: righe piene, senza colonne ---
	unsigned int todoCount = tl->size();
if (todoCount > 0)
{
	int textCol = colWidth[0] + colWidth[1] + colWidth[2] + colWidth[3] + 2;
	drawHLine(mainWin, row++);
	for (unsigned int i = 0; i < todoCount; i++)
	{
		Todo *t = dynamic_cast<Todo *>(items.at(i));
		if (i == currentRow)
			wattron(mainWin, A_REVERSE);
		if (t->isDone())
			wattron(mainWin, COLOR_PAIR(1));
		else
			wattron(mainWin, A_BOLD);
		mvwprintw(mainWin, row, textCol, "%s", t->getText().c_str());
		wattroff(mainWin, A_REVERSE);
		wattroff(mainWin, COLOR_PAIR(1));
		wattroff(mainWin, A_BOLD);
		row++;
	}
}

	// --- Bordo con colonne, inizio tabella progetti ---
	drawHLine(mainWin, row);
	drawCol(mainWin, colWidth, 4, row++, ACS_TTEE);

	for (int i = (int)todoCount; i < (int)items.size(); i++)
	{
		Project *p = dynamic_cast<Project *>(items.at(i));
		if (p == nullptr)
			continue;

		// --- resto invariato, identico a prima ---
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
		if (i == currentRow)
			wattron(mainWin, A_REVERSE);
		wattron(mainWin, A_BOLD);
		setText(mainWin, colWidth, row, 4, p->getText());
		wattroff(mainWin, A_REVERSE);
		wattroff(mainWin, A_BOLD);
		drawCol(mainWin, colWidth, 4, row++, ACS_VLINE);

		for (unsigned int j = 0; j < p->size(); j++)
		{
			Task *t = p->getTask(j);
			if (t->isDone())
				wattron(mainWin, COLOR_PAIR(1));
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

	unsigned int currentLine = getCurrentLine(items, currentRow, todoCount);
	int l = lines - 5;
	unsigned int gap = (int)(currentLine / l) * l;
	prefresh(mainWin, gap, 0, 3, 0, lines - 2, cols);
}

void TableProject::navigate(Displayable *d)
{
	Workspace *ws = dynamic_cast<Workspace *>(d);
	if (ws == nullptr)
		return;
	ProjectLibrary *pl = ws->getProjectLibrary();
	TodoLibrary *tl = ws->getTodoLibrary();

	display(ws);
	int ch = 0;
	while ((ch = wgetch(mainWin)) != 'q')
	{
		std::vector<WorkItem *> items = buildTopLevelList(pl, tl);
		WorkItem *selected = nullptr;
		if (items.size() > 0)
			selected = items.at(currentRow);

		Project *project = dynamic_cast<Project *>(selected); // nullptr se è un Todo
		Todo *todo = dynamic_cast<Todo *>(selected);           // nullptr se è un Project

		switch (ch)
		{
			case '?':
				showHelp();
				break;
			case 's':
				if (pl->isChanged() || tl->isChanged())
					view->save(ws);
				else
					view->infoBox(5, 50, "Nothing to save.", "Info");
				break;
			case 'S':
				view->saveOnFtp(pl, tl);
				break;
			case 'n':
				view->createNewProject(pl);
				break;
			case 'e':
				if (project == nullptr)
					break;
				view->editProject(project);
				display(ws);
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
				if (todo != nullptr)
					todo->setDone(!todo->isDone());
				else if (project != nullptr)
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
			case 'T':
				view->createTodo(tl);
				break;
			case 'D':
{
	if (selected == nullptr)
		break;
	bool isLast = currentRow == (int)items.size() - 1;
	if (todo != nullptr)
		view->deleteTodo(tl, todo);
	else if (project != nullptr)
		view->deleteProject(pl, project);
	if (isLast && currentRow > 0)
		currentRow--;
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
				if (items.size() == 0)
					break;
				highlight(ws, -1);
				break;
			case KEY_DOWN:
				if (items.size() == 0)
					break;
				highlight(ws, 1);
				break;
			case '\n':
			case '\r':
			case KEY_ENTER:
				if (project != nullptr)
					view->navigateTable(dynamic_cast<Displayable *>(project));
				// se è un Todo, non facciamo nulla
				break;
			case KEY_RESIZE:
				{
					getmaxyx(stdscr, lines, cols);
					wresize(mainWin, 200, cols);
					display(ws);
					break;
				}
		}
		display(ws);
	}
}

void TableProject::highlight(Displayable *d, int i)
{
	Workspace *ws = dynamic_cast<Workspace *>(d);
	if (ws == nullptr)
		return;
	std::vector<WorkItem *> items = buildTopLevelList(ws->getProjectLibrary(), ws->getTodoLibrary());
	currentRow += i;
	if (currentRow < 0)
		currentRow = (int)items.size() - 1;
	if (currentRow == (int)items.size())
		currentRow = 0;
}

void TableProject::showHelp()
{
	wclear(mainWin);
	wclear(headerWin);
	wprintw(mainWin, "Help\n\n");
	wprintw(mainWin, "[Enter]: Enter Task window\n");
	wprintw(mainWin, "n: new project\n");
	wprintw(mainWin, "T: new todo\n");
	wprintw(mainWin, "e: edit current project\n");
	wprintw(mainWin, "2: set end date\n");
	wprintw(mainWin, "C: add comment\n");
	wprintw(mainWin, "d: set done/undone\n");
	wprintw(mainWin, "c: view comment\n");
	wprintw(mainWin, "g: show Gantt diagramm\n");
	wprintw(mainWin, "k: show Subtasks\n");
	wprintw(mainWin, "t: add new Task\n");
	wprintw(mainWin, "D: delete current row\n");
	wprintw(mainWin, "f: show File window\n");
	wprintw(mainWin, "s: save\n");
	wprintw(mainWin, "S: save on ftp-Server\n");
	wprintw(mainWin, "?: show this help\n\n\n");
	wprintw(mainWin, "Press any key to continue.");
	wrefresh(mainWin);
	wrefresh(headerWin);
	wgetch(mainWin);
}