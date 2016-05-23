#ifndef DEMO_H_
#define DEMO_H_

#include "message.h"
#include "routing.h"
#include "routingalgo.h"

class Demo;

class State{
	friend class Demo;
	virtual void handle(Demo&) = 0;
public:
	State() {}
	~State() {}
};

class StateEntrance : public State{
	friend class Demo;
	void handle(Demo&);
public:
	StateEntrance() : State() {}
	~StateEntrance() {}

};

class StateMain : public State{
	friend class Demo;
	void handle(Demo&);
public:
	StateMain() : State() {}
	~StateMain() {}
};

class StateEdit : public State{
	friend class Demo;
	void handle(Demo&);
public:
	StateEdit() : State() {}
	~StateEdit() {}
};

class StateSave : public State{
	friend class Demo;
	void handle(Demo&);
public:
	StateSave() : State() {}
	~StateSave() {}
};

class StateDisplay : public State{
	friend class Demo;
	void handle(Demo&);
public:
	StateDisplay() : State() {}
	~StateDisplay() {}
};

class Demo{
	State *state;
	RoutingSatAlgoPointPrune algo;
	RoutingSolver solver;
	int width, height;
	int ask_exit;
	int nx, ny;
	message ans;
	std::string filename, error;
	std::vector<std::vector<int> > board;
	friend class State;
	friend class StateEntrance;
	friend class StateMain;
	friend class StateEdit;
	friend class StateSave;
	friend class StateDisplay;
public:
	Demo() : algo(), solver(&algo), ans(){
		state = new StateEntrance();
		width = 0;
		height = 0;
		ask_exit = 0;
		filename = "";
	}
	int handle(){
		state->handle(*this);
		return ask_exit;
	}
};

#endif