/*
 * linea.h
 *
 *  Created on: Sep 8, 2020
 *      Author: rafap
 */

#ifndef LINEA_H_
#define LINEA_H_

#include <stdint.h>
#include <stdbool.h>
#include "driverlib2.h"
#include "config_pines.h"

void linea_init(void);
void lee_linea(uint32_t line[6]);
void calibra_lin(uint32_t l_min[6],uint32_t l_max[6]);

#endif /* LINEA_H_ */
