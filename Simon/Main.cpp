/*
 * Author: Jahel Carmona, Gabriel Valencia (2019)
 * Universitat Politecnica de Valencia. 
 */

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "StateManager.cpp"
#include "HW/GPIOuniv.h"
#include "HW/PWMuniv.h"
#include "HW/Buzzer.h"

using namespace BBB;
using namespace std;

#define LDR_PATH "/sys/bus/iio/devices/iio:device0/in_voltage"

GPIO start_puls(GPIO::P9_17); // P9_17
GPIO pausa_puls(GPIO::P9_18); // P9_18
GPIO azul_led(GPIO::P9_27); //P9_27
GPIO azul_puls(GPIO::P9_13);//P9_13
GPIO rojo_led(GPIO::P9_23); //P9_23
GPIO rojo_puls(GPIO::P9_11); //P9_11
GPIO verde_led(GPIO::P9_21); //P9_21
GPIO verde_puls(GPIO::P9_12); //P9_12
GPIO amar_led(GPIO::P9_30); //P9_30
GPIO amar_puls(GPIO::P9_15); //P9_15
GPIO uni_A(GPIO::P8_7); //P8_7
GPIO uni_B(GPIO::P8_8); //P8_8
GPIO uni_C(GPIO::P8_9);
GPIO uni_D(GPIO::P8_10);
GPIO dec_A(GPIO::P8_11);
GPIO dec_B(GPIO::P8_12);
GPIO dec_C(GPIO::P8_14);
GPIO dec_D(GPIO::P8_15);

PWM pwmservo1(PWM::P9_14); // Timeout
PWM pwmservo2(PWM::P9_16); // Velocidad
PWM pwmbuzzer(PWM::P8_19); // Buzzer

StateMonitor stateManager;

static const int BOTONROJO = 1;
static const int BOTONAZUL = 2;
static const int BOTONVERDE = 3;
static const int BOTONAMARILLO = 4;
static const double MAXVALPOTENCIOMETRO = 4095.0;
static const double MAXVALPOTENCIOMETRO_T = 4025.0;
static const int MAXNIVEL = 100;
static const int velocidadm_op[] = {2000000, 1000000, 400000, 300000, 100000};
static const int timeout_op[] = {120000000, 60000000, 30000000, 15000000, 7000000};


int display_unidades_7segmentos(int number){
	// Se convierte a binario en secuencia DCBA
	switch(number){
		// 0 = 0000
		case 0: uni_D.setValue(GPIO::LOW);
			uni_C.setValue(GPIO::LOW);
			uni_B.setValue(GPIO::LOW);
			uni_A.setValue(GPIO::LOW);
			break;
		// 1 = 0001
		case 1: uni_D.setValue(GPIO::LOW);
                        uni_C.setValue(GPIO::LOW);
                        uni_B.setValue(GPIO::LOW);
                        uni_A.setValue(GPIO::HIGH);
                        break;
		// 2 = 0010
		case 2: uni_D.setValue(GPIO::LOW);
                        uni_C.setValue(GPIO::LOW);
                        uni_B.setValue(GPIO::HIGH);
                        uni_A.setValue(GPIO::LOW);
                        break;
		//3  = 0011
                case 3: uni_D.setValue(GPIO::LOW);
                        uni_C.setValue(GPIO::LOW);
                        uni_B.setValue(GPIO::HIGH);
                        uni_A.setValue(GPIO::HIGH);
                        break;
		 //4  = 0100
                case 4: uni_D.setValue(GPIO::LOW);
                        uni_C.setValue(GPIO::HIGH);
                        uni_B.setValue(GPIO::LOW);
                        uni_A.setValue(GPIO::LOW);
                        break;
		 //5  = 0101
                case 5: uni_D.setValue(GPIO::LOW);
                        uni_C.setValue(GPIO::HIGH);
                        uni_B.setValue(GPIO::LOW);
                        uni_A.setValue(GPIO::HIGH);
                        break;
		 //6  = 0110
                case 6: uni_D.setValue(GPIO::LOW);
                        uni_C.setValue(GPIO::HIGH);
                        uni_B.setValue(GPIO::HIGH);
                        uni_A.setValue(GPIO::LOW);
                        break;
		//7  = 0111
                case 7: uni_D.setValue(GPIO::LOW);
                        uni_C.setValue(GPIO::HIGH);
                        uni_B.setValue(GPIO::HIGH);
                        uni_A.setValue(GPIO::HIGH);
                        break;
		//8  = 1000
                case 8: uni_D.setValue(GPIO::HIGH);
                        uni_C.setValue(GPIO::LOW);
                        uni_B.setValue(GPIO::LOW);
                        uni_A.setValue(GPIO::LOW);
                        break;
		//9  = 1001
                case 9: uni_D.setValue(GPIO::HIGH);
                        uni_C.setValue(GPIO::LOW);
                        uni_B.setValue(GPIO::LOW);
                        uni_A.setValue(GPIO::HIGH);
                        break;
	}
	return 0;
}

