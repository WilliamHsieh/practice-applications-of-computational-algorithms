#pragma once
#include <bits/stdc++.h>
#include "state.hpp"
#define all(x) begin(x),end(x)
#define what_is(x) std::cout << "[what] " << #x << " is " << x << std::endl
#define exec(x) std::cout << "[exec] " << #x << std::endl; x

// #Declaration
struct DPLL {
	DPLL(int _num_vars, vector<vector<int>> &_clauses)
		: num_vars(_num_vars), num_clauses(_clauses.size()), clauses(_clauses) {}
	void init();
	void watch_not_false(int&, int&, int, int);
	bool watch_is_true(int, int);
	bool unit_propagate();
	bool backtrack();
	std::optional<std::vector<int>> solve();

	int num_vars;
	int num_clauses;
	std::queue<int> prop;
	std::stack<int> branch;
	std::stack<State, std::vector<State>> call_stack;
	std::vector<std::vector<int>> &clauses;
};

// #Initialize
void DPLL::init() {
	assert(clauses.size() != 0);
#ifdef DEBUG
	for (auto &v : clauses) {
		for (auto x : v) std::cout << x << ' '; std::cout << std::endl;
	}
#endif
	// remove both -x or x exist clause && make literal unique
	// TODO: should be written more efficient
	for (auto &v : clauses) {
		auto s = std::set<int>(all(v));
		int idx = 0, ok = true;
		for (auto x : s) {
			if (s.count(-x)) {
				ok = false;
				break;
			}
			v[idx++] = x;
		}
		if (!ok) {
			v = vector(1, 0);
		} else {
			v.resize(s.size());
		}
	}

	// TODO: should be deleted after implementing pick_variable
	std::sort(all(clauses), [](const std::vector<int> &v1, const std::vector<int> &v2) {
		auto x1 = (v1.size() == 1 ? LLONG_MAX : v1.size());
		auto x2 = (v2.size() == 1 ? LLONG_MAX : v2.size());
		return x1 < x2;
//TODO: why wrong?
//		if (v1.size() == 1 and v2.size() == 1) return true;
//		if (v2.size() == 1) return true;
//		if (v1.size() == 1) return false;
//		return v1.size() < v2.size();
	});

	// assign unit clauses as decision
	auto &state = call_stack.emplace(num_vars, num_clauses);
	for (size_t i = 0; i < clauses.size(); i++) {
		if (clauses[i].size() == 1) {
			state.watch[i] = {-1, -1};
			state.set_variable(clauses[i][0], prop);
		}
	}
}

// #Watch not false
void DPLL::watch_not_false(int &self, int &other, int i, int false_literal) {
	// already watching at not false literal
	if (clauses[i][self] != false_literal) return;

	for (size_t j = std::max(self, other) + 1; j < clauses[i].size(); j++) {
		auto &var = call_stack.top().var(clauses[i][j]);
		if (!var.has_value()) {
			// not false
			self = j;
			return;
		} else if (*var == (clauses[i][j] > 0)) {
			// true
			self = other = -1;
			return;
		}
	}
	self = -1;
}

// #Watch is true
bool DPLL::watch_is_true(int watched, int i) {
	auto &var = call_stack.top().var(clauses[i][watched]);
	if (var.has_value() and *var == (clauses[i][watched] > 0)) {
		return true;
	}
	return false;
}

// #Unit propagate
bool DPLL::unit_propagate() {
	auto &state = call_stack.top();
	while (!prop.empty()) {
		int false_literal = prop.front(); prop.pop();
//		what_is(false_literal);

		state.done = true;
		for (int i = 0; i < num_clauses; i++) {
			if (clauses[i].size() == 1) break;

			// this clause is satisfied
			auto &[la, lb] = state.watch[i];
			if (la == -1 and lb == -1) continue;

			// one of the watched literal is already true
			if (watch_is_true(la, i) or watch_is_true(lb, i)) {
				la = lb = -1;
				continue;
			}

			// update watching literals that are not false
			watch_not_false(la, lb, i, false_literal);
			watch_not_false(lb, la, i, false_literal);

			// check if this became unit clause (implication)
			if (la == -1) std::swap(la, lb);
			if (la != -1 and lb == -1) {
				int tmp = clauses[i][la];
				if (state.var(tmp) and *state.var(tmp) != (tmp > 0)) {
					prop = {};
					return false; //conflict
				}
				state.set_variable(tmp, prop);
				la = -1;
			}
			if (la != -1 or lb != -1) state.done = false;

			// output debug info
			#ifdef DEBUG
			assert((la == -1) == (lb == -1));
			std::cout << "[col: " << i << "]\n" << state;
			#endif
		}
	}
	return true;
}

// #backtrack
bool DPLL::backtrack() {
//	std::cout << "propagation failed\n";
	while (!branch.empty() and branch.top() == 0) {
		call_stack.pop();
		branch.pop();
	}
	if (branch.empty()) return false;

	// make opposite decision
	call_stack.pop();
	call_stack.push(call_stack.top());
	call_stack.top().set_variable(branch.top(), prop);
	branch.top() = 0;

	return true;
}

// #DPLL
std::optional<std::vector<int>> DPLL::solve() {
	// 0. init
	init();

	while (!call_stack.empty()) {
		// 1. unit propagate
		if (!unit_propagate()) {
			if (!backtrack()) return std::nullopt; //UNSAT
			continue;
		}

		// 2. check if curent state is satisfied already
//		std::cout << "decision level: " << call_stack.size() << '\n';
		if (call_stack.top().done) break;

		// 3. if not, apply new decision
		call_stack.push(call_stack.top());
		int g = call_stack.top().pick_variable();
		call_stack.top().set_variable(g, prop);
		branch.push(-g);
	}

	// 4. SAT: return ans;
	auto res = std::vector<int>(num_vars);
	auto &state = call_stack.top();
	for (int i = 1; i <= num_vars; i++) {
		if (state.var(i) and *state.var(i)) {
			res[i-1] = i;
		} else {
			res[i-1] = -i;
		}
	}
	return res;
}
