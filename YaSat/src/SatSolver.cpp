#include <bits/stdc++.h>
#include "parser.h"
#include "state.h"
#include "SatSolver.h"
#define all(x) begin(x),end(x)
#define what_is(x) std::cerr << "[what] " << #x << " is " << x << std::endl
#define exec(x) std::cout << "[exec] " << #x << std::endl; x

using namespace std;

// #Constructor
SatSolver::SatSolver(string &fname) {
	parse_DIMACS_CNF(clauses, num_vars, fname.data());
	num_clauses = clauses.size();
	decision_level.assign(num_vars + 1, 0);
	antecedent.assign(num_vars + 1, -1);
	timestamp.assign(num_vars + 1, -1);
	VSIDS.assign(num_vars + 1, {0.0, 0.0});
}

// #Initialize
void SatSolver::init() {
	assert(clauses.size() != 0);
	// remove clause if both -x and x exist && make literal unique
	max_clauses = 0;
	auto tmp = vector<vector<int>>{};
	for (auto &v : clauses) {
		auto s = set<int>(all(v));
		bool ok = find_if(all(s), [&](int x) {
			return s.count(-x);
		}) == s.end();

		if (ok) {
			tmp.emplace_back(all(s));
			max_clauses = max(max_clauses, static_cast<int>(s.size()));
		}
	}
	clauses.swap(tmp);
	num_clauses = clauses.size();
	orig_clauses = num_clauses;

	// assign unit clauses as decision
	auto &state = stk.emplace(num_vars, num_clauses);
	for (size_t i = 0; i < clauses.size(); i++) {
		if (clauses[i].size() == 1) {
			state.watch[i] = {-1, -1};
			set_variable(clauses[i][0], i);
		}
	}

	// initialize VSIDS
	for (auto &v : clauses) {
		for (auto &x : v) {
			VSIDS[abs(x)][x > 0] += 1;
		}
	}

	#ifdef DEBUG
	int idx = 0;
	for (auto &v : clauses) {
		cout << '[' << idx++ << ']' << ' ';
		for (auto x : v) cout << x << ' ';
		cout << endl;
	}
	#endif
}

// #Set a variable
void SatSolver::set_variable(int x, int cid) {
	if (x == 0) return;

	auto &state = stk.top();
//	assert(state.var(x).has_value() == false);

	// if this is a new decision, reset time
	if (cid == -1) time = 0;
	state.var(x) = (x >= 0);

	decision_level[abs(x)] = stk.size();
	antecedent[abs(x)] = cid;
	timestamp[abs(x)] = time++;

	// VSIDS decay
	const double decay = 0.95;
	if (cid == -1) {
		for (auto &p : VSIDS) {
			p[0] *= decay;
			p[1] *= decay;
		}
	}
}

// #Pick a variable
int SatSolver::pick_variable() {
	static auto gen = std::default_random_engine(std::random_device{}());
	static auto dis = std::uniform_int_distribution<int>(0, 1);

	int choice = 0;
	VSIDS[0][0] = VSIDS[0][1] = 0;
	for (int g = 1; g <= num_vars; g++) if (!stk.top().var(g)) {
		// randomly pick between all max_element
		if (VSIDS[g][0] + VSIDS[g][1] > VSIDS[choice][0] + VSIDS[choice][1]) {
			choice = g;
		} else if (VSIDS[g][0] + VSIDS[g][1] == VSIDS[choice][0] + VSIDS[choice][1]) {
			choice = dis(gen) ? choice : g;
		}
	}

	if (choice == 0) {
		std::cerr << "shouldn't reach here!\n";
		for (auto x : VSIDS) cerr << x[0] + x[1] << ' ';
		return 0;
	}

	if (VSIDS[choice][1] < VSIDS[choice][0]) {
		choice = -choice;
	} else if (VSIDS[choice][1] == VSIDS[choice][0]) {
		choice = dis(gen) ? choice : -choice;
	}
	return choice;
}

// #Watch not false
void SatSolver::watch_not_false(int &self, int &other, int cid) {
	// already watching at not false literal
	if (!stk.top().var(clauses[cid][self]).has_value()) return;

	for (size_t j = max(self, other) + 1; j < clauses[cid].size(); j++) {
		auto &var = stk.top().var(clauses[cid][j]);
		if (!var.has_value()) {
			// not false
			self = j;
			return;
		} else if (*var == (clauses[cid][j] > 0)) {
			// true
			self = other = -1;
			return;
		}
	}
}

// #Watch is true
bool SatSolver::watch_is_true(int watched, int i) {
	auto &var = stk.top().var(clauses[i][watched]);
	if (var.has_value() and *var == (clauses[i][watched] > 0)) {
		return true;
	}
	return false;
}

