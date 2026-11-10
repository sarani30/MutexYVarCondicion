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

//EJERCICIO 8

/*8. Realiza un programa en C que calcule la suma de los elementos de un vec
tor. El hilo principal deber´a crear un n´umero determinado hilos y repartir
la suma del vector entre todos ellos. La funci´on que ejecuta cada hilo debe
recibir como par´ametros el ´ındice inicial y final del subvector a sumar. Y
acumular´a la suma de los elementos del subvector que le corresponde sobre
una variable accesible por todos los hilos.

Tanto el tama˜no del vector como el n´umero de hilos a crear ser´an constantes.
Y los elementos del vector se calcular´an de manera aleatoria.

Para realizar el ejercicio puedes tomar como referencia el ejercicio 13 de las
transparencias del tema 2. */

//Anotaciones:

//sumar elementos de un vector

//crear número determinado de hilos
//repartir los componentes enre los hilos y hacer sumas parciales
//parametros de la funcion -> indice inical y final del subvector a sumar
//acumular la suma de los elementos del vector en una variable compartida

//tamaño del vector y el número de hilos constante
//elementos calculados de manera aleatoria

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

pthread_mutex_t mutex; //mutex que controla el acceso a la SC
#define NTHREADS 4
#define TAMANYO_VECTOR 8

int v[TAMANYO_VECTOR];
int suma = 0;

typedef struct {
    int ini;
    int fin;
} indices;

