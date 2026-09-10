*This project has been created as part of the 42 curriculum by [aelhadja](https://github.com/achrafelhadjaoui/codexion).*

## Description
Codexion is a concurrency project simulating developers competing for limited shared resources (USB dongles) in order to compile, debug, and refactor code.
The goal is to manage thread synchronization, prevent deadlocks, handle resource cooldowns, avoid starvation, and detect burnout conditions precisely using custom heap scheduling algorithms (FIFO/EDF).

## Compilation
Compile the program using the provided `Makefile`:
make

## Resources
* AI Usage: explanations for concurrency concepts.

## Blocking Cases Handled
* Deadlocks: Controlled resource locking order to avoid circular wait issues.
* Starvation: Used custom priority heaps to ensure fair access to dongles under fifo and edf schedulers.
* Cooldown handling: Kept dongles locked for the specific cooldown duration after release.
* Precise burnout detection: Used a dedicated monitor thread to detect and print burnout logs within 10 ms.
* Log serialization: Used a mutex to protect output so log messages never mix up on the same line.

## Thread Synchronization Mechanisms
* pthread_mutex_t: Used to protect shared resources like dongles, logs, and queue states from race conditions.
* pthread_cond_t: Used alongside mutexes to manage waiting lists when coders wait for dongles.
* Custom communication: Managed safe communication between the coder threads and the monitor thread.