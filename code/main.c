

/**
 * main.c
 */
#include <stdint.h>
#include <stdbool.h>

#include "driverlib2.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "config_pines.h"
#include "linea.h"
#include "motor.h"
#include "distancia.h"

#define MSEC 40000

#define line
#define sigue_lin
#define vel_med
#define distancia


uint32_t RELOJ, time;
double v_ml = 0, v_mr = 0;
uint32_t pulse_ml = 0, pulse_mr = 0;
extern uint32_t p_l, p_r;
int f_mili = 0, f_dist = 0;
int dist;

uint32_t linea[6], line_min[6], line_max[6];

int e_l, e_r, e1_l, e1_r;
double ei_l, ei_r;

void m_control(int ref_ml, int ref_mr, int *pot_l, int *pot_r){

    *pot_l = ref_ml*300;//
    *pot_r = ref_mr*300;//

}

void mili (void){
    f_mili=1;
    TimerIntClear(TIMER0_BASE, TIMER_A);
}

void tick (void){
    time++;
}

int main(void)
{
    time = 0;
    e_l=0;
    e1_l=0;
    ei_l=0;
    e_r=0;
    e1_r=0;
    ei_r=0;

    RELOJ=SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

    motor_config();
#ifdef vel_med
    encoder_config();
#endif
    dist_config();
    linea_init();

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerClockSourceSet(TIMER0_BASE, TIMER_CLOCK_SYSTEM);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, RELOJ/1000-1); // timer de 1ms
    TimerIntRegister(TIMER0_BASE, TIMER_A, mili); //temporizador
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER0_BASE, TIMER_A);

    // Inicializar UART 6 bluetooth.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART6);
    GPIOPinConfigure(GPIO_PP0_U6RX);
    GPIOPinConfigure(GPIO_PP1_U6TX);
    GPIOPinTypeUART(GPIO_PORTP_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTStdioConfig(6, 115200, RELOJ);

    time=0;
    SysTickPeriodSet(12000);
    SysTickIntRegister(tick);
    SysTickIntEnable();
    SysTickEnable();

    IntMasterEnable();
    calibra_lin(&line_min, &line_max);
    int count=0,  i;
    p_l = 0;
    p_r = 0;
    while(1){
        while(!f_mili);
        f_mili=0;
#ifdef distancia
        if ((count+5)%20==0){
            mide_dist(&dist);
            if (dist<18) {
                f_dist=1;
            }
            else f_dist=0;
        }
#endif
#ifdef line
        if ((count+1)%20==0) {
            lee_linea( &linea);
#ifdef ADC_Lin_Base
            for (i=0; i<6; i++) {
                int wn=(linea[i]-line_min[i])*100/(line_max[i]-line_min[i]);
#ifdef b_line
                if (wn<10 | wn >1000) linea[i]=0;
                else {
                linea[i]=1;
                }
#else
                if (wn<0 | wn >1000) linea[i]=0;
                else {
                linea[i]=wn;
                }
#endif
            }
#endif
            UARTprintf("%d, %4d, %4d, %4d, %4d, %4d, %4d\n", time, linea[0],linea[1],linea[2],linea[3],linea[4],linea[5]);
        }
#endif
#ifdef sigue_lin
        if (count%20==0) {
            if(!f_dist) {
                if (linea[2] | linea[3]) avanza();
                else if (linea[1]) giro_izq();
                else if (linea[4]) giro_der();
                else if (linea[0]) rot_izq();
                else if (linea[5]) rot_der();
                else avanza();
            }
            else para();
            UARTprintf("%d, %2d, %4d, %4d, ", time, dist, p_l, p_r);
            UARTprintf("%d, %d\n", (int)(v_ml*100), (int)(v_mr*100));
        }
#endif
        count++;
    }
	return 0;
}


