//
// Created by saran on 13/11/2026.
//

#include "lab3.h"

//EJERCICIO 1

/*El trabajo principal a realizar en este ejercicio es el desarrollo de un programa completo multihilo en C. Este
 *programa creará un número determinado de hilos que deberán sincronizarse para imprimir por consola. El número de hilos
 *deberá pasarse como argumento al programa.
 *
 *El programa también recibirá dos números enteros: los valores inicial y
 *final que se van a imprimir. A continuación, los hilos creados imprimirán, de forma alterna y ordenada, los valores
 *comprendidos entre el punto inicial y el final. Los tres parámetros se pasarán al programa a través de la línea de
 *comandos. El número de elementos a imprimir por consola debe ser mayor que el número de hilos a crear.
 *
 *El hilo principal (main) es el responsable de crear los hilos trabajadores (workers) y de imprimir por consola un mensaje
 *una vez que todos los hilos hayan finalizado.
 *
 *Si el número dado de hilos es $N$, el punto de inicio $s$ y el punto
 *final $e$, el hilo 0 imprimirá $s$, $s+N$, $s+2N$ y así sucesivamente; el hilo 1 imprimirá $s+1$, $s+N+1$, $s+2N+1$;
 *el hilo $i$ imprimirá $s+i$, $s+N+i$, $s+2N+i$, ... De este modo, cada hilo finalizará cuando el valor que intente
 *imprimir sea mayor que $e$. Al terminar los hilos trabajadores, los datos impresos serán $s, s+1, s+2, \dots, e$; es
 *decir, todos los números en orden entre $s$ y $e$.
 *
 *Para hacer más abordable la implementación del ejercicio, el problema puede resolverse en dos pasos:
 *Paso 1: Sin sincronización de hilos. Cada hilo podrá imprimir un elemento
 *por consola en cualquier momento.
 *
 *Paso 2: Sincronizando los hilos tal y como se especifica en el enunciado anterior.
 *
 *Implementa un programa para resolver la especificación propuesta utilizando mutex y un vector de variables de
 *condición. La implementación debe ser lo más eficiente posible.
 *
 *Se puede utilizar como referencia el ejercicio 15
 *de la relación de ejercicios sobre mutex del tema 3. */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//NOTAS
//núm hilos -> argumento del programa
//recibirán los valores a imprimir inicial y final

//número de elementos > núm de hilos

pthread_mutex_t mutex;
pthread_cond_t *cond;
int n_hilos;
int turno = 0;

typedef struct {

    int id;
    int i;
    int f;
} v;

void *hilo(void *arg) {
    int indice, res = 0, j = 1;
    v *param = (v *)arg;

    indice = param->id;

    while (res <= param->f) {

        pthread_mutex_lock(&mutex);

        while (turno != indice) {
            pthread_cond_wait(&cond[indice], &mutex);
        }


        printf("%d", res);
        turno = (turno + 1) % n_hilos;
        pthread_cond_signal(&cond[turno]);

        pthread_mutex_unlock(&mutex);

        j++;
        res = param->i+indice + j*n_hilos;

    }

    pthread_mutex_lock(&mutex);


    pthread_mutex_unlock(&mutex);

    pthread_exit(0);
}