void *hilo(void *arg) {

    indices *p = (indices *) arg;

    int suma_parcial = 0; // Acumulador local (fuera del mutex)

    // Cómputo local sin bloqueo
    for (int i = p->ini; i <= p->fin; i++) {
        suma_parcial += v[i];
    }

    // Sección crítica: se añade la suma parcial una única vez por hilo
    pthread_mutex_lock(&mutex);
    suma += suma_parcial;
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

int main() {

    int i;
    time_t t;
    pthread_t hilos[NTHREADS];
    int dim_subvector = TAMANYO_VECTOR / NTHREADS;
    indices ind[NTHREADS];

    pthread_mutex_init(&mutex,NULL);

    //inicializar el vector con números random
    srandom(time(&t));
    printf("VECTOR\n");
    for (i=0;i<TAMANYO_VECTOR;i++)
    {
        v[i]=random()%10;
        printf("V[%d]=%d\n",i,v[i]);
    }
    printf("\n");

    //crear hilos
    for (i = 0; i < NTHREADS; i++) {

        ind[i].ini = i * dim_subvector;

        if (i != (NTHREADS - 1)) ind[i].fin = ind[i].ini + dim_subvector - 1;
        else ind[i].fin = TAMANYO_VECTOR - 1;

        pthread_create(&hilos[i], NULL, hilo, (void *) &ind[i]);
    }

    //ejecución hilos
    for (i = 0; i < NTHREADS; i++) {
        pthread_join(hilos[i], NULL);
    }

    printf("La suma es %d\n", suma);

    pthread_mutex_destroy(&mutex);
    exit(0);
}

//EJERCICIO 9

/*9. ¿Qu´e ocurrir´ıa en la soluci´on planteada para el ejercicio 3 de las trans
parencias del tema 3 si se eliminan en las funciones fhiloA y fhiloB
la funci´on pthread_mutex_unlock que precede inmediatamente al primer printf y la funci´on pthread_mutex_lock
que precede inmediatamente al segundo printf?


SOLUCIÓN:Perderíamos la concurrencia porque ya no permitiría que varios hilos de tipo A o B accedieran a los datos
simultáneamente
*/

//EJERCICIO 10

/*10. En las transparencias del tema 3 se plantea una posible soluci´on al problema
del productor-consumidor con buffer limitado mediante mutex y variables
de condici´on. A partir de ella, responde a las preguntas que aparecen a
continuaci´on:

(a) ¿Se puede sustituir en la funci´on Productor la sentencia while por un
if? ¿Y en la funci´on Consumidor?

SOLUCIÓN: No, porque de normal si varios hilos pueden hacer cierta la condición asociada a la variable de condición,
ha de ir un while

(b) ¿Ser´ıa necesario sustituir en la funci´on Productor la funci´on
pthread_cond_signal(&novacio); por lo siguiente: if (n elementos==1) pthread cond signal(&no vacio);

SOLUCIÓN: No es necesario porque las variables de condición no tienen memoria, si estuviera vacía de por sí, no se ejecutaría.

(c) ¿Qu´e cambios habr´ıa que hacer en el c´odigo de las transparencias para
que hubiese dos hilos productores y un hilo consumidor?

SOLUCIÓN:

// Ejemplo en main:
pthread_t th_prod1, th_prod2, th_cons;

pthread_create(&th_prod1, NULL, Productor, NULL);
pthread_create(&th_prod2, NULL, Productor, NULL);
pthread_create(&th_cons,  NULL, Consumidor, NULL);

pthread_join(th_prod1, NULL);
pthread_join(th_prod2, NULL);
pthread_join(th_cons,  NULL);

Con dos productores independientes, cada uno tendría su propia variable pos local inicializada a 0, por lo que ambos
escribirían sobre la misma casilla del buffer circular machacando datos.

// Variable global añadida
int pos_in = 0;

// Dentro de Productor:
buffer[pos_in] = dato;
pos_in = (pos_in + 1) % MAX_BUFFER;

Si la constante DATOS_A_PRODUCIR representa el total de elementos a procesar, se debe ajustar el bucle de cada productor
para que entre los dos generen esa cantidad (por ejemplo, haciendo que cada uno produzca DATOS_A_PRODUCIR / 2 O
pasándoles exactamente los datos a producir al crear el hilo como argumento.

(d) ¿Se podr´ıa sustituir en la funci´on Productor del c´odigo del apartado
anterior la sentencia while por un if? ¿Y en la funci´on Consumidor?

SOLUCIÓN: En el Productor (2 hilos): Teóricamente SÍ funcionaría un if. Al usarse signal, el consumidor solo despierta
a un único productor cuando libera un espacio, por lo que no hay competencia entre productores para ocupar ese hueco.

En el Consumidor (1 hilo): Teóricamente SÍ funcionaría un if. Al ser el único consumidor que existe en el sistema, no
hay otro hilo que pueda "robarle" el dato recién producido mientras intenta obtener el mutex.

Sin embargo, siempre es mejora un while.
*/

//EJERCICIO 11

/* 11. Realiza el ejercicio 4 de las trasparencias del tema 3 de la asignatura.*/

/*Dado el código que aparece a continuación, indicar  razonadamente la veracidad
de las siguientes afirmaciones:
a)hilo1ehilo2nuncaejecutaránpthread_cond_signal(&cond).

SOLUCIÓN: El hilo (hilo1 o hilo2) que realice el incremento cont = cont + 1 que lleve el contador de 19 a 20 evaluará
la condición if (cont == MAXCONT) como verdadera (20 == 20) y ejecutará inmediatamente la instrucción
pthread_cond_signal(&cond) para notificar a hilo3.

b)Antesde la instrucción pthread_mutex_destroy(&mutex)seríanecesaria
lainstrucciónpthread_cancel(hilo3).

SOLUCIÓN: No será necesario porque el programa principal ya se esperará a que hilo 3 acabe, ya que existe una instrucción
join. Hilo 1 y 2 no la tienen.

c)Un hilo que ejecute la instrucción pthread_cond_wait(&cond2,&mutex)
nunca saldrá de la situación de bloqueo y será eliminado por el hilo principal
con la correpondiente instrucción pthread_cancel

SOLUCIÓN: Verdadero. No existe la condición pthread_cond_signal(&cond2). Por tanto, ningún hilo dentro de esa variable
de condición, saldrá de ella

d) El funcionamiento del programa sería el mismo si eliminamos de la función
f_hilo1y2la línea:
if (cont== MAXCONT) pthread_cond_wait(&cond2,&mutex);

SOLUCIÓN: El funcionamiento no sería el mismo porque entonces todos los hilos, aunque el buffer no esté lleno, entrarían en esa
condición y al no haber un signal de esa variable de condición, el programa nunca acabaría de ejecutarse.

e) Elprogramanofinalizanunca.

SOLUCIÓN: El programa sí finaliza. Cuando cont llega a 220 y se envía la señal sobre la variable de condición cond, el hilo 3 despierta
y termina la ejecución. Y cancela hilo 1 e hilo2.

f) La variable  contnunca tendrá un valor superior a 20.

SOLUCIÓN: Verdadero. Porque en el momento en que cont llega a 20, la siguiente vez que cualquier hilo entre en la sección
crítica, la evaluación if (cont == MAXCONT) resulta cierta y ejecuta pthread_cond_wait(&cond2, &mutex) antes de llegar a
la instrucción cont = cont + 1. Como ambos hilos se bloquean sucesivamente en cond2 al llegar a 20, la instrucción de
incremento nunca vuelve a ejecutarse.

g) El funcionamiento del programa sería diferente si en la función f_hilo1y2
se pusiese la instrucción pthread_mutex_lockjusto antes del fory la
instrucción pthread_mutex_unlockjusto después del for

SOLUCIÓN: Al colocar la exclusión mutua fuera del bucle for, se destruye la concurrencia e intercalado de iteraciones
entre hilo1 e hilo2

*/

//EJERCICIO 12

/*12. Se pretende que los hilos del c´odigo que aparece a continuaci´on se sincronicen
de tal forma que se imprima en primer lugar la primera fila de la matriz,
a continuaci´on la segunda, despu´es la tercera y as´ı sucesivamente hasta la
´ultima fila de la matriz. ¿Garantiza el programa que las filas de la matriz se
impriman en el orden especificado? En caso negativo, ¿qu´e modificaciones
realizar´ıas en el c´odigo para garantizar dicho orden?
Justifica la respuesta o respuestas.

    #include <pthread.h>
    #include <stdio.h>
    #include <unistd.h>
    #include <stdlib.h>

    #define MAX_DIM 10

    int n=4;
    typedef int TMat [MAX_DIM][MAX_DIM];
    TMat A={{1,2,5,1},{2,3,8,0},{3,8,5,1},{4,0,4,1}};
    int turno=0;

    pthread_mutex_t mutex;
    pthread_cond_t TocaTurnoFila;

    void *Impr (void *arg)
        { int i;
        int f;
        f=*(int *)arg;
        pthread_mutex_lock(&mutex);

        while ( turno != f )
            pthread_cond_wait(&TocaTurnoFila,&mutex);

        printf("Fila %d (Hilo %lu): ", f, pthread_self());

        for (i=0; i<n; i++) printf("%d ", A[f][i]);

        printf("\n");
        turno++;

        pthread_cond_signal (&TocaTurnoFila);
        pthread_mutex_unlock(&mutex);
        pthread_exit(0);
    }

    int main()
    {
        int i,j;
        int v[MAX_DIM];
        pthread_t hilo[MAX_DIM];

        pthread_mutex_init(&mutex, NULL);
        pthread_cond_init(&TocaTurnoFila,NULL);

        for (i=0; i<n; i++)
            { v[i]=i;
            pthread_create(&hilo[i], NULL, Impr, (void *)&v[i]);
        }
        for (i=0; i<n; i++) pthread_join(hilo[i], NULL);

        pthread_cond_destroy(&TocaTurnoFila);
        pthread_mutex_destroy(&mutex);
        exit(0) */

//SOLUCIÓN 1: Se puede solucionar ejecutando un broadcast en vez de un signal para asegurarse de desbloquear al hilo que toca.
//Porque puede desbloquear al hilo 2 y en realidad es necesario el hilo 1. Con el broadcast se desbloquean todos

//SOLUCIÓN 2: Utilizar un vector de variables de condición de forma que cada hilo espere a su propia variable de condición.

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_DIM 10

int n=4;
typedef int TMat [MAX_DIM][MAX_DIM];
TMat A={{1,2,5,1},{2,3,8,0},{3,8,5,1},{4,0,4,1}};
int turno=0;

pthread_mutex_t mutex;
pthread_cond_t TocaTurnoFila[MAX_DIM];

void *Impr (void *arg)
{ int i;
    int f;
    f=*(int *)arg;
    pthread_mutex_lock(&mutex);

    while ( turno != f )
        pthread_cond_wait(&TocaTurnoFila[f],&mutex);

    printf("Fila %d (Hilo %lu): ", f, pthread_self());

    for (i=0; i<n; i++) printf("%d ", A[f][i]);

    printf("\n");
    turno++;

    pthread_cond_signal (&TocaTurnoFila[turno]); //despierta específicamente al hilo del turno siguiente
    pthread_mutex_unlock(&mutex);
    pthread_exit(0);
}

int main()
{
    int i,j;
    int v[MAX_DIM];
    pthread_t hilo[MAX_DIM];

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&TocaTurnoFila[MAX_DIM],NULL);

    for (i=0; i<n; i++)
    { v[i]=i;
        pthread_create(&hilo[i], NULL, Impr, (void *)&v[i]);
    }
    for (i=0; i<n; i++) pthread_join(hilo[i], NULL);

    pthread_cond_destroy(&TocaTurnoFila);
    pthread_mutex_destroy(&mutex);
    exit(0);
}


