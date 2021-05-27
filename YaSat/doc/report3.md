###### tags: `Report`

# YaSat milestone 3
309551130 謝柏威

## file structure
```
.
├── makefile
├── benchmarks
├── include
│   ├── SatSolver.h
│   ├── parser.h
│   └── state.h
├── lib
│   ├── MiniSat_v1.14_linux
│   └── sat_comparer
├── scripts
│   ├── check.cpp
│   ├── queen.cpp
│   └── test.sh
└── src
    ├── SatSolver.cpp
    ├── parser.cpp
    ├── state.cpp
    └── sat.cpp
```

# Usage
* `makefile` is provided.
* All the source code can be compiled and run on `linux{id}.cs.nctu.edu.tw` by simply typing `make`. The resulting binary will be generated named `yasat`.
```
$ make
g++ -Iinclude/ -std=c++17 -Ofast -Wall -Wextra -Wshadow -Wold-style-cast -c -o obj/parser.o src/parser.cpp
g++ -Iinclude/ -std=c++17 -Ofast -Wall -Wextra -Wshadow -Wold-style-cast -c -o obj/sat.o src/sat.cpp
g++ -Iinclude/ -std=c++17 -Ofast -Wall -Wextra -Wshadow -Wold-style-cast -o yasat obj/parser.o obj/sat.o

$ la yasat
-rwxr-xr-x 1 pwhsieh gcs 48K Apr 19 20:24 yasat
```

# Feature
In this project We are going to implement a SAT solver, and these are the features I've implemented in this milestone.
1. fixing non-chronological backtracking
2. VSIDS
3. random restart
4. conflict clause minimization
5. 8-queen encoding

## 1. fixing non-chronological backtracking
* In previous milestone, I failed to deliver the correct version of CDCL(conflict-driven clause learning), so I spend some time fixing the bug before this milestone.
* It turns out that the only thing missing is how I backtrack. Here's the detail.

### Conflict-driven clause learning
* This is the main part of the CDCL. The main difference is the backtracking part.
* In CDCL, I no longer need to maintain another branch in our decision(eg. when we decide to go `11 == true` first, the other branch is `11 == false`).

```cpp
// #Conflict-driven clause learning
optional<vector<int>> SatSolver::solve() {
    // 0. init
    init();

    while (!stk.empty()) {
        // 1. BCP
        if (int cid = bcp(); cid != -1) {
            // UNSAT
            if (stk.size() == 1) return nullopt;

            // non-chronological backtracking
            conflict_learning(cid);
            continue;
        }

        // 2. check if curent state is satisfied already
        if (stk.top().done) break;

        // 3. if not, apply new decision
        if (int g = pick_variable(); g != 0) {
            stk.push(stk.top());
            set_variable(g);
        } else {
            break;
        }
    }

    // 4. SAT: return ans;
    auto res = vector<int>(num_vars);
    auto &state = stk.top();

    for (int i = 1; i <= num_vars; i++) {
        res[i - 1] = state.var(i).value_or(true) ? i : -i;
    }
    return res;
}
```

## 2. VSIDS
* Variable State Independent Decaying Sum
* The frequency of the variable will be bumped each time a new clause is learned.
* The frequency of the variable is going to decay when a new decision is made.

```cpp
// VSIDS bump
for (int p : C) VSIDS[abs(p)][p > 0] += 1;
```

```cpp
// VSIDS decay
const double decay = 0.95;
if (cid == -1) {
    for (auto &p : VSIDS) {
        p[0] *= decay;
        p[1] *= decay;
    }
}
```

```cpp
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
```
## 3. random restart
* All the intermediate decision will be discarded when there's 100 conflicts.
* I comment out this part in my code, because I didn't notice any performance boost.

```cpp
//	static int conflict_cnt = 0;
//	if (++conflict_cnt == 100) {
//		conflict_cnt = 0;
//		while (stk.size() > 1) stk.pop();
//	}
```

## 4. conflict clause minimization
* When I learn from a conflict clause, I immediately check if there's any literal can be removed.
* The concept is borrowed from the paper published by MiniSat.
* https://www.researchgate.net/publication/228943311_Minisat_v113-a_SAT_solver_with_conflict-clause_minimization

```cpp
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
```

## 5. 8-queen encoding
* This is an application of this project.
* `make queen` will generate `queen` executable.
```
$ ./queen 5 data.cnf
[SAT]
1 0 0 0 0
0 0 0 1 0
0 1 0 0 0
0 0 0 0 1
0 0 1 0 0
```

# Results
* These result are verified by my own written `checker.cpp`, which is placed under `scripts/`.
* It checks the correctness of the SAT solver by taking the `.cnf` file and `.sat` file as input, and check if there's any confict between them.

## 75 variable, 325 clauses
* These are some randomly generated 3-SAT dataset.
* 5 SAT and 5 UNSAT, 10 dataset in total.
* 10 cnf test cases take about ___0.4 second___.
> make test will run the test.sh script to run the benchmark
```
$ time make test
./scripts/test.sh
[SAT]
[ACCEPTED]
[SAT]
[ACCEPTED]
[SAT]
[ACCEPTED]
[SAT]
[ACCEPTED]
[SAT]
[ACCEPTED]
[UNSAT]
[UNSAT]
[UNSAT]
[UNSAT]
[UNSAT]
[UNSAT]
[UNSAT]
[UNSAT]
[UNSAT]
[UNSAT]
make test  0.09s user 0.02s system 24% cpu 0.445 total
```

## milestone 2 benchmarks
* Also, I'm able to run most of the benchmarks. But I'm not able to present it on time. This is my bad.

# Discussion
* The result is definitely exciting. Yasat had gain a huge performance since milestone 1.
* There's something I still not understand. Such as `random restart` doesn't work as expected.
* VSIDS definitely feels like magic! This feature gain a HUGE performance!
* Also, the algorithm mensioned in the MiniSat paper, conflict clause minimization, is effective and easy to implement.
