/*
 * motor.c
 *
 *  Created on: Sep 1, 2020
 *      Author: rafap
 */

#include "motor.h"

extern uint32_t RELOJ;
extern double v_ml, v_mr;
extern int pulse_ml, pulse_mr;
uint32_t p_l, p_r;

void encoders (void)
{
    if(GPIOIntStatus(GPIO_PORTK_BASE,true)==GPIO_PIN_6) //Motor izquierdo
    {
        pulse_ml++;
        GPIOIntClear(GPIO_PORTK_BASE, GPIO_PIN_6);
    }
    else if(GPIOIntStatus(GPIO_PORTK_BASE,true)==GPIO_PIN_7) //Motor izquierdo
    {
        pulse_mr++;
        GPIOIntClear(GPIO_PORTK_BASE, GPIO_PIN_7);
    }
    else if(TimerIntStatus(TIMER1_BASE,TIMER_A)){
#ifdef TT_rueda
        v_ml=(double)pulse_ml;
        v_mr=(double)pulse_mr;
#endif
#ifdef TT_eje
        v_ml=(double)pulse_ml/12.8;
        v_mr=(double)pulse_mr/12.8;
#endif
#ifdef N20_100
        v_ml=(double)pulse_ml/15;
        v_mr=(double)pulse_mr/15;
#endif
#ifdef N20_65
        v_ml=pulse_ml;
        v_mr=pulse_mr;
#endif
        pulse_ml=0;
        pulse_mr=0;
        TimerIntClear(TIMER1_BASE, TIMER_A);
    }
}

void motor_config (void){
//    SysCtlPeripheralEnable(MOTOR_PERIPH);
//    SysCtlPeripheralEnable(MOTOR_PWM_PERIPH);
//    PWMClockSet(MOTOR_PWM_BASE,PWM_SYSCLK_DIV_64);    // al PWM le llega un reloj de 30MHz
//
//    GPIOPinConfigure(LM_GPIO);           //Conectar el pin a PWM
//    GPIOPinTypePWM(MOTOR_BASE, LM_PIN); //Tipo de pin PWM
//
//    GPIOPinConfigure(RM_GPIO);           //Conectar el pin a PWM
//    GPIOPinTypePWM(MOTOR_BASE, RM_PIN); //Tipo de pin PWM
//
//    PWMGenConfigure(MOTOR_PWM_BASE, MOTOR_PWM_GEN, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
//
//    PWMGenPeriodSet(MOTOR_PWM_BASE, MOTOR_PWM_GEN, PWM_PERIOD-1); //frec:50Hz
//    PWMPulseWidthSet(MOTOR_PWM_BASE, LM_PWM_O, 20000);   // velocidad inicial
//    PWMPulseWidthSet(MOTOR_PWM_BASE, RM_PWM_O, 20000);
//
//    PWMGenEnable(MOTOR_PWM_BASE, MOTOR_PWM_GEN);     //Habilita el generador
//    PWMOutputState(MOTOR_PWM_BASE, PWM_OUT_6_BIT , true);    //Habilita la salida 6
//    PWMOutputState(MOTOR_PWM_BASE, PWM_OUT_7_BIT , true);    //Habilita la salida 7
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    PWMClockSet(PWM0_BASE,PWM_SYSCLK_DIV_64);    // al PWM le llega un reloj de 30MHz

    GPIOPinConfigure(GPIO_PK4_M0PWM6);           //Conectar el pin a PWM
    GPIOPinTypePWM(GPIO_PORTK_BASE, GPIO_PIN_4); //Tipo de pin PWM

    GPIOPinConfigure(GPIO_PK5_M0PWM7);           //Conectar el pin a PWM
    GPIOPinTypePWM(GPIO_PORTK_BASE, GPIO_PIN_5); //Tipo de pin PWM

    PWMGenConfigure(PWM0_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_3, PWM_PERIOD); //frec:50Hz
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_6, 1);   // velocidad inicial
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7, 1);

    PWMGenEnable(PWM0_BASE, PWM_GEN_3);     //Habilita el generador
    PWMOutputState(PWM0_BASE, PWM_OUT_6_BIT , true);    //Habilita la salida 6
    PWMOutputState(PWM0_BASE, PWM_OUT_7_BIT , true);    //Habilita la salida 7

}

