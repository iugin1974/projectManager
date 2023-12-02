#ifndef OBSERVABLE_H
#define OBSERVABLE_H
#include <vector>
#include "Observer.h"

class Observable {

	public:
		virtual ~Observable() {};
		virtual void addObserver (Observer* o) = 0;
		virtual void removeObserver (Observer* o) = 0;
		virtual void stateChanged () = 0;
};

#endif
