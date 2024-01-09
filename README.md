# Atom reconfiguration algorithms

## Usage

### Compiling
Default build type is `dev`.
```sh
make all <build-type=dev|release|pedantic|scan-build>
```

### Running main file
```sh
./build/main.out
```

### Running unit tests
```sh
./build/unit-test.out
```

### Running performance tests
```sh
./build/performance-test.out
```

### Visualizing performance results
Note that `file` is a required argument, while `port` is optional. 
```sh
make visualize file=<path-to-performance-results> port=<any-unused-port>
```

## References
```
Karp, Richard M. and Shuo-Yen R. Li. 1975. "Two Special Cases of the Assignment Problem." Discrete Mathematics 13 (2): 129-142.
Aggarwal, A., A. Bar-Noy, S. Khuller, D. Kravets, and B. Schieber. 1992. "Efficient Minimum Cost Matching using Quadrangle Inequality."IEEE Comput. Soc. Press. doi:10.1109/SFCS.1992.267793.
```
