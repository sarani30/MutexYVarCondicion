//EJERCICIOS SOBRE MUTEX Y VARIABLES DE CONDICIÓN

//EJERCICIO 1

/*1. ¿Para qu´e sirve un mutex? ¿Y una variable de condici´on?
 *
 * Un mutex sirve para solucionar los problemas de ejecución concurrente debido a condiciones de carrera,
 * donde varios hilos hacen uso a la vez de la misma variable compartida, lo cual puede hacer variar
 * el resultado final en cada ejecución. Para ello, se emplea la exclusión mutua en la sección crítica del
 * código donde se modifica la variable global.
*Se emplea para obtener acceso exclusivo a recursos compartidos y para
“serializar” el acceso a la SC en exclusión mutua
♦ Sólo un hilo puede tener acceso simultáneamente al mutex
 *
 * Las variables de condición se emplean para permitir a uno o varios hilos suspender su ejecución (dormirse)
 * hasta que otro hilo les notifique que se ha cumplido
 * una condición lógica o evento determinado en el estado compartido. Por tanto, pausan un hilo hasta que un evento global
 * finalice y despertarlos a todos simultáneamente.
 */

//EJERCICIO 2

/*2. Responde a las siguientes preguntas (sin necesidad de justificarlas):
(a) ¿Se bloquea un hilo cuando ´este adquiere un mutex (con una operaci´on
lock)?

SOLUCIÓN: No. Cuando un hilo ejecuta una operación lock y el mutex está ibre o desbloqueado
, el hilo entra dentro del cerrojo y continúa con su ejecución. El bloqueo solo ocurre cuando otro
hilo que no tiene el mutex trata de entrar dentro de él.

(b) ¿Se bloquea un hilo cuando intenta (con una operaci´on lock) bloquear
un mutex que ya est´a bloqueado?

SOLUCIÓN: Sí, cuando un hilo ya está dentro del mútex, se bloquea hasta que finaliza su ejecución en la
sección crítica y realiza una operación unlock. Por tanto, el mutex bloquea al resto de hilos hasta que se
desbloquea, par evitar condiciones de carrera.

(c) ¿Se bloquea siempre un hilo cuando ejecuta una operaci´on wait sobre
una variable de condici´on?

SOLUCIÓN: siempre que llega a entrar a esa operación, el hilo libera el mutex, suspende su ejecución
lo lleva a la cola de la variable de condición, en estado bloqueado hasta que otro hilo invoque la señal signal
o broadcast.

(d) ¿Tiene alg´un efecto una operaci´on signal si no hay hilos esperando en
la cola variable de condici´on sobre la que se aplica?

SOLUCIÓN: Si no hay hilos esperando en la variable condición, se descarta de inmediato sin alterar el estado del sistema
, es decir, se ignora totalmente la operación.
*/

//EJERCICIO 3

/*3. Lasoluci´on dada en las transparencias 39 y 40 del tema 3 resuelve el problema
que se plantea en el ejemplo de las transparencias 11 y 12.

(a) ¿Qu´eocurre si ponemos en el c´odigo que ejecutan los hilos subordinados,
hilo1 e hilo2, la funci´on pthread_mutex_unlock detr´as de sleep?

SOLUCIÓN:Si sleep está antes del unlock, sería un fallo grave de eficiencia, puesto que si el hilo duerme mientras tiene
el cerrojo del mutex, ningún otro hilo podrá entrar en la sección crítica durante el tiempo que esté bloqueado. Destruiría
la concurrencia y gastaría ciclos de CPU innecesarios.

(b) ¿Qu´e ocurre si extraemos las funciones pthread_mutex_lock y pthread_mutex_unlock fuera del bucle for?

SOLUCIÓN: Si lock y unlock están encerrando un bucle for, no habría concurrencia de ningún tipo, puesto que hilo no desbloquearía
el mutex hasta que finalizara todas las iteraciones del bucle for. Entonces, ningún otro hilo podría ir intercalando operaciones.

*/

//EJERCICIO 4