// #Resolve
void SatSolver::resolve(vector<int> &C, int p) {
	int cid = antecedent[abs(p)];

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
vector<int> SatSolver::FirstUIP(int cid) {
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
void SatSolver::conflict_learning(int cid) {
	auto C = FirstUIP(cid);

	// Conflict clause minimization
	for (int &p : C) {
		if (antecedent[abs(p)] < 0) continue;

		bool exist = true;
		for (auto &p_bar : clauses[antecedent[abs(p)]]) {
			if (p == -p_bar) continue;
			if (p == p_bar or find(all(C), p_bar) == end(C)) {
				exist = false;
				break;
			}
		}

		// remove p from C
		if (exist) p = INT_MAX;
	}
	sort(all(C));
	while (C.back() == INT_MAX) C.pop_back();

	// VSIDS bump
	for (int p : C) VSIDS[abs(p)][p > 0] += 1;

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
void SatSolver::update() {
	// prune large clauses
//	if (num_clauses > 2 * orig_clauses) {
//		what_is(clauses.size());
//		clauses.erase(remove_if(orig_clauses + all(clauses), [&](vector<int> &v) {
//			return v.size() > 3 * max_clauses;
//		}), clauses.end());
//		what_is(clauses.size());

//		num_clauses = clauses.size();
//		stk.top().watch.resize(num_clauses);
//		stk.top().num_clauses = num_clauses;
//	}

	// random restart
//	static int conflict_cnt = 0;
//	if (++conflict_cnt == 100) {
//		conflict_cnt = 0;
//		while (stk.size() > 1) stk.pop();
//	}

	// update the watched literal
	auto &state = stk.top();
	for (int i = state.watch.size(); i < num_clauses; i++) {
		if (clauses[i].size() == 1) {
			state.watch.emplace_back(array{-1, -1});
			set_variable(clauses[i][0], i);
		} else {
			state.watch.emplace_back(array{0, 1});
		}
	}

	// update decision_level and antecedent and timestamp
	for (int i = 1; i <= num_vars; i++) {
		if (!state.var(i)) {
			decision_level[i] = 0;
			antecedent[i] = -1;
			timestamp[i] = -1;
		}
	}
}

// #Unit propagate
bool SatSolver::unit_propagate(int cid, bool &modified) {
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
	watch_not_false(la, lb, cid);
	watch_not_false(lb, la, cid);

	// check if this became unit clause (implication)
	if (la != -1 and lb != -1) {
		// both are false literal -> conflict
		if (state.var(clauses[cid][la]) and state.var(clauses[cid][lb])) {
			return false;

		// imply lb
		} else if (state.var(clauses[cid][la])) {
			set_variable(clauses[cid][lb], cid);
			la = lb = -1;
			modified = true;

		// imply la
		} else if (state.var(clauses[cid][lb])) {
			set_variable(clauses[cid][la], cid);
			la = lb = -1;
			modified = true;
		}
	}
	if (la != -1 or lb != -1) done = false;

	// output debug info
	#ifdef DEBUG
	assert((la == -1) == (lb == -1));
	cout << "[col: " << cid << "]\n" << state;
	#endif

	return true;
}

// #Boolean constraint propagation
int SatSolver::bcp() {
	#ifdef DEBUG
	cout << endl;
	what_is(stk.size());
	#endif

	for (bool modified = true; modified; ) {
		done = true;
		modified = false;

		for (int i = num_clauses - 1; i >= 0; i--) {
			if (unit_propagate(i, modified) == false) {
				return i;
			}
		}
	}
	return -1;
}

// #Conflict-driven clause learning
result_type SatSolver::solve() {
	// 0. init
	init();

	while (!stk.empty()) {
		// 1. BCP
		if (int cid = bcp(); cid != -1) {
			// UNSAT
			if (stk.size() == 1) return {};

			// non-chronological backtracking
			conflict_learning(cid);
			continue;
		}

		// 2. check if curent state is satisfied already
		if (done) break;

		// 3. if not, apply new decision
		if (int g = pick_variable(); g != 0) {
			stk.push(stk.top());
			set_variable(g);
		} else {
			break;
		}
	}

	// 4. SAT: return ans;
	auto res = result_type(num_vars);
	auto &state = stk.top();

	for (int i = 1; i <= num_vars; i++) {
		res[i - 1] = state.var(i).value_or(true) ? i : -i;
	}
	return res;
}

// #operator<< for result_type
ostream& operator<< (ostream &os, result_type &res) {
	if (!res.empty()) {
		cout << "[SAT]" << endl;
		os << "s SATISFIABLE\n";
		os << 'v';
		for (auto x : res) os << ' ' << x;
		os << " 0\n";
	} else {
		cout << "[UNSAT]" << endl;
		os << "s UNSATISFIABLE\n";
	}

	return os;
}
