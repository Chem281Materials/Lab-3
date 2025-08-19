# Use Ubuntu 22.04 as the base image
FROM ubuntu:22.04

RUN apt-get clean && \
    apt-get update && \
    apt-get install -y \
                       cmake \
                       vim \
                       g++ \
                       git \
                       lcov \
                       valgrind

# Set up a working directory
WORKDIR /workspace

# Set the default command to bash
CMD ["/bin/bash"]
