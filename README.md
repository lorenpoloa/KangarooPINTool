![Logo](https://github.com/lorenpoloa/KangarooPINTools/blob/main/Logo1_1_mini.png)


# KangarooPINTool
Some PIN Tool for Computer Architecture Labs.


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


## JumpsStatsByType 📘 Manual y Descripción

📝 Descripción General
JumpsStatsByType es una herramienta de instrumentación desarrollada con Intel® Pin Tool. Su objetivo es recolectar estadísticas detalladas de instrucciones de salto (branch instructions) ejecutadas por un programa, clasificadas por tipo de salto (por ejemplo: je, jne, jmp, call, etc.).

La herramienta analiza si cada salto fue tomado o no tomado, y presenta un resumen tanto por tipo de instrucción como global.

### 🔍 ¿Qué mide?
Por cada tipo de instrucción de salto (como je, jmp, jne, etc.), la herramienta reporta:

Número de veces que el salto fue tomado.

Número de veces que el salto no fue tomado.

Total de veces que se ejecutó esa instrucción de salto.

Porcentaje de veces que el salto fue tomado.

Además, proporciona un resumen total de todos los saltos encontrados durante la ejecución del programa.

###🏗️ Compilación
Asegúrate de tener el entorno de Intel® Pin configurado correctamente.

Guarda el código fuente como jumps_stats_by_type.cpp.

Compílalo con el sistema de compilación de Pin (Makefile o línea de comandos), por ejemplo:

```bash

make obj-intel64/JumpsStatsByType.so TARGET=intel64

```

### ▶️ Uso
Para usar la herramienta, ejecuta tu programa objetivo con Pin y esta herramienta como sigue:

```bash

pin -t obj-intel64/JumpsStatsByType.so -o resultados.txt -- ./mi_programa

```

Parámetros
```
-t obj-intel64/JumpsStatsByType.so: ruta al archivo compilado de la herramienta.

-o resultados.txt: (opcional) nombre del archivo de salida. Si se omite, la salida se mostrará en la consola.

-- ./mi_programa: el programa que deseas instrumentar y analizar.
```

### 📤 Ejemplo de salida
```text

======= Estadisticas de saltos =======
Tipo
	Tomados
	No tomados
	Total
	Porcentaje Tomado
type: je
	Taken: 152
	Not-Taken: 389
	Total: 541
	Percent-Taken: 28.08%
type: jmp
	Taken: 742
	Not-Taken: 0
	Total: 742
	Percent-Taken: 100.00%
...
=======================================
Total:
  Tomados     = 1234
  No tomados  = 456
  Saltos totales = 1690

```

### ⚠️ Consideraciones
La herramienta solo cuenta instrucciones de tipo salto (branches) y las clasifica por su mnemonic (por ejemplo, jne, jmp).

Funciona con programas binarios compatibles con Pin (normalmente en formato ELF o PE).

No diferencia entre saltos directos e indirectos, pero puedes extenderla para hacerlo.


## License

This project is licensed under the CC0 License.
**Note**: Intel PIN has its own proprietary license. Please refer to Intel's terms before using PIN.
