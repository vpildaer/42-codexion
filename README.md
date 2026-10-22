*This project has been created as part of the 42 curriculum by vpildaer.*

# Codexion - 42 Belgium

This project is an adapation of Dijkstra's [Dining Philosophers Problem](https://en.wikipedia.org/wiki/Dining_philosophers_problem). 

## Table of Contents

* [Description](#description)
    * [What is a (p)Thread ?](#what-is-a-pthread-)
    * [Blocking Cases Handled](#blocking-cases-handled)
    * [Thread Synchronization Mechanisms](#thread-synchronization-mechanisms)

* [Instructions](#instructions)
    * [Running Tests](#running-tests)

* [Resources](#resources)
    * [AI Use](#ai-use)

## Description

This project is an introduction to manipulating [(p)threads](#what-is-a-pthread-) in C language, based on the Dining Philosophers Problem by Dijkstra. 

Where in Dijkstra's problem, the Philosophers sitting around a circular table were sharing forks to eat in-between thinking sessions, in Codexion, we have `n` coders sharing `n` dongles connected to a Quantum Compiler. 

The coders alternatively compile, debug or refactor, in this exact order. They need two dongles to compile, and a dongle needs a cooldown time before being used again. 



### What is a (P)Thread ?

POSIX threads, also known as pthreads after its header `<pthread.h>`, is a model of execution independant from any programming language, allowing a program to control a multitude of workflows that overlap in time.  

Basically, threads (often) perform tasks concurrently. 



### Blocking Cases Handled

Throughout the project, attention has to be 

#### Deadlock Prevention & Coffman's Conditions

A [deadlock](https://en.wikipedia.org/wiki/Deadlock_(computer_science)) occurs when a thread enters a state of waiting for a resource held by another waiting process.

For a deadlock to happen, the 4 **Coffman's conditions** must be fulfilled :

* **Mutual Exclusion**: no dongle can be simultaneously used by multiple coders
* **Resource Holding**: coders hold a dongle while waiting for the second 
* **Non-Preemption**: no coder can take a dongle from another
* **Circular Wait**: each coder wait for the dongle on the left

To prevent a deadlock from happening, one of the 4 Coffman's conditions has to be neglected. By design, mutual exclusion and non-preemption cannot be neglected, thus leaving only resource holding or circular wait to be neglected. 

During the development of the code, neglecting circular wait seemed like the natural choice. 

To do so, I implemented a partial ordering of resources in the `compile` function.

```
if (coder->left_dongle < coder->right_dongle)
	{
		l_res = take_dongle(coder, coder->left_dongle);
		r_res = take_dongle(coder, coder->right_dongle);
	}
	else
	{
		r_res = take_dongle(coder, coder->right_dongle);
		l_res = take_dongle(coder, coder->left_dongle);
	}
```

This implementation means that the waiting inside `take_dongle` 

#### Starvation prevention

#### Cooldown Handling

#### Precise Burnout Detection

#### Log Serialization

### Thread Synchronization Mechanisms

#### Threading Primitives

#### Race Conditions Prevention

#### Thread-Safe Communication Between Coders and the Monitor

## Instructions

### Running Tests

The following tests were run against the project during development.

#### Memory test (valgrind)

Testing for memory leaks with valgrind.

```
valgrind --leak-check=full --show-leak-kinds=all ./codexion 3 2000 50 50 50 10 20 edf
valgrind --leak-check=full --show-leak-kinds=all ./codexion 3 1000 50 10 10 20 10 fifo
```
**Result**: 0 errors, all heap blocks freed.

#### Lock discipline (helgrind)

Testing for thread errors with helgrind, a valgrind thread error detector tool.

```
valgrind --tool=helgrind ./codexion 3 1000 50 10 10 20 10 fifo
```

**Result**: warnings reported only on `pthread_cond_timedwait` inside `wait_dongle`. Verified as a [known Helgrind/glibc ≥2.34 false positive](https://bugzilla.redhat.com/show_bug.cgi?id=1974357) (Helgrind's instrumentation hasn't been updated for glibc's internal condition-variable implementation), not a real race, confirmed by the clean ThreadSanitizer (TSan) run below and by manual review (the mutex is correctly held at every call site).

#### Data Races (Thread Sanitizer)

**Data Races** happen when **threads** try to access or modify the same piece of data at the same time.

Testing data races using the ThreadSanitizer (TSan) tool.

```
gcc -Wall -Wextra -Werror -fsanitize=thread -pthread codexion_dir/*.c -o codexion_tsan  && ./codexion_tsan 3 1000 50 10 10 20 10 fifo
```

**Result**: 0 warning. This confirms there is no data race.

#### Cooldown Correctness

```
./codexion 2 1000 10 10 10 5 15 edf
./codexion 2 5000 50 10 10 10 2000 fifo
./codexion 2 5000 50 10 10 10 3500 edf
```

**Result**: a dongle that has never been used is available immediately at `t=0` (no artificial wait), while a dongle that has actually been released stays unavailable until exactly `dongle_cooldown` ms after release.

#### Burnout Precision

```
./codexion 2 5000 50 10 10 10 3500 edf
```

**Result**: a coder that cannot reacquire a dongle in time burns out within 10ms of the real deadline, and nothing is logged for that coder afterward.

#### Full Completion without Burnout

```
./codexion 3 100000 20 10 10 5 10 fifo
```

**Result**: once every coder reaches `number_of_compiles_required`, the simulation stops cleanly with no `burned out` message, and any in-flight log for a coder still finishing its cycle is correctly suppressed.


## Resources

* [Dining Philosophers Problem](https://en.wikipedia.org/wiki/Dining_philosophers_problem)
* [pthreads](https://en.wikipedia.org/wiki/Pthreads)
* [POSIX Threads (pthreads) — The Simplest Way to Understand Real Multithreading in C](https://medium.com/@techdhaba.training/posix-threads-pthreads-the-simplest-way-to-understand-real-multithreading-in-c-c2f591ab7a03)
* [Deadlock](https://en.wikipedia.org/wiki/Deadlock_(computer_science))
* [Mutex lock for Linux Thread Synchronization](https://www.geeksforgeeks.org/linux-unix/mutex-lock-for-linux-thread-synchronization/)
* [Mutex Introduction (pthreads)](https://www.youtube.com/watch?v=raLCgPK-Igc)
* [C Program to Implement Priority Queue](https://www.geeksforgeeks.org/c/c-program-to-implement-priority-queue/)
* [Conditional wait and signal in multi-threading](https://www.geeksforgeeks.org/linux-unix/condition-wait-signal-multi-threading/)
* [Condition variables in C](https://www.youtube.com/watch?v=0sVGnxg6Z3k&list=PLfqABt5AS4FmuQf70psXrsMLEDQXNkLq2&index=13)
* [Measure execution time with high precision in C/C++](https://www.geeksforgeeks.org/c/measure-execution-time-with-high-precision-in-c-c/)
* [Thread Management Functions in C](https://www.geeksforgeeks.org/c/thread-functions-in-c-c/)
* [pthread_cond_timedwait, pthread_cond_wait - wait on a condition](https://pubs.opengroup.org/onlinepubs/009604599/functions/pthread_cond_timedwait.html)
* [Atomic types](https://en.cppreference.com/c/language/atomic)
* [Heap in C](https://www.geeksforgeeks.org/c/heap-in-c/)
* [How heap work? Heapify algorithm](https://medium.com/@garggourav012/how-heap-work-heapify-algorithm-0bc1fdcede15)
* [Helgrind: a thread error detector](https://valgrind.org/docs/manual/hg-manual.html)
* [Understanding Data Races in Programming: A Quick Overview](https://medium.com/@efebalikofc/understanding-data-races-in-programming-a-quick-overview-1058047c0c6f)
* [Understanding Synchronization Primitives and Techniques in Multithreaded Programming](https://medium.com/@ibrahim__/understanding-synchronization-primitives-and-techniques-in-multithreaded-programming-565b71017d0e)
* [Coffman conditions](https://faq.computersciencewiki.org/index.php/home/article/coffman-conditions)

### AI Use 

AI was first used to design all structures needed for this project, then used throughout the project as a debugging tool. Finally, it was used to design tests to make sure all the requirements of the subject were met.