#pragma once
#include "state.h"

struct State;

// TODO: rename to SatSolver
struct DPLL {
	// constructor
	DPLL(int, std::vector<std::vector<int>>&);

	// function
	void init();
	void watch_not_false(int&, int&, int, int);
	bool watch_is_true(int, int);
	bool unit_propagate(int);
	int bcp();
	bool backtrack();
	std::optional<std::vector<int>> solve();

	void resolve(std::vector<int>&, int);
	std::vector<int> FirstUIP(int);
	void conflict_learning(int);
	void update();
	void set_variable(int, int cid = -1);

	// member
	int num_vars;
	int num_clauses;
	std::queue<int> prop;
	std::stack<State, std::vector<State>> stk;
	std::vector<int> decision_level;
	std::vector<int> antecedent;
	std::vector<int> timestamp;
	std::vector<std::vector<int>> &clauses;
};