int main (int argc, char *argv[]) {
    int i;

    //comprobación de argumentos
    if (argc < 4) {
        perror("Número de argumentos insuficiente");
        exit(-1);
    }

    int v_inicial = atoi(argv[2]);
    int v_final = atoi(argv[3]);

    int n_elementos = v_final - v_inicial + 1;

    n_hilos = atoi(argv[1]);

    if (n_elementos <= n_hilos) {
        perror("Tienen que haber más elementos que hilos");
        exit(-1);
    }

    pthread_t *hilos;
    hilos = (pthread_t *) malloc(sizeof(pthread_t) * n_hilos);

    v *param;
    param = (v *) malloc(sizeof(v) * n_hilos);

    pthread_mutex_init(&mutex, NULL);

    cond = (pthread_cond_t *)malloc(sizeof(pthread_cond_t) * n_hilos);

    for (i = 0; i < n_hilos; i++) {
        pthread_cond_init(&cond[i], NULL);
    }

    for (i = 0; i < n_hilos; i++) {
        param[i].id = i;
        param[i].i = v_inicial;
        param[i].f = v_final;
        pthread_create(&hilos[i], NULL, hilo, (void *) &param[i]);
    }

    for (i = 0; i < n_hilos; i++) {
        pthread_join(hilos[i], NULL);
    }

    pthread_mutex_destroy(&mutex);

    for (i = 0; i < n_hilos; i++) {
        pthread_cond_destroy(&cond[i]);
    }

    printf("\nTodos los hilos han terminado su ejecución\n");

    free(hilos);
    free(param);
    exit(0);
}

//EJERCICIO 2

/*2. El código del programa `producer_consumer.c` implementa un problema de productor-consumidor.

El hilo principal crea dos hilos que ejecutan las funciones `produce_numbers` y `count_numbers`, respectivamente. La
función `produce_numbers`, ejecutada por el productor, genera `TOTAL_NUMBERS` números enteros aleatorios y los almacena
en un búfer. Este búfer está implementado como un vector (*array*), `numbers`. El hilo consumidor ejecuta la función
`count_numbers`. Esta función comprueba si cada entero del búfer es un número par o impar y acumula la cantidad total
de enteros pares generados (`total_even_numbers`) y la cantidad total de enteros impares generados (`total_odd_numbers`).
Finalmente, el hilo consumidor muestra estos valores por consola.

Asume que el tamaño del búfer es mucho menor que el total de números que se van a producir (y consumir). Por tanto, el
número de elementos del búfer `numbers` debe ser mucho menor que la cantidad de enteros a producir. Modifica el programa
`producer_consumer.c` para resolver esta situación utilizando mutex y variables de condición.

Recuerda que el productor y el consumidor no se ejecutan de forma concurrente en el código original del programa
`producer_consumer.c`. */

/* producer_consumer.c */

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define TOTAL_NUMBERS  39// Total numbers to be produced and consumed
#define BUFFER 20

int numbers[BUFFER]; // Vector to store the numbers
pthread_mutex_t mutex;
pthread_cond_t noVacio;
pthread_cond_t noLleno;

int anadidos = 0;

void *produce_numbers(void *arg)
{
  int  i, pos;

  for (i=0; i<TOTAL_NUMBERS; i++){

      pos = i % BUFFER;
      pthread_mutex_lock(&mutex);

      while (anadidos == BUFFER) {
          pthread_cond_wait(&noLleno, &mutex);
      }

      numbers[i] = random() % 100;         // Produce an integer between 0 and 99
      anadidos++;
      printf("Thread_produce_numbers: Produces in position %d number %d \n",i,numbers[i]);

      pthread_cond_signal(&noVacio);
      pthread_mutex_unlock(&mutex);

  }
  pthread_exit(0);
}

void *count_numbers(void *arg)
{
  int i, total_even_numbers=0, total_odd_numbers=0, pos;

  for (i=0; i<TOTAL_NUMBERS; i++){

      pos = i % BUFFER;

      pthread_mutex_lock(&mutex);

      while (anadidos == 0) {
          pthread_cond_wait(&noVacio, &mutex);
      }

        if ((numbers[pos]%2) == 0)
        {
           total_even_numbers++;
           printf("          Thread_count_numbers: Read in position %d number %d \t(even)   \n", i, numbers[i]);
        } else {
           total_odd_numbers++;
           printf("          Thread_count_numbers: Read in position %d number %d \t(odd)   \n", i, numbers[i]);
        }

      anadidos--;

      pthread_cond_signal(&noLleno);
      pthread_mutex_unlock(&mutex);

  }
  printf("Thread_count_lines: Total even numbers= %d Total odd numbers = %d Total numbers = %d \n", total_even_numbers, total_odd_numbers, total_even_numbers+total_odd_numbers);
  pthread_exit(0);
}

