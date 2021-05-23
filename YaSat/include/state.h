#pragma once
#include "dpll.h"

struct State {
	// constructor
	State(int, int);

	// function
	std::optional<bool>& var(int);
	int pick_variable();
	friend std::ostream& operator<< (std::ostream&, State&);

	// member
	bool done;
	int num_vars;
	int num_clauses;
	int time;
	std::vector<std::optional<bool>> guess; //decision of each variable
	std::vector<std::array<int, 2>> watch; //record the index of watched literal
};
