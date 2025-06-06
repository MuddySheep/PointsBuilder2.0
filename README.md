# Points Builder

A library for generating and storing the X coordinates of secp256k1 points, associated with consecutive scalars.

This library and the demonstrative CLI binary were tested to be compatible with:

- Ubuntu 22.04;
- macOS (Apple Silicon).

To compile and run on Windows, you might try with WSL (not tested, but likely works).

## Features

- extremely fast batched addition, built using libsecp256k1 primitives;
- batches are built from a middle-point + left and right additions, using the common inverse;
- multi-threading: the range is split into N distinct working areas
- automatic computation of how many launches are required to fully scan an entire range;
- use a minimal number of EC point multiplication operations:
  - one multiplication, for the first group element in the range;
  - a second multiplication, for the distance between the thread workers;
- SIMD-friendly tree computations for batch inversion
  - independent up-sweep products, and independent down-sweep inverted values allow parallelized work;
- streamable results with arbitrary trade-off about how much memory to use;
- store results into an SQLite database file (this is a bottleneck as writing can only be done from a single thread, or
   serialized anyway if doing it under multi-threading mode)

## Build

1. Clone and build [libsecp256k1](https://github.com/bitcoin-core/secp256k1) if you don't already have it.
2. Edit `tasks/build_utils` and set the correct path to the location of your libsecp256k1.
3. Run `tasks/build_release` to build the program.

On macOS, you will need to install OpenMP and GMP support via homebrew, or else the build will fail:
```shell
brew install gmp libomp
```

## Usage

Command line arguments:

| Arg            | Description.                                                                                 |
|----------------|----------------------------------------------------------------------------------------------|
| -b BASE_KEY    | Hexidecimal. Either the first scalar, or the first public key of the range.                  |
| -s RANGE_SIZE  | Range size for which to generate points, in decimal and < 2**64                              |
| -n NUM_LOOPS   | Number of batched additions, per thread, per launch.                                         |
| -t NUM_THREADS | Number of threads to use.                                                                    |
| -p NUM_SECONDS | Minimum time interval for printing progress update.<br/>Set to 0 to disable progress output. |
| -o DB_NAME     | Optional. Database filename in which to store X bytes and key offsets.                       |

To benchmark the performance, do not specify an output database.

If the base key is longer than 64 characters, then it is interpreted as a serialized
public key (either compressed or uncompressed). In this case, the user is responsible
for ensuring that the public key does not lie in the interval (N - rangeSize ... N).

Example high performance usage, that computes all public keys from scalars 1 to
1 billion. The threads are bound to the first 4 physical cores, to prevent
context switching. Hyper-threading logical cores are not used.

```shell
export OMP_PLACES="{0},{2},{4},{6}"
export OMP_PROC_BIND=close

taskset -c 0,2,4,6 \
./pb -b 1 -s 1000000000 -t 4 -n 4
```

Using more loops only affects how much work is being done by each thread, during
a launch phase, and hence requires more RAM to store the results. However, this
may not really help much in terms of performance, or might even decrease it.

The code can be tweaked to experiment with a larger constant points array.

Different values may yield better or worse performance, depending on the C compiler,
the CPU specs, and so on.

## Is it fast or not?

For the point generation: this is most likely one of the fastest possible CPU-based
libraries that can effectively compute all the points in a sequential range of scalars.

Some benchmarks:

i9 13900H: 18 MK/s when using 1 thread and 4 loops; 78 MK/s using 6 P-threads.

The performance per core may vary by the CPU's cache size. In some cases, using less cores
but more loops, may show a larger through-put than using all cores.

However, saving the results from memory into a SQLite database is the real bottleneck anyway,
but it is likely the best way to actually store the results in the fastest way,
which allows later indexing by the X coordinate bytes (to get the scalar).

YMMV depending on your CPU, memory, storage type, and so on.