/*¿Por qu´e la funci´on pthread_cond_wait del est´andar POSIX requiere un
mutex como segundo argumento?
 *
 *Lo requiere porque al llevar el hilo en ejecución a la cola de la variable de condición, se libera el mutex y por tanto, otro
 *hilo puede ocupar el cerrojo.*/

//EJERCICIO 5

/* 5. Sea el siguiente fragmento de c´odigo que ejecuta un hilo determinado:

  1: pthread_mutex_lock(&mutex);
  2: while (ocupado == true)
  3:    pthread_cond_wait(&cond, &mutex);
  4: ocupado = true;
  5: pthread_mutex_unlock(&mutex);

Y sea el siguiente fragmento de c´odigo que permite desbloquear al hilo que
ejecute el c´odigo anterior:

  6: pthread_mutex_lock(&mutex);
  7: ocupado = false;
  8: pthread_cond_signal(&cond);
  9: pthread_mutex_unlock(&mutex);

Supongamos que en un instante determinado el valor de la variable ocupado
es true y que en el sistema se est´an ejecutando dos hilos que denotamos por
A y B, respectivamente. Indicar de manera justificada si ser´ıan posibles y
por qu´e las siguientes secuencias de ejecuci´on, donde Xi denota que el hilo
X ejecuta la instrucci´on i.

    (a) A1 A2 A3 B6 B7 B8 A2 A4 A5 B9
    (b) A1 B6 A2 A3 B7 B8 B9 A4 A5
    (c) A1 A2 B6 A3 B7 B8 B9 A2 A4 A5
    (d) B6 A1 B7 B8 B9 A2 A4 A5
    (e) B6 B7 A1 A2 A4 A5 B8 B9

Ninguna de las secuencias propuestas es posible. A continuación se justifica minuciosamente el motivo de la imposibilidad de cada una de ellas:

---

### (a) `A1 A2 A3 B6 B7 B8 A2 A4 A5 B9` — **NO ES POSIBLE**

* **Razón:** En `A3`, el hilo A ejecuta `pthread_cond_wait`, lo que libera el mutex y suspende a A. El hilo B entra
* en la sección crítica con `B6`, cambia la variable en `B7` y emite la señal en `B8`.


* **Fallo:** La llamada a `pthread_cond_signal` (`B8`) despierta al hilo A, pero **para que `pthread_cond_wait` (`A3`)
* finalice y el hilo A vuelva a evaluar la condición en `A2`, A debe volver a adquirir el mutex**. Como el hilo B
* todavía posee el mutex (no ha ejecutado `B9`), el hilo A no puede reanudar la ejecución en `A2` antes de `B9`.



---

### (b) `A1 B6 A2 A3 B7 B8 B9 A4 A5` — **NO ES POSIBLE**

* **Razón:** En `A1`, el hilo A adquiere el mutex.


* **Fallo:** Inmediatamente después se pretende que el hilo B ejecute `B6` (`pthread_mutex_lock`). Dado que el mutex se
* encuentra bloqueado por A y este aún no lo ha liberado (no ha ejecutado `A3`), **el hilo B queda bloqueado en la
* instrucción `B6**` y no puede avanzar a las instrucciones `B7`, `B8` o `B9` hasta que A libere el cerrojo.



---

### (c) `A1 A2 B6 A3 B7 B8 B9 A2 A4 A5` — **NO ES POSIBLE**

* **Razón:** El hilo A ejecuta `A1` (adquiere el mutex) y `A2` (evalúa `ocupado == true`).


* **Fallo:** En este punto, el hilo A todavía no ha ejecutado `A3` (que es la instrucción donde se libera el mutex
* al entrar en espera). Por tanto, cuando el hilo B intenta ejecutar `B6` (`pthread_mutex_lock`), **se bloquea en `B6`
* por exclusión mutua** y no puede intercalarse antes de `A3`.



---

### (d) `B6 A1 B7 B8 B9 A2 A4 A5` — **NO ES POSIBLE**

* **Razón:** El hilo B ejecuta `B6` y adquiere el mutex.


* **Fallo:** Al estar el mutex retenido por B, el hilo A no puede ejecutar `A1` (`pthread_mutex_lock`); **el hilo A
* queda bloqueado en la propia instrucción `A1**` hasta que B ejecute `B9` (`pthread_mutex_unlock`). Por lo tanto,
* `A1` no puede suceder entre `B6` y `B9`.



---

### (e) `B6 B7 A1 A2 A4 A5 B8 B9` — **NO ES POSIBLE**

* **Razón:** El hilo B ejecuta `B6` y `B7`, manteniendo el mutex bloqueado en todo momento.


* **Fallo:** El hilo A no puede ejecutar `A1` ni avanzar a `A2`, `A4` o `A5` dentro de la sección crítica de B.
* Al estar el mutex ocupado por B, **el hilo A se bloquea en `A1**` y no podrá ejecutar ninguna instrucción posterior
* hasta que B libere el mutex con `B9`.



---

(Nota: Para que una secuencia fuera válida, el hilo A debería poder ejecutar `A2` únicamente **después** de
que B ejecute `B9` (por ejemplo: `A1 A2 A3 B6 B7 B8 B9 A2 A4 A5`), o bien B debería ejecutar de principio a
fin su bloque antes de que A intente entrar).
*/

