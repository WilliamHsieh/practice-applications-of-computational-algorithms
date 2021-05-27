#include <bits/stdc++.h>
#include "state.h"

using namespace std;

// #Constructor
State::State(int num_vars, int num_clauses)
	: guess(num_vars + 1),
	  watch(num_clauses, {0, 1})
{}

// #Get guessed variable
std::optional<bool>& State::var(int x) {
	return guess[abs(x)];
}

// #operator<<
std::ostream& operator<< (std::ostream &os, State &state) {
	// show current variable
	int idx = 0;
	for (auto &x : state.guess) {
		os << "[" << idx++ << ": ";
		if (!x) {
			os << "no";
		} else {
			os << std::boolalpha << *x;
		}
		os << "] ";
	}
	os << std::endl;

	// show watched literal
	for (size_t i = 0; i < state.watch.size(); i++) {
		auto &[la, lb] = state.watch[i];
		os << '[' << i << "] " << la << ' ' << lb << std::endl;
	}

	getchar();
	return os;
}

