# compact MSA

### Build requirements

An C++ Compiler (e.g. GNU G++ or other C++)

### Install

```sh
git clone https://github.com/universal-cnpq-edc/msa.git
cd msa
make
```

### Input format:

At the first line, we have the following: 

+  *dim*: number of dimensions
+  *refs*: number of reference objects
+  *n*: number of objects
+  *q*: number of queries

The next lines give the reference objects, objects and queries (all given as an array of *dim* integers).

### Input example:
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

### Available options:

```sh
-A a  algorithm to use (default 2)
-o    output computed file
-t    time (seconds)
-k K  knn-queries
-r R  number of (used) references
-v    verbose output
-h    this help message
```

### Running example:

```c
./main dataset/example.txt -t -k 5 -o -A 2
```

```sh
## cMSA ##
Indexing:
CLOCK = 0.000077 TIME = 0.000000
########
Encoded size (bits) = 34
########
Searching:
CLOCK = 0.000287 TIME = 0.000000
########
malloc_count ### exiting, total: 19,258, peak: 18,710, current: 1,024
```

### Output example:
```sh
$ more dataset/example.out
0 1 2 3 4 
1 2 3 4 0 
0 1 2 3 4 
1 2 3 4 0 
```
