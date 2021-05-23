#pragma once
#include "state.h"

struct State;

struct SatSolver {
	// constructor
	SatSolver(std::string&);

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
	friend std::ostream& operator<< (std::ostream&, SatSolver&);

	// member
	int num_vars;
	int num_clauses;
	std::vector<std::vector<int>> clauses;
	std::stack<State, std::vector<State>> stk;
	std::queue<int> prop;
	std::vector<int> decision_level;
	std::vector<int> antecedent;
	std::vector<int> timestamp;
};
