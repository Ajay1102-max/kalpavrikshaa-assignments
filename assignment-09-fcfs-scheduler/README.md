# FCFS Process Scheduling Simulator (C)

This simulator implements a FCFS (First-Come-First-Serve) process scheduler with:
- HashMap (PID -> PCB) using separate chaining
- Ready queue, Waiting (I/O) queue, Terminated queue
- Tick-by-tick CPU simulation
- I/O start/duration handling
- Kill events executed at exact ticks
- Final summary with completion, turnaround, waiting, response times

## Files
- fcfs_scheduler.c   : main C program
- testcases.txt      : example input file

## Input format (stdin)
First line: N (number of processes)
Next N lines (one per process):