# Redis Clone

A Redis-inspired in-memory key-value database built from scratch in C++.

This project explores the core concepts behind modern in-memory databases, including networking, command processing, data storage, persistence, and performance-oriented systems design.

## Motivation

Redis is one of the most widely used databases for caching, session storage, rate limiting, message queues, and real-time applications.

Rather than using Redis as a black box, this project aims to understand and implement the underlying mechanisms that make Redis fast and reliable.

## Features

Current functionality:

* In-memory key-value storage
* TCP server for client communication
* Command parsing and execution
* SET command
* GET command
* DEL command
* EXISTS command
* PING command

Planned functionality:

* Redis Serialization Protocol (RESP)
* Key expiration (TTL)
* Snapshot persistence
* Append-only file (AOF) persistence
* Transactions
* Publish/Subscribe messaging
* Replication
* Benchmarking suite

## Example

```text
SET name Kamsy
OK

GET name
Kamsy

EXISTS name
1

DEL name
1

PING
PONG
```

## Getting Started

### Prerequisites

* C++17 or later
* CMake 3.16+
* GCC, Clang, or MSVC

### Build

```bash
git clone https://github.com/aweony/redis-clone.git

cd redis-clone

mkdir build
cd build

cmake ..
cmake --build .
```

### Run

```bash
./redis_clone
```

## Project Structure

```text
redis-clone/
├── src/
├── tests/
├── docs/
├── CMakeLists.txt
├── README.md
├── LICENSE
└── .gitignore
```

## Documentation

Additional project documentation can be found below:

* Design Document: [Redis Clone Design Document](file:///C:/Users/kamsy/Downloads/Rebis%20clone%20desgin%20doc.pdf)

## Testing

Tests can be executed using:

```bash
ctest
```

Additional test coverage and benchmarking tools will be added as development progresses.

## Roadmap

### Phase 1

* Core key-value storage
* TCP networking
* Basic commands

### Phase 2

* RESP protocol
* TTL support
* Persistence

### Phase 3

* Replication
* Concurrency improvements
* Performance benchmarking

### Phase 4

* Distributed features
* Cluster support
* Monitoring

## License

This project is licensed under the MIT License. See the LICENSE file for details.