int main()
{
    pthread_t thread_produce_numbers; // Producer thread
    pthread_t thread_count_numbers;   // Consumer thread

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&noVacio, NULL);
    pthread_cond_init(&noLleno, NULL);

    // The thread_count_numbers is created when the thread_produce_numbers ends, so that syncronization is not required
    if  ( pthread_create(&thread_produce_numbers, NULL, produce_numbers, NULL) ) {
      perror("Error creating producer thread.");
      exit(-1);
    }

    if  ( pthread_create(&thread_count_numbers, NULL, count_numbers, NULL) ) {
      perror("Error creating producer thread.");
      exit(-1);
    }

    if  ( pthread_join(thread_produce_numbers,NULL)) {
        perror("Error joining producer thread.");
        exit(-1);
    }

    if  ( pthread_join(thread_count_numbers,NULL) ) {
      perror("Error joining producer thread.");
      exit(-1);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&noVacio);
    pthread_cond_destroy(&noLleno);

    exit (0);
}


//EJERCICIO 3

/*3. Diseña un código que cree 10 hilos de tipo H1, 10 hilos de tipo H2, 10 hilos de tipo H3 y 10 hilos de tipo H4.
 *Todos los hilos se ejecutarán de forma concurrente. Además, existen dos bases de datos compartidas en el sistema:
 *A y B. Los hilos H1 utilizan la base de datos A, los hilos H2 utilizan la base de datos B, los hilos H3 utilizan
 *simultáneamente las bases de datos A y B, y los hilos H4 utilizan simultáneamente las bases de datos A y B. Ambas
 *bases de datos, A y B, deben utilizarse en exclusión mutua; es decir, solo un hilo puede acceder a cada base de datos
 *de forma simultánea. Por lo tanto, un hilo H1 puede acceder a la base de datos A mientras un hilo H2 accede a la base
 *de datos B. Sin embargo, cuando un hilo H3 o un hilo H4 esté accediendo a las bases de datos A y B, ningún otro hilo
 *(de tipo H1, H2, H3 o H4) podrá acceder a ellas.

Implementa un programa en lenguaje C para resolver la especificación propuesta anteriormente utilizando mutex y
variables de condición. La implementación debe ser lo más eficiente posible.

El uso de una base de datos puede simularse mediante la función `sleep(random() % 3);`.

Una posible salida mostrada por pantalla por el programa podría ser la siguiente, donde los dos números entre paréntesis
representan la cantidad de hilos que están accediendo actualmente a las bases de datos A y B, respectivamente:

```text
Thread h3 number 0 (id.976129280) accessing databases A and B (1 1)
Thread h3 number 0 (id.976129280) releases databases A and B (0 0)
Thread h4 number 0 (id.967736576) accessing databases A and B (1 1)
Thread h4 number 0 (id.967736576) releases databases A and B (0 0)
Thread h1 number 0 (id.959343872) accessing database A (1 0)
Thread h2 number 0 (id.950951168) accessing database B (1 1)
Thread h2 number 0 (id.950951168) releases database B (1 0)
Thread h2 number 1 (id.917380352) accessing database B (1 1)
Thread h1 number 0 (id.959343872) releases database A (0 1)
Thread h2 number 1 (id.917380352) releases database B (0 0)
Thread h3 number 2 (id.810118400) accessing databases A and B (1 1)
...

``` */


#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define THREADS 10
pthread_mutex_t mutex;
pthread_cond_t condA;
pthread_cond_t condB;
pthread_cond_t condC;

int baseA = 0; //0: no se usa, 1: se usa
int baseB = 0; //0: no se usa, 1: se usa

