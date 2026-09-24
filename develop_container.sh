#!/bin/bash
podman run -it -v $(pwd)/example_library/:/workspace cppdev/latest /bin/bash
