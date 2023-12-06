#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "ftp.h"
#include "View.h"
#include "ProjectLibrary.h"
#include "Subtask.h"
#include "tinyxml.h"
#include <string>

class Controller
{
private:
	ProjectLibrary pl;
	bool use_ftp = false;
	std::string rootContent = "";
	ftp _ftp;
	std::string password;
	std::string getpass(const char *prompt, bool show_asterisk);
	int get_char();
	void load(ProjectLibrary *pl);
	bool attributeExists(const char *name, TiXmlElement *element);
	bool readConfigFile(const std::string key, std::string *value);
	void saveTask(TiXmlElement* parent, Task* t);
	void saveSubtask(TiXmlElement* parent, Subtask* s);

public:
	enum errorCode
	{
		DATE_VALID,
		STARTS_BEFORE_PARENT,
    STARTS_AFTER_PARENT_END,
    STARTS_AFTER_ENDING,
    ENDS_AFTER_PARENT_END,
    ENDS_BEFORE_PARENT_START,
    ENDS_BEFORE_START
	};
	void save(ProjectLibrary *pl);
	void saveOnFtp(ProjectLibrary *pl);
	void start();
	int main();
	void set_useFtp(bool f);
	void addFile(Files *f, std::string path);
	bool fileExists(std::string path);
	void deleteFile(Files *f, int i);
	void openFile(std::string file);
	bool setRoot(std::string txt);
	bool writeOnFile(std::string path, std::string txt);
	void changeTaskPriority(Task *t);
	void changeSubtaskPriority(Subtask* s);
	void deleteProject(ProjectLibrary *pl, unsigned int i);
	void deleteTask(Project *p, unsigned int i);
	void deleteSubtask(Task* t, unsigned int i);
	bool createTask(Project *p, std::string *text);
	bool createSubtask(Task* t, std::string *text);
	int checkDateValid(WorkItem* parent, WorkItem* children, tm *newDate, int type);
};

#endif