int display_decimas_7segmentos(int number){
	// Se convierte a binario en secuencia DCBA
        switch(number){
                // 0 = 0000
                case 0: dec_D.setValue(GPIO::LOW);
                        dec_C.setValue(GPIO::LOW);
                        dec_B.setValue(GPIO::LOW);
                        dec_A.setValue(GPIO::LOW);
                        break;
                // 1 = 0001
                case 1: dec_D.setValue(GPIO::LOW);
                        dec_C.setValue(GPIO::LOW);
                        dec_B.setValue(GPIO::LOW);
                        dec_A.setValue(GPIO::HIGH);
                        break;
                // 2 = 0010
                case 2: dec_D.setValue(GPIO::LOW);
                        dec_C.setValue(GPIO::LOW);
                        dec_B.setValue(GPIO::HIGH);
                        dec_A.setValue(GPIO::LOW);
                        break;
                //3  = 0011
                case 3: dec_D.setValue(GPIO::LOW);
                        dec_C.setValue(GPIO::LOW);
                        dec_B.setValue(GPIO::HIGH);
                        dec_A.setValue(GPIO::HIGH);
                        break;
                 //4  = 0100
                case 4: dec_D.setValue(GPIO::LOW);
                        dec_C.setValue(GPIO::HIGH);
                        dec_B.setValue(GPIO::LOW);
                        dec_A.setValue(GPIO::LOW);
                        break;
                 //5  = 0101
                case 5: dec_D.setValue(GPIO::LOW);
                        dec_C.setValue(GPIO::HIGH);
                        dec_B.setValue(GPIO::LOW);
                        dec_A.setValue(GPIO::HIGH);
                        break;
                 //6  = 0110
                case 6: dec_D.setValue(GPIO::LOW);
                        dec_C.setValue(GPIO::HIGH);
                        dec_B.setValue(GPIO::HIGH);
                        dec_A.setValue(GPIO::LOW);
                        break;
                //7  = 0111
                case 7: dec_D.setValue(GPIO::LOW);
                        dec_C.setValue(GPIO::HIGH);
                        dec_B.setValue(GPIO::HIGH);
                        dec_A.setValue(GPIO::HIGH);
                        break;
                //8  = 1000
                case 8: dec_D.setValue(GPIO::HIGH);
                        dec_C.setValue(GPIO::LOW);
                        dec_B.setValue(GPIO::LOW);
                        dec_A.setValue(GPIO::LOW);
                        break;
                //9  = 1001
                case 9: dec_D.setValue(GPIO::HIGH);
                        dec_C.setValue(GPIO::LOW);
                        dec_B.setValue(GPIO::LOW);
                        dec_A.setValue(GPIO::HIGH);
                        break;
        }
        return 0;
}

int set7Segmentos(int number){
        if (number < 0){
                return -1;
        } else {
                if (number < 10){
                        display_decimas_7segmentos(0);
                        display_unidades_7segmentos(number);
                } else {
                        int decimas = (int)(number/10);
                        int unidades = number;
                        while(unidades >= 10){
                                unidades = unidades % 10;
                        }
			printf("Se calcula DECIMAS: %d y UNIDADES: %d\n", decimas, unidades);
                        display_unidades_7segmentos(unidades);
                        display_decimas_7segmentos(decimas);
                }
        }
        return 0;
}

// Number es AIN y el numero que acompaña. Si es P9_39, es 0 (AIN0)
int readAnalog(int number){
   stringstream ss;
   ss << LDR_PATH << number << "_raw";
   fstream fs;
   fs.open(ss.str().c_str(), fstream::in);
   fs >> number;
   fs.close();
   return number;
}

