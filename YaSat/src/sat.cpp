#include <bits/stdc++.h>
#include "parser.h"
#include "dpll.hpp"

using namespace std;

int main(int argc, char** argv) {
	if (argc < 2) {
		cout << "usage: ./yasat input.cnf [output.sat]" << endl;
		return 0;
	}

	// parse DIMACS cnf
	auto num_vars = 0;
	auto clauses = vector<vector<int>>{};
	auto fin_name = string(argv[1]);
	parse_DIMACS_CNF(clauses, num_vars, fin_name.data());

	// solve
	auto fout_name = fin_name.substr(0, fin_name.size() - 3) + "sat";
	if (argc == 3) fout_name = argv[2];
	auto fout = fstream(fout_name.data(), ios::out | ios::trunc);

	if (auto res = DPLL(num_vars, clauses).solve(); res) {
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
