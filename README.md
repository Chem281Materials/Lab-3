# 💻 CHEM 281 - Development Lifecycle and Debugging

## Orientation

In this lab, we will focus on preparing you for your journey into containerized C++ development using cmake and other build tools. In the first part of this exercise, you will create a Dockerfile that allows you to build an image that encapsulates your C++ development environment. Though it's not the case that every development project can be achieved using the same image (projects may require different sets of depencies, for example, or versions of dependencies), it's the case that many projects may require a common set of software to develop (e.g. a compiler, or build tools like cmake). In this project we will build a base docker image which you can use for development during later exercises in this course. We will also illustrate how you can _extend_ that base development image to create new images that contain project specific dependencies like BLAS. Afterward, you will implement a simple linear algebra library and then write a test suite for it using the GoogleTest framework. Lastly, you will use valgrind to debug some memory leaks in a dummy database. If you have extra time left over, explore the `gcov` and `lcov` code coverage package.

> [!TIP]
> For this lab, it will be very valuable for everyone to perform the instructions on their own machines at the same time. Help each other debug problems that crop up. You can use the development images you produce here in future assignments.

### Part 1: Building your base development image

#### Background

A "development image" is simply an image that contains the dependencies required to develop your software. This could entail any or all of the following:

- A compiler
- Build tools like cmake
- A testing harness
- Code coverage tools
- A debugger
- Version control (if you are going to development inside your container)

The minimum depenendcies for this lab have been added and created for you!

Feel free to include anything you think will be useful in your development container, remember: you can use and develop this image throughout the semester!