int generadorSecuencias(){
	// int result[n];
	int i;
	srand((unsigned)time(0));
	return (rand() % 4) + 1;
}

/*
 * Hilo que muestra la secuencia a repetir por el jugador
 */
void *hilo_muestra_sec(void *param){

	struct sched_param sp;
	sp.sched_priority = 5;
	// Iniciamos RT en hilo
	if(pthread_setschedparam(pthread_self(), SCHED_FIFO, &sp)){
		fprintf(stderr,"WARNING: Failed to set thread to real-time priority\n");
	}

	int array[]={1};
	int length = 1;
	int nivel;
	int secuencia[MAXNIVEL];
	int *secresult;
	int selectedvelmuestreo;
	int i;
	for(;;){
		stateManager.waitState(array,length); // Espera a estado 1
		detenerDescuento(&pwmbuzzer);
		stateManager.setSecuencia(generadorSecuencias());
		secresult = stateManager.getSecuencia();
		nivel = stateManager.incrementarNivel();
		for(i = 0; i < nivel; i++){
			stateManager.waitState(array,length);
			selectedvelmuestreo = stateManager.getVelocidad();
			if (secresult[i] == BOTONROJO){
				rojo_led.setValue(GPIO::HIGH);
				usleep(selectedvelmuestreo);
				rojo_led.setValue(GPIO::LOW);
			}
			if (secresult[i] == BOTONAZUL){
				azul_led.setValue(GPIO::HIGH);
				usleep(selectedvelmuestreo);
				azul_led.setValue(GPIO::LOW);
			}
			if (secresult[i] == BOTONVERDE){
				verde_led.setValue(GPIO::HIGH);
				usleep(selectedvelmuestreo);
				verde_led.setValue(GPIO::LOW);
			}
			if (secresult[i] == BOTONAMARILLO){
				amar_led.setValue(GPIO::HIGH);
				usleep(selectedvelmuestreo);
				amar_led.setValue(GPIO::LOW);
			}
			if(i != (nivel-1)) {usleep(selectedvelmuestreo/2);}

		}
		empezarDescuento(&pwmbuzzer);
		stateManager.changeState(2);
	}
	return(0);
}

/*
 * Lee cada X segundos que boton se ha encendido. Si lo hace,
 * se comprueba al stateManager que sea la especificada.
 */
void *hilo_lector_jugador(void *param) {
  struct sched_param sp;
  sp.sched_priority = 5;
  // Iniciamos RT en hilo
  if(pthread_setschedparam(pthread_self(), SCHED_FIFO, &sp)){
	fprintf(stderr,"WARNING: Failed to set thread to real-time priority\n");
  }

  int array[]={2};
  int length = 1;
  int statemanager_acierto;
  int timeoutexcedido;
  for (;;) {
	usleep(500000); // Cada 0.5 segundos miramos si ya ha acertado
      	stateManager.waitState(array,length);
      	timeoutexcedido = stateManager.aumentarMicrosecAcumulados(500000);
      	if(timeoutexcedido == 1){
    	  printf("2 - Lectura de secuencia. Timeout excedido: %d\n", timeoutexcedido);
	  detenerDescuento(&pwmbuzzer);
    	  stateManager.changeState(0);
      	} else {
    	  statemanager_acierto = stateManager.aciertoSecuencia();
    	  if (statemanager_acierto == 2){
    		  printf("2 - Hilo lectura secuencia del jugador: ACIERTO\n");
		  stateManager.resetMicrosecAcumulados();
		  detenerDescuento(&pwmbuzzer);
    		  stateManager.changeState(1);
    	  } else if (statemanager_acierto == 0){
    		  printf("2 - Hilo lectura secuencia del jugador: FALLO\n");
    		  stateManager.changeState(0);
    	  }
      }
  }
  return(0);
}

/**
 * Hilo que lee del potenciometro y setea el valor en el servo (conjunto 1, timeout)
 */
