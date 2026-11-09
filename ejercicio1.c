#include <pthread.h>         
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int num_hilos;
int inicio;
int fin;
int turno_actual;

pthread_mutex_t mutex;
pthread_cond_t *cond_hilos;

void* cuenta_hilos(void* arg) {
int id = *((int*) arg);

for (int valor = inicio + id; valor <= fin; valor += num_hilos) {

		pthread_mutex_lock(&mutex);
while (turno_actual != valor) {
		pthread_cond_wait(&cond_hilos[id], &mutex);
} 

printf("El hilo %d imprime este valor %d\n", id, valor);
turno_actual++;
int siguiente_hilo = (id + 1) % num_hilos;
pthread_cond_signal(&cond_hilos[siguiente_hilo]); 

		pthread_mutex_unlock(&mutex);		
}
pthread_exit(NULL);
}

int main(int argc, char * argv[]) {

    	if (argc != 4) {
     	   	perror("Error: Faltan argumentos. Uso: <num_hilos> <inicio> <fin>");
  		exit(-1);
   	 }

    	num_hilos = atoi(argv[1]);
    	inicio = atoi(argv[2]);
   	fin = atoi(argv[3]);

	if (num_hilos <= 0) { 
perror("Error: El numero de hilos debe ser mayor que 0"); exit(-1); 
} 

int total_elementos = (fin - inicio) + 1; 
if (fin < inicio) { 
perror("El limite final debe ser mayor al inicial"); 
exit(-1); 
} 
if (total_elementos <= num_hilos) { 
perror("Error: El numero de elementos a imprimir debe ser mayor que el numero de hilos"); 
exit(-1); 
} 

turno_actual = inicio;
pthread_t * hilos = malloc(num_hilos * sizeof(pthread_t)); 
int* ids = malloc(num_hilos * sizeof(int));
pthread_mutex_init(&mutex, NULL); 
cond_hilos = malloc(num_hilos * sizeof(pthread_cond_t));
if (cond_hilos == NULL) { 
perror("Error al asignar memoria con malloc"); 
exit(-1); 
} 
	for ( int i = 0; i < num_hilos; i++) {
		pthread_cond_init(&cond_hilos[i], NULL);
}
for (int i = 0; i < num_hilos; i++) { 
ids[i] = i; 
if (pthread_create(&hilos[i], NULL, cuenta_hilos, &ids[i]) != 0) { 
perror("Error al crear hilo con pthread_create"); 
exit(-1); 
} 
}
for (int i = 0; i < num_hilos; i++) { 
if (pthread_join(hilos[i], NULL) != 0) { 
perror("Error al esperar al hilo con pthread_join"); 
exit(-1); 
} 
}

printf("\n[Hilo Principal] Todos los hilos han terminado correctamente.\n"); 
	free(hilos);
free(cond_hilos);
free(ids);
return 0; 

	
}