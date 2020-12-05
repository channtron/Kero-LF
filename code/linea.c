/*
 * linea.c
 *
 *  Created on: Sep 8, 2020
 *      Author: rafap
 */

#include "linea.h"

void linea_init(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH3);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH2);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_CH1);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 3, ADC_CTL_CH0);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 4, ADC_CTL_CH9);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 5, ADC_CTL_CH8|ADC_CTL_IE|ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 0);
}

void lee_linea(uint32_t line[6]){
    uint32_t ADC[6];
    int i;
        ADCIntClear(ADC0_BASE, 0);
        ADCProcessorTrigger(ADC0_BASE, 0);
        while(!ADCIntStatus(ADC0_BASE, 0, false));
        ADCSequenceDataGet(ADC0_BASE, 0, ADC);
    for (i=0; i<=5; i++){
        line[i]=ADC[i];
    }
}

void calibra_lin(uint32_t l_min[6],uint32_t l_max[6]){
    /* inicializacion del boton para el incio */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0);   //J0 y J1: entradas
    GPIOPadConfigSet(GPIO_PORTJ_BASE,GPIO_PIN_0,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU); //Pullup en J0 y J1

    int i, j;
    uint32_t lin[6];
    // colocar sobre blanco y pulsar
    while((GPIOPinRead(GPIO_PORTJ_BASE,GPIO_PIN_0)));  //Si se pulsa B1
    SysCtlDelay(400000);
    while(!(GPIOPinRead(GPIO_PORTJ_BASE,GPIO_PIN_0)));
    for (i=0; i<6; i++) l_min[i]=0;
    for (i=0; i<20; i++){
        lee_linea(&lin);
        for (j=0; j<6; j++) l_min[j] += lin[j];
    }
    // colocar sobre negro y pulsar
    while((GPIOPinRead(GPIO_PORTJ_BASE,GPIO_PIN_0)));  //Si se pulsa B1
    SysCtlDelay(400000);
    while(!(GPIOPinRead(GPIO_PORTJ_BASE,GPIO_PIN_0)));
    for (i=0; i<6; i++) l_max[i]=0;
    for (i=0; i<20; i++){
        lee_linea(&lin);
        for (j=0; j<6; j++) l_max[j] += lin[j];
    }
    for (i=0; i<6; i++) {
        l_min[i]=l_min[i]/20;
        l_max[i]=l_max[i]/20;
    }
    while((GPIOPinRead(GPIO_PORTJ_BASE,GPIO_PIN_0)));
}