void *hilo_servo1(void *param){

	struct sched_param sp;
	sp.sched_priority = 5;
	// Iniciamos RT en hilo
	if(pthread_setschedparam(pthread_self(), SCHED_FIFO, &sp)){
		fprintf(stderr,"WARNING: Failed to set thread to real-time priority\n");
	}

	unsigned int sval_min = 350000;
	unsigned int sval_max = 3000000;

	pwmservo1.setPeriod(20000000);
	pwmservo1.run();
	int ant_val_pot = 0; // Valor anterior (guardado en StateManager)
	int act_val_pot = 0; // Valor actual leido del potenciometro
	double divisiones = MAXVALPOTENCIOMETRO/8;
	stateManager.setTimeout(timeout_op[ant_val_pot]);

	for (;;) {
		int value = readAnalog(0); // AIN0 (P9_39) + servo en P9_14
		double aval_pu = (double)(MAXVALPOTENCIOMETRO - value)/MAXVALPOTENCIOMETRO;
		unsigned duty_ref = sval_min + aval_pu * (sval_max - sval_min);
		pwmservo1.setDutyCycle(duty_ref);

		if ((double)value < 890.0){
			act_val_pot = 0;
		} else if((double)value >= 890.0 && (double)value < divisiones*3){
			act_val_pot = 1;
		} else if((double)value >= divisiones*3 && (double)value < divisiones*5){
			act_val_pot = 2;
		} else if((double)value >= divisiones*5 && (double)value < 3400.0){
			act_val_pot = 3;
		} else if((double)value >= 3400.0){
			act_val_pot = 4;
		} else { // Error!!!
			act_val_pot = 0;
		}

		if(act_val_pot != ant_val_pot){
			printf("HILO SETTIMEOUT: Seleccionado timeout: %d\n",act_val_pot);
			stateManager.setTimeout(timeout_op[act_val_pot]);
			ant_val_pot = act_val_pot;
		}

		usleep(100000);
	}

	pwmservo1.stop();
}

/**
 * Hilo que lee del potenciometro y setea el valor en el servo (conjunto 2, vel muestreo)
 */
void *hilo_servo2(void *param){

	struct sched_param sp;
	sp.sched_priority = 5;	
	// Iniciamos RT en hilo
	if(pthread_setschedparam(pthread_self(), SCHED_FIFO, &sp)){
		fprintf(stderr,"WARNING: Failed to set thread to real-time priority\n");
	}

	unsigned int sval_min = 500000;
	unsigned int sval_max = 2300000;

	pwmservo2.setPeriod(20000000);
	pwmservo2.run();
	int ant_val_pot = 0; // Valor anterior (guardado en StateManager)
	int act_val_pot = 0; // Valor actual leido del potenciometro
	double divisiones = MAXVALPOTENCIOMETRO/8;
	stateManager.setVelocidad(velocidadm_op[ant_val_pot]);

	for (;;) {
		int value = readAnalog(1); // AIN1 (P9_40) + servo en P9_16
		double aval_pu = (double)(MAXVALPOTENCIOMETRO - value)/MAXVALPOTENCIOMETRO;
		unsigned duty_ref = sval_min + aval_pu * (sval_max - sval_min);
		pwmservo2.setDutyCycle(duty_ref);

		if ((double)value < divisiones){
			act_val_pot = 0;
		} else if((double)value >= divisiones && (double)value < divisiones*3){
			act_val_pot = 1;
		} else if((double)value >= divisiones*3 && (double)value < divisiones*5){
			act_val_pot = 2;
		} else if((double)value >= divisiones*5 && (double)value < divisiones*7){
			act_val_pot = 3;
		} else if((double)value >= divisiones*7){
			act_val_pot = 4;
		} else { // Error!!!
			act_val_pot = 0;
		}

		if(act_val_pot != ant_val_pot){
			printf("HILO SETVELMUESTREO: Seleccionada vel muestreo: %d\n",act_val_pot);
			stateManager.setVelocidad(velocidadm_op[act_val_pot]);
			ant_val_pot = act_val_pot;
		}

		usleep(100000);
	}

	pwmservo2.stop();
}

void *hilo_inicio(void *param){
	inicio(&pwmbuzzer);
	detenerDescuento(&pwmbuzzer);
}

