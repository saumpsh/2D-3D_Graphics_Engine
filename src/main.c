/*
 * main.c
 *
 *  Created on: Nov 18, 2018
 *      Author: SaumPC
 */

#include "LPC17xx.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "DrawLine.h"

#include "main.h"


uint8_t src_addr[SSP_BUFSIZE];
uint8_t dest_addr[SSP_BUFSIZE];


/*************************************************
 Main Function  main()
 **********************************************/
int main(void)
{
	// Axes
	int16_t _height = ST7735_TFTHEIGHT;
	int16_t _width = ST7735_TFTWIDTH;

	uint32_t i, portnum;
	portnum = PORT_NUM;

	/****************** initialize SSP port and LCD *************/
	if (portnum == 0)
		SSP0Init();
	else if (portnum == 1)
		SSP1Init();

	for (i = 0; i < SSP_BUFSIZE; i++)
	{
		src_addr[i] = (uint8_t) i;
		dest_addr[i] = 0;
	}

	// To initialize the LCD
	lcd_init();
	//Refresh background with white color
	background(0, 0, _width, _height, WHITE);
	/****************** ******* **************************************/

	//Compute sin_theta, cos_theta, sin_phi, cos_phi, rho
	//For world- viewer transforamtion
	compute_transformation_parameter();

	// To draw the 3D world coordinate system
	drawCoordinate();

	/*********************To draw a 3D cubes **********************/
	//float cube1size = 40.00, x_base_1 = -20.00, y_base_1 = 40.00, z_base_1 = 00.00;
	float cube1size = 40.00, x_base_1 = 0.00, y_base_1 = 0.00, z_base_1 = 00.00;
	float cube2size = 25.00, x_base_2 = 50.00, y_base_2 = 00.00, z_base_2 = 0.00;
	float cube3size = 30.00, x_base_3 = 00.00, y_base_3 = 50.00, z_base_3 = 0.00;

	struct cube_t cube1, cube2, cube3, temp1, temp2;

	printf("Cube1\n");
	draw3dcube1(&cube1, cube1size, x_base_1, y_base_1, z_base_1);
	lcddelay(500);

	//printf("Cube2\n");
	draw3dcube1(&cube2, cube2size, x_base_2, y_base_2, z_base_2);
	lcddelay(500);

	printf("Cube3\n");
	draw3dcube1(&cube3, cube3size, x_base_3, y_base_3, z_base_3);
	lcddelay(500);

	rotateCube(&cube2, &temp1, 0.7, 45.00);
	rotateCube(&cube3, &temp2, 0.9, -45.00);
	printf("End0\n");
	//float __temp = temp.P1_3D.x;
	//printf("Value = %f\n",__temp);
	drawcube2(&temp1, cube2size, x_base_3, x_base_3, x_base_3);
	drawcube2(&temp2, cube3size, x_base_3, x_base_3, x_base_3);

	/****************** ******* **************************************/

	/*********************Light point source & Shadow **********************/
	//float xs = 0.00, ys = 70.00, zs = 150.00;
	float xs = 00.00, ys = 130.00, zs = 130.00;
	PointOf_Light_Source(&cube1, xs, ys, zs);
	PointOf_Light_Source(&temp1, xs, ys, zs);
	PointOf_Light_Source(&temp2, xs, ys, zs);

	//Draw shadow
	draw_shadow(&cube1, cube1size);
	draw_shadow(&cube2, cube2size);
	draw_shadow(&cube3, cube3size);

	/****************** ******* **************************************/

	printf("Program Ends..\n");

	return 0;
}

