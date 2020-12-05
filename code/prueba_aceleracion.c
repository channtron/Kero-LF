

/**
 * main.c
 */
#include <stdint.h>
#include <stdbool.h>

#include "driverlib2.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "config_pines.h"
//#include "motor.h"

#define MSEC 40000

uint32_t RELOJ, time;
double v_ml = 0, v_mr = 0;
uint32_t pulse_ml = 0, pulse_mr = 0;
uint32_t cycle, potencia = 1;
int f_mili = 0;

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
//    e_l=0;
//    e1_l=0;
//    ei_l=0;
//    e_r=0;
//    e1_r=0;
//    ei_r=0;

    RELOJ=SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

    motor_config();
    encoder_config();

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerClockSourceSet(TIMER0_BASE, TIMER_CLOCK_SYSTEM);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, 120000); // timer de 1ms
    TimerIntRegister(TIMER0_BASE, TIMER_A, mili); //f_control
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
    int count=0;
    potencia = 0;
    while(1){
        while(!f_mili);
        f_mili=0;
        if (count>2400) {
            potencia=0;
            cycle=2;
            PWMPulseWidthSet(MOTOR_PWM_BASE, LM_PWM_O, cycle-1);
            PWMPulseWidthSet(MOTOR_PWM_BASE, RM_PWM_O, cycle-1);
        }
        else if (count==400) {
            potencia=70; //Incrementa el periodo, saturando
            cycle=PWM_PERIOD*potencia/100;
            PWMPulseWidthSet(MOTOR_PWM_BASE, LM_PWM_O, cycle-1);
            PWMPulseWidthSet(MOTOR_PWM_BASE, RM_PWM_O, cycle-1);
        }
        if (count%10==0) UARTprintf("%d, %d, %d ,%d\n", time, potencia, (int)(v_ml*100), (int)(v_mr*100));
        count++;

    }
	return 0;
}


