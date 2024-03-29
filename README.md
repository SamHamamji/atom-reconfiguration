# Atom reconfiguration algorithms

## Usage

### Compiling
Default build type is `dev`.
```sh
make all <BUILD_TYPE=dev|release|scan-build>
```

### Running main file
```sh
./bin/main.out
```

### Running unit tests
```sh
./bin/unit_test.out
```

### Running fuzz tests
```sh
./bin/fuzz_test.out
```

### Running performance tests
```sh
./bin/grid_performance_test.out
```

```sh
./bin/linear_performance_test.out
```

### Visualizing performance results
Note that `file` is a required argument, while `port` is optional. 
```sh
make visualize file=<path_to_performance_results> port=<any_unused_port>
```

## License
Licensed under the [MIT](./LICENSE) license.
