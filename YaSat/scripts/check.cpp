#include <bits/stdc++.h>
using namespace std;

auto sol(fstream &fcnf, fstream &fsat) {
	// read sat
	auto trash = string{};
	fsat >> trash >> trash;
	if (trash != "SATISFIABLE") return "[UNSAT]";
	fsat >> trash;

	auto ans = vector(1, 0);
	for (int x; fsat >> x and x; ans.push_back(x));

	// read cnf
	int num_vars, num_clauses;
	fcnf >> trash >> trash >> num_vars >> num_clauses;
	assert(static_cast<size_t>(num_vars) == ans.size() - 1);
	while (num_clauses--) {
		bool ok = false;
		for (int x; fcnf >> x and x; ) if (ans[abs(x)] == x) {
			ok = true;
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
	auto fcnf = fstream(cnf_fname, ios::in);
	auto fsat = fstream(sat_fname, ios::in);
	cout << sol(fcnf, fsat) << endl;

	fsat.close();
	fcnf.close();
	return 0;
}
