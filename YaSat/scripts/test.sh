#!/bin/bash

for i in {0..9}; do
	./yasat benchmarks/75_325_${i}.cnf && ./checker benchmarks/75_325_${i}
done
