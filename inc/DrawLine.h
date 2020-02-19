/*
 * DrawLine.h
 *
 *  Created on: Oct 12, 2018
 *      Author: SaumPC
 */

#ifndef DRAWLINE_H_
#define DRAWLINE_H_

#include "lcd_ssp_Operations.h"

void drawline(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color);
void drawPixel(int16_t x, int16_t y, uint32_t color);

#endif /* DRAWLINE_H_ */
