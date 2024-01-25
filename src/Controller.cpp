#include "Controller.h"
#include "View.h"
#include "Date.h"
#include "ProjectLibrary.h"
#include "tinyxml.h"
#include "ftp.h"
#include "FileUtilities.h"
#include "Subtask.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <locale.h>
#include <string>
#include <cstdlib>
#include <sys/stat.h>
#include <filesystem>
#include <map>

int main(int argc, char *argv[])
{
	bool use_ftp = false;
	if (argc == 2 && strcmp(argv[1], "--ftp") == 0)
	{
		use_ftp = true;
	}
	setlocale(LC_ALL, "de_DE");
	Controller c;
	c.set_useFtp(use_ftp);
	c.start();
	return 0;
}

void Controller::set_useFtp(bool f)
{
	use_ftp = f;
}

void Controller::start()
{
	if (use_ftp)
	{
		std::string user_ftp;
		std::string host_ftp;
		std::string pwd;
		bool found = false;
		found = readConfigFile("user_ftp", &user_ftp);
		if (!found)
		{
			std::cerr << "The .pmrc file was not found or the key 'user_ftp' does not exist.\nEdit the .pmrc file by adding the string 'user_ftp=<username>'" << std::endl;
			exit(1);
		}
		found = readConfigFile("host_ftp", &host_ftp);
		if (!found)
		{
			std::cerr << "The .pmrc file was not found or the key 'host_ftp' does not exist.\nEdit the .pmrc file by adding the string 'host_ftp=<hostname>'" << std::endl;
			exit(1);
		}

		found = readConfigFile("pass_ftp", &pwd);
		// controlla se il file di configurazione esiste
		// Se si, prende da lì le informazioni
		// Altrimenti chiede all'utente
		if (!found) {
		pwd = getpass("FTP-Password", true);
		}
		_ftp.setPassword(pwd);
		_ftp.setHost(host_ftp);
		_ftp.setUserName(user_ftp);
		int i = _ftp.downloadFile("pm.xml", "/tmp/pm.xml");
		if (i != 0)
		{
			std::cout << "Error " << i << std::endl;
			std::cout << "End" << std::endl;
			return;
		}
	}

	std::string rootContent;
	bool found = readConfigFile("root_file", &rootContent);
	if (found)
	{
		Files::setRoot(rootContent);
	}
	else
	{
		Files::setRoot(FileUtilities::home() + FileUtilities::separator());
	}
	std::string editor;
	readConfigFile("default_editor", &editor);
	View v(this);
	v.setEditor(editor);
	ProjectLibrary pl;
	load(&pl);
	pl.addObserver(&v);

	v.initScreen(&pl);
	/* if (pl->isChanged())*/
	if (use_ftp)
	{
		saveOnFtp(&pl);
		_ftp.free_memory();
	}
	else
	{
		save(&pl);
	}
}

bool Controller::readConfigFile(const std::string key, std::string *value)
{
	// Apri il file in lettura
	std::ifstream file(FileUtilities::configFile());

	// Verifica se il file è stato aperto correttamente
	if (!file.is_open())
	{
		std::cerr << "Error opening the file " << FileUtilities::configFile() << std::endl;
		return false;
	}

	// Mappa per memorizzare le coppie chiave-valore
	std::map<std::string, std::string> configMap;

	// Leggi il file linea per linea
	std::string line;
	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		std::string fileKey, fileValue;

		// Usa un separatore (qui "=") per dividere la chiave e il valore
		if (std::getline(iss, fileKey, '=') && std::getline(iss, fileValue))
		{
			// Aggiungi la coppia chiave-valore alla mappa
			configMap[fileKey] = fileValue;
		}
	}

	// Chiudi il file
	file.close();
	// Cerca la chiave nella mappa
	auto it = configMap.find(key);
	if (it != configMap.end())
	{
		// Se la chiave è presente, restituisci il valore associato
		*value = it->second;
		return true;
	}
	else
	{
		// Se la chiave non è presente, restituisci false
		return false;
	}
}

void Controller::load(ProjectLibrary *pl)
{
	TiXmlDocument doc;
	if (use_ftp)
		doc.LoadFile("/tmp/pm.xml");
	else
		doc.LoadFile("/home/eugenio/pm.xml");
	bool loaded = doc.LoadFile();
	if (!loaded)
		return;
	TiXmlHandle docHandle(&doc);
	TiXmlElement *project = docHandle.FirstChild("ProjectLibrary").Child("Project", 0).ToElement();
	Project *p = NULL;
	while (project)
	{
		p = pl->getNewProject();
		const char *attr = project->Attribute("name");
		p->addText(attr);
		// TODO controlla che la data sia valida.
		// Se si assegnala, altrimenti no e setta
		// data non valida (se necessario)
		// La stessa cosa per le task

		/* tinyXML ritorna NULL se l'attributo non esiste */
		if (project->Attribute("starts"))
		{
			p->setDate(Project::START_DATE, project->Attribute("starts"));
		}

		if (project->Attribute("ends"))
			p->setDate(Project::END_DATE, project->Attribute("ends"));

		if (project->Attribute("starts"))
			p->setDate(WorkItem::START_DATE, project->Attribute("starts"));

		if (attributeExists("comment", project))
			p->addComment(project->Attribute("comment"));

		TiXmlElement *task = project->FirstChildElement("Task");
		loadTask(task, p);

		TiXmlElement *file = project->FirstChildElement("File");
		Files *f = p->getFileList();
		while (file)
		{
			const char *entry = file->Attribute("path");
			f->add(entry);
			file = file->NextSiblingElement("File");
		}

		pl->addProject(p);
		project = project->NextSiblingElement("Project");
	}
}

