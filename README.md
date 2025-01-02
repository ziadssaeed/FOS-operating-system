# FOS - Faculty Operating System

FOS (Faculty Operating System) is a specialized educational project developed for the Operating Systems Course (CSW355) at Ain Shams University, Faculty of Computer and Information Science. The project is forked and refactored from MIT’s 6.828 Operating Systems Lab, originally created by Dr. Mahmoud Hossam and currently maintained by Dr. Ahmed Salah.

## Project Overview

FOS provides students with a hands-on experience in building core components of an operating system. It enables a deeper understanding of critical concepts such as memory management, scheduling, and synchronization by implementing them from scratch. The project is structured into three milestones:

### Milestone 1: Dynamic Memory Allocation and Command Prompt
- **Objective**: Lay the foundation of the operating system by building a dynamic memory allocator and integrating system calls.
- **Implemented Features**:
  - A basic command prompt interface
  - System call implementation for communication between user and kernel modes
  - Dynamic memory allocation strategies:
    - First Fit
    - Next Fit
    - Best Fit
  - Support for locks to handle concurrency

### Milestone 2: Memory Management and Fault Handling
- **Objective**: Develop essential memory management functions and handle basic page faults.
- **Implemented Features**:
  - Kernel Heap dynamic allocator for efficient memory utilization
  - Paging functions for memory management:
    - Allocate and free memory chunks
    - Translate between virtual and physical addresses
  - User Heap allocator for user-mode memory management
  - Shared memory mechanisms to facilitate inter-process communication

### Milestone 3: Advanced Fault Handling and CPU Scheduling
- **Objective**: Enhance memory fault handling, implement semaphores, and introduce a priority-based round-robin CPU scheduler.
- **Implemented Features**:
  - Nth Chance Clock page replacement algorithm
  - User-level semaphores for synchronization
  - Priority-based round-robin scheduler for efficient CPU management
  - Mechanisms to handle resource sharing and prevent deadlocks


Each milestone comes with a set of test cases to validate your implementation. Navigate to the test directory for the respective milestone and execute the provided scripts.

## About

This project is part of the curriculum for Operating Systems (CSW355) at the Faculty of Computer and Information Science, Ain Shams University. It provides students with practical experience in implementing fundamental operating system functionalities.

## License

This project is intended for educational purposes and is governed by an academic use license. Redistribution or modification outside of Ain Shams University requires explicit permission.
