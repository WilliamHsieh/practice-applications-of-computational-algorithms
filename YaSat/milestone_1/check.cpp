#include <bits/stdc++.h>
using namespace std;

auto sol(fstream &fsat, fstream &fcnf) {
	auto trash = string{};
	int num_vars, num_clauses;

	// read sat
	fsat >> trash >> trash;
	if (trash != "SATISFIABLE") return "[UNSAT]";
	fsat >> trash;

	auto ans = vector(1, 0);
	for (int x; fsat >> x and x; ans.push_back(x));

	// read cnf
	fcnf >> trash >> trash >> num_vars >> num_clauses;
	assert((size_t)num_vars == ans.size() - 1);
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
	if (argc != 2) {
		cout << "usage: ./checker [cnf_fname_prefix]\n";
		return 0;
	}

	auto fsat = fstream(string(argv[1]) + ".sat", ios::in);
	auto fcnf = fstream(string(argv[1]) + ".cnf", ios::in);
	cout << sol(fsat, fcnf) << endl;

	fsat.close();
	fcnf.close();
	return 0;
}
