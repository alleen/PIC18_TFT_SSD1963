/*
 * File:   common.c
 * Author: Alleen Wang
 *
 * Created on 2014/12/22, PM 5:31
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <htc.h>

#include "common.h"

#define _XTAL_FREQ 64000000

void msleep(int time)
{
    while (--time)
        __delay_ms(1);
}

void usleep(int time)
{
    while (--time)
        __delay_us(1);
}
