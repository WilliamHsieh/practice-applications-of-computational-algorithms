#include <bits/stdc++.h>
#include "parser.h"
using namespace std;

auto sol(string cnf_fname, string sat_fname) {
	// read sat
	auto fsat = ifstream(sat_fname);
	auto trash = string{};
	fsat >> trash >> trash;
	if (trash != "SATISFIABLE") return "[UNSAT]";
	fsat >> trash;

	auto ans = vector(1, 0);
	for (int x; fsat >> x and x; ans.push_back(x));

	// read cnf
	auto num_vars = 0;
	auto clauses = vector<vector<int>>{};
	parse_DIMACS_CNF(clauses, num_vars, cnf_fname.data());
	assert(static_cast<size_t>(num_vars) == ans.size() - 1);

	for (auto &v : clauses) {
		bool ok = false;
		for (int x : v) if (ans[abs(x)] == x) {
			ok = true;
			break;
		}

		if (!ok) {
			return "[WRONG ANSWER]";
		}
	}
	return "[ACCEPTED]";
}

int main(int argc, char **argv) {
	if (argc < 2) {
		cout << "usage: ./checker [cnf_fname_prefix]\n";
		cout << "usage: ./checker [input.cnf] [input.sat]\n";
		return 0;
	}

	auto cnf_fname = string(argv[1]) + (argc == 2 ? ".cnf" : "");
	auto sat_fname = (argc == 2) ? string(argv[1]) + ".sat" : string(argv[2]);
	cout << sol(cnf_fname, sat_fname) << endl;

	return 0;
}