//EJERCICIO 14

/*14. El programa que aparece m´as abajo crea tantos hilos como n´ucleos (“cores”)
tiene la m´aquina. Cada hilo ejecuta un determinado c´odigo en un n´ucleo,
mediante la funci´on ejecuta, y almacena el resultado en un fichero accesible
por todos los hilos. Modifica este programa para sincronizar el acceso de los
hilos al fichero que comparten de forma que estos guarden en ´el los resultados
proporcionados por la funci´on ejecuta en el siguiente orden: hilo 0, hilo 1,
..., hilo num hilos-1, hilo 0, hilo 1, .... Los hilos podr´ıan ejecutar la funci´on
ejecuta simult´aneamente, cada uno en su respectivo n´ucleo. Por tanto, no
ser´a necesario sincronizar la ejecuci´on de dicha funi´on entre los hilos.


Utiliza mutex y variables de condici´on como herramienta de sincronizaci´on.
F´ıjate c´omo se crean los hilos en el c´odigo de partida. La implementaci´on
ha de ser lo m´as eficiente posible. */


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define N_CORES 8

int num_hilos, fd;

//MODIFICACION
int turno = 0; //establece el orden
pthread_mutex_t mutex;
pthread_cond_t TocaTurno[N_CORES];

int ejecuta (int hilo, int n_core)
{ int resultado;
    ... // Ejecuta en n_core un c´odigo y devuelve resultado
    return resultado;
}

