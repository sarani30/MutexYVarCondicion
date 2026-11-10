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