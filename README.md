# compact MSA

## Build requirements

An C++ Compiler (e.g. GNU G++)

**Compilation:**

```sh
make
```

**Input format:**
At the first line, we have the following:  
  *dim*, number of dimensions
  *refs*, number of reference objects
  *n*, number of objects
  *q*, number of queries

Then, each of the reference object is given as array of *dim* integers, followed by the *n* objects and at the end we have the *q* queries.

**Input example:**
```
3 2 5 4
1 1 1
0 0 1
0 0 0
1 0 1
0 1 0
1 0 0 
3 1 3
0 -1 0
1 1 1
0 0 0
1 2 1
```

**Available options:**

```sh
-o  output computed file
-t  time (seconds)
-k  knn-queries
-v  verbose output
-h  this help message
```

**Running example:**

```c
./main dataset/example.txt -t -k 5 -o
```


**Output example:**
0 1 2 3 4 
1 2 3 4 0 
0 1 2 3 4 
1 2 3 4 0 