void escribe_en_fichero (int iteracion, int n_core, int resultado)
{ ... // Escribe en fd
}

void *f_hilo(void *indice)
{ int i, n_core, resultado;
    n_core = *(int *)indice;

    for (i=0; i<10; i++)
    {   pthread_mutex_lock(&mutex);
        while ( turno != n_core )
            pthread_cond_wait(&TocaTurno[n_core], &mutex);


        resultado = ejecuta (i, n_core);
        escribe_en_fichero (i, n_core, resultado);

        turno;
        pthread_cond_signal(&TocaTurno[turno]);

        pthread_mutex_unlock(&mutex);
    }


    pthread_exit(0);
}

int main()
{ int i, tamanyo, dato, *v;

    pthread_t *mis_hilos;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&TocaTurno[N_CORES],NULL);

    fd=open("fichero",O_WRONLY);
    num_hilos = N_CORES;

    v = malloc(num_hilos * sizeof(int));
    mis_hilos = malloc(num_hilos * sizeof(pthread_t));

    for (i = 0; i < num_hilos; i++)
    { v[i] = i;
        pthread_create(&mis_hilos[i], NULL, f_hilo, (void *)&v[i]);
    }

    for (i = 0; i < num_hilos; i++) {
        pthread_join(mis_hilos[i], NULL);
    }

    close(fd);

    pthread_mutex_destroy(&mutex);

    //importante para eliminar todos los elementos de la variable de condicion
    for (int i = 0; i < N_CORES; i++) {
        pthread_cond_destroy(&TocaTurno[i]);
    }

    free(v);
    free(mis_hilos); //cuando usamos mallocs es importante ponerlo

    exit(0);
}


//EJERCICIO 15

