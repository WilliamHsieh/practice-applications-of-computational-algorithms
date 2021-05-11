#include <bits/stdc++.h>
#include "dpll.hpp"
#include "state.hpp"

// #Constructor
State::State(int num_vars_, int num_clauses_)
	: num_vars(num_vars_),
	  num_clauses(num_clauses_),
	  guess(num_vars + 1),
	  watch(num_clauses, {0, 1})
{
	guess[0] = true;
	done = false;
}

// #Get guessed variable
std::optional<bool>& State::var(int x) {
	return guess[abs(x)];
}

// #Pick a variable
int State::pick_variable() {
	static auto gen = std::default_random_engine(std::random_device{}());
	static auto dis = std::uniform_int_distribution<int>(1, num_vars);

//	for (int x = 1; x <= num_vars; x++) {
	for (int x = 0; ; x = dis(gen)) {
		if (!this->guess[x]) {
			return x;
		}
	}

	std::cerr << "shouldn't reach here!\n";
	exit(1);
}

// #Set a variable
void State::set_variable(int x, DPLL &super) {
	var(x) = (x > 0);
	super.prop.push(-x);
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
	for (int i = 0; i < state.num_clauses; i++) {
		auto &[la, lb] = state.watch[i];
		os << '[' << i << "] " << la << ' ' << lb << std::endl;
	}

	getchar();
	return os;
}