void *iniciarJuego(int nivel, int stTo) {
	printf("Callback: Iniciar juego. Se pasa a estado: %d. Nivel: %d.\n",stTo,nivel);
	set7Segmentos(nivel+1);
	detenerDescuento(&pwmbuzzer);
	return NULL;
}
void *fallo_callback(int nivel, int stTo){
	printf("FALLO CALLBACK: Fallo!\n");
	detenerDescuento(&pwmbuzzer);
	usleep(100000);
	fallo(&pwmbuzzer);
	return NULL;
}

void *volverInicio(int nivel, int stTo) {
	printf("Callback: Juego reiniciado. Se pasa a estado: %d. Nivel: %d.\n",stTo,nivel);
	detenerDescuento(&pwmbuzzer);
	return NULL;
}

void *aciertoSecuencia(int nivel, int stTo) {
	printf("Callback: Acierto secuencia. Se pasa a estado: %d. Nivel: %d.\n", stTo, nivel);
	// Si va a estado 1, que es muestra, aumenta el 7segmentos
	if (stTo == 1){
		set7Segmentos(nivel+1);
	}
	acierto(&pwmbuzzer);
	detenerDescuento(&pwmbuzzer);
	return NULL;
}

/**
 * Pulsar STOP/PLAY desencadena esta funcion
 */
int GPIOEventHandlerStart(int arg){
	printf("STOP/START Pulsado.\n");
	int estado = stateManager.getState();
	stateManager.resetNivel(); // n=0
	stateManager.resetMicrosecAcumulados();
	stateManager.borrarSecuencias();
	if (estado == 0){
		printf("0 - Handler boton. START JUEGO. Cambiando a estado 1...\n");
		stateManager.changeState(1);
	} else {
		printf("0 - Handler boton. STOP JUEGO. Cambiando a estado 0...\n");
		stateManager.changeState(0);
	}
	return(0);
}

/*
 * Pulsar PAUSE/PLAY pasa de estado X a 3.
 */
int GPIOEventHandlerPausa(int arg){
	printf("PAUSA pulsado.\n");
	int estado = stateManager.getState();
	cout << "PAUSA -> Antes de if: " << estado << endl;
	// Para estado 0 no se hace nada.
	if (estado == 1 || estado == 2){
		cout << "PAUSA -> Si es 1 o 2: Estado actual: " << estado << endl;
		stateManager.changeState(3);
		detenerDescuento(&pwmbuzzer);
		// Si esta en pausa y hay que darle al play, volvemos al estado anterior de la pausa.
	} else if(estado == 3){
		int prev = stateManager.getPrevState();
		cout << "PAUSA -> Estado actual: " << estado << " # Estado previo: " << prev << endl; 
		if (prev == 2){
			empezarDescuento(&pwmbuzzer);
		}
		stateManager.changeState(prev);
	}
	return(0);
}
/*
 * Estos metodos colocan la secuencia generada por el jugador
 */
int GPIOEventHandlerAzul(int arg){
	if(stateManager.getState()==2){
		stateManager.setBotonPulsado(BOTONAZUL);
		suenaAzul(&pwmbuzzer);
		empezarDescuento(&pwmbuzzer);
	} else {
		detenerDescuento(&pwmbuzzer);
	}
	return(0);
}
int GPIOEventHandlerVerde(int arg){
	if(stateManager.getState()==2){
		stateManager.setBotonPulsado(BOTONVERDE);
		suenaVerde(&pwmbuzzer);
		empezarDescuento(&pwmbuzzer);
	} else {
		detenerDescuento(&pwmbuzzer);
	}
	return(0);
}
int GPIOEventHandlerAmarillo(int arg){
	if(stateManager.getState()==2){
		stateManager.setBotonPulsado(BOTONAMARILLO);
		suenaAmar(&pwmbuzzer);
		empezarDescuento(&pwmbuzzer);
	} else {
		detenerDescuento(&pwmbuzzer);
	}
	return(0);
}
int GPIOEventHandlerRojo(int arg){
	if(stateManager.getState()==2){
		stateManager.setBotonPulsado(BOTONROJO);
		suenaRojo(&pwmbuzzer);
		empezarDescuento(&pwmbuzzer);
	} else {
		detenerDescuento(&pwmbuzzer);
	}
	return(0);
}

/*
 * Estados:
 * 0: Inicio
 * 1: Mostrar
 * 2: Introducir
 * 3: Pausa
 */