/*15. Se desea imprimir las tablas de multiplicar del 1 al 10 de tal manera que ´estas
se muestren respectivamente en 10 columnas consecutivas. Por lo tanto, en
la primer l´ınea se imprimir´a el primer elemento de cada una de las sucesivas
tablas (esto es, 1x1, 2x1, ..., 10x1). En la segunda l´ınea se imprimir´a el
segundo elemento de cada una de las sucesivas tablas (es decir, 1x2, 2x2,
..., 10x2). Y as´ı hasta llegar a imprimir (en la d´ecima l´ınea) el ´ultimo
elemento de cada tabla (esto es, 1x10, 2x10, ..., 10x10).

Para llevar esto a cabo se crear´an 10 hilos, uno por tabla, que se ejecutar´an
concurrentemente. Y la sincronizaci´on entre hilos se realizar´a mediante mu
tex y variables de condici´on. Todos los hilos ejecutar´an una misma funci´on a
la que se le pasar´a como argumento el n´umero de su correspondiente tabla.
El hilo principal finalizar´a cuando se hayan impreso todas las tablas.
La implementaci´on y la ejecuci´on han de ser lo m´as eficientemente posibles.
Una posible salida del programa podr´ıa ser la siguiente:

$ tablas_multiplicar
1x1=1
2x1=2
1x2=2
1x3=3
...
2x2=4
2x3=6
1x10=10 2x10=20
$
... 10x1=10
... 10x2=20
... 10x3=30
... 10x10=100
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define N_HILOS 10

int turno = 1;
pthread_mutex_t mutex;
pthread_cond_t TocaTurno[N_HILOS];

void *multi_hilo(void *arg) {

    int indice, i, resultado;

    indice = *(int *)arg;

    for (i = 1; i <= N_HILOS; i++) {
        pthread_mutex_lock(&mutex);

        while (turno != indice + 1) {
            pthread_cond_wait(&TocaTurno[indice],&mutex);
        }

        //multiplicacion
        resultado = (indice+1)*i;
        printf("%dx%d=%d\n", indice + 1, i, resultado);

        pthread_cond_signal(&TocaTurno[turno]);
        pthread_mutex_unlock(&mutex);

    }

    pthread_exit(0);

}


int main() {

    int i, *v;

    pthread_t *hilo;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&TocaTurno[N_HILOS], NULL);

    //reservar espacio para las variables en función del número de hilos

    hilo = (pthread_t *) malloc(N_HILOS * sizeof(pthread_t));
    v = (int *) malloc(N_HILOS * sizeof(int));

    for (i = 0; i < N_HILOS; i++) {
        v[i] = i;
        pthread_create(&hilo[i], NULL, multi_hilo, (void *) &v[i]);
    }

    for ( i = 0; i < N_HILOS; i++) {
        pthread_join(hilo[i], NULL);
    }

    pthread_mutex_destroy(&mutex);

    for (i = 0; i < N_HILOS; i++) {
        pthread_cond_destroy(&TocaTurno[i]);
    }

    free(v);
    free(hilo);
    exit(0);

}

//EJERCICIO 16

/*16. Resuelve el ejercicio 2 de las transparencias del tema 3 utilizando una ´unica
variable de condici´on.

Realizarunprogramaquecreedoshilos:El primerodeellosgenerará los100
primeros números pares y el segundo los 100 primeros números impares,
mostrándose todos estos por pantalla con la restricción de que no pueden
aparecendosnúmerosparesseguidosodosnúmerosimparesseguidos.
Utilizarmutexyvariablesdecondicióncomoherramientadesincronización.*/

#include <pthread.h>
#include <stdio.h>
#define MAX 200

pthread_mutex_t mutex;
pthread_cond_t esperarNum;

// 0: Le toca al hilo par | 1: Le toca al hilo impar
int turno = 0;

