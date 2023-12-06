#ifndef VIEW_H
#define VIEW_H
#include <ncurses.h>
#include <string>
#include "ProjectLibrary.h"
#include "Observer.h"
#include "Files.h"
#include "WorkItem.h"

class TableTask;
class TableSubtask;
class Controller;
class Displayable;
class Subtask;

class View : public Observer
{
private:
	WINDOW *pad;
	WINDOW *headerWin;
	WINDOW *menuBar;
	Controller *controller;
	std::string editor;

	void setInputMaske(std::string *input, std::string txt);
	ProjectLibrary *getProjectLibrary();
	void openVim(char *fileName);
	void readFileContent(char *fileName, std::string *fileContent);
	void writeCommentToFile(char *fileName, std::string *comment);
	int cols = 0;
	int lines = 0;
	WINDOW *mainWin;
	WINDOW *inputBar;
	void drawHLine(int row, const unsigned int GAP, int intersections[], chtype chars[], int n_cols);
	void drawVLines(int row, const unsigned int GAP, int intersections[], int n_cols);
	void setHeader(std::string headers[], int pos[], int length);
	void wrapText(const std::string &input, int maxLineWidth, std::vector<std::string> &lines);

public:
	View(Controller *controller);
	~View();

	void navigateTable(Displayable *d);
	void addFile(Files *f);
	void deleteFile(Files *f, int i);
	void openFile(Files *f, int i);
	void initScreen(ProjectLibrary *pl);
	void updateView(ProjectLibrary *pl) override;
	void deleteProject(ProjectLibrary *pl, unsigned int i);
	void infoBox(unsigned int h, unsigned int w, const char *txt, const char *title);
	void gannt(Project *p);
	void setDate(WorkItem *t, int type);
	void changeTaskPriority(Task *t);
	void changeSubtaskPriority(Subtask *s);
	void addComment(WorkItem *wi);
	void showComment(std::string comment);
	void editProject(Project *p);
	void editTask(Task *t);
	void editSubtask(Subtask *s);
	void deleteTask(Project *p, unsigned int i);
	void deleteSubtask(Task* t, unsigned int i);
	void createNewTask(Project *p);
	void createNewSubtask(Task* t);
	void brainstorming(Project *p, TableTask* t);
	void brainstorming(Task* t, TableSubtask* s);
	void createNewProject(ProjectLibrary *pl);
	void save(ProjectLibrary *pl);
	void saveOnFtp(ProjectLibrary *pl);
	void setRoot();
	void setEditor(std::string editor);
};

#endif