void Controller::loadTask(TiXmlElement *task, Project *p)
{
	while (task)
	{
		Task *t = p->getNewTask(p);
		const char *tsk = task->Attribute("text");
		t->addText(tsk);

		if (task->Attribute("ends"))
		{
			const char *date = task->Attribute("ends");
			t->setDate(Project::END_DATE, date);
		}

		if (task->Attribute("starts"))
		{
			const char *date = task->Attribute("starts");
			t->setDate(WorkItem::START_DATE, date);
		}

		if (attributeExists("priority", task))
		{
			const char *pr = task->Attribute("priority");
			t->setPriority(atoi(pr));
		}
		else
			t->setPriority(0);

		if (attributeExists("done", task))
		{
			const char *dn = task->Attribute("done");
			if (strcmp(dn, "1") == 0)
			{
				t->setDone(true);
			}
			else
			{
				t->setDone(false);
			}
			if (attributeExists("comment", task))
			{
				t->addComment(task->Attribute("comment"));
			}
		}

		TiXmlElement *subtask = task->FirstChildElement("Subtask");

		loadSubtask(subtask, t);
		task = task->NextSiblingElement("Task");
	}
}

void Controller::loadSubtask(TiXmlElement *subtask, Task *t)
{
	while (subtask)
	{
		Subtask *s = t->getNewSubtask();
		const char *sbtsk = subtask->Attribute("text");
		s->addText(sbtsk);

		if (subtask->Attribute("ends"))
		{
			const char *date = subtask->Attribute("ends");
			s->setDate(Project::END_DATE, date);
		}

		if (subtask->Attribute("starts"))
		{
			const char *date = subtask->Attribute("starts");
			s->setDate(WorkItem::START_DATE, date);
		}

		if (attributeExists("priority", subtask))
		{
			const char *pr = subtask->Attribute("priority");
			s->setPriority(atoi(pr));
		}
		else
			s->setPriority(0);
		if (attributeExists("done", subtask))
		{
			const char *dn = subtask->Attribute("done");
			if (strcmp(dn, "1") == 0)
			{
				s->setDone(true);
			}
			else
			{
				s->setDone(false);
			}
		}
		if (attributeExists("comment", subtask))
		{
			s->addComment(subtask->Attribute("comment"));
		}
		subtask = subtask->NextSiblingElement("Subtask");
	}
}

void Controller::save(ProjectLibrary *pl)
{
	TiXmlDocument doc;
	TiXmlDeclaration *decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);
	TiXmlElement *root = new TiXmlElement("ProjectLibrary");
	doc.LinkEndChild(root);
	for (unsigned int j = 0; j < pl->size(); j++)
	{
		Project *p = pl->getProject(j);
		TiXmlElement *project = new TiXmlElement("Project");
		root->LinkEndChild(project);
		project->SetAttribute("name", p->getText().c_str());
		project->SetAttribute("starts", p->getDateAsString(Project::START_DATE).c_str());
		if (p->dateIsValid())
			project->SetAttribute("ends", p->getDateAsString(Project::END_DATE).c_str());

		if (p->hasComment())
		{
			project->SetAttribute("comment", p->getComment().c_str());
		}

		for (unsigned int i = 0; i < p->size(); i++)
		{
			Task *t = p->getTask(i);
			saveTask(project, t);
		}

		Files *f = p->getFileList();
		for (int i = 0; i < f->size(); i++)
		{
			TiXmlElement *file = new TiXmlElement("File");
			project->LinkEndChild(file);
			file->SetAttribute("path", f->at(i).c_str());
		}
	}
	doc.SaveFile(FileUtilities::pmFile().c_str());
	pl->setChanged(false);
}

void Controller::saveTask(TiXmlElement *parent, Task *t)
{
	TiXmlElement *task = new TiXmlElement("Task");
	parent->LinkEndChild(task);
	task->SetAttribute("text", t->getText().c_str());
	task->SetAttribute("starts", t->getDateAsString(WorkItem::START_DATE).c_str());
	task->SetAttribute("ends", t->getDateAsString(Project::END_DATE).c_str());
	if (t->isDone())
		task->SetAttribute("done", t->isDone());
	if (t->getPriority() != 0)
		task->SetAttribute("priority", t->getPriority());
	if (t->hasComment())
	{
		task->SetAttribute("comment", t->getComment().c_str());
	}
	for (unsigned int i = 0; i < t->size(); i++)
	{
		saveSubtask(task, t->getSubtask(i));
	}
}

