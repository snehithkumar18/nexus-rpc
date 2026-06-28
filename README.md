# NexusRPC

NexusRPC is a high-performance, stateful RPC message broker and document routing engine written in C++. It is designed for low-latency, real-time message dissemination and structured document query routing in distributed environments.

## Features
* **Stateful Session Management:** Tracks client connections, authentication states, and subscriptions.
* **Custom Binary Protocol:** Low-overhead packet serialization and deserialization.
* **Document Routing Engine:** Evaluates SQL-like queries against message payloads to dynamically route documents to matched subscribers.
* **Topic Subscription Trie:** Prefix-tree topic matcher supporting single-level (`+`) and multi-level (`#`) wildcards.
* **Packet Defragmenter:** Reassembles fragmented, multi-part payloads.
* **Trace Ring Buffer:** High-throughput circular logging buffer for diagnostics.

## Codebase Organization
The project is organized as follows:
* `src/` — Core implementation files:
  * `broker.h` / `broker.cc`: Main message broker orchestrating all client actions and routing.
  * `session_manager.h` / `session_manager.cc`: Manages connection sessions and authentication.
  * `subscription_trie.h` / `subscription_trie.cc`: Topic matcher prefix tree.
  * `packet_parser.h` / `packet_parser.cc`: Binary packet encoder and decoder.
  * `packet_assembler.h` / `packet_assembler.cc`: Fragmented packet reassembler.
  * `routing_engine.h` / `routing_engine.cc`: SQL-like query evaluation engine.
  * `ring_buffer.h` / `ring_buffer.cc`: Diagnostics trace logging buffer.
  * `payload.h` / `payload.cc`: Variant type container for message payloads.
  * `storage_engine.h` / `storage_engine.cc`: Persistence and transaction logging layer.
  * `utils.h`: Hashing, base64, and utility helpers.
* `fuzz/` — Fuzzing harnesses:
  * `fuzz_nexus_broker.cc`: Exercises the entire message broker and session states.
  * `fuzz_packet_parser.cc`: Fuzzes the packet parser deserialization logic.
* `.clusterfuzzlite/` — Integration configuration:
  * `Dockerfile`: Container image configuration for building the project under sanitizers.
  * `build.sh`: Build script used by the platform to compile the codebase and link the fuzzers.

## Building and Testing
The project uses CMake for building. To compile:

```bash
mkdir build
cd build
cmake ..
make
```
