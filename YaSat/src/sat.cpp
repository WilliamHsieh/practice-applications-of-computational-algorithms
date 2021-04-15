#include <bits/stdc++.h>
#include "parser.h"
#include "dpll.hpp"

using namespace std;

int main(int argc, char** argv) {
	if (argc != 2) {
		cout << "usage: ./yasat [input.cnf]" << endl;
		return 0;
	}

	// parse DIMACS cnf
	auto solver = DPLL();
	auto fin_name = string(argv[1]);

	int num_vars = 0;
	parse_DIMACS_CNF(solver.clauses, num_vars, fin_name.data());

	int num_clauses = solver.clauses.size();
	solver.init(num_vars, num_clauses);

	// solve
	auto fout_name = fin_name.substr(0, fin_name.size() - 3) + "sat";
	auto fout = fstream(fout_name.data(), ios::out | ios::trunc);
	if (auto res = solver.solve(); res) {
		cout << "[SAT]" << endl;
		fout << "s SATISFIABLE\n";
		fout << "v";
		for (auto x : *res) fout << " " << x;
		fout << " 0";
	} else {
		cout << "[UNSAT]" << endl;
		fout << "s UNSATISFIABLE\n";
	}

	return 0;
}