//EJERCICIO 6

/*6. En el ejercicio 2 de las transparencias del tema 3 se plantean dos posibles
soluciones para dicho ejercicio. La primera es correcta y la segunda, no.
¿Por qu´e no es correcta la segunda soluci´on?

SOLUCIÓN: No es correcta porque no hay ninguna condición dentro del while que evalúa si el hilo debe de esperar
en la variable de condición. Falta pthread_cond_wait();
*/

//EJERCICIO 7

/* 7. Sea el siguiente programa escrito en C:
        #include <pthread.h>
        #include <stdio.h>
        #include <stdlib.h>
        #define NTHREADS 4
        int contador=0;

    void *hilo(void *arg)
    {
        int i;
        i = *(int *) arg;
        contador = contador + i;
        printf("Hilo %lu, contador = %u\n",pthread_self(),contador);
        pthread_exit(0);
    }
    int main()
    {

        int i;
        int v[NTHREADS];
        pthread_t thread[NTHREADS]; //vector para definir los hilos
        for (i=0;i<NTHREADS;i++)
        {   v[i] = i;
            pthread_create(&thread[i], NULL, hilo, (void *)&v[i]);
            pthread_join(thread[i],NULL);
        }
        printf("Hilo principal, contador = %d\n",contador);
        pthread_exit (0);
    }

(a) Indica, de manera justificada, lo que muestra por la pantalla tanto la
hebra principal como cada uno de los hilos subordinados.

SOLUCIÓN:
    Hilo 0, contador = 0
    Hilo 1, contador = 1
    Hilo 2, contador = 3
    Hilo 3, contador = 6
    Hilo principal, contador = 6

(b) Modifica este c´odigo para que todos los hilos puedan ejecutarse en par
alelo (concurrentemente) y acceder a la variable contador de manera
correcta.


SOLUCIÓN:*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NTHREADS 4
pthread_mutex_t mutex;
int contador=0;

void *hilo(void *arg)
{
    int i;
    i = *(int *) arg;

    pthread_mutex_lock(&mutex);
    contador = contador + i;
    printf("Hilo %lu, contador = %u\n",pthread_self(),contador);
    pthread_mutex_unlock(&mutex);

    pthread_exit(0);
}
int main()
{

    int i;
    int v[NTHREADS];
    pthread_t thread[NTHREADS]; //vector para definir los hilos

    pthread_mutex_init(&mutex,NULL);

    for (i=0;i<NTHREADS;i++)
    {   v[i] = i;
        pthread_create(&thread[i], NULL, hilo, (void *)&v[i]);
    }

    for (i = 0; i < NTHREADS; i++) {
        pthread_join(thread[i], NULL);
    }
    printf("Hilo principal, contador = %d\n",contador);

    pthread_mutex_destroy(&mutex);
    pthread_exit (0);
}