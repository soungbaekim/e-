# Eluvio Coding Challenge

## Overview

Each request will be first processed by the main thread. The main thread will create a new thread to be responsible for this request. The new thread will first check the cache (status) then call curl functions if data has not been requested yet.

If it has been requested, the thread will wait for the data to be placed into the struct.

## Data Structures

I've decided to go pretty simple on the data structures since I was more focused on the multithreading aspect of the program.

Each uuid is put into a struct with some other metadata that connected to other structs by a singly linked list that any thread can access through status.c. Since this list is shared amongst different threads, semaphores and mutex locks had to be used to control data access.

## Limitations
Since the status struct are all in a singly linked list, it takes a considerable time to traverse through the entire list every time a request is made. This distance get increasing worse as more requests with new uuids come in since it will grow the status list. This can be improved upon by creating a data structure to index the uuids such that it takes fewer comparisons to find the correct struct.

I've also noticed that here and there, one of the result would come out NULL. I believe that this is caused by the delay in receiving data from the request. Since this happens very rarely, I did not implementing a fix for it. However, if I was to implement a fix, I would add another semaphore to each struct elem such that the parent thread knows when the write_callback has finished.

## Building and Running
This was my first stab at writing my own Makefile so please excuse the mess.

`make`: builds executables for normal `run` and `debug`\
`make run`: runs the program without DEBUG_PRINT\
`make debug`: runs the program with DEBUG_PRINT\
`make test`: runs the program and sets input as res/uuids.txt\
`make clean`: discards /build directory\
`exit`: exits the program (after `make run`)

The program can take in as many inputs as it wants as long as the uuids are separated by a whitespace. Thus, the user can input a single uuid or a batch of uuids (separated by whitespace).

## Note
Since threads are independently scheduled by the operating system, outputs may not be in the same order as input
For example, inputting `1 2 3 4 5 6 7 8 9 10` after running `make run` may return results not in ascending order.




