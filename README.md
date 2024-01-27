# Atom reconfiguration algorithms

## Usage

### Compiling
Default build type is `dev`.
```sh
make all <build-type=dev|release|scan-build>
```

### Running main file
```sh
./build/main.out
```

### Running unit tests
```sh
./build/unit-test.out
```

### Running fuzz tests
```sh
./build/fuzz-test.out
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

## License
Licensed under the [MIT](./LICENSE) license.
