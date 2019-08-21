#ifndef BUZZER_H_
#define BUZZER_H_
#include <string>
using std::string;

int tono(int nota, int tiempo, BBB::PWM *buzzer);
int tono_inicio(int nota, int tiempo, BBB::PWM *buzzer);
int silencio(int ns, BBB::PWM *buzzer);
int empezarDescuento(BBB::PWM *buzzer);
int detenerDescuento(BBB::PWM *buzzer);
int suenaRojo(BBB::PWM *buzzer);
int suenaVerde(BBB::PWM *buzzer);
int suenaAzul(BBB::PWM *buzzer);
int suenaAmar(BBB::PWM *buzzer);
int fallo(BBB::PWM *buzzer);
int inicio(BBB::PWM *buzzer);
int acierto(BBB::PWM *pwmbuzzer);
/*int dummy();*/


#endif /* BUZZER_H_ */
