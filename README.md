# Atom reconfiguration algorithms

## Usage

### Compiling
The default build type is `dev`.
```sh
make all <BUILD_TYPE=dev|release|static-analysis>
```

All the executables can then be found in `./bin`

There are four types of executables:
- `./bin/main.out` For custom scripts
- `./bin/unit_test.out` For unit testing
- `./bin/*fuzz_test.out` For fuzz testing
- `./bin/*performance_test.out` For performance testing, the output logs are put in `./performance_results`

### Visualizing performance results
```sh
make visualize file=<path_to_performance_results> [port=<any_unused_port>]
```

## License
Licensed under the [MIT](./LICENSE) license.
