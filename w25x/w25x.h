/**
 * @file    w25x.h
 * @author  XUMUMI
 * @version V1.0.0
 * @date    17-October-2018
 * @brief   This file contains the functions prototypes for the flash library.
**/

#ifndef __W25X_H
#define __W25X_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef uint8_t* addrtype;

/* Control function */
void spiInWait(void);
void spiInSw(bool sw);
void spiStErase(uint8_t *addr);
void spiInPage(addrtype addr, char *cont, size_t len);
void spiReadData(char *cont, addrtype addr, size_t len);


#endif