void *hilo_pares(void *arg) {
    int i, num_par = 0;

    for (i = 0; i < MAX; i++) {
        pthread_mutex_lock(&mutex);

        // El hilo par espera mientras sea el turno del impar
        while (turno != 0) {
            pthread_cond_wait(&esperarNum, &mutex);
        }

        printf("%d ", num_par);
        fflush(stdout);
        num_par += 2; // Siguiente número par

        // Cede el turno al hilo impar y le notifica
        turno = 1;
        pthread_cond_signal(&esperarNum);

        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(0);
}

void *hilo_impares(void *arg) {
    int i, num_impar = 1;

    for (i = 0; i < MAX; i++) {
        pthread_mutex_lock(&mutex);

        // El hilo impar espera mientras sea el turno del par
        while (turno != 1) {
            pthread_cond_wait(&esperarNum, &mutex);
        }

        printf("%d ", num_impar);
        fflush(stdout);
        num_impar += 2; // Siguiente número impar

        // Cede el turno al hilo par y le notifica
        turno = 0;
        pthread_cond_signal(&esperarNum);

        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(0);
}

int main() {

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&esperarNum, NULL);
    pthread_t hilo1, hilo2;

    pthread_create(&hilo1, NULL, hilo_pares, NULL);
    pthread_create(&hilo2, NULL, hilo_impares, NULL);

    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&esperarNum);
    exit(0);
}

//EJERCICIO 17

/*17. (Examen noviembre 2013) El siguiente programa no funciona correcta
mente. Analiza lo que debe hacer y soluci´onalo usando mutex y variables de
condici´on. Se deber´a colocar en la secci´on cr´ıtica (esto es, dentro del mutex)
s´olo las instrucciones estrictamente imprescindibles. Se valorar´a la eficiencia
de la soluci´on propuesta.

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXBUF 10

int BUF [MAXBUF];
int valor=0;

void *A (void *arg){
    int i, j;

    for (i=0; i<MAXBUF; i++)
        { scanf("%d",&j);
        BUF[i]=j;
        valor++;
    }
    pthread_exit(0);
    }

void *B (void *arg){
    int i, j;

    for (i=0; i<MAXBUF; i++)
        { j=BUF[i];
        valor--;
        printf("Leido %d\n",j);
    }
    pthread_exit(0);
    }

int main(){
    int v=0;

    pthread_t thread_A, thread_B;

    pthread_create (&thread_A, NULL, A, NULL);
    pthread_create (&thread_B, NULL, B, NULL);

    pthread_join (thread_A,NULL);
    pthread_join (thread_B,NULL);

    pthread_exit(0);
}
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXBUF 10

int BUF [MAXBUF];
int valor=0;

//mutex
pthread_mutex_t mutex;
pthread_cond_t no_Lleno;
pthread_cond_t no_Vacio;

void *A (void *arg){ //ESCRITOR
    int i, j;

    for (i=0; i<MAXBUF; i++)
    {
        pthread_mutex_lock(&mutex);

        while (valor == MAXBUF) {
            pthread_cond_wait(&no_Lleno,&mutex);
        }

        scanf("%d",&j);
        BUF[i]=j;
        valor++;

        pthread_cond_signal(&no_Vacio);
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(0);
}

void *B (void *arg){ //LECTOR
    int i, j;

    for (i=0; i<MAXBUF; i++)
    {
        pthread_mutex_lock(&mutex);
        while (valor == 0) {
            pthread_cond_wait(&no_Vacio,&mutex);
        }
        j=BUF[i];
        valor--;
        printf("Leido %d\n",j);

        pthread_cond_signal(&no_Lleno);
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(0);
}

int main(){
    int v=0;

    pthread_t thread_A, thread_B;

    //inicializar el mutex y la vc
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&no_Lleno, NULL);
    pthread_cond_init(&no_Vacio, NULL);

    pthread_create (&thread_A, NULL, A, NULL);
    pthread_create (&thread_B, NULL, B, NULL);

    pthread_join (thread_A,NULL);
    pthread_join (thread_B,NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&no_Vacio);
    pthread_cond_destroy(&no_Lleno);
    pthread_exit(0);
}

//EJERCICIO 18

/*18. (Examen noviembre 2013) Indica de forma razonada la veracidad o
falsedad de las siguientes afirmaciones:

(a) El hilo threadA del c´odigo del ejercicio anterior tiene acceso tanto a la
variable v definida en la funci´on main como a la variable valor definida
justo antes de la funci´on A.

SOLUCIÓN: Falso, threadA no tiene acceso a v porque es una variable exclusiva de main y tampoco se le pasa como argumento

(b) Cuando se crea un hilo el contador de programa del nuevo hilo apunta
a la instrucci´on siguiente a la funci´on pthread create con la que fue creado.

SOLUCIÓN: Al crear un hilo con pthread_create, el Contador de Programa (PC) del nuevo hilo se inicializa apuntando a la
primera instrucción de la función de inicio pasada como parámetro (en el ejercicio anterior, la función A). Es el hilo
creador (el hilo main) el que continúa ejecutando las instrucciones secuenciales situadas inmediatamente después de
pthread_create.

(c) Cuando se crea un hilo se le asigna una pila de ejecuci´on diferente a la
del hilo que lo cre´o.

SOLUCIÓN: Verdadero, cada hilo cuenta con su propia pila de ejecución independiente (stack). Esta pila privada es
imprescindible para gestionar sus propias variables locales, llamadas a funciones y registros de activación de manera
aislada frente a otros hilos.
*/

