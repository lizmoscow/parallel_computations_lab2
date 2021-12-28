# This is my second lab work for the Parallel Computations course

I have implemented a BFS algorithm both sequentially and in parallel. 

To start this code do

```
mkdir build
cd build
cmake ../source
make
./ParallelComputations
```

An example run of this code on computer with X86 architecture:
```
@./BFS cube500 500 5 out
Iteration done in 20103 milliseconds!
Iteration done in 19190 milliseconds!
Iteration done in 19188 milliseconds!
Iteration done in 19192 milliseconds!
Iteration done in 19095 milliseconds!

Sequential BFS took 19353 milliseconds

Iteration done in 7989 milliseconds!
Iteration done in 7886 milliseconds!
Iteration done in 7956 milliseconds!
Iteration done in 8021 milliseconds!
Iteration done in 7909 milliseconds!

Parallel BFS took 7952 milliseconds
```