void Controller::saveSubtask(TiXmlElement *parent, Subtask *s)
{
	TiXmlElement *task = new TiXmlElement("Subtask");
	parent->LinkEndChild(task);
	task->SetAttribute("text", s->getText().c_str());
	task->SetAttribute("starts", s->getDateAsString(WorkItem::START_DATE).c_str());
	task->SetAttribute("ends", s->getDateAsString(Project::END_DATE).c_str());
	if (s->isDone())
		task->SetAttribute("done", s->isDone());
	if (s->getPriority() != 0)
		task->SetAttribute("priority", s->getPriority());
	if (s->hasComment())
	{
		task->SetAttribute("comment", s->getComment().c_str());
	}
}
bool Controller::attributeExists(const char *name, TiXmlElement *element)
{
	if (element->Attribute(name) != NULL)
		return true;
	return false;
}

void Controller::saveOnFtp(ProjectLibrary *pl)
{
	save(pl);
	_ftp.uploadFile("/home/eugenio/pm.xml");
}

bool Controller::ftpPasswordSet() {
	return _ftp.isPasswordSet();
}

int Controller::get_char()
{
	int ch;
	struct termios t_old, t_new;

	tcgetattr(STDIN_FILENO, &t_old);
	t_new = t_old;
	t_new.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &t_new);

	ch = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &t_old);
	return ch;
}

std::string Controller::getpass(const char *prompt, bool show_asterisk = true)
{
	const char BACKSPACE = 127;
	const char RETURN = 10;

	std::string password;
	unsigned char ch = 0;

	std::cout << prompt << std::endl;

	while ((ch = get_char()) != RETURN)
	{
		if (ch == BACKSPACE)
		{
			if (password.length() != 0)
			{
				if (show_asterisk)
					std::cout << "\b \b";
				password.resize(password.length() - 1);
			}
		}
		else
		{
			password += ch;
			if (show_asterisk)
				std::cout << '*';
		}
	}
	std::cout << std::endl;
	return password;
}

void Controller::addFile(Files *f, std::string input)
{
	f->add(input);
}

bool Controller::fileExists(std::string path)
{
	std::ifstream file(path);
	return file.good();
}

void Controller::openFile(std::string file)
{
	std::string command = "xdg-open " + Files::getRoot() + file;
	system(command.c_str());
}

void Controller::deleteFile(Files *f, int i)
{
	f->remove(i);
}

bool Controller::setRoot(std::string txt)
{
	if (!fileExists(txt))
		return false;
	std::string path = std::string(getenv("HOME")) + "/.projectmanager/root";
	bool ok = writeOnFile(path, txt);
	if (ok)
	{
		Files::setRoot(txt);
		return true;
	}
	else
	{
		return false;
	}
}

bool Controller::writeOnFile(std::string path, std::string txt)
{
	std::ofstream of(path);
	if (of.is_open())
	{
		of << txt;
		of.close();
		return true;
	}
	else
	{
		std::cerr << "Impossibile aprire il file per la scrittura." << std::endl;
		return false;
	}
}

void Controller::changeTaskPriority(Task *t)
{
	if (t->getPriority() == Task::LOW)
		t->setPriority(Task::HIGH);
	else
		t->setPriority(Task::LOW);
}

void Controller::deleteProject(ProjectLibrary *pl, unsigned int i)
{
	pl->removeProject(i);
}

void Controller::deleteTask(Project *p, unsigned int i)
{
	p->removeTask(i);
}

void Controller::deleteSubtask(Task *t, unsigned int i)
{
	t->removeSubtask(i);
}

bool Controller::createTask(Project *p, std::string *text)
{
	Task *t = p->getNewTask(p);
	t->addText(*text);
	return true;
}

bool Controller::createSubtask(Task *t, std::string *text)
{
	Subtask *s = t->getNewSubtask();
	s->addText(*text);
	return true;
}

int Controller::checkDateValid(WorkItem *p, WorkItem *t, tm *newDate, int type)
{
	tm taskStartDate = t->getDate(WorkItem::START_DATE);
	tm taskEndDate = t->getDate(WorkItem::END_DATE);
	tm pStartDate = p->getDate(WorkItem::START_DATE);
	tm pEndDate = p->getDate(WorkItem::END_DATE);

	if (type == WorkItem::START_DATE)
	{
		if (Date::isEarlier(newDate, &pStartDate))
			return STARTS_BEFORE_PARENT;
		if (Date::isEarlier(&pEndDate, newDate))
			return STARTS_AFTER_PARENT_END;
		if (Date::isEarlier(&taskEndDate, newDate))
			return STARTS_AFTER_ENDING;
		return DATE_VALID;
	}

	if (type == WorkItem::END_DATE)
	{
		if (Date::isEarlier(&pEndDate, newDate))
			return ENDS_AFTER_PARENT_END;
		if (Date::isEarlier(newDate, &pStartDate))
			return ENDS_BEFORE_PARENT_START;
		if (Date::isEarlier(newDate, &taskStartDate))
			return STARTS_AFTER_ENDING;
		return DATE_VALID;
	}
	return -1; // solo affinché il compilatore non dia un warning
}