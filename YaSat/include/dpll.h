#pragma once
#include "state.h"

struct State;

struct DPLL {
	// constructor
	DPLL(int, std::vector<std::vector<int>>&);

	// function
	void init();
	void watch_not_false(int&, int&, int, int);
	bool watch_is_true(int, int);
	bool unit_propagate();
	bool backtrack();
	std::optional<std::vector<int>> solve();

	// member
	int num_vars;
	int num_clauses;
	std::queue<int> prop;
	std::stack<int> branch;
	std::stack<State, std::vector<State>> call_stack;
	std::vector<std::vector<int>> &clauses;

	// which decision level
	std::vector<int> decided_level; //TODO: should I backtrack this?

	// which clause imply the decision
	std::vector<int> antecedent; //TODO: should I backtrack this?
};
