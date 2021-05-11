#pragma once
#include "dpll.h"

struct DPLL;

struct State {
	// constructor
	State(int, int);

	// function
	std::optional<bool>& var(int);
	int pick_variable();
	void set_variable(int, DPLL&);

	// member
	bool done;
	int num_vars;
	int num_clauses;
	std::vector<std::optional<bool>> guess; //decision of each variable
	std::vector<std::array<int, 2>> watch; //record the index of watched literal
};
