#ifndef OBSERVER_H
#define OBSERVER_H

#include <vector>
#include "Project.h"

class Observer {
	public:
		virtual ~Observer() {};
		virtual void updateView (ProjectLibrary* pl) = 0;

};

#endif
