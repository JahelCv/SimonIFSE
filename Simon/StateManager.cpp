/*
 * StateManager.cpp
 *
 *  Created on: 30 mar. 2019
 *      Author: jahel
 */
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

class StateMonitor {

	private:

		static const int MAXLISTENERS = 6;
		static const int MAXSTATES = 6;
		static const int MAXVELMOSTRAR = 5;
		static const int MAXTIMEOUT = 5;
		static const int BOTONROJO = 1;
		static const int BOTONAZUL = 2;
		static const int BOTONVERDE = 3;
		static const int BOTONAMARILLO = 4;
		static const int MAXNIVELES = 30;
		int prev_state; // Antes de la pausa, que estado habia.
		int contador_btn; // indice de btn_colores_pulsados
		int btn_colores_pulsados[MAXNIVELES]; // Acumulo aqui los valores leidos en orden de llegada
		int internalState; // 0,1,2
		int nivel; // nivel actual de juego
		int velmostrar; // velocidad de muestreo de luces en estado 1
		int timeout; // timeout limite del jugador para introducir la secuencia en estado 2
		int microsec_acumulados; // tiempo acumulado por el jugador. Si excede timeout, falla.
		int secuencia_generada[MAXNIVELES]; // secuencia generada en estado 1.
		pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
		pthread_cond_t condition = PTHREAD_COND_INITIALIZER; // @suppress("Invalid arguments")
		void* (*func_table[MAXSTATES][MAXSTATES][MAXLISTENERS])(int stFrom, int stTo);

	public:

	  StateMonitor() {
		prev_state = 0;
		internalState = 0;
		nivel = 0;
		microsec_acumulados = 0;
		contador_btn = 0;
		velmostrar = 1;
		timeout = 1;
		int i,j,k;
		for(i=0;i<MAXSTATES;i++)
		  for(j=0;j<MAXSTATES;j++)
			for(k=0;k<MAXLISTENERS;k++) {
		  		func_table[i][j][k] = NULL;
		    }
		for(i=0;i<MAXNIVELES;i++){
			btn_colores_pulsados[i] = 0;
		}
	  }

		int waitState(int st[], int length) {
			int flag = 0;

			pthread_mutex_lock(&mutex);

			for (int n=0; n<length; ++n){
				if (st[n] != internalState) {
					flag = 0;
				} else {
					//printf("Estado: %d, WaitState: Se encuentra estado %d\n",internalState, st[n]);
					flag = 1;
					break;
				}
			}
			//printf("Estado: %d, WaitState: Antes del while\n",internalState);
			while (flag == 0){
				pthread_cond_wait(&condition, &mutex);
				//printf("Estado: %d, WaitState: Despues del condwait\n",internalState);
				for (int n=0; n<length; ++n){
					if (st[n] != internalState) {
						flag = 0;
					} else {
						flag = 1;
						break;
					}
				}
			}

		  pthread_mutex_unlock(&mutex);
		  return(0);
		}

		int addStateChangeListener(int fromState, int toState, void* (*handle)(int,int)) {
			pthread_mutex_lock(&mutex);
			int i = 0;
			int retval = 1;
			for(i=0;i<MAXLISTENERS;i++) {
				if (func_table[fromState][toState][i] == NULL) {
					retval = 0;
					func_table[fromState][toState][i] = handle;
					break;
				}
			}
			pthread_mutex_unlock(&mutex);
			return(retval);
		}

		int changeState(int st) {
			if (st>MAXSTATES) return(1);
			pthread_mutex_lock(&mutex);
			int i;
			for(i=0;i<MAXLISTENERS;i++) {
				if (func_table[internalState][st][i] != NULL) {
				 (*func_table[internalState][st][i])(nivel,st);
				}
			}
			prev_state = internalState;
			internalState = st;
			pthread_cond_broadcast(&condition);
			//printf("Estado: %d, ChangeState: Despues del cond_broadcast.\n",internalState);
			pthread_mutex_unlock(&mutex);
			return(0);
		}

		int getState() {
			int aux;
			pthread_mutex_lock(&mutex);
			aux = internalState;
			pthread_mutex_unlock(&mutex);
			return(aux);
		}

		int getPrevState(){
			int aux;
			pthread_mutex_lock(&mutex);
			aux = prev_state;
			pthread_mutex_unlock(&mutex);
			return(aux);
		}

		/**
		 * Operaciones sobre timeout
		 */

		// Poner directamente ya
		int setTimeout(int tim) {
			int aux = 0;
			pthread_mutex_lock(&mutex);
			timeout = tim;
			printf("STATE MANAGER, SetTimeout: %d\n",timeout);
			pthread_mutex_unlock(&mutex);
			return(aux);
		  }

