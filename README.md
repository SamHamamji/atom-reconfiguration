# Linear assignment problem
Algorithms solving the 1d transportation problem on a line

## Usage:

### Main file
```sh
make build/main.out
./build/main.out
```

### Unit tests
```sh
make build/unit-test.out
./build/unit-test.out
```

### Performance tests
```sh
make build/performance-test.out
./build/performance-test.out
```

### Performance visualization
Note that `file` is a required argument, while `port` is optional. 
```sh
make visualize file=<path-to-performance-results> port=<any-unused-port>
```

## References
```
Karp, Richard M. and Shuo-Yen R. Li. 1975. "Two Special Cases of the Assignment Problem." Discrete Mathematics 13 (2): 129-142.
Aggarwal, A., A. Bar-Noy, S. Khuller, D. Kravets, and B. Schieber. 1992. "Efficient Minimum Cost Matching using Quadrangle Inequality."IEEE Comput. Soc. Press. doi:10.1109/SFCS.1992.267793.
```
