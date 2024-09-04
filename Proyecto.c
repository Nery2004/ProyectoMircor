#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <float.h>

#define MAX_LINE_LENGTH 1000  // Definicion de la longitud maxima de una linea en el archivo

int main() {
    int opcion;  // Variable para almacenar la opcion seleccionada por el usuario

    // Ruta del archivo CSV que se va a procesar
    char *file_path = "C:\\Users\\Nery Molina\\Documents\\Microprocesadores\\Proyecto\\weather_stations.csv";

    // Abrir el archivo en modo lectura
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        printf("Error al abrir el archivo.\n");
        return 1;
    }

    char line[MAX_LINE_LENGTH];  // Arreglo para almacenar cada linea leida del archivo
    long long num_lines = 0;  // Contador para el numero de lineas en el archivo

    // Contar el numero de lineas en el archivo
    while (fgets(line, MAX_LINE_LENGTH, file)) {
        num_lines++;
    }
    fclose(file);  // Cerrar el archivo despues de contar las lineas

    do {
        // Mostrar menu de opciones al usuario
        printf("Menu:\n");
        printf("1. Procesar el archivo de manera secuencial\n");
        printf("2. Procesar el archivo de forma paralela\n");
        printf("3. Salir\n");
        printf("Por favor, elige una opcion: ");
        scanf("%d", &opcion);  // Leer la opcion seleccionada por el usuario

        switch(opcion) {
            case 1: {  // Procesar el archivo de manera secuencial
                // Reabrir el archivo en modo lectura
                file = fopen(file_path, "r");
                if (file == NULL) {
                    printf("Error al abrir el archivo.\n");
                    return 1;
                }

                double min = DBL_MAX;  // Variable para almacenar el minimo valor
                double max = -DBL_MAX;  // Variable para almacenar el maximo valor
                double start_time = omp_get_wtime();  // Registrar el tiempo de inicio

                // Leer y procesar cada linea del archivo
                while (fgets(line, MAX_LINE_LENGTH, file)) {
                    char *token = strtok(line, ";");  // Extraer el primer token
                    token = strtok(NULL, ";");  // Extraer el segundo token (coordenada)
                    if (token != NULL) {
                        double coord = atof(token);  // Convertir el token a double
                        if (coord < min) {
                            min = coord;  // Actualizar el valor minimo
                        }
                        if (coord > max) {
                            max = coord;  // Actualizar el valor maximo
                        }
                    }
                }
                double end_time = omp_get_wtime();  // Registrar el tiempo de finalizacion
                fclose(file);  // Cerrar el archivo despues de proocesarlo

                // Mostrar resultados
                printf("Numero de lineas procesadas secuencialmente: %lld\n", num_lines);
                printf("Minimo: %f, Maximo: %f\n", min, max);
                printf("Tiempo de ejecucion: %f segundos\n", end_time - start_time);
                break;
            }
            case 2: { //Procesar el archivo de forma paralela
                // Reabrir el archivo en modo lectura
                file = fopen(file_path, "r");
                if (file == NULL) {
                    printf("Error al abrir el archivo.\n");
                    return 1;
                }

                // Reserbar memoria para almacenar todas las lineas del archivo
                char **lines = malloc(num_lines * sizeof(char *));
                if (lines == NULL) {
                    printf("Error al asignar memoria.\n");
                    return 1;
                }

                long long i = 0;
                // Leer y almacenar cada linea del archivo en el arreglo 'lines'
                while (fgets(line, MAX_LINE_LENGTH, file)) {
                    lines[i] = strdup(line);  // Duplicar la linea y almacenarla
                    i++;
                }
                fclose(file);  // Cerrar el archivo despues de leerlo

                double global_min = DBL_MAX;  // Variable global para el minimo valor
                double global_max = -DBL_MAX;  // Variable global para el maximo valor

                double start_time = omp_get_wtime();  // Registrar el tiempo de inicio

                // Procesar las lineas en paralelo usando OpenMP
                #pragma omp parallel for reduction(min : global_min) reduction(max : global_max)
                for (i = 0; i < num_lines; i++) {
                    char *token = strtok(lines[i], ";");  // Extraer el primer token
                    token = strtok(NULL, ";");  // Extraer el segundo token (coordenada)
                    if (token != NULL) {
                        double coord = atof(token);  // Convertir el token a double
                        if (coord < global_min) {
                            global_min = coord;  // Actualizar el valor minimo global
                        }
                        if (coord > global_max) {
                            global_max = coord;  // Actualizar el valor maximo global
                        }
                    }
                }
                double end_time = omp_get_wtime();  // Registrar el tiempo de finalizacion

                // Liberar la memoria reservada para las lineas
                for (i = 0; i < num_lines; i++) {
                    free(lines[i]);
                }
                free(lines);

                // Mostrar resultados
                printf("Numero de lineas procesadas en paralelo: %lld\n", num_lines);
                printf("Minimo: %f, Maximo: %f\n", global_min, global_max);
                printf("Tiempo de ejecucion: %f segundos\n", end_time - start_time);
                break;
            }
            case 3:  // Salir del programa
                printf("Saliendo del programa....\n");
                break;
            default:  // Manejar opcion no valida
                printf("Opcion no valida. Por favor, elige de nuevo.\n");
        }
    } while(opcion != 3);  // Repetir hasta que el usuario elija salir

    return 0;
}
