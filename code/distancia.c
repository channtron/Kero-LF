/*
 * distancia.c
 *
 *  Created on: Sep 2, 2020
 *      Author: rafap
 */
#include "distancia.h"

void dist_config (void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    GPIOPinTypeADC(GPIO_PORTK_BASE, GPIO_PIN_0 );
    ADCSequenceConfigure(ADC1_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC1_BASE, 1, 0, ADC_CTL_CH16|ADC_CTL_IE|ADC_CTL_END);

    ADCSequenceEnable(ADC1_BASE, 1);
}

void mide_dist (uint32_t *dist){
    uint32_t ADC;
    ADCIntClear(ADC1_BASE, 1);
    ADCProcessorTrigger(ADC1_BASE, 1);
    while(!ADCIntStatus(ADC1_BASE, 1, false));
    ADCSequenceDataGet(ADC1_BASE, 1, &ADC);
    *dist=35000/(ADC-139);//ADC/2.3 // 81147/(ADC-139) //15929/(ADC-182)// 16229/(ADC-139)
}