typedef struct {
    int id;   // Número del 0 al 9
    int tipo; // 3 para H3, 4 para H4
} t_param;

void  *h1(void *arg) {
    int i;
    int id = *(int *)arg;

    pthread_mutex_lock(&mutex);

    while (baseA == 1) {
        pthread_cond_wait(&condA, &mutex);
    }
    baseA = 1;

    printf("Thread h1 number %d (id.%lu) accessing database A (%d %d)\n",
           id, pthread_self(), baseA, baseB);

    pthread_mutex_unlock(&mutex);

    sleep(random() % 3);

    pthread_mutex_lock(&mutex);

    baseA = 0;
    printf("Thread h1 number %d (id.%lu) releases database A (%d %d)\n",
       id, (unsigned long)pthread_self(), baseA, baseB);
    pthread_cond_signal(&condA);
    pthread_cond_signal(&condC);

    pthread_mutex_unlock(&mutex);


    pthread_exit(0);

}

void *h2(void *arg) {
    int i;
    int id = *(int *)arg;

    pthread_mutex_lock(&mutex);

    while (baseB == 1) {
        pthread_cond_wait(&condB, &mutex);
    }
    baseB = 1;

    printf("Thread h2 number %d (id.%lu) accessing database A (%d %d)\n",
           id, pthread_self(), baseA, baseB);
    pthread_mutex_unlock(&mutex);

    sleep(random() % 3);

    pthread_mutex_lock(&mutex);

    baseB = 0;
    pthread_cond_signal(&condB);
    pthread_cond_signal(&condC);

    pthread_mutex_unlock(&mutex);

    pthread_exit(0);
}

void *h34 (void *arg) {
    int i;
    t_param param = *(t_param *)arg;

    for (i = 0; i < THREADS; i++) {
        pthread_mutex_lock(&mutex);

        while (baseA == 1 || baseB == 1) {
            pthread_cond_wait(&condC, &mutex);
        }
        baseA = 1;
        baseB = 1;


        printf("Thread h%d number %d (id.%lu) accessing databases A and B (%d %d)\n",
            param.tipo, param.id, (unsigned long)pthread_self(), baseA, baseB);
        pthread_mutex_unlock(&mutex);

        sleep(random() % 3);

        pthread_mutex_lock(&mutex);

        baseA = 0;
        baseB = 0;

        printf("Thread h%d number %d (id.%lu) releases databases A and B (%d %d)\n",
           param.tipo, param.id, (unsigned long)pthread_self(), baseA, baseB);
        pthread_cond_signal(&condA); // Avisa a un H1
        pthread_cond_signal(&condB); // Avisa a un H2
        pthread_cond_signal(&condC); // Avisa a otro H3/H4

        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(0);

}

int main() {
    int i, v[THREADS];
    t_param params3[THREADS];
    t_param params4[THREADS];

    pthread_t h1[THREADS];
    pthread_t h2[THREADS];
    pthread_t h3[THREADS];
    pthread_t h4[THREADS];

    srandom(time(NULL));

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&condA, NULL);
    pthread_cond_init(&condB, NULL);
    pthread_cond_init(&condC, NULL);

    for (i = 0; i < THREADS; i++) {

        v[i] = i;

        params3[i].id = i;
        params3[i].tipo = 3;

        params4[i].id = i;
        params4[i].tipo = 4;

        pthread_create(&h1[i], NULL, h1, (void *) &v[i]);
        pthread_create(&h2[i], NULL, h2, (void *) &v[i]);
        pthread_create(&h3[i], NULL, h34, (void *) &v[i]);
        pthread_create(&h4[i], NULL, h34, (void *) &v[i]);
    }

    for (i = 0; i < THREADS; i++) {
        pthread_join(h1[i], NULL);
        pthread_join(h2[i], NULL);
        pthread_join(h3[i], NULL);
        pthread_join(h4[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condA);
    pthread_cond_destroy(&condB);
    exit(0);

}