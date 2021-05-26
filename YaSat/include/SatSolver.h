#pragma once
#include "state.h"

struct State;

struct result_type : std::vector<int> {
	result_type() = default;
	result_type(int n) : std::vector<int>(n) {}

	friend std::ostream& operator<< (std::ostream&, result_type&);
};

struct SatSolver {
	// constructor
	SatSolver(std::string&);

	// function
	void init();
	void watch_not_false(int&, int&, int);
	bool watch_is_true(int, int);
	bool unit_propagate(int, bool&);
	int bcp();
	bool backtrack();
	result_type solve();

	void resolve(std::vector<int>&, int);
	std::vector<int> FirstUIP(int);
	void conflict_learning(int);
	void update();
	void set_variable(int, int cid = -1);
	int pick_variable();

	// member
	bool done;
	int time;
	int num_vars;
	int num_clauses;
	int orig_clauses;
	int max_clauses;
	std::vector<std::vector<int>> clauses;
	std::stack<State, std::vector<State>> stk;
	std::vector<int> decision_level;
	std::vector<int> antecedent;
	std::vector<int> timestamp;
	std::vector<std::array<double, 2>> VSIDS;
};
