# Eluvio Coding Challenge

## Design

Each request will be processed by the main thread which will be in a loop taking in single/batches of requests and putting it into buffer

Each thread will pull from the buffer


### Data Structures

Data Struct

```
struct item {
  char *uuid;
  char *data;
};
```


items have to be in a list of some sort so that we can iterate through it
- fifo? eh
 - sync between threads at that level might need another thread?
- need to have



limitations of have a singly linked list for status dictionary
