# 42 Philosophers Project

The **Philosophers** project simulates a classic synchronization problem where multiple philosophers sit around a table, thinking and eating. Each philosopher needs two forks to eat, but there is only a limited number of forks available. The goal is to ensure that the philosophers can eat without deadlocking and starving, respecting specific time constraints.

## Project Overview

This project implements the **Dining Philosophers Problem** using **pthreads** to create multiple threads representing philosophers. It focuses on synchronizing access to shared resources (the forks) to avoid deadlock and ensure that all philosophers can finish their meals in a timely manner.

Key features:
- **Mutexes** for synchronizing access to forks.
- Each philosopher has a **right** and **left** fork, represented by `t_fork` structs.
- Philosophers need to eat and think while managing time and avoiding starvation or deadlock.
- The program simulates philosophers' behavior and their interactions with forks using mutexes to control access to shared resources.

## Requirements

- **C** programming language
- **pthread** library for multithreading

## Compilation

To compile the program, use the following command:

```bash
make
