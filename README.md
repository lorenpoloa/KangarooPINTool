# KangarooPINTools
Some PIN Tools for Computer Architecture Labs.

This is a tool developed using the [Intel PIN](https://www.intel.com/content/www/us/en/developer/articles/tool/pin-a-dynamic-binary-instrumentation-tool.html) framework.
## Requirements

- Intel PIN (not included, download from the official website)
- Compatible compiler (gcc/g++, clang, etc.)

## License

This project is licensed under the CC0 License.
**Note**: Intel PIN has its own proprietary license. Please refer to Intel's terms before using PIN.


## What is Kangaroo PIN Tools
Is a set of PIN Tools to get stats about jumps indstructions in a program.

### JumpsStatsSimple
Simple stats about the number of conditional jumps instructions and the ratio of taken jumps.

### TypeJumpsStats
More detailed stats about each type of jumps instructions and the ratio of them that are taken.

## How to use

🛠️ Compilación y uso
Guarda este código como JumpsStats.cpp.

Compile:
```bash

make obj-intel64/JumpsStats.so TARGET=intel64 TOOL_ROOTS=JumpsStats
```

Exec with PIN:
```bash

pin -t JumpsStats.so -- ./your_program

```
