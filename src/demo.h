#ifndef DEMO_H_
#define DEMO_H_

#include "message.h"
#include "routing.h"
#include "routingalgo.h"

class Demo;

// this is just an abstract class of the design pattern state
class State{
	friend class Demo;
	virtual void handle(Demo&) = 0;
public:
	State() {}
	~State() {}
};

// this is the state of entrance interface
// let user to define the size of the map
class StateEntrance : public State{
	friend class Demo;
	void handle(Demo&);
public:
	StateEntrance() : State() {}
	~StateEntrance() {}

};

// this is the state of main interface
// user can view the map, save the map, edit the map or let the algorithm calculate the answer
class StateMain : public State{
	friend class Demo;
	void handle(Demo&);
public:
	StateMain() : State() {}
	~StateMain() {}
};

// this is the state of edit interface
// user can edit the map by
// + move the cursor by w/a/s/d
// + enter '1' - '9' to add terminals
// + enter 'o' to add obstacles
class StateEdit : public State{
	friend class Demo;
	void handle(Demo&);
public:
	StateEdit() : State() {}
	~StateEdit() {}
};

// this is the state of save interface
// user can enter the filename of the map
class StateSave : public State{
	friend class Demo;
	void handle(Demo&);
public:
	StateSave() : State() {}
	~StateSave() {}
};

// this is the state of display interface
// user can view the answer and see how droplets path to get further understanding of the idea of droplet
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