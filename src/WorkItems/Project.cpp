#include <vector>
#include "Task.h"
#include "Project.h"
#include <ctime>
#include <string>
#include "ProjectLibrary.h"
#include <algorithm>
#include <random>
#include "Files.h"
#include "TableTask.h"

Project::Project()
{
	files = new Files();
}

Project::~Project()
{
	for (unsigned int i = 0; i < taskList.size(); i++)
	{
		delete taskList.at(i);
	}
	delete files;
}

void Project::setProjectLibrary(ProjectLibrary *_pl)
{
	pl = _pl;
}

void Project::addFile(std::string f)
{
	listFiles.push_back(f);
}

Files *Project::getFileList()
{
	return files;
}

bool Project::hasFiles()
{
	return listFiles.size() > 0;
}

/*
 * Crea una Task vuota e ritorna un puntatore
 * alla Task stessa */
void Project::addTask(Task *t)
{
	taskList.push_back(t);
	done = false; // una nuova task non può essere done
	update();
}

Task *Project::getNewTask(Project* p)
{
	Task *t = new Task(p);
	return t;
}

Task *Project::getTask(unsigned int i)
{
	return taskList.at(i);
}

std::vector<Task *> Project::getTaskList()
{
	return taskList;
}

void Project::setDone(bool d)
{
	done = d;
	update();
}

bool Project::isDone()
{
	return done;
}

void Project::checkAndUpdateDoneStatus()
{
	done = false;
	// Verifica se tutte le Task sono done, escludendo la chiamante (callerTask)
	for (Task *t : taskList)
	{
		if (!t->isDone()) {
			return;
		}
	}
	// Aggiorna lo stato "done" del Project ma non posso chiamare
	// setDone(true) altrimenti inizia un ciclo infinito
	done = true;
}

unsigned int Project::getPercent()
{
	// poiché l'utente può settare un progetto a done
	// (un progetto senza task), viene effettuato un controllo:
	// se il progetto è done, allora percent sarà = 100.
	// Altrimenti viene calcolato
	if (done) {
		return 100;
	}
	// se non vi sono task e il progetto non è done,
	// allora percent sarà 0.
	int taskNumber = taskList.size();
	if (!done && taskNumber == 0) {
		return 0;
	}
	// da qui calcola la percentuale
	int taskDone = 0;
	for (Task *t : taskList)
	{
		if (t->isDone())
			taskDone++;
	}
	return taskDone * 100 / taskNumber;
}

unsigned int Project::size()
{
	return taskList.size();
}

void Project::removeTask(unsigned int i)
{
	Task *t = taskList.at(i);
	delete t;
	taskList.erase(taskList.begin() + i);
	update();
}

void Project::update()
{
	sort(taskList.begin(), taskList.end(), WorkItem::sortVector);
	pl->stateChanged();
}

Table *Project::getTable()
{
	return new TableTask();
}

std::string Project::getFormattedInfo()
{
	return std::string((isDone() == true) ? "d" : "-") +
		   std::string((hasComment()) ? "c" : "-");
}