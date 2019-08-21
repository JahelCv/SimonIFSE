#include <stdio.h>
#include <unistd.h>
#include "PWMuniv.h"

using namespace BBB;
using namespace std;

static const int period[] = {1912045, 1805054, 1703577, 1607717, 1517450, 1432664, 1351351, 1282051, 1203369, 1136363, 1072961, 1012145, 955109, 5000000};
static const unsigned int duty_cycle[] = {318674, 300842, 283929, 267952, 252908, 238777, 225225, 213675, 200561, 189393, 178826, 168690, 159184, 2500000};
// E7, C7, G7, G6, E6, A6, B6, AS6, E5, G5, C6, D6
static const int period_mario[] = {379218, 477783, 318877, 637755, 758150, 568181, 506072, 536193, 1517450, 1275510, 955109, 851063};
static const unsigned int duty_cycle_mario[] = {63203, 79630, 53146, 106292, 126358, 94696, 84345, 89365, 252908, 212585, 159184, 141843};
// E5, G5, E6, C6, D6, G6
// Notas inicio
static int nE7 = 0;
static int nC7 = 1;
static int nG7 = 2;
static int nG6 = 3;
static int nE6 = 4;
static int nA6 = 5;
static int nB6 = 6;
static int nAS6 = 7;
static int nE5 = 8;
static int nG5 = 9;
static int nC6 = 10;
static int nD6 = 11;

// Notas normales
static int long_notas = 14;
static int nDO = 0;
static int nDOM = 1;
static int nRE = 2;
static int nREM = 3;
static int nMI = 4;
static int nFA = 5;
static int nFAM = 6;
static int nSOL = 7;
static int nSOLM = 8;
static int nLA = 9;
static int nLAM = 10;
static int nSI = 11;
static int nDOMM = 12;
static int nDO2 = 13;

int tono_mario(int nota, int tiempo, BBB::PWM *pwmbuzzer){
	pwmbuzzer->setDutyCycle(duty_cycle_mario[nota]);
	pwmbuzzer->setPeriod(period_mario[nota]);
	pwmbuzzer->run();
	usleep(tiempo);
	return 0;
}

int tono(int nota, int tiempo, BBB::PWM *pwmbuzzer){
        //pwmbuzzer->setPeriod(period[nota]);
        pwmbuzzer->setDutyCycle(duty_cycle[nota]);
        pwmbuzzer->setPeriod(period[nota]);
        pwmbuzzer->run();
        usleep(tiempo);
        return 0;
}

int silencio(int ns, BBB::PWM *pwmbuzzer){
        pwmbuzzer->stop();
        usleep(ns);
        pwmbuzzer->run();
        return 0;
}

int empezarDescuento(BBB::PWM *pwmbuzzer){
        pwmbuzzer->setPeriod(500000000);
	unsigned int dc = 3000000;
	pwmbuzzer->setDutyCycle(dc);
        pwmbuzzer->run();
        return 0;
}

int detenerDescuento(BBB::PWM *pwmbuzzer){
	pwmbuzzer->stop();
	return 0;
}

int suenaRojo(BBB::PWM *pwmbuzzer){
	silencio(100000, pwmbuzzer);
	tono(nDO, 200000, pwmbuzzer);
	detenerDescuento(pwmbuzzer);
	return 0;
}

int suenaVerde(BBB::PWM *pwmbuzzer){
        silencio(100000, pwmbuzzer);
        tono(nRE, 200000, pwmbuzzer);
        detenerDescuento(pwmbuzzer);
        return 0;
}

int suenaAzul(BBB::PWM *pwmbuzzer){
        silencio(100000, pwmbuzzer);
        tono(nMI, 200000, pwmbuzzer);
        detenerDescuento(pwmbuzzer);
        return 0;
}

int suenaAmar(BBB::PWM *pwmbuzzer){
        silencio(100000, pwmbuzzer);
        tono(nFA, 200000, pwmbuzzer);
        detenerDescuento(pwmbuzzer);
        return 0;
}

int fallo(BBB::PWM *pwmbuzzer){
	silencio(200000, pwmbuzzer);
	//tono(nDO2, 500000, pwmbuzzer);
	//silencio(400000, pwmbuzzer);
	tono(nDO2, 1000000, pwmbuzzer);
	detenerDescuento(pwmbuzzer);
	return 0;
}

int inicio(BBB::PWM *pwmbuzzer){
	tono_mario(nE7, 120000, pwmbuzzer);
	tono_mario(nE7, 120000, pwmbuzzer);
	silencio(120000, pwmbuzzer);
	tono_mario(nE7, 120000, pwmbuzzer);
	silencio(120000, pwmbuzzer);
	tono_mario(nC7, 120000, pwmbuzzer);
	tono_mario(nE7, 120000, pwmbuzzer);
	silencio(120000, pwmbuzzer);
	tono_mario(nG7, 120000, pwmbuzzer);
	silencio(120000, pwmbuzzer);
	silencio(120000, pwmbuzzer);
	silencio(120000, pwmbuzzer);
	tono_mario(nG6, 120000, pwmbuzzer);
	silencio(120000, pwmbuzzer);
        silencio(120000, pwmbuzzer);
        silencio(120000, pwmbuzzer);
	tono_mario(nC7, 120000, pwmbuzzer);
	silencio(120000, pwmbuzzer);
        silencio(120000, pwmbuzzer);
	tono_mario(nG6, 120000, pwmbuzzer);
	silencio(120000, pwmbuzzer);
        silencio(120000, pwmbuzzer);
	tono_mario(nE6, 120000, pwmbuzzer);
	silencio(120000, pwmbuzzer);
        silencio(120000, pwmbuzzer);
	tono_mario(nA6, 120000, pwmbuzzer);
	silencio(120000, pwmbuzzer);
	tono_mario(nB6, 120000, pwmbuzzer);
	silencio(120000, pwmbuzzer);
	tono_mario(nAS6, 120000, pwmbuzzer);
	tono_mario(nA6, 120000, pwmbuzzer);
        silencio(120000, pwmbuzzer);
	tono_mario(nG6, 120000, pwmbuzzer);
	return 0;
}

int acierto(BBB::PWM *pwmbuzzer){
	tono_mario(nE5, 100000, pwmbuzzer);
	tono_mario(nG5, 100000, pwmbuzzer);
	tono_mario(nE6, 100000, pwmbuzzer);
	tono_mario(nC6, 100000, pwmbuzzer);
	tono_mario(nD6, 100000, pwmbuzzer);
	tono_mario(nG6, 100000, pwmbuzzer);
	return 0;
}


/*int dummy(BBB::PWM *pwmbuzzer){
        tono(nSI, 100000);
        silencio(100000);
        tono(nSOL, 100000);
        silencio(100000);
        //tono(nMI, 100000);
        //silencio(100000);
        //tono(nDO, 100000);
        //silencio(100000);
        //tono(nMI, 100000);
        //silencio(100000);
        tono(nSOL, 100000);
        silencio(100000);
        tono(nSI, 100000);
        silencio(100000);
        pwmbuzzer->stop();
        return 0;
}*/
