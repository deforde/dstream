# dstream

Stream data from a C/C++ application, in real-time, to a server that plots the data via `ImGui`.

`dstream` consists of two software components: a client-side library and a server-side application.

The client library can be used to stream data from within a C/C++ application, via Unix domain sockets, to a `dstream` application instance, for subequent realtime plotting.
The client library is designed to be as simple as possible to use, such that it can be integrated with extant projects very easily as a routine part of the debugging process.
For an example of a client implementation, please see: `examples/client/client.c`.

The `dstream` application server will accept a single client connection and will process the data packets received over said connection in order to extract and plot the data contained within.

`dstream` makes use of `ImGui` and `ImPlot` to generate the data plots (the rendering backend used is `SDL2` and `OpenGL3`).

# Building and Installation

To build `dstream`, simply run:
```
apt-get install libsdl2-dev
make
```

To install the `dstream` application, client libraries, and include files, simply run `make install` (to remove, run: `make uninstall`).
All `dstream` related files will be installed to `/usr/local`.

To build a redistributable `dstream` archive, run: `make pkg`.

# Examples

For an example of a `dstream` client implementation, see `examples/client`. To build and run the example, run: `make run`. Then, in a separate terminal, run `dstream` to recieve the data stream.

An example of how data can be streamed to `dstream` from Python code is shown in `examples/pyclient`. To run this example, simply execute the contained file directly: `./client.py` and run a separate `dstream` instance as per usual.

# Tests

Run `make test` to build and execute all `dstream` tests.

-------------------------------------------------------------------------------

### Future Work

I am working on a way to use `gdb` and Python extensions thereto in order to extract data from a C/C++ application without needing to modify the source code to use `dstream` client functions.

The beginnings of this work can be found in `examples/gdb` and in `src/config.c`.
