#!/bin/bash
#$ -S /bin/bash
#$ -cwd
#$ -e $JOB_ID.err
#$ -o $JOB_ID.out


#ruta del directorio donde se encuentran los benchmark a ejecutar
NPB_BIN_PATH="/share/apps/aca/benchmarks/NPB3.2/NPB3.2-OMP/bin/"
PIN_BIN_PATH="/share/apps/aca/pin-2.12-58423-gcc.4.4.7-linux/pin"
PIN_TOOL_PATH="/home/acab29/ACA_PR3/custom_pin_PR3/source/tools/MyPinTool/obj-intel64/JumpsStats"
CSV_FILE="$PWD/all_results.csv"

#variable para el benchmark que se desea ejecutar
BENCHMARK="is"

export OMP_STACKSIZE=128M


#Ejecucion de los benchmark

#Para el benchmark is en el directorio NPB_BIN_PATH
for class in "W"; do

	#Ejecutar cada benchmark

	echo -e "\n$BENCHMARK.$class\n" >> ./$JOB_ID.err
	$PIN_BIN_PATH -t $PIN_TOOL_PATH -o ./$BENCHMARK.${class}.out -- $NPB_BIN_PATH/$BENCHMARK.${class}
	
done
