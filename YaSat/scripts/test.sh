#!/bin/bash

for i in {0..9}; do
	fname="benchmarks/random/75_325_"
	./yasat ${fname}${i}.cnf && ./checker ${fname}${i}
done