Currently it includes a compiler (g++), make, cmake, lcov, and valgrind. Check out the [build-essentials](https://packages.debian.org/sid/build-essential) meta-package as well. This is a bundle of useful tools for C++ development. Though it also includes some extras, which are related to generating Debian packages,(which goes against our small-image ethos), this image is meant for your personal use, so you don't need to be super strict about size.

#### Tasks

1. Build your development image and tag it with `cppdev/latest`. *NOTE*: You can specify the dockerfile when you build like this:

```docker build -t cppdev/latest -f DevelopmentBase.Dockerfile .```

2. Add a line to `develop_container.sh` that runs an interactive container based on your image and mounts the `example_library` directory to a directory in your container (I recommend `/workspace` - avoid using the filesystem root).
3. Using the interactive container, navigate to the `src` directory (inside `example_library` on the host, but inside `/workspace` in the container) and create a new file called `README.md` with contents "Linear Algebra Tools". Confirm that this file appears on the host filesystem.

### Part 2: Creating a specific development image that includes BLAS

In some cases (and in this example), you will be developing a package that has requirements beyond the development tools in your base development image. For example, in this lab, we will be writing a small C++ program that utilizes functionality from BLAS. Not every program you write may require BLAS, but you will need it for this project, so it needs to be inside your development container. 

#### Tasks

1. Create a new image definition in DevelopmentBLAS.Dockerfile that extends your general base image from Part 1
2. Add an installation of the BLAS development dependency, libblas-dev to this image
3. Build this image, and tag it with `cppdev/blas`
3. Update your `develop_container.sh` script so that it runs a container based on this new `cppdev/blas` image instead of the original `cppdev/latest`.

### Part 3: Writing a small program in your development image that uses BLAS

For this section, we will implement a few functions, as if we were writing a library for ourselves. The `example_library/src` directory contains a couple files:

- linear_alg.cpp: this is where you will complete the function definitions
- main.cpp: this file can be compiled to an executable to run one of the linear_alg functions, no need to change it.

To finish our library, you need to complete the implementations of the following functions, all found in `example_library/src/linear_alg.cpp`:

1. `calculateTwoNorm`: Calculates the 2-norm of a std::vector. Use the BLAS implementation for this, `cblas_dnrm2`.
2. `calculateDotProduct`: Calculates the dot product of two std::vectors. Use the BLAS dot product implementation, `cblas_ddot` for this.
3. `calculateProjection`: Calculates the projection of one vector onto another. Use the two other functions you just implemented for this.

- Recall the scalar projection of vector a onto b is given by: (a dot b) / norm(b)
- You can access the BLAS functions using `#include <cblas.h>`

4. Complete the empty parts of the `example_library/CMakeLists.txt` file. Use `find_package(BLAS REQUIRED)` to ensure the BLAS dependency is included in the compiled output.

### Part 4: Writing unit tests using the Google Testing framework

Depending on your personality, writing tests can feel boring or comforting or anywhere in between. Regardless, it is a foundational aspect of software development. If you are interviewed for a job by a company that does no automated testing, I recommend vigorously searching for a different position. Without automated tests it is fantastically difficult to be aware of regressions that occur with each change to large codebase. As a result, even scientists and engineers who think of test writing as boring or secondary will come to enjoy the security guaranteed by a thorough test suite.

[GoogleTest information can be found here!](https://google.github.io/googletest/primer.html)

To ensure that your function implementations are correct, use the Google Test Framework to write test cases for your projection function.

To start, try running the tests. Do accomplish this, first compile the code:

```
mkdir build
cd build
cmake ..
make
```

An executable called `LinearAlgTest` should have been created in the `build` directory. When you run it, you should see something like this:

```
root@9ae1d023b024:/workspace/build# ./LinearAlgTest
Running main() from /workspace/build/_deps/googletest-src/googletest/src/gtest_main.cc
[==========] Running 3 tests from 3 test suites.
[----------] Global test environment set-up.
[----------] 1 test from TwoNormTests
[ RUN      ] TwoNormTests.BasicAssertions
/workspace/test/linear_alg_test.cpp:7: Failure
Expected equality of these values:
  calculateTwoNorm(v1)
    Which is: 1
  1.1
[  FAILED  ] TwoNormTests.BasicAssertions (0 ms)
[----------] 1 test from TwoNormTests (0 ms total)

[----------] 1 test from DotProductTests
[ RUN      ] DotProductTests.BasicAssertions
[       OK ] DotProductTests.BasicAssertions (0 ms)
[----------] 1 test from DotProductTests (0 ms total)

[----------] 1 test from ProjectionTests
[ RUN      ] ProjectionTests.BasicAssertions
[       OK ] ProjectionTests.BasicAssertions (0 ms)
[----------] 1 test from ProjectionTests (0 ms total)

[----------] Global test environment tear-down
[==========] 3 tests from 3 test suites ran. (0 ms total)
[  PASSED  ] 2 tests.
[  FAILED  ] 1 test, listed below:
[  FAILED  ] TwoNormTests.BasicAssertions
```

A test failed! Find the file `example_library/test/linear_alg_test.cpp` and fix it. To rerun the tests, recompile the code, and run `LinearAlgTest` again.

For now, add the following test cases:

For `calculateTwoNorm`:

1. Ensure the norm of the zero vector is zero
2. Ensure the norm of the Cartesian unit basis vector (1,0) is 1
3. Ensure the norm of (1,1) is the square root of two

Ensure `calculateDotProduct` gives the correct results for the following cases:

Ensure `calculateProjection` gives the correct results for the following cases:

1. Two parallel vectors of equal length
2. Two orthogonal vectors
3. Vectors of equal length at a 45 degree angle to eachother

Invoke your tests using `ctest` or `make test`.

### Part 5: Debug using valgrind
Memory leaks are very difficult and annoying class of bugs caused by improper memory mangement. They arise when memory is not allocated and deallocated correctly, such that they are persistant even after the program has finished executing. Look at `src/db_debug.cpp`, it is a simple program that contains 2 classes, one for creating a record and the other for storing them in a database.

To compile and run:
```
cd src
g++ -g -O0 db_debug.cpp -o db_debug
./db_debug
```

You should see the following output:
```
Alice: 30
Bob: 25
Charlie: 40
```

And everything looks good? Not quite, there are subtle memory bugs that exist, to find where they occur and how much memory is being leaked we can use valgrind:

```
valgrind --leak-check=full --track-origins=yes ./db_debug
```

There are 2 memory leaks in this code, as you debug and fix them the memory being leaked will decrease! Find and fix both the leaks.

```
==571== LEAK SUMMARY:
==571==    definitely lost: 120 bytes in 3 blocks
==571==    indirectly lost: 12 bytes in 3 blocks
==571==      possibly lost: 0 bytes in 0 blocks
==571==    still reachable: 0 bytes in 0 blocks
==571==         suppressed: 0 bytes in 0 blocks
```

### Extra time: Assessing test coverage using lcov

Test coverage metrics are an important tool for inspecting the completeness of the test suite that you've written. These tools tell you the fraction of your source code that is executed when your test suite runs. If a line of code is not run during the test suite, it follows that there are no assertions in the test suite that depend on its behavior - in other words, it is untested, and if a commit inadvertently changes or breaks its behavior, no one will be the wiser until a bug report from a disgruntled user comes floating in.

Different organizations will adopt differing testing philosophies, that is, what to test, how to test it, when the tests should be run, and when the tests should be written.

The most optimistic of engineers sometimes strive for 100% test coverage. In this case, every line of source code is invoked by the test runner. This doesn't guarantee that the test suite thoroughly the confirms behavior of every corner case in your development project, but it at least means that there are no completely blind spots in your project.

In your CMakeLists.txt file we are installing LCOV, which builds on top of GCOV and gives us more comprehensive reporting. To build and run:
```
cmake .. -DCMAKE_BUILD_TYPE=Debug
make
`ctest` or `make test` (If you follow our gtest set up)
make coverage

View build/out/index.html
```

You'll notice that you need to run the tests before you try to get the coverage and that if you run any executables that use the code you are testing your coverage report will include that.
