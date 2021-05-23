###### tags: `Report`

# YaSat milestone 2
309551130 謝柏威

## file structure
```
.
├── makefile
├── benchmarks
├── doc
│   └── report2.md
├── include
│   ├── dpll.h
│   ├── parser.h
│   └── state.h
├── lib
│   ├── MiniSat_v1.14_linux
│   └── sat_comparer
├── scripts
│   ├── check.cpp
│   └── test.sh
└── src
    ├── dpll.cpp
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
1. two literal watching
2. buggy non-chronological backtracking

**05/23 UPDATE**: finish non-chronological backtracking

## 1. two literal watching
* This method drastically decrease the complexity of the unit propagation, since we at most look at all the clauses and literal __once__.
* `prop` record all the false literal. (ex: decision on `1 == true`, imply that `1 == false` is the false literal)

### unit propagation
* During __unit propagation__, we only look at the first two literal in each clause. This so-called `two literal watching` method enable us to do quick unit propagation without iterating through all the clauses.

```cpp
bool DPLL::unit_propagate() {
    auto &state = call_stack.top();
    while (!prop.empty()) {
        int false_literal = prop.front(); prop.pop();

        state.done = true;
        for (int i = 0; i < num_clauses; i++) {

            // this clause is already satisfied
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
                int last = clauses[i][la];

                // conflict
                if (state.var(last) and *state.var(last) != (last > 0)) {
                    conflict_learning(i);
                    return false;
                }

                // imply
                state.set_variable(last, *this, i);
                la = -1;
            }
            if (la != -1 or lb != -1) state.done = false;
        }
    }
    return true;
}
```

### watch literal that is not false
This method will update the watched literal to the one that is not false.
```cpp
void DPLL::watch_not_false(int &self, int &other, int i, int false_literal) {
    // already watching at not false literal, TODO: maybe don't just check false_literal
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
```

## 2. non-chronological backtracking
* This is the buggy part. The result will be faster, but it will sometime mis-computed as UNSAT.
* If you would like to demo the two-literal watching part without this buggy part, please comment out `conflict_learning(i);` in `DPLL::unit_propagate()`.

### DPLL::conflict_learning
It all start from here. It will get the `FirstUIP` clause and backjump.
```cpp
void DPLL::conflict_learning(int cid) {
    auto C = FirstUIP(cid);

    // backjump to specific level
    int cur_level = call_stack.size();
    int lvl = 0;
    for (int &x : C) if (int tmp = call_stack.top().decision_level[abs(x)]; tmp < cur_level) {
        lvl = max(lvl, tmp);
    }

    assert(lvl > 0); //holding true

    for (int i = cur_level; i > lvl + 1; i--) {
        call_stack.pop();
        branch.pop();
    }
}
```

### DPLL::FirstUIP
This part will continuously resolve `C` with ancedence of `P`, until we get a learned conflict clause.
```cpp
vector<int> DPLL::FirstUIP(int cid) {
    int cur_level = call_stack.size();
    auto C = clauses[cid];

    // if C has more than one literal assigned at current decision level
    auto ok = [&cur_level, this](auto &v) {
        int p = 0;
        int cnt = 0;
        for (auto &x : v) if (call_stack.top().decision_level[abs(x)] == cur_level) {
            cnt += 1;
            if (call_stack.top().antecedent[abs(x)] != -1) p = x;
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
```

### DPLL::resolve
The resolve function will resolve two given clause(C and antecedent of p) into one.
```
void DPLL::resolve(vector<int> &C, int p) {
    assert(p != 0);
    int cid = call_stack.top().antecedent[abs(p)];
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
```

# Results
* These result are verified by my own written `checker.cpp`, which is placed under `scripts/`.
* It checks the correctness of the SAT solver by taking the `.cnf` file and `.sat` file as input, and check if there's any confict between them.

## 75 variable, 325 clauses
* These are some randomly generated 3-SAT dataset.
* 5 SAT and 5 UNSAT, 10 dataset in total.
* 10 cnf test cases take about ___0.67 second___.
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
make test  0.67s user 0.06s system 87% cpu 0.825 total
```

# Discussion
* The concept of `two-literal watching` itself is not hard to understand, but quite challenging to implement. However, after tracing the algorithm with pen and paper, it can be implemented easily.
* The non-chronological part is the hard part. I failed to implement bug-free version in the given time. If TA would like to test `two-literal watching` only, please comment out `conflict_learning(i);` under `src/dpll.cpp`, thanks.
* But I won't give up. I will make use of the remaining time to make sure find out the bug and keep adding more heuristic method.
