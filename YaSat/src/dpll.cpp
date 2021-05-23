#include <bits/stdc++.h>
#include "state.h"
#include "dpll.h"
#define all(x) begin(x),end(x)
#define what_is(x) std::cout << "[what] " << #x << " is " << x << std::endl
#define exec(x) std::cout << "[exec] " << #x << std::endl; x

using namespace std;

// #Constructor
DPLL::DPLL(int _num_vars, vector<vector<int>> &_clauses)
	: num_vars(_num_vars),
	  num_clauses(_clauses.size()),
	  decision_level(num_vars + 1, 0),
	  antecedent(num_vars + 1, -1),
	  timestamp(num_vars + 1, -1),
	  clauses(_clauses)
{}

// #Initialize
void DPLL::init() {
	assert(clauses.size() != 0);
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
			v = vector(1, 0); //TODO: get rid of this
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
	auto &state = stk.emplace(num_vars, num_clauses);
	for (size_t i = 0; i < clauses.size(); i++) {
		if (clauses[i].size() == 1) {
			state.watch[i] = {-1, -1};
			set_variable(clauses[i][0], i);
		}
	}

	#ifdef DEBUG
	int idx = 0;
	for (auto &v : clauses) {
		cout << '[' << idx++ << ']' << ' ';
		for (auto x : v) std::cout << x << ' ';
		std::cout << std::endl;
	}
	#endif
}

// #Set a variable
void DPLL::set_variable(int x, int cid) {
	auto &state = stk.top();
//	assert(state.var(x).has_value() == false);

	// if this is a new decision, reset time
	if (cid == -1) state.time = 0;
	state.var(x) = (x >= 0);

	prop.push(-x);
	decision_level[abs(x)] = stk.size();
	antecedent[abs(x)] = cid;
	timestamp[abs(x)] = state.time++;
}

