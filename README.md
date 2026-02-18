Use: ./SOchain init_balance n_wallets n_servers buff_size max_txs
Example: ./SOchain 100.0 2 1 5 5


SOCHAIN — Simplified Transaction Chain (Academic Project)
=======================================================

Description
-----------
SOCHAIN is a C project that implements a simplified simulation of a transaction ledger. The project manages wallets, servers and transactions, records precise timing for transaction events, and demonstrates concurrent communication and process handling.

Key features
------------
- Management of wallets and servers (processes).
- Recording of transaction start and end timestamps.
- A dedicated thread that periodically prints transaction information.
- Use of shared memory and dynamic data structures.

Where to look in the code
-------------------------
- Timing and printing logic: [src/csignal.c](src/csignal.c)
- Main entry point and program flow: [src/main.c](src/main.c)
- Wallets / servers / memory implementations: [src/*.c](src/)

Build and run
-------------
This repository includes a `makefile` for Unix-like systems. On Windows you can use MinGW/Make or compile manually with your preferred compiler.

Build with make (Linux / macOS / MinGW):

```sh
make
./bin/SOchain_profs 
```


Transaction monitoring thread
----------------------------
The project has a dedicated thread that runs a `while` loop and prints the elapsed time for each active transaction. The thread is started via `start_print_transactions_info(int period, struct info_container *info)` (implemented in `src/csignal.c` and declared in `inc/csignal.h`).

Implementation notes
----------------------------------
- The control structures (`txi`) are initialized by `initialize_transaction_info(max_txs)`; ensure `start` and `end` arrays are allocated before use.
- The thread checks `info->terminate` to exit gracefully.






