# Dining Philosophers Problem Solution (phsp.c)

## Description

This C program implements a solution for the Dining Philosophers Problem using Pthreads, mutex, and condition variables. The solution is deadlock-free and supports maximum concurrency. Philosophers are represented as threads, and they go through a cycle of thinking, attempting to pick up chopsticks, eating, and putting down chopsticks.

## Usage

Compile the program using the following command:

```bash
gcc -o phsp phsp.c -lpthread -lm

Run the program with the following arguments:

./phsp <num_phsp> <min_think> <max_think> <min_dine> <max_dine> <dst> <num>

<num_phsp>: Number of philosophers (threads).
<min_think>: Minimum thinking time in milliseconds.
<max_think>: Maximum thinking time in milliseconds.
<min_dine>: Minimum dining time in milliseconds.
<max_dine>: Maximum dining time in milliseconds.
<dst>: Distribution type (either "uniform" or "exponential").
<num>: Number of dining times for each philosopher.

Example:

## Random Value Generation
Random values for thinking and dining times are generated based on the specified distribution. The random_range function generates uniformly distributed random values, and the exponential distribution is achieved using the inverse transform method.

## Measurement of Hungry State Duration
The program measures the duration of the hungry state for each philosopher. This information can be used to calculate the average hungry state and standard deviation.

## Compilation and Execution
Make sure to adjust the compilation command according to your system if needed. The program will terminate after all philosophers complete dining the specified number of times.