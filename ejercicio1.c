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

SOLUCIÓN:

(b) ¿Qu´e ocurre si extraemos las funciones pthread_mutex_lock y pthread_mutex_unlock fuera del bucle for?


*/