int main (void) {
	pthread_t hilo_ini, hilo_mues, hilo_lec, hilo_setservo1, hilo_setservo2;
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	stateManager.addStateChangeListener(0,1,iniciarJuego); //Inicio
	stateManager.addStateChangeListener(1,0,volverInicio); //Boton inicio
	stateManager.addStateChangeListener(2,0,fallo_callback); //Boton inicio, fallo o timeout
	stateManager.addStateChangeListener(2,1,aciertoSecuencia); //Jugador acierta secuencia

	printf("Empezando....\n");
	stateManager.changeState(0);

	azul_led.setDirection(GPIO::OUTPUT);
	rojo_led.setDirection(GPIO::OUTPUT);
	verde_led.setDirection(GPIO::OUTPUT);
	amar_led.setDirection(GPIO::OUTPUT);
	uni_B.setDirection(GPIO::OUTPUT);
        uni_C.setDirection(GPIO::OUTPUT);
        uni_D.setDirection(GPIO::OUTPUT);
        uni_A.setDirection(GPIO::OUTPUT);
        dec_B.setDirection(GPIO::OUTPUT);
        dec_C.setDirection(GPIO::OUTPUT);
        dec_D.setDirection(GPIO::OUTPUT);
        dec_A.setDirection(GPIO::OUTPUT);

	start_puls.setDirection(GPIO::INPUT);
	start_puls.setActiveHigh();
	start_puls.setEdgeType(GPIO::RISING);
	start_puls.setDebounceTime(500);
	start_puls.waitForEdge(GPIOEventHandlerStart);

	pausa_puls.setDirection(GPIO::INPUT);
	pausa_puls.setActiveHigh();
	pausa_puls.setEdgeType(GPIO::RISING);
	pausa_puls.setDebounceTime(500);
	pausa_puls.waitForEdge(GPIOEventHandlerPausa);

	azul_puls.setDirection(GPIO::INPUT);
	azul_puls.setActiveHigh();
	azul_puls.setEdgeType(GPIO::RISING);
	azul_puls.setDebounceTime(300);
	azul_puls.waitForEdge(GPIOEventHandlerAzul);

	rojo_puls.setDirection(GPIO::INPUT);
	rojo_puls.setActiveHigh();
	rojo_puls.setEdgeType(GPIO::RISING);
	rojo_puls.setDebounceTime(300);
	rojo_puls.waitForEdge(GPIOEventHandlerRojo);

	verde_puls.setDirection(GPIO::INPUT);
	verde_puls.setActiveHigh();
	verde_puls.setEdgeType(GPIO::RISING);
	verde_puls.setDebounceTime(300);
	verde_puls.waitForEdge(GPIOEventHandlerVerde);

	amar_puls.setDirection(GPIO::INPUT);
	amar_puls.setActiveHigh();
	amar_puls.setEdgeType(GPIO::RISING);
	amar_puls.setDebounceTime(300);
	amar_puls.waitForEdge(GPIOEventHandlerAmarillo);

	int estados_lector_jugador[] = {2};
	pthread_create(&hilo_ini,&attr,hilo_inicio,estados_lector_jugador);

	for(int i = 0; i < 6; i++){
		rojo_led.setValue(GPIO::HIGH);
		azul_led.setValue(GPIO::HIGH);
		verde_led.setValue(GPIO::HIGH);
		amar_led.setValue(GPIO::HIGH);
		usleep(500000);
		rojo_led.setValue(GPIO::LOW);
                azul_led.setValue(GPIO::LOW);
                verde_led.setValue(GPIO::LOW);
                amar_led.setValue(GPIO::LOW);
		usleep(250000);
	}

    	pthread_create(&hilo_lec,&attr,hilo_lector_jugador,estados_lector_jugador);
	// Muestrea en el estado 1
    	int estados_muestra_jugador[] = {1};
    	pthread_create(&hilo_mues,&attr,hilo_muestra_sec,estados_muestra_jugador);

    	pthread_create(&hilo_setservo1,&attr,hilo_servo1,NULL);
    	pthread_create(&hilo_setservo2,&attr,hilo_servo2,NULL);

	pthread_join(hilo_ini,NULL);
    	pthread_join(hilo_setservo1,NULL);
    	pthread_join(hilo_setservo1,NULL);
    	pthread_join(hilo_lec,NULL);
    	pthread_join(hilo_mues,NULL);
}
