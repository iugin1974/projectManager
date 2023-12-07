#include "Controller.h"
#include "View.h"
#include "Controller.h"
#include "TableProject.h"
#include "TableTask.h"
#include "TableSubtask.h"
#include "TableFiles.h"
#include "Files.h"
#include "Gannt.h"
#include "Date.h"
#include "ProjectLibrary.h"
#include "WorkItem.h"
#include "Displayable.h"

#include <unistd.h>
#include <curses.h>
#include <iostream>
#include <fstream>
#include <cstring> // for strlen
#include <string>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <algorithm> // per std::min

/*
 * Vengono visualizzati i progetti con le task.
 * In questa schermata si può premere n e creare un
 * nuovo progetto.
 * Premendo ENTER su un progetto selezionato si
 * arriva su una seconda schermata, dove si possono
 * aggiungere o editare le task
 */

View::View(Controller *c)
{
    controller = c;
}

View::~View()
{
}

/*

+---------------------------------------
| menuBar
+---------------------------------------
| (vuoto)
+---------------------------------------
| headerWin
+---------------------------------------
| mainWin -> Table
|
|
|
|
+---------------------------------------
| inputBar
+---------------------------------------


*/
void View::initScreen(ProjectLibrary *pl)
{
    init_pair(5, COLOR_MAGENTA, COLOR_GREEN); // for test
    initscr();
    getmaxyx(stdscr, lines, cols);
    curs_set(0);
    menuBar = newwin(1, cols, 0, 0);
    mainWin = newpad(200, cols);
    inputBar = newwin(1, cols, lines - 1, 0);
    headerWin = newwin(1, cols, 2, 0);
    keypad(mainWin, TRUE);
    scrollok(mainWin, true);
    noecho();
    start_color();
    use_default_colors();
    init_pair(1, COLOR_YELLOW, -1);
    init_pair(2, COLOR_RED, -1);
    cbreak();
    navigateTable(pl);
    delwin(menuBar);
    delwin(mainWin);
    delwin(inputBar);
    delwin(headerWin);
    endwin();

}

void View::setEditor(std::string e) {
    editor = e;
}

void View::wrapText(const std::string &input, int maxLineWidth, std::vector<std::string> &lines)
{
    std::string line;
    lines.clear();

    for (char ch : input)
    {
        if (ch == ' ')
        {
            if (line.length() > 0)
            {
                if (line.length() + 1 > maxLineWidth)
                {
                    lines.push_back(line);
                    line.clear();
                }
                else
                {
                    line += ' ';
                }
            }
        }
        else if (ch == '\n' || ch == '\x0A')
        {
            if (line.length() > 0)
            {
                lines.push_back(line);
                line.clear();
            }
        }
        else
        {
            line += ch;
        }
    }

    if (line.length() > 0)
    {
        lines.push_back(line);
    }
}


void View::infoBox(unsigned int h, unsigned int w, const char *txt, const char *title)
{
    int l, c;
    getmaxyx(stdscr, l, c);
    WINDOW *win = newwin(h, w, (l - h) / 2, (c - w) / 2);
    keypad(win, TRUE); // Abilita la lettura di tasti speciali come le frecce
    scrollok(win, TRUE); // Abilita lo scrolling della finestra
    int maxLineWidth = w - 4;

    std::vector<std::string> lines;
    wrapText(txt, maxLineWidth, lines);

    int row = 1;
    int offset = 0;
    int ch;

    while (true)
    {
        wclear(win);
        box(win, 0, 0);
        wattron(win, A_BOLD);
        mvwprintw(win, 0, (w - strlen(title)) / 2, title);
        wattroff(win, A_BOLD);

        int visibleLines = std::min(static_cast<unsigned int>(h - 2), static_cast<unsigned int>(lines.size()));
        for (int i = 0; i < visibleLines; ++i)
        {
            mvwprintw(win, i + 1, 1, lines[offset + i].c_str());
        }

        wrefresh(win);

        ch = wgetch(win);

        switch (ch)
        {
        case KEY_DOWN:
            if (offset + visibleLines < lines.size())
                offset++;
            break;
        case KEY_UP:
            if (offset > 0)
                offset--;
            break;
        default: 
            wclear(win);
            wrefresh(win);
            delwin(win);
            return;
        }
    }
}

void View::navigateTable(Displayable *d)
{
    Table* table = d->getTable();
    table->setWindows(this, mainWin, menuBar, headerWin);
    table->initTable(lines, cols);
    table->navigate(static_cast<ProjectLibrary*>(d));
    delete table;
}

void View::gannt(Project *p)
{
    Gannt g;
    g.setWindows(this, mainWin, menuBar, headerWin);
    g.initTable(lines, cols);
    g.navigate(p);
}

