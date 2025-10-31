# Hash Factor

## Overview
Factorizes large integers using POSIX multiprocessing and inter-process communication.

## Features
- Parallel factorization with multiple child processes
- Uses pipes, shared memory, semaphores, and message queues
- Reports results to an external program via standard input

## Tech
- C++
- POSIX system calls: fork, exec, pipe, shared memory, message queues, semaphores
- Parallel computation and synchronization