// #Watch not false
void DPLL::watch_not_false(int &self, int &other, int i, int false_literal) {
	// TODO: maybe don't just check false_literal
//	auto is_false_literal = [](auto &state, int x) {
//		return state.var(x).has_value() and *state.var(x) != (x > 0);
//	};
//	if (!is_false_literal(stk.top(), clauses[i][self])) {
//		what_is(clauses[i][self]);
//		what_is(*stk.top().var(clauses[i][self]));
//		return;
//	}

	// already watching at not false literal
	if (clauses[i][self] != false_literal) return;

	for (size_t j = std::max(self, other) + 1; j < clauses[i].size(); j++) {
		auto &var = stk.top().var(clauses[i][j]);
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
	auto &var = stk.top().var(clauses[i][watched]);
	if (var.has_value() and *var == (clauses[i][watched] > 0)) {
		return true;
	}
	return false;
}

// #Resolve
void DPLL::resolve(vector<int> &C, int p) {
	assert(p != 0);
	int cid = antecedent[abs(p)];
	assert(cid != -1);

	// merge C and antecedent of p
	copy(all(clauses[cid]), back_inserter(C));
	for (auto &x : C) if (abs(x) == abs(p)) {
		x = INT_MAX;
	}

	// resolve p
	sort(all(C));
	C.erase(unique(all(C)) - 1, C.end());
}

// #First UIP
vector<int> DPLL::FirstUIP(int cid) {
	int cur_level = stk.size();
	auto C = clauses[cid];

	// if C has more than one literal assigned at current decision level
	auto ok = [&cur_level, this](auto &v) {
		int p = 0;
		int cnt = 0;
		for (auto &x : v) if (decision_level[abs(x)] == cur_level) {
			cnt += 1;
			if (antecedent[abs(x)] != -1) {
				if (p == 0 or timestamp[abs(x)] < timestamp[abs(p)]) {
					p = x;
				}
			}
		}

		if (cnt >= 2) return p;
		return 0;
	};

	// get learned conflict clause
	while (int P = ok(C)) {
		resolve(C, P);
	}

	return C;
}

// #Conflict learning
void DPLL::conflict_learning(int cid) {
	auto C = FirstUIP(cid);

	// find second max decision level in C
	int cur_level = stk.size();
	int lvl = 1;
	for (int &x : C) {
		if (int tmp = decision_level[abs(x)]; tmp < cur_level) {
			lvl = max(lvl, tmp);
		}
	}

	// backjump to specific level
	for (int i = cur_level; i > lvl; i--) {
		stk.pop();
	}

	// save the learned conflict clause
	clauses.emplace_back(std::move(C));
	num_clauses += 1;

	// update
	update();

	#ifdef DEBUG
	cout << "[backjump]" << endl;
	what_is(cid);
	what_is(cur_level);
	what_is(lvl);
	for (auto x : clauses.back()) cout << x << ' ';
	cout << endl << stk.top() << endl;
	#endif
}

// #Update
void DPLL::update() {
	// update the clause
	auto &state = stk.top();
	for (int &i = state.num_clauses; i < num_clauses; i++) {
		state.watch.emplace_back(array{0, 1});
	}

	// update decision_level and antecedent and timestamp
	for (int i = 1; i <= num_vars; i++) {
		if (!state.var(i)) {
			decision_level[i] = 0;
			antecedent[i] = -1;
			timestamp[i] = -1;
		}
	}

	// clear the prop queue
	if (prop.size()) prop = {};
	int implied = 0;
	for (auto &x : clauses.back()) if (!state.var(x).has_value()) {
		state.watch.back() = {-1, -1};
		implied = x;
		break;
	}
	set_variable(implied, num_clauses - 1);
}

// #Unit propagate
bool DPLL::unit_propagate(int cid) {
	auto false_literal = prop.front();
	auto &state = stk.top();

	// this clause is already satisfied
	auto &[la, lb] = state.watch[cid];
	if (la == -1 and lb == -1) return true;

	// one of the watched literal is already true
	if (watch_is_true(la, cid) or watch_is_true(lb, cid)) {
		la = lb = -1;
		return true;
	}

	// update watching literals that are not false
	watch_not_false(la, lb, cid, false_literal);
	watch_not_false(lb, la, cid, false_literal);

	// check if this became unit clause (implication)
	if (la == -1) std::swap(la, lb);
	if (la != -1 and lb == -1) {
		int last = clauses[cid][la];

		// conflict
		if (state.var(last) and *state.var(last) != (last > 0)) {
			return false;
		}

		// imply
		set_variable(last, cid);
		la = -1;
	}
	if (la != -1 or lb != -1) state.done = false;

	// output debug info
	#ifdef DEBUG
	assert((la == -1) == (lb == -1));
	what_is(false_literal);
	std::cout << "[col: " << cid << "]\n" << state;
	#endif

	return true;
}

// #BCP
int DPLL::bcp() {
	#ifdef DEBUG
	cout << endl;
	what_is(stk.size());
	#endif

	//TODO: change to look up all variable
	for ( ; !prop.empty(); prop.pop()) {
		stk.top().done = true;
		for (int i = 0; i < num_clauses; i++) {
			if (unit_propagate(i) == false) {
				return i;
			}
		}
	}
	return -1;
}

// #Conflict-driven clause learning
std::optional<std::vector<int>> DPLL::solve() {
	// 0. init
	init();

	while (!stk.empty()) {
		// 1. BCP
		if (int cid = bcp(); cid != -1) {
			if (stk.size() == 1) {
				return std::nullopt;
			}
			conflict_learning(cid);
			continue;
		}

		// 2. check if curent state is satisfied already
		if (stk.top().done) break;

		// 3. if not, apply new decision
		if (int g = stk.top().pick_variable(); g != -1) {
			stk.push(stk.top());
			set_variable(g);
		} else {
			break;
		}
	}

	// 4. SAT: return ans;
	auto res = std::vector<int>(num_vars);
	auto &state = stk.top();

	for (int i = 1; i <= num_vars; i++) {
		if (state.var(i) and *state.var(i)) {
			res[i-1] = i;
		} else {
			res[i-1] = -i;
		}
	}
	return res;
}
