#pragma once
#include <bits/stdc++.h>

// #Declaration
struct State {
	State(int, int);

	std::optional<bool>& var(int);
	int pick_variable();
	void set_variable(int, std::queue<int>&);

	bool done;
	int num_vars;
	int num_clauses;
	std::vector<std::optional<bool>> guess; //decision of each variable
	std::vector<std::array<int, 2>> watch; //record the index of watched literal
};

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
void State::set_variable(int x, std::queue<int> &prop) {
	var(x) = (x > 0);
	prop.push(-x);
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

