# Proyecto Inteligencia Artificial 2021-2 (ETP Greedy - HCFI)

Para ejecutar el algoritmo:

1) Clone el repositorio
2) Ubique el repositorio en la terminal
3) ejecute el siguiente comando:  make run i=instancia , donde instancia es el set de datos a utilizar. Si por ejemplo, se quiere utilizar el set de datos Carleton91, el comando sería -> make run i=Carleton91
4) El programa creara 3 archivos:
- problema.sol : contiene el timeslot asignado a cada examen del set de datos de la instancia
- problema.res : contiene la cantidad de timeslots generada por el algoritmo para dicha instancia
- problema.pen : contiene la penalización promedio de la planificación generada por el programa.

nota: Para que funcione el comando, los archivos .exm y .stu de la instancia deben estar dentro de la carpeta "instances" presente en el repositorio.