void View::addFile(Files *f)
{
    std::string path = Files::getRoot();
    std::string input;
    setInputMaske(&input, "File: " + path);
    bool e = controller->fileExists(path + input);
    if (e)
    {
        controller->addFile(f, input);
    }
    else
    {
        infoBox(5, 50, "The file does not exist.", "Error");
    }
}

void View::deleteFile(Files *f, int i)
{
    controller->deleteFile(f, i);
}

void View::openFile(Files *f, int i)
{
    controller->openFile(f->at(i));
}

void View::readFileContent(char *fileName, std::string *fileContent)
{
    std::ifstream inFile;
    inFile.open(fileName);
    if (inFile.is_open())
    {
        while (inFile)
        {
            std::string line;
            if (std::getline(inFile, line))
            {
                *fileContent += line;
                // Aggiungi una nuova linea solo se non sei alla fine del file
                if (!inFile.eof())
                {
                    *fileContent += "\n";
                }
            }
        }
        inFile.close();
    }
    else
    {
        exit(1);
    }
}

void View::showComment(std::string comment) {
infoBox(10, 50, comment.c_str(), "Comment");

}

void View::writeCommentToFile(char *fileName, std::string *comment)
{
    std::ofstream outFile;
    outFile.open(fileName);
    outFile << *comment;
    outFile.close();
}

void View::addComment(WorkItem *wi)
{
    /* TODO
     * se l'utente chiude vim senza salvare
     * non deve cercare di leggere il file
     */
    char fileName[] = "/tmp/pm_XXXXXXXXXX";
    mkstemp(fileName);
    if (wi->hasComment())
    {
        std::string c = wi->getComment();
        writeCommentToFile(fileName, &c);
    }
    openVim(fileName);
    std::string fileContent;
    readFileContent(fileName, &fileContent);
    wi->addComment(fileContent);
}

/*
 * exec crea un nuovo processo e abbandona quello precedente.
 * Questo vuol dire che non è possibile tornare al processo precedente e continuare il programma.
 * Allora occorre chiamare fork
 * */
void View::openVim(char *fileName)
{
    pid_t pid = fork();
    if (pid < 0)
    {
        /* Error */
    }
    /* Il processo figlio */
    else if (pid == 0)
    {
        def_prog_mode();
        endwin();
        execl(editor.c_str(), editor.c_str(), fileName, NULL);
        exit(0);
    }
    /* Il processo padre */
    else
    {
        wait(NULL);
        reset_prog_mode();
        refresh();
    }
}

void View::setRoot()
{
    std::string input;
    setInputMaske(&input, "Root: ");
    bool ok = controller->setRoot(input);
    if (!ok)
    {
        infoBox(5, 50, "An error occurred", "Error");
    }
}

void View::deleteProject(ProjectLibrary *pl, unsigned int i)
{
    std::string input;
    setInputMaske(&input, "Are you sure? [Y/n]:");
    if (input != "Y")
        return;
    controller->deleteProject(pl, i);
}

void View::deleteTask(Project *p, unsigned int i)
{
    std::string input;
    setInputMaske(&input, "Are you sure? [Y/n]:");
    if (input != "Y")
        return;

    controller->deleteTask(p, i);
}

void View::deleteSubtask(Task* t, unsigned int i)
{
    std::string input;
    setInputMaske(&input, "Are you sure? [Y/n]:");
    if (input != "Y")
        return;

    controller->deleteSubtask(t, i);
}

void View::updateView(ProjectLibrary *pl)
{
    clear();
    // TODO   display(pl);
}

void View::setInputMaske(std::string *input, std::string txt)
{
    mvwprintw(inputBar, 0, 0, txt.c_str());
    wmove(inputBar, 0, strlen(txt.c_str()));
    wrefresh(inputBar);
    nocbreak();
    echo();
    char c[500];
    wgetnstr(inputBar, c, 499);
    *input = std::string(c);
    noecho();
    cbreak();
    wclear(inputBar);
    wrefresh(inputBar);
}

void View::createNewTask(Project *p)
{
    std::string input;
    setInputMaske(&input, "Task name: ");
    controller->createTask(p, &input);
}

void View::createNewSubtask(Task *t) {
    std::string input;
    setInputMaske(&input, "Subtask name: ");
    controller->createSubtask(t, &input);
}

void View::brainstorming(Project *p, TableTask* t)
{
    std::string input;
    while (1)
    {
        setInputMaske(&input, "Task name (type 'exit' to finish): ");
        if (input.empty() || input == "exit")
            break;
        controller->createTask(p, &input);
        t->display(p);
    }
}

void View::brainstorming(Task* t, TableSubtask* s)
{
    std::string input;
    while (1)
    {
        setInputMaske(&input, "Subtask name (type 'exit' to finish): ");
        if (input.empty() || input == "exit")
            break;
        controller->createSubtask(t, &input);
        s->display(t);
    }
}

