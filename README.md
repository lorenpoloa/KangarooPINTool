# KangarooPINTools
Some PIN Tools for Computer Architecture Labs.


This is a tool developed using the [Intel PIN](https://www.intel.com/content/www/us/en/developer/articles/tool/pin-a-dynamic-binary-instrumentation-tool.html) framework.
## Requirements

- Intel PIN (not included, download from the official website)
- Compatible compiler (gcc/g++, clang, etc.)


## What is Kangaroo PIN Tools
Is a set of PIN Tools to get stats about jumps instructions in a program.

### JumpsStatsSimple
Simple stats about the number of conditional jumps instructions and the ratio of taken jumps.

### JumpsStatsByType
More detailed stats about each type of jumps instructions and the ratio of them that are taken.

### JumpsStatsWithPredictor
Stats about differents jumps predictors simulations

## How to use

🛠️ Compiling and Using <br>


Compile:
```bash

make obj-intel64/JumpsStatsSimple.so TARGET=intel64 TOOL_ROOTS=JumpsStats
```

Exec with PIN:
```bash

pin -t JumpsStatsSimple.so -- ./your_program

```


## License

This project is licensed under the CC0 License.
**Note**: Intel PIN has its own proprietary license. Please refer to Intel's terms before using PIN.
