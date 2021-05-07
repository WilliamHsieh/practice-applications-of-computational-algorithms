import sys
import math
from pyeda.inter import *

## apply sudoku input constrain
def input_constrain(grid):
    return And(*[ X[i // N + 1, i % N + 1, int(c)]
        for i, c in enumerate(grid) if c in DIGITS ])

## apply sudoku rule constrain
def rule_constrain():
    # value constrain
    V = And(*[
            And(*[
                OneHot(*[ X[r, c, v]
                    for v in range(1, N + 1) ])
                for c in range(1, N + 1) ])
            for r in range(1, N + 1) ])

    # row constrain
    R = And(*[
            And(*[
                OneHot(*[ X[r, c, v]
                    for c in range(1, N + 1) ])
                for v in range(1, N + 1) ])
            for r in range(1, N + 1) ])

    # column constrain
    C = And(*[
            And(*[
                OneHot(*[ X[r, c, v]
                    for r in range(1, N + 1) ])
                for v in range(1, N + 1) ])
            for c in range(1, N + 1) ])

    # box constrain
    B = And(*[
            And(*[
                OneHot(*[ X[n*br+r, n*bc+c, v]
                    for r in range(1, n + 1) for c in range(1, n + 1) ])
                for v in range(1, N + 1) ])
            for br in range(n) for bc in range(n) ])

    return And(V, R, C, B)

## Main function
if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage:")
        print("python3 main.py [input file] [output file]")
        exit(0)

    sys.setrecursionlimit(10000)
    with open(sys.argv[1], "r") as fin, open(sys.argv[2], "w") as fout:
        grid = [c for c in fin.read() if c.isdigit()]
        N = int(math.sqrt(len(grid)))
        assert len(grid) == N ** 2
        n = int(math.sqrt(N))
        assert N == n ** 2

        DIGITS = "".join([str(i + 1) for i in range(N)])
        X = exprvars('x', (1, N + 1), (1, N + 1), (1, N + 1))
        solver = And(rule_constrain(), input_constrain(grid))
        solver = expr2bdd(solver)

        print(solver.satisfy_count(), file = fout)