//EJERCICIO 19

/* 19. (Examen noviembre 2013) Explica qu´e cambios de estado se pueden
producir en un hilo cuando este ejecuta cada una de las siguientes funciones:
(a) pthread create ->listo para la ejecución el nuevo hilo. el otro sigue en ejecucion
(b) pthread self ->no hay cambios de estado, el hilo sigue en ejecución
(c) pthread cond wait -> pasa de en ejecución a bloqueado
(d) pthread join -> pasa de ejecución a bloqueado ( invocador) y el hilo indicado pasa de listo a en ejecucion
*/


//EJERCICIO 20

/*20. (Examen enero 2014) Una serie de hilos utilizan simultaneamente un
fichero. Unos acceden al fichero sin modificarlo y otros, modific´andolo. A
los primeros hilos les denominaremos lectores y a los segundos, escritores.
Para acceder al fichero se establecen las siguientes restricciones:
S´olo un escritor puede tener acceso al fichero al mismo tiempo. Mientras
el escritor est´e accediendo al fichero, ning´un otro hilo lector o escritor
podr´a acceder a ´el.

Se permite, sin embargo, que m´ultiples lectores tengan acceso al fichero,
ya que ellos nunca van a modificar el contenido del mismo.
Modifica el programa que aparece a continuaci´on para que los hilos lectores
y escritores sincronicen su ejecuci´on de la forma arriba descrita utilizando
mutex y variables de condici´on.

No es necesario implementar la lectura y la escritura en el fichero. Estas op
eraciones se simulan ya en el c´odigo de partida con la funci´on sleep(random()%3).
Asumir que siempre hay datos para leer en el fichero y espacio suficiente
para escribir en ´el.

Se valorar´a la eficiencia de la soluci´on propuesta */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define N_HILOS 10

int n_lectores, n_escritores;
pthread_mutex_t mutex;
pthread_cond_t cond_Escritores;
pthread_cond_t cond_Lectores;

// Variables globales para el estado del recurso compartido
int n_lectores = 0;
int hay_escritor = 0; // 0: libre, 1: escritor en el fichero

void *f_lector (void *indice)
{ int i, n_lector, resultado;
    n_lector = *(int *)indice;
    sleep(random()%5);

    pthread_mutex_lock(&mutex);

    while (hay_escritor) {
        pthread_cond_wait(&cond_Lectores, &mutex);
    }

    n_lectores++;
    printf("Lector %d va a leer\n",n_lector);

    pthread_mutex_unlock(&mutex);

    sleep(random()%3); /* Simula acceso lectura fichero */

    pthread_mutex_lock(&mutex);
    printf("Lector %d deja de leer\n",n_lector);
    n_lectores--;

    // Si es el último lector en salir, notifica al escritor que pudiera estar esperando
    if (n_lectores == 0) {
        pthread_cond_signal(&cond_Escritores);
    }

    pthread_mutex_unlock(&mutex);

    pthread_exit(0);
}

