// Observer.h
#ifndef OBSERVER_H
#define OBSERVER_H

class Observable;  // forward declaration, basta per un puntatore

class Observer {
public:
	virtual ~Observer() {}
	virtual void updateView(Observable *o) = 0;
};
#endif