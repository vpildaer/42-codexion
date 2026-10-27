*This project has been created as part of the 42 curriculum by vpildaer.*

# Codexion - 42 Belgium

This project is an adaptation of Dijkstra's [Dining Philosophers Problem](https://en.wikipedia.org/wiki/Dining_philosophers_problem). 

## Table of Contents

* [Description](#description)
    * [What is a (p)Thread ?](#what-is-a-pthread-)
    * [Blocking Cases Handled](#blocking-cases-handled)
		* [Deadlock Prevention & Coffman's Conditions](#deadlock-prevention--coffmans-conditions)
		* [Starvation Prevention](#starvation-prevention)
		* [Cooldown Handling](#cooldown-handling)
		* [Precise Burnout Detection](#precise-burnout-detection)
		* [Log Serialization](#log-serialization)
    * [Thread Synchronization Mechanisms](#thread-synchronization-mechanisms)
		* [Threading Primitives](#threading-primitives)
		* [Race Conditions Prevention](#race-conditions-prevention)
		* [Thread-Safe Communication Between Coders and the Monitor](#thread-safe-communication-between-coders-and-the-monitor)

* [Instructions](#instructions)
    * [Running Tests](#running-tests)

* [Resources](#resources)
    * [AI Use](#ai-use)

## Description

This project is an introduction to manipulating [(p)threads](#what-is-a-pthread-) in C language, based on the Dining Philosophers Problem by Dijkstra. 

Where in Dijkstra's problem, the Philosophers sitting around a circular table were sharing forks to eat in-between thinking sessions, in Codexion, we have `n` coders sharing `n` dongles connected to a Quantum Compiler. 

The coders alternatively compile, debug or refactor, in this exact order. They need two dongles to compile, and a dongle needs a cooldown time before being used again. 



### What is a (P)Thread ?

POSIX threads, also known as pthreads after its header `<pthread.h>`, is a model of execution independent from any programming language, allowing a program to control a multitude of workflows that overlap in time.  

Basically, threads (often) perform tasks concurrently. 



### Blocking Cases Handled


#### Deadlock Prevention & Coffman's Conditions

A [deadlock](https://en.wikipedia.org/wiki/Deadlock_(computer_science)) occurs when a thread enters a state of waiting for a resource held by another waiting process.

For a deadlock to happen, the 4 **Coffman's conditions** must be fulfilled :

* **Mutual Exclusion**: no dongle can be simultaneously used by multiple coders
* **Resource Holding**: coders hold a dongle while waiting for the second 
* **Non-Preemption**: no coder can take a dongle from another
* **Circular Wait**: each coder wait for the dongle on the left

To prevent a deadlock from happening, one of the 4 Coffman's conditions has to be neglected. By design, mutual exclusion and non-preemption cannot be neglected, thus leaving only resource holding (Dijkstra's solution) or circular wait (Resource Hierarchy solution) to be neglected. 

During the development of the code, **neglecting circular wait** seemed like the natural choice. 

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

This pointer-order acquisition rule implies that the waiting inside `take_dongle` will always take the lower-address dongle before the higher-address one, regardless of which is left or right (for a given coder). This means that every coder around the table requests resources in the same global order, and not in a circular way. 

That makes it impossible for a cycle of coders to each be holding one dongle while waiting on the next one in the ring (the classic dining-philosophers deadlock), because whichever coder would be “last” in the cycle is actually first in acquisition order and never ends up waiting on someone who’s waiting on them.

#### Starvation Prevention

Starvation prevention comes from the per-dongle priority queue : the heap in `t_dongle.pq`.

I decided to design a per-dongle priority queue rather than a global one for all coders because that seemed more relevant to me. 

The mechanism works as followed: 

When a coder can’t take a dongle immediately, `wait_dongle` first calls `add_to_queue` to register its request (with arrival time and deadline, regardless of whether the scheduler is FIFO or EDF, that checks happens later) before it starts waiting. 

Then its wait condition explicitly checks `peek(&dongle->pq) != coder->coder_id`, meaning even after `release_dongle`’s `pthread_cond_broadcast` wakes every waiter on that dongle, a coder is only allowed to proceed once it’s actually at the front of that dongle’s queue. 

Everyone else loops back into `pthread_cond_timedwait`.

Without this queue check, there would be a starvation risk:

`pthread_cond_broadcast` inside `release_dongle` wakes all waiters, but only one re-acquires the mutex first and nothing would stop the same coder from consistently winning that race.

The queue makes the order of who gets served next an explicit application-level decision, FIFO by arrival order or EDF by earliest deadline, rather than leaving it to whichever thread the OS happens to wake first.

This design also prevents the scenario where a coder's request ends up buried at the end of the queue, behind all others' requests.

#### Cooldown Handling

After each use, a dongle needs a cooldown time before being used again. The cooldown handling happens in `take_dongle`, then in the `wait_dongle` function.

Here is how it works:

1. On release

After a dongle has been used, `release_dongle` timestamps the release time after setting the dongle's status to `AVAILABLE`, before broadcasting (with `pthread_cond_broadcast`) to all waiters the release of the mutex.

2. On acquisition

In `take_dongle`, a dongle cannot be taken just because its status is set to `AVAILABLE`, there are two other checks to be passed, otherwise the coder wanting to take the dongle will be sent to `wait_dongle`. These two checks make sure that the simulation is not in stop mode and that cooldown time has passed:
```
timespec_diff(dongle->release_time, now) 
	< coder->data->parsed_data.cooldown
```
This is where some of the handling takes place. The rest of it is handled in `wait_dongle`, that has the same check as `take_dongle` in its while loop, as well as a real computing of the waiting time passed to `pthread_cond_timedwait` using the cooldown time:
```
wait = get_condwait(dongle->release_time,
		coder->data->parsed_data.cooldown);
pthread_cond_timedwait(&dongle->dongle_cond,
	&dongle->dongle_mutex, &wait);
```
So a coder waiting out a cooldown wakes up right when the cooldown expires.

3. First pass

One subtlety tho ! A dongle that’s never been used at all shouldn’t be penalized by a cooldown it never earned right ?

So `init_coders`/`init_dongles` backdate every dongle’s initial release_time to start_time minus a full cooldown (via `init_time`, with proper borrow-into-tv_sec normalization so the resulting timespec is never invalid). 

That makes `timespec_diff(release_time, now)` already read as “cooldown already elapsed” from `t=0`, so the very first acquisition of any dongle isn’t artificially delayed, while every real release afterward still enforces the full cooldown correctly.

#### Precise Burnout Detection

Precise burnout detection comes from a dedicated monitor thread polling at fine granularity, plus checking the right clock depending on whether the coder has ever compiled.

As required by the subject, rather than having each coder thread detect its own burnout (which would require it to notice the deadline passed while it might be blocked waiting on a dongle, exactly the situation where it’s at risk of burning out), a dedicated thread checks on all coders independently : `monitor_routine`.

It sleeps only for 1ms between each sweep of all coders, which allows the program to respect the precision requirement of displaying burnout logs within 10ms of the actual burnout time.

In `monitor`, there is a two-branch deadline check:

```
if ((timespec_diff(data->all_coders[i].last_compile_start, now)
			>= data->parsed_data.burnout
			&& data->all_coders[i].compiles_count > 0)
	|| (timespec_diff(data->start_time, now) >= data->parsed_data.burnout
			&& data->all_coders[i].compiles_count == 0
			&& data->all_coders[i].coder_status != COMPILING))
```

The two branches differ by which case they check.

A coder that has already compiled at least once is measured against its own last_compile_start (set fresh each time it actually starts compiling, in finish_compile). A coder that hasn’t compiled yet or is mid-first-compile is measured against the simulation’s start_time instead.

If any of these two checks passes, the `simulation_stop` flag is turned on and from that cascades throughout the whole simulation to break all functions.

Then, once threads have joined, `ft_free` is called and all the freeing happens normally.

#### Log Serialization

Log serialization comes down to one shared mutex guarding every `printf` call in the program. 

It is a dedicated mutex, reserved only for all logs (`t_data.log_mutex`), locked before any log is being print and unlocked after every print. 

Since every thread (coder threads and the monitor thread alike) goes through this same mutex before calling printf, no two `printf` calls can ever be mid-flight at the same time.

Without it, two threads calling `printf` concurrently could interleave their output mid-line (`printf`’s internal buffering isn’t atomic across threads), producing garbled combined lines.


### Thread Synchronization Mechanisms

#### Threading Primitives

##### Pthreads

`pthread_t` / `pthread_create` / `pthread_join`

One thread per coder, created in `init_coder_threads` + one monitor thread created in `start` right before the call to `init_coder_threads`.

`pthread_join` is called on all coders right after the `pthread_create` call in `init_coder_threads`, then on the monitor in `start`

##### Mutexes

`pthread_mutex_t`: `dongle_mutex`, `coder_mutex` and `log_mutex`

One `coder_mutex` by coder, protecting a coder's own mutables `coder_status` and `last_compile_start` in `compile` and `finish_compile`, as well as `coder_status` only in `debug_and_refact`.

One `dongle_mutex` by dongle, protecting a dongle's own mutables `dongle_status`, `release_time` and the dongle's priority queue `t_dongle.pq`. Every operation that touches a dongle’s state (`take_dongle`, `wait_dongle`, `release_dongle`) holds this lock, which is also what `pthread_cond_timedwait` requires to be held when called.

One global `log_mutex` (see [Log Serializatiion](#log-serialization))

##### Cond variables

`pthread_cond_t`: `dongle_cond`

One `dongle_cond` per dongle, used as a mean for the coders to wait (with `pthread_cond_timedwait`) for the dongle to be available, then to wake up all of the waiting coders on a particular dongle in `release_dongle` calling `pthread_cond_broadcast`, waking up all waiting threads (and not `pthread_cond_signal` that wakes at least one waiting thread).

`pthread_cond_timedwait` rather than plain `pthread_cond_wait`

A coder isn’t just waiting for “someone to release this dongle,” it’s waiting for the earlier of two conditions: the dongle gets released and it’s at the front of the queue, or its own cooldown-driven deadline simply elapses on its own with nobody broadcasting anything. 

A plain `cond_wait` would never wake up for the second case, only an explicit broadcast/signal wakes it, and nothing broadcasts purely because cooldown expired. That absolute-deadline wake-up is also what lets a coder self-wake out of a cooldown wait without depending on another thread’s timing.

##### Atomic Integers

`atomic_int`: `simulation_stop` and `compiles_count`

These are the two pieces of shared state read very frequently (checked in nearly every function, every loop iteration) but don’t need the richer coordination a mutex gives (they’re single scalars, not compound state, and nothing needs to hold them locked across multiple operations). `atomic_int` gives lock-free, race-free reads and writes without paying mutex overhead on every check. 

Appropriate here specifically because each is just a flag/counter, unlike `coder_status` or `dongle_status` which are read and written together with other fields and genuinely need the mutex’s broader critical section.

#### Race Conditions Prevention

**Data Races** happen when **threads** try to access or modify the same piece of data at the same time.

**Race prevention** here comes down to three separate techniques, matched to three different kinds of shared state:

**Compound state → mutex with the whole operation inside the lock.** The cases that actually need protection are ones where multiple fields have to change together, or where a check-then-act sequence would be unsafe if another thread could sneak in between the check and the act. 

`take_dongle` does `clock_gettime` → check `dongle_status`/`release_time`/cooldown → (maybe wait) → set `dongle_status = TAKEN` → log, all under `dongle_mutex`. If that lock weren't held across the whole sequence, two neighboring coders could both read `dongle_status == AVAILABLE` at the same instant, both pass the check, and both set it to `TAKEN`, a classic check-then-act race that would let the same physical dongle be "held" by two coders simultaneously, exactly what the mutex exists to rule out. 

Same logic for `coder_mutex`: `finish_compile` sets `coder_status = COMPILING` and stamps `last_compile_start` together as one atomic unit, because the monitor thread reads both of those fields together when deciding whether a coder has burned out. If they could be updated separately and read mid-update, the monitor could see a `COMPILING` status paired with a stale timestamp from a previous cycle.

**Simple scalars → `atomic_int`, not a mutex.** `simulation_stop` and `compiles_count` are read and written far more often than any mutex-protected state, by every thread, but each is a single independent value with no companion field that has to change in lockstep. A plain `int` here would be a textbook race (a write from one thread and a read from another with no ordering guarantee, and `compiles_count++` specifically would be a lost-update hazard if two operations interleaved). `atomic_int` gives the read-modify-write and the cross-thread visibility guarantee without needing a full lock for something this simple.

**A condition that can change between "woken up" and "acting on it" → re-check in a loop, never trust a single wake.** `wait_dongle`'s `while` loop is deliberate, not incidental: `pthread_cond_broadcast` wakes *every* waiter on that dongle at once, but only one of them should actually proceed (whichever is at the front of the queue). If the code just did `if (...) cond_wait(...)` once and then assumed the resource was available after waking, that would be a race between "I was woken" and "the thing I actually need is true" 

**One race that actually showed up and got caught by TSan:** the original `start()` created the monitor thread *before* the coder mutexes were initialized, so the monitor could call `pthread_mutex_lock` on a mutex that hadn't been through `pthread_mutex_init` yet : a race between initialization and first use. 

The fix was ordering: all `pthread_mutex_init`/`pthread_cond_init` calls happen in the main thread, synchronously, before any other thread is created via `pthread_create`. 


#### Thread-Safe Communication Between Coders and the Monitor

This is the one subsection where the two threads aren't talking in the same direction — it's worth laying out as two separate channels.

**Coders → Monitor** (the monitor needs to *read* what coders are doing): `coder_status` and `last_compile_start` are written by a coder's own thread (in `finish_compile`/`debug_and_refact`) and read by the monitor thread (in `monitor()`), both under that coder's `coder_mutex`. `compiles_count` is written by the coder thread and read by the monitor too, but as an `atomic_int` rather than through the mutex. The monitor never gets a "stale" or half-updated view of a coder's state, because the coder thread's writes and the monitor's reads can't interleave mid-update.

**Monitor → Coders** (the monitor needs to *tell* every coder thread to stop, all at once): this is `simulation_stop`, an `atomic_int` the monitor sets exactly once, the moment it detects either a burnout or full completion. There's no dedicated "shutdown" condition variable, instead, every coder thread polls this same flag at several checkpoints: the top of `routine`'s loop, the entry check in `take_dongle`, the wait condition inside `wait_dongle`'s `while` loop, and the start of `output_coder` and `debug_and_refact`. 

## Instructions

Thanks to the **Makefile** required by the subject, it is quite easy to run this project. 

At the root of the repository, simply:
```
make
```
and you will get the executable !

All other instructions available are:

```
make re		#cleans and creates a new executable
make clean	#cleans object files
make fclean	#cleans object files and executable
```

### Running Tests

The following tests were run against the project during development.

#### Full Completion without Burnout

```
./codexion 3 100000 20 10 10 5 10 fifo
```

**Result**: once every coder reaches `number_of_compiles_required`, the simulation stops cleanly with no `burned out` message, and any in-flight log for a coder still finishing its cycle is correctly suppressed.

#### Deadlock Avoidance

Increasing the number of coders, decreasing the burnout time, checking that it can be scaled up.

```
./codexion 10 1000 50 10 10 10 10 fifo
./codexion 10 1000 50 10 10 10 10 edf
```

**Result**: same as before.

#### Starvation

Tight burnout window relative to compile/debug/refactor times, so a coder genuinely has to win dongle contention regularly to avoid burning out.

```
./codexion 4 300 50 10 10 20 10 edf
```

**Result**: not tight enough.

Tighter test:

```
./codexion 4 150 50 10 10 20 10 edf
```

**Result**: too tight, coder 4 burns out.

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

Testing data races using the ThreadSanitizer (TSan) tool.

```
gcc -Wall -Wextra -Werror -fsanitize=thread -pthread codexion_dir/*.c -o codexion_tsan  && ./codexion_tsan 3 1000 50 10 10 20 10 fifo
```

**Result**: 0 warning. This confirms there is no data race.

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
* [pthread_cond_wait or pthread_cond_timedwait Subroutine](https://www.ibm.com/docs/pl/aix/7.2.0?topic=p-pthread-cond-wait-pthread-cond-timedwait-subroutine)
* [pthread_cond_signal or pthread_cond_broadcast Subroutine](https://www.ibm.com/docs/en/aix/7.3.0?topic=p-pthread-cond-signal-pthread-cond-broadcast-subroutine)
* [Atomic types](https://en.cppreference.com/c/language/atomic)
* [Heap in C](https://www.geeksforgeeks.org/c/heap-in-c/)
* [How heap work? Heapify algorithm](https://medium.com/@garggourav012/how-heap-work-heapify-algorithm-0bc1fdcede15)
* [Helgrind: a thread error detector](https://valgrind.org/docs/manual/hg-manual.html)
* [Understanding Data Races in Programming: A Quick Overview](https://medium.com/@efebalikofc/understanding-data-races-in-programming-a-quick-overview-1058047c0c6f)
* [Understanding Synchronization Primitives and Techniques in Multithreaded Programming](https://medium.com/@ibrahim__/understanding-synchronization-primitives-and-techniques-in-multithreaded-programming-565b71017d0e)
* [Coffman conditions](https://faq.computersciencewiki.org/index.php/home/article/coffman-conditions)
* [Global interpreter lock](https://en.wikipedia.org/wiki/Global_interpreter_lock)

### AI Use

AI was first used to design all structures needed for this project, then used throughout the project as a debugging tool. Finally, it was used to design tests to make sure all the requirements of the subject were met, as well as a helper during writing this README.md file, that could have been a bit shorter, I confess.