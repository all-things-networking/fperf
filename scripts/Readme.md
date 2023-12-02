# Finding Good Random Seeds

The `find_seed.sh` script can be used to find some good random seeds for fperf.
Script runs in several rounds.
The goal of each round is to find the best seed within a time limit.
Each round executes a specified number of `fperf` processes concurrently, each with a different random seed.
The seed of the process that terminates first is considered the best seed of that round.
When a process terminates, other processes of that round are killed and scripts proceeds to the next round.
Total execution of the terminated processes are also stored alongside the seeds and in the subsequent rounds,
the minimum time is used as a time limit.
In each round, if time limit is passed and no process terminated, we kill all the processes and proceed to the next
round.
