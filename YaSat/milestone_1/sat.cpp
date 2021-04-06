#include <iostream>
#include <vector>
#include "parser.h"

using namespace std;

// #Helper
std::ostream& operator<< (std::ostream& os, const std::vector<int>& v) {
	for (size_t i = 0, s = v.size(); i < s; i++) {
		if (i != 0) os << " ";
		os << v[i];
	}
	return os;
}

// #Sat_solver
void sat_solver(vector<vector<int>> &clauses, int num_vars, string_view in_name) {
	in_name.remove_suffix(in_name.size() - in_name.find(".cnf"));
	auto out_name = string(in_name) + ".sat";
	cout << out_name << endl;
}

// #Main
int main(int argc, char** argv) {
	if (argc != 2) {
		cout << "usage: ./yasat [input.cnf]" << endl;
		return 0;
	}

	int num_vars;
	auto clauses = std::vector<std::vector<int>>{};
	auto in_name = string_view(argv[1]);

	parse_DIMACS_CNF(clauses, num_vars, in_name.data());
	for (auto &v : clauses) {
		cout << v << endl;
	}

	sat_solver(clauses, num_vars, in_name);
	return 0;
}