/*
Per questo metodo occorre tenere conto che alla creazione della Task,
questa prenderà di default le date iniziali e finali del progetto a
cui appartiene.
*/
void View::setDate(WorkItem *t, int type)
{
    std::string input;
    setInputMaske(&input, "End date (dd/mm/yyyy): ");
    if (!Date::isValidDateFormat(&input) && !Date::isValidStringFormat(&input))
    {
        infoBox(5, 50, "Invalid date format.", "Error");
        return;
    }

    /*
    Crea una data vuota. Se l'utente scrive la data in formato dd/mm/yyyy,
    in questa viene inserita la data scritta dall'utente.
    Se l'utente scrive qualcosa del tipo +2d, qui viene inserita
    la data risultante (cioè la somma della data di partenza con l'aggiunta dell'utente)
    */
    tm tNewDate = {0};
    if (Date::isValidDateFormat(&input))
    {
        Date::convertDate(&input, &tNewDate);
    }
    else if (Date::isValidStringFormat(&input))
    {
        tNewDate = t->getDate(type);
        Date::parseDateString(&input, &tNewDate);
    }

    // controlla la validità della data
    WorkItem *p = t->getParent();
    int result = controller->checkDateValid(p, t, &tNewDate, type);

    switch (result)
    {
    case Controller::STARTS_BEFORE_PARENT:
        infoBox(5, 50, "The children starts before the parent.", "Error");
        return;
    case Controller::STARTS_AFTER_PARENT_END:
        infoBox(5, 50, "The children starts after the end of the parent.", "Error");
        return;
    case Controller::STARTS_AFTER_ENDING:
        infoBox(5, 50, "The starting date of the children is after the ending date.", "Error");
        return;
    case Controller::ENDS_AFTER_PARENT_END:
        infoBox(5, 50, "The end date of the children is after the end of the parent.", "Error");
        return;
    case Controller::ENDS_BEFORE_PARENT_START:
        infoBox(5, 50, "The end date is before the start of the parent.", "Error");
        return;
    case Controller::ENDS_BEFORE_START:
        infoBox(5, 50, "The final date is earlier than the initial date.", "Error");
        return;
    default:
        t->setDate(type, tNewDate);
        break;
    }
}

void View::changeTaskPriority(Task *t)
{
    controller->changeTaskPriority(t);
}

void View::editTask(Task *t)
{
    std::string input;
    setInputMaske(&input, "Task name [" + t->getText() + "]: ");
    if (!input.empty())
    {
        t->addText(input);
        input.clear();
    }
    setInputMaske(&input, "End date (dd/mm/yyyy) [" + t->getDateAsString(Project::END_DATE) + "]: ");
    if (!input.empty())
    {
        if (Date::isValidDateFormat(&input))
        {
            t->setDate(Project::END_DATE, input);
        }
        else
        {
            infoBox(5, 50, "Invalid date format.", "Error");
            t->setDate(Project::END_DATE, t->getDateAsString(Project::END_DATE));
        }
    }
    input.clear();
    setInputMaske(&input, std::string("Priority (l/h) [") + (t->getPriority() == Task::LOW ? "l" : "h") + "]: ");
    if (input.empty())
        return;
    if (input == "h")
        t->setPriority(Task::HIGH);
    else
        t->setPriority(Task::LOW);
}

// TODO
// check if date is early
void View::createNewProject(ProjectLibrary *pl)
{
    std::string input;
    setInputMaske(&input, "Project name: ");
    if (input.empty())
        return;
    Project *p = pl->getNewProject();
    p->addText(input);

    input.clear();

    setInputMaske(&input, "End date (dd/mm/yyyy): ");
    if (!Date::isValidDateFormat(&input))
    {
        infoBox(5, 50, "Invalid date format.", "Error");
        return;
    }
    p->setDate(Project::END_DATE, input);
    pl->addProject(p);
}

void View::editProject(Project *p)
{
    std::string input;
    setInputMaske(&input, "Project name [" + p->getText() + "]: ");
    if (!input.empty())
    {
        p->addText(input);
        input.clear();
    }

    setInputMaske(&input, "End date (dd/mm/yyyy) [" + p->getDateAsString(Project::END_DATE) + "]: ");
    if (input.empty())
        return;
    if (Date::isValidDateFormat(&input))
        p->setDate(Project::END_DATE, input);
    else
        infoBox(5, 50, "Invalid date format.", "Error");
}

void View::save(ProjectLibrary *pl)
{
    controller->save(pl);
}

void View::saveOnFtp(ProjectLibrary *pl)
{
    controller->saveOnFtp(pl);
}