		// Devuelve 1 si se ha pasado del timeout. Sino, 0.
		int aumentarMicrosecAcumulados(int microsec){
			pthread_mutex_lock(&mutex);
			int timeoutvencido = 0;
			microsec_acumulados = microsec_acumulados + microsec;
			//printf("## Microsec acumulados: %d\n", microsec_acumulados);
			if (microsec_acumulados > timeout){
				timeoutvencido = 1;
			}
			pthread_mutex_unlock(&mutex);
			return(timeoutvencido);
		}

		int resetMicrosecAcumulados(){
			pthread_mutex_lock(&mutex);
			microsec_acumulados = 0;
			pthread_mutex_unlock(&mutex);
			return(0);
		}

		/**
		 * Operaciones sobre velocidad de muestra de secuencia al jugador
		 */

		int setVelocidad(int vel) {
			int aux = 0;
			pthread_mutex_lock(&mutex);
			velmostrar = vel;
			printf("STATE MANAGER, SetVelocidad: %d\n",velmostrar);
			pthread_mutex_unlock(&mutex);
			return(aux);
		  }

		int getVelocidad() {
			int aux = 0;
			pthread_mutex_lock(&mutex);
			aux = velmostrar;
			pthread_mutex_unlock(&mutex);
			return(aux);
		  }

		/**
		 * Operaciones sobre nivel
		 */

		int incrementarNivel() {
			int aux;
			pthread_mutex_lock(&mutex);
			nivel = nivel + 1;
			aux = nivel;
			pthread_mutex_unlock(&mutex);
			//printf("Nivel: %d.\n",aux);
			return(aux);
		}

		int resetNivel() {
			int aux;
			pthread_mutex_lock(&mutex);
			nivel = 0;
			aux = nivel;
			pthread_mutex_unlock(&mutex);
			return(aux);
		}

		int getNivel() {
			int aux;
			pthread_mutex_lock(&mutex);
			aux = nivel;
			pthread_mutex_unlock(&mutex);
			return(aux);
		}

		/**
		 * Operaciones sobre las secuencias
		 */
		int borrarSecuencias(){
			pthread_mutex_lock(&mutex);
                        int i;
                        // Refrescamos secuencia de btns introducida por usuario
                        for(i = 0; i < MAXNIVELES; i++){
                                btn_colores_pulsados[i] = 0;
				secuencia_generada[i] = 0;
                        }
			pthread_mutex_unlock(&mutex);
                        return(0);

		}

		int setBotonPulsado(int btn){
			pthread_mutex_lock(&mutex);
			btn_colores_pulsados[contador_btn] = btn;
			contador_btn = contador_btn + 1;
			printf("setBotonPulsado: Pulsador: %d ## Contador: %d\n",btn,contador_btn);
			pthread_mutex_unlock(&mutex);
			return(0);
		}

		int* getSecuencia(){
			//int result[MAXNIVELES];
			//int result*;
			//for (int i = 0; i < MAXNIVELES; i++){
			//	result[i] = secuencia_generada[i];
			//}
			return secuencia_generada;
		}

		// Incluye un nuevo item a la secuencia
		int setSecuencia(int sec){
			pthread_mutex_lock(&mutex);
			int i;
			// Borra los botones pulsados
			for(i = 0; i < contador_btn; i++){
				btn_colores_pulsados[i] = 0;
			}
			contador_btn = 0;
			printf("STATE MANAGER -- Secuencia en monitor: ");
			// Guardamos secuencia generada
			for(i = 0; i < MAXNIVELES; i++){
				if(i == nivel){
					secuencia_generada[i] = sec;
				} else if (i > nivel){
					secuencia_generada[i] = 0;
				}
				printf("%d, ",secuencia_generada[i]);
			}
			printf("\n");
			pthread_mutex_unlock(&mutex);
			return(0);
		}

		// Devuelve 1 si valores correcto, si 2 toda secuencia correcta
		int aciertoSecuencia(){
			pthread_mutex_lock(&mutex);
			int acierto = 0;
			int i;
			//printf("STATEMANAGER: Contador_btn = %d\n",contador_btn);
			if(contador_btn == 0){
				acierto = 1;
			} else {
				for(i = 0; i < contador_btn; i++){
                                	 //printf("btn_colores_pulsados[] = %d\n",btn_colores_pulsados[i]);
                                 	//printf("secuencia_generada[] = %d\n", secuencia_generada[i]);

	                                if (btn_colores_pulsados[i] != secuencia_generada[i]){
        	                                //printf("btn_colores_pulsados[] = %d\n",btn_colores_pulsados[i]);
                	                        //printf("secuencia_generada[] = %d\n", secuencia_generada[i]);
                        	                acierto = 0;
                                	        break;
	                                } else {
        	                                acierto = 1;
                	                }
                       		 }
                        	// Si ha acertado la secuencia y ha alcanzado el nivel, se pone 2
	                        if (acierto == 1){
        	                        if (contador_btn == nivel){
                	                        acierto = 2;
                        	        }
                        	}

			}
			pthread_mutex_unlock(&mutex);
			return(acierto);
		}
};

