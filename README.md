# compact MSA

### Build requirements

An C++ Compiler (e.g. GNU G++)

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
-o  output computed file
-t  time (seconds)
-k  knn-queries
-v  verbose output
-h  this help message
```

### Running example:

```c
./main dataset/example.txt -t -k 5 -o
```
```c
dim = 3; n = 5; refs = 2; q = 4
Tempo para indexar:
CLOCK = 0.000134 TIME = 0.000000
########
Encoded size (bits) = 34
########
Tempo para buscas:
CLOCK = 0.000243 TIME = 0.000000
########
malloc_count ### exiting, total: 19,218, peak: 18,670, current: 1,024
```

### Output example:
```sh
$ more dataset/example.out
0 1 2 3 4 
1 2 3 4 0 
0 1 2 3 4 
1 2 3 4 0 
```
