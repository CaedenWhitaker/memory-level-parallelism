#!/usr/bin/env python3
import random
import itertools
import pandas
import subprocess

REPLICATES = 10
MIN_SIZE = 4
MAX_SIZE = 6
MAX_SEED = 2**31

algorithms = {"array", "list", "nnlist"}
sizes = [10**i for i in range(MIN_SIZE, MAX_SIZE+1)]
seeds = random.sample(range(-MAX_SEED, MAX_SEED), k=REPLICATES)

jobs = list(itertools.product(algorithms, sizes, seeds))
random.shuffle(jobs)

chart = pandas.DataFrame([
    (algorithm, size, 
     int(
         subprocess.run(
            f"./{algorithm} {size} {seed}",
            shell=True, capture_output=True
         ).stdout.decode()
        )
    )
    for algorithm, size, seed in jobs
], columns=["algorithm", "size", "us"])
# breakpoint()
chart = chart.groupby(["algorithm", "size"]).mean()
chart.sort_values(by=["algorithm", "size"], inplace=True)

chart.to_csv("data.csv")
