#ifndef TABLE_H
#define TABLE_H

#include <curses.h>

#include "Displayable.h"
#include "View.h"

class Table
{

protected:
	const int GAP = 5; // lo spazio a sinistra della tabella. Sposta tutta la tabella a dx del suo valore
	const int LEFT_ALIGN = 0;
	const int CENTER_ALIGN = 1;
	int cols = 0;
	int lines = 0;
	int tableH = 0;
	WINDOW *headerWin = NULL;
	WINDOW *mainWin = NULL;
	WINDOW *menuBar = NULL;
	View *view;
	void setTextMenuBar(std::string txt);
	void drawHLine(WINDOW *win, int line);
	void drawCol(WINDOW *win, int colW[], int arrayLength, int line, chtype ch);
	void printHeader(std::string text[], int colW[], int arrayLength, int alignement, int line);
	void setText(WINDOW *win, int colW[], int line, int col, std::string text);
	int getCurrentLine(ProjectLibrary *pl, unsigned int currentProject);
	virtual void display(Displayable *d){};
public:
	Table();
	virtual ~Table();
	void setWindows(View *w, WINDOW *mainWin, WINDOW *menuBar, WINDOW *headerWin);
	void initTable(int lines, int columns);
	virtual void navigate(Displayable *d){};
	virtual void highlight(Displayable *d, int i){};

};
#endif