void *f_escritor (void *indice)
{ int i, n_escritor, resultado;
    n_escritor= *(int *)indice;
    sleep(random()%3);

    pthread_mutex_lock(&mutex);

    while (hay_escritor || n_lectores > 0) {
        pthread_cond_wait(&cond_Escritores, &mutex);
    }

    hay_escritor = 1;
    printf("Escritor %d va a escribir\n",n_escritor);

    pthread_mutex_unlock(&mutex);

    sleep(random()%3); /* Simula acceso escritura fichero */

    pthread_mutex_lock(&mutex);

    printf("Escritor %d deja de escribir\n",n_escritor);
    hay_escritor = 0;

    // Al salir el escritor, despierta a todos los lectores esperando y a un escritor
    pthread_cond_broadcast(&cond_Lectores);
    pthread_cond_signal(&cond_Escritores);

    pthread_mutex_unlock(&mutex);

    pthread_exit(0);
}

int main()
{ int i, *v;
    pthread_t *hilos_lectores, *hilos_escritores;
    time_t t;

    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&cond_Escritores, NULL);
    pthread_cond_init(&cond_Lectores, NULL);

    srandom(time(&t));
    hilos_lectores = malloc(N_HILOS * sizeof(pthread_t));
    hilos_escritores = malloc(N_HILOS * sizeof(pthread_t));
    v = malloc(N_HILOS * sizeof(int));
    for (i = 0; i < N_HILOS; i++)
    { v[i] = i;
        pthread_create(&hilos_lectores[i], NULL, f_lector, (void *)&v[i]);
    }
    for (i = 0; i < N_HILOS; i++)
        pthread_create(&hilos_escritores[i], NULL, f_escritor, (void *)i);

    for (i = 0; i < N_HILOS; i++) pthread_join(hilos_lectores[i], NULL);
    for (i = 0; i < N_HILOS; i++) pthread_join(hilos_escritores[i], NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_Escritores);
    pthread_cond_destroy(&cond_Lectores);
    exit(0);
}

//EJERCICIO 21

/*21. (Examen junio 2014)RealizaunprogramaenCquecreeunhiloproductor
y 2 hilos consumidores y resuelva el problema del productor-consumidor con
buffer circular limitado mediante mutex y variables de condici´on. Se valorar´a
la eficiencia de la soluci´on propuesta */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_BUFFER 5
#define DATOS 20

int buffer[MAX_BUFFER]; //array que almacena los elementos

int n_elementos = 0; //elementos actuales del buffer
int pos_cons = 0; //variable global porque hay más de un consumidor

pthread_cond_t noLleno;
pthread_cond_t noVacio;
pthread_mutex_t mutex;

void *Productor(void *arg) {
    int i, dato, pos = 0;

    for (i = 0; i < DATOS; i++) {
        dato = i+1; //generar el dato fuera del mutex

        pthread_mutex_lock(&mutex);

        while (n_elementos == MAX_BUFFER) {
            pthread_cond_wait(&noLleno, &mutex);
        }

        buffer[pos] = dato;
        pos = (pos + 1) % MAX_BUFFER;
        n_elementos++;

        pthread_cond_signal(&noVacio);
        pthread_mutex_unlock(&mutex);

    }

    pthread_exit(0);
}

void *Consumidor(void * arg) {
    int i, dato;

    for (i = 0; i < DATOS; i++) {

        pthread_mutex_lock(&mutex);

        while (n_elementos == 0) {
            pthread_cond_wait(&noVacio, &mutex);
        }

        dato = buffer[pos_cons];
        pos_cons = (pos_cons + 1) % MAX_BUFFER;
        n_elementos--;

        pthread_cond_signal(&noLleno);
        pthread_mutex_unlock(&mutex);

    }
    pthread_exit(0);
}

int main() {

    pthread_t productor, consumidor1, consumidor2;

    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&noLleno, NULL);
    pthread_cond_init(&noVacio, NULL);

    pthread_create(&productor, NULL, Productor, NULL);
    pthread_create(&consumidor1, NULL, Consumidor, NULL);
    pthread_create(&consumidor2, NULL, Consumidor, NULL);

    pthread_join(productor, NULL);
    pthread_join(consumidor1, NULL);
    pthread_join(consumidor2, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&noVacio);
    pthread_cond_destroy(&noLleno);

    exit(0);
}
