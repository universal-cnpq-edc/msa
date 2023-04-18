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

### Output example:
```c
0 1 2 3 4 
1 2 3 4 0 
0 1 2 3 4 
1 2 3 4 0 
```
