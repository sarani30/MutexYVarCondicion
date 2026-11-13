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
