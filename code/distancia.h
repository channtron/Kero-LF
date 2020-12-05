/*
 * distancia.h
 *
 *  Created on: Sep 2, 2020
 *      Author: rafap
 */

#ifndef DISTANCIA_H_
#define DISTANCIA_H_

#include <stdint.h>
#include <stdbool.h>
#include "driverlib2.h"
#include "config_pines.h"

void dist_config (void);
void mide_dist (uint32_t *ui32ACCValue);


#endif /* DISTANCIA_H_ */