void encoder_config (void){
    // Timer encoder
    SysCtlPeripheralEnable(ENCODER_TIMER_PERIPH);
    TimerClockSourceSet(ENCODER_TIMER_BASE, TIMER_CLOCK_SYSTEM);
    TimerConfigure(ENCODER_TIMER_BASE, TIMER_CFG_PERIODIC);
#ifdef TT_rueda
    TimerLoadSet(ENCODER_TIMER, RELOJ/12-1);  //Timer para los encoder, se reinicia cada 1/4s
#endif
#ifdef TT_eje
    TimerLoadSet(ENCODER_TIMER, RELOJ/15-1);  //Timer para los encoder, se reinicia cada 1/4s
#endif
#ifdef N20_100
    TimerLoadSet(ENCODER_TIMER, RELOJ/70-1);  //Timer para los encoder, se reinicia cada 1/15s
#endif
#ifdef N20_65
    TimerLoadSet(ENCODER_TIMER, RELOJ/68-1);  //Timer para los encoder, se reinicia cada 1/15s
#endif
    TimerIntRegister(ENCODER_TIMER, encoders); //f_control
    IntEnable(ENCODER_TIMER_INT);
    TimerIntEnable(ENCODER_TIMER_BASE, TIMER_TIMA_TIMEOUT);
    TimerEnable(ENCODER_TIMER);

    GPIOPinTypeGPIOInput(ENCODER_BASE, LE_PIN | RE_PIN);   //J0 y J1: entradas
    GPIOPadConfigSet(ENCODER_BASE, LE_PIN | RE_PIN,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    GPIOIntEnable(ENCODER_BASE, LE_PIN | RE_PIN);  // Habilitar pines de interrupcion PK6 y PK7
    GPIOIntTypeSet(ENCODER_BASE, LE_PIN | RE_PIN, GPIO_RISING_EDGE); // interrupcion al subir el nivel de la liena
    GPIOIntRegister(ENCODER_BASE, encoders);  //Registrar (definir) la rutina de interrupcion
    IntEnable(ENCODER_INT);                                   //Habilitar interrupcion del pto K
}

void para (void){
    p_l=2;
    p_r=2;
    PWMPulseWidthSet(MOTOR_PWM_BASE, LM_PWM_O, p_l-1);
    PWMPulseWidthSet(MOTOR_PWM_BASE, RM_PWM_O, p_r-1);
}
void avanza (void){
#ifdef N20_65
    p_l=70;
    p_r=70;
#endif
#ifdef N20_100
    p_l=20;
    p_r=20;
#endif
#ifdef TT_rueda
    p_l=50;
    p_r=50;
#endif
#ifdef TT_eje
    p_l=60;
    p_r=60;
#endif
    p_l=PWM_PERIOD*p_l/100;
    p_r=PWM_PERIOD*p_r/100;
    PWMPulseWidthSet(MOTOR_PWM_BASE, LM_PWM_O, p_l-1);
    PWMPulseWidthSet(MOTOR_PWM_BASE, RM_PWM_O, p_r-1);
}
void giro_der (void){
#ifdef N20_65
    p_l=70;
    p_r=50;
#endif
#ifdef N20_100
    p_l=20;
    p_r=10;
#endif
#ifdef TT_rueda
    p_l=50;
    p_r=30;
#endif
#ifdef TT_eje
    p_l=50;
    p_r=30;
#endif
    p_l=PWM_PERIOD*p_l/100;
    p_r=PWM_PERIOD*p_r/100;
    PWMPulseWidthSet(MOTOR_PWM_BASE, LM_PWM_O, p_l-1);
    PWMPulseWidthSet(MOTOR_PWM_BASE, RM_PWM_O, p_r-1);
}
void giro_izq (void){
#ifdef N20_65
    p_l=50;
    p_r=70;
#endif
#ifdef N20_100
    p_l=10;
    p_r=20;
#endif
#ifdef TT_rueda
    p_l=30;
    p_r=50;
#endif
#ifdef TT_eje
    p_l=30;
    p_r=50;
#endif
    p_l=PWM_PERIOD*p_l/100;
    p_r=PWM_PERIOD*p_r/100;
    PWMPulseWidthSet(MOTOR_PWM_BASE, LM_PWM_O, p_l-1);
    PWMPulseWidthSet(MOTOR_PWM_BASE, RM_PWM_O, p_r-1);
}
void rot_der (void){
#ifdef N20_65
    p_l=50;
    p_r=1;
#endif
#ifdef N20_100
    p_l=15;
    p_r=1;
#endif
#ifdef TT_rueda
    p_l=30;
    p_r=1;
#endif
#ifdef TT_eje
    p_l=30;
    p_r=1;
#endif
    p_l=PWM_PERIOD*p_l/100;
    p_r=PWM_PERIOD*p_r/100;
    PWMPulseWidthSet(MOTOR_PWM_BASE, LM_PWM_O, p_l-1);
    PWMPulseWidthSet(MOTOR_PWM_BASE, RM_PWM_O, p_r-1);
}
void rot_izq (void){
#ifdef N20_65
    p_l=1;
    p_r=50;
#endif
#ifdef N20_100
    p_l=1;
    p_r=15;
#endif
#ifdef TT_rueda
    p_l=1;
    p_r=30;
#endif
#ifdef TT_eje
    p_l=1;
    p_r=30;
#endif
    p_l=PWM_PERIOD*p_l/100;
    p_r=PWM_PERIOD*p_r/100;
    PWMPulseWidthSet(MOTOR_PWM_BASE, LM_PWM_O, p_l-1);
    PWMPulseWidthSet(MOTOR_PWM_BASE, RM_PWM_O, p_r-1);
}
