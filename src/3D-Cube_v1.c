/*
 * main.c
 *
 *  Created on: Nov 3, 2018
 *      Author: SaumPC
 * Description : This program allows us, using the 1.8" Color TFT LCD via ssp ports,
 	 	 	 	 to be able to display 3D world coordinates and three 3D cubes on the screen
 */

#include "LPC17xx.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "DrawLine.h"
#include "3D-Cube_v1.h"

// To draw Vertical line
void drawFastVLine(int16_t x, int16_t y, int16_t h, uint32_t color)
{
	drawline(x, y, x, y + h, color);
}

// To fill the background with given color and dimensions
void background(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color)
{
	int16_t i;
	for (i = x; i < x + w; i++)
	{
		drawFastVLine(i, y, h, color);
	}
}

static float sin_theta, cos_theta, sin_phi, cos_phi, rho;
//float xcamera = 100.00, ycamera = 100.00, zcamera = 100.00;
const float xcamera = 300.00, ycamera = 300.00, zcamera = 300.00;

void compute_transformation_parameter()
{
	rho = sqrt((pow(xcamera, 2)) + (pow(ycamera, 2)) + (pow(zcamera, 2)));
	sin_theta = (ycamera / sqrt(pow(xcamera, 2) + pow(ycamera, 2)));
	cos_theta = (xcamera / sqrt(pow(xcamera, 2) + pow(ycamera, 2)));
	sin_phi = sqrt(pow(xcamera, 2) + pow(ycamera, 2)) / rho;
	cos_phi = (zcamera / rho);
}

void physical_coordinate(int16_t *x, int16_t *y)
{
	/*printf("Virtual Value of x: %d\t", *x);
	printf("Virtual Value of y: %d\n", *y);*/

	//Convert virtual imagined coordinate to physical coordinate
	*x =  (ST7735_TFTWIDTH/2) + *x;
	*y = (ST7735_TFTHEIGHT/2) - *y;

	//Allow coordinates to be in the range of LCD resolution
	*x = (*x < 0) ? 0 : ((*x > ST7735_TFTWIDTH) ? ST7735_TFTWIDTH : *x);
	*y = (*y < 0) ? 0 : ((*y > ST7735_TFTHEIGHT) ? ST7735_TFTHEIGHT : *y);

	/*printf("Physical Value of x: %d\t", *x);
	printf("Physical Value of y: %d\n", *y);*/
}

// To convert world to viewer coordinates - Viewer to 2D Coordinate
//virtual coordinate to physical/native coordinate
struct coordinates_2D Transformation_pipeline(float xw, float yw, float zw)
{
	int16_t xdoubleprime = 0, ydoubleprime = 0; /*D = 100.00*/
	float xPrime = 0.00, yPrime = 0.00, zPrime = 0.00;
	struct coordinates_2D projection = {0, 0};

	/************************** To convert world to viewer coordinates *************/

	xPrime =   ((-sin_theta) * xw) + (yw * cos_theta);
	yPrime = (-(xw) * cos_theta * cos_phi) + (-(yw) * cos_phi * sin_theta) + (zw * sin_phi);
	zPrime = (- (xw) * sin_phi * cos_theta) + (- (yw) * sin_phi * cos_theta)
			+ (- (zw) * cos_phi) + rho;
	//printf("(xPrime, yPrime, zPrime): (%f, %f, %f)\n", xPrime, yPrime, zPrime);

	/*******************************************************************************/

	//Convert Viewer to 2D Coordinate
	xdoubleprime = (xPrime * D) / zPrime;
	ydoubleprime = (yPrime * D) / zPrime;
	//printf("(xdoubleprime, ydoubleprime): (%f, %f)\n", xdoubleprime, ydoubleprime);

	//Convert virtual imagined coordinate to physical/native coordinate
	physical_coordinate(&xdoubleprime, &ydoubleprime);

	projection.x = xdoubleprime;
	projection.y = ydoubleprime;
	return projection;
}

// To draw the world 3D coordinate system - x,y,z
void drawCoordinate()
{
	struct coordinates_2D axis;
	int16_t x1, y1, x2, y2, x3, y3, x4, y4;

	//Origin
	axis = Transformation_pipeline(0.00, 0.00, 0.00);
	x1 = axis.x;
	y1 = axis.y;

	//X-Axis
	axis = Transformation_pipeline(180.00, 0.00, 0.00);
	x2 = axis.x;
	y2 = axis.y;

	//Y-Axis
	axis = Transformation_pipeline(0.00, 180.00, 0.00);
	x3 = axis.x;
	y3 = axis.y;

	//Z-Axis
	axis = Transformation_pipeline(0.00, 0.00, 180.00);
	x4 = axis.x;
	y4 = axis.y;

	drawline(x1, y1, x2, y2, RED);			   //x axis  Red
	drawline(x1, y1, x3, y3, GREEN);		  //y axis  Green
	drawline(x1, y1, x4, y4, BLUE);  		   //z axis  Blue
}


void fillcube(struct cube_t *cube)
{
	float i,j;
	struct coordinates_3D t1, t2;
	struct coordinates_2D m1, m2;
	/************************************SIDE SURFACE***************************************************/
	for(i = cube->P3_3D.x, j = cube->P6_3D.x; i > cube->P4_3D.x && j > cube->P7_3D.x; i--, j--)
	{
		t1.x = i;
		t1.y = (((cube->P4_3D.y - cube->P3_3D.y) / (cube->P4_3D.x - cube->P3_3D.x)) * (t1.x - cube->P3_3D.x)) + cube->P3_3D.y;
		t1.z = (((cube->P4_3D.z - cube->P3_3D.z) / (cube->P4_3D.x - cube->P3_3D.x)) * (t1.x - cube->P3_3D.x)) + cube->P3_3D.z;
		//printf("Value of P1: (%f, %f, %f)\n", t1.x, t1.y, t1.z);

		t2.x = j;
		t2.y = t1.y;
		t2.z = (((cube->P7_3D.z - cube->P6_3D.z) / (cube->P7_3D.x - cube->P6_3D.x)) * (t1.x - cube->P6_3D.x)) + cube->P6_3D.z;
		//printf("Value of P2: (%f, %f, %f)\n\n", t2.x, t2.y, t2.z);

		m1 = Transformation_pipeline(t1.x, t1.y, t1.z);
		m2 = Transformation_pipeline(t2.x, t2.y, t2.z);
		drawline(m1.x, m1.y, m2.x, m2.y, BLUE);
	}

	/************************************TOP SURFACE***************************************************/
	for (i = cube->P2_3D.x, j = cube->P3_3D.x; 	i > cube->P1_3D.x && j > cube->P4_3D.x; i--, j--)
	{
		t1.x = i;
		t1.y = (((cube->P1_3D.y - cube->P2_3D.y)/ (cube->P1_3D.x - cube->P2_3D.x)) * (t1.x - cube->P2_3D.x)) + cube->P2_3D.y;
		t1.z = cube->P2_3D.z;
		//printf("Value of P1: (%f, %f, %f)\n", t1.x, t1.y, t1.z);

		t2.x = j;
		t2.y = (((cube->P4_3D.y - cube->P3_3D.y)/ (cube->P4_3D.x - cube->P3_3D.x)) * (t1.x - cube->P3_3D.x)) + cube->P3_3D.y;
		t2.z = cube->P2_3D.z;
		//printf("Value of P2: (%f, %f, %f)\n\n", t2.x, t2.y, t2.z);

		m1 = Transformation_pipeline(t1.x, t1.y, t1.z);
		m2 = Transformation_pipeline(t2.x, t2.y, t2.z);
		drawline(m1.x, m1.y, m2.x, m2.y, RED);
	}
	/************************************FRONT SURFACE***************************************************/
		for(i = cube->P2_3D.y, j = cube->P5_3D.y; i < cube->P3_3D.y && j < cube->P6_3D.y; i++, j++)
		{
			t1.y = i;
			//For x-y slope
			float slope1 = ((cube->P3_3D.y - cube->P2_3D.y) / (cube->P3_3D.x - cube->P2_3D.x));
			t1.x = ((t1.y - cube->P2_3D.y) / slope1) + cube->P2_3D.x;
			//t1.x = cube->P2_3D.x;
			t1.z = (((cube->P3_3D.z - cube->P2_3D.z) / (cube->P3_3D.y - cube->P2_3D.y)) * (t1.y - cube->P2_3D.y)) + cube->P2_3D.z;
			//printf("Value of P1: (%f, %f, %f)\n", t1.x, t1.y, t1.z);

			t2.y = j;
			t2.x = t1.x;
			t2.z = (((cube->P6_3D.z - cube->P5_3D.z) / (cube->P6_3D.y - cube->P5_3D.y)) * (t2.y - cube->P5_3D.y)) + cube->P5_3D.z;
			//printf("Value of P2: (%f, %f, %f)\n\n", t2.x, t2.y, t2.z);

			m1 = Transformation_pipeline(t1.x, t1.y, t1.z);
			m2 = Transformation_pipeline(t2.x, t2.y, t2.z);
			drawline(m1.x, m1.y, m2.x, m2.y, MAGENTA);
		}
}


// To draw a 3D cube
void draw3dcube1(struct cube_t *cube, float cube_size, float x_base, float y_base, float z_base)
{
	//Compute eight vertices for the cube
								//(0, 0, 60)
	cube->P1 = Transformation_pipeline(cube->P1_3D.x = x_base, cube->P1_3D.y = y_base, cube->P1_3D.z = (cube_size + z_base));
	//printf("Native values of P1: (%d, %d)\n", cube->P1.x, cube->P1.y);

								//(60, 0, 60)
	cube->P2 = Transformation_pipeline(cube->P2_3D.x = (x_base + cube_size), cube->P2_3D.y = y_base, cube->P2_3D.z = (z_base + cube_size));
	//printf("Native values of P2: (%d, %d)\n", cube->P2.x, cube->P2.y);

								//(60, 60, 60)
	cube->P3 = Transformation_pipeline(cube->P3_3D.x = (x_base + cube_size), cube->P3_3D.y = (y_base + cube_size), cube->P3_3D.z = (z_base + cube_size));
	//printf("Native values of P3: (%d, %d)\n", cube->P3.x, cube->P3.y);

								//(0, 60, 60)
	cube->P4 = Transformation_pipeline(cube->P4_3D.x = x_base, cube->P4_3D.y = (y_base + cube_size), cube->P4_3D.z = (z_base + cube_size));
	//printf("Native values of P4: (%d, %d)\n", cube->P4.x, cube->P4.y);

								//(60, 0, 0)
	cube->P5 = Transformation_pipeline(cube->P5_3D.x = (x_base + cube_size), cube->P5_3D.y = y_base , cube->P5_3D.z = z_base);
	//printf("Native values of P5: (%d, %d)\n", cube->P5.x, cube->P5.y);

								//(60, 60, 0)
	cube->P6 = Transformation_pipeline(cube->P6_3D.x = (x_base + cube_size), cube->P6_3D.y = (y_base + cube_size), cube->P6_3D.z = z_base );
	//printf("Native values of P6: (%d, %d)\n", cube->P6.x, cube->P6.y);

								//(0, 60, 0)
	cube->P7 = Transformation_pipeline(cube->P7_3D.x = x_base, cube->P7_3D.y = (y_base + cube_size), cube->P7_3D.z = z_base);
	//printf("Native values of P7: (%d, %d)\n", cube->P7.x, cube->P7.y);

	//(0, 0, 0)
	cube->P8 = Transformation_pipeline(cube->P8_3D.x = x_base, cube->P8_3D.y = y_base, cube->P8_3D.z = z_base);
	//printf("Native values of P8: (%d, %d)\n", cube->P8.x, cube->P8.y);

	//Join edges to form a cube
	drawline(cube->P1.x, cube->P1.y, cube->P2.x, cube->P2.y, BLACK);
	drawline(cube->P2.x, cube->P2.y, cube->P3.x, cube->P3.y, BLACK);
	drawline(cube->P3.x, cube->P3.y, cube->P4.x, cube->P4.y, BLACK);
	drawline(cube->P4.x, cube->P4.y, cube->P1.x, cube->P1.y, BLACK);
	drawline(cube->P2.x, cube->P2.y, cube->P5.x, cube->P5.y, BLACK);
	drawline(cube->P5.x, cube->P5.y, cube->P6.x, cube->P6.y, BLACK);
	drawline(cube->P6.x, cube->P6.y, cube->P3.x, cube->P3.y, BLACK);
	drawline(cube->P6.x, cube->P6.y, cube->P7.x, cube->P7.y, BLACK);
	drawline(cube->P7.x, cube->P7.y, cube->P4.x, cube->P4.y, BLACK);
	drawline(cube->P7.x, cube->P7.y, cube->P8.x, cube->P8.y, BLACK);
	drawline(cube->P1.x, cube->P1.y, cube->P8.x, cube->P8.y, BLACK);
	drawline(cube->P5.x, cube->P5.y, cube->P8.x, cube->P8.y, BLACK);

}

void drawcube2(struct cube_t *cube, float cube_size, float x_base, float y_base, float z_base)
{
	//Compute eight vertices for the cube
								//(0, 0, 60)
	cube->P1 = Transformation_pipeline(cube->P1_3D.x , cube->P1_3D.y , cube->P1_3D.z + (cube_size ));
	//printf("Native values of P1: (%d, %d)\n", cube->P1.x, cube->P1.y);

								//(60, 0, 60)
	cube->P2 = Transformation_pipeline(cube->P2_3D.x +( cube_size), cube->P2_3D.y, cube->P2_3D.z + (cube_size));
	//printf("Native values of P2: (%d, %d)\n", cube->P2.x, cube->P2.y);

								//(60, 60, 60)
	cube->P3 = Transformation_pipeline(cube->P3_3D.x + (cube_size), cube->P3_3D.y + (cube_size), cube->P3_3D.z + (cube_size));
	//printf("Native values of P3: (%d, %d)\n", cube->P3.x, cube->P3.y);

								//(0, 60, 60)
	cube->P4 = Transformation_pipeline(cube->P4_3D.x, cube->P4_3D.y + (cube_size), cube->P4_3D.z + (cube_size));
	//printf("Native values of P4: (%d, %d)\n", cube->P4.x, cube->P4.y);

								//(60, 0, 0)
	cube->P5 = Transformation_pipeline(cube->P5_3D.x + (cube_size), cube->P5_3D.y  , cube->P5_3D.z );
	//printf("Native values of P5: (%d, %d)\n", cube->P5.x, cube->P5.y);

								//(60, 60, 0)
	cube->P6 = Transformation_pipeline(cube->P6_3D.x + (cube_size), cube->P6_3D.y + ( cube_size), cube->P6_3D.z );
	//printf("Native values of P6: (%d, %d)\n", cube->P6.x, cube->P6.y);

								//(0, 60, 0)
	cube->P7 = Transformation_pipeline(cube->P7_3D.x, cube->P7_3D.y + ( cube_size), cube->P7_3D.z);
	//printf("Native values of P7: (%d, %d)\n", cube->P7.x, cube->P7.y);

	//(0, 0, 0)
	cube->P8 = Transformation_pipeline(cube->P8_3D.x , cube->P8_3D.y , cube->P8_3D.z );
	//printf("Native values of P8: (%d, %d)\n", cube->P8.x, cube->P8.y);

	//Join edges to form a cube
	drawline(cube->P1.x, cube->P1.y, cube->P2.x, cube->P2.y, BLACK);
	drawline(cube->P2.x, cube->P2.y, cube->P3.x, cube->P3.y, BLACK);
	drawline(cube->P3.x, cube->P3.y, cube->P4.x, cube->P4.y, BLACK);
	drawline(cube->P4.x, cube->P4.y, cube->P1.x, cube->P1.y, BLACK);
	drawline(cube->P2.x, cube->P2.y, cube->P5.x, cube->P5.y, BLACK);
	drawline(cube->P5.x, cube->P5.y, cube->P6.x, cube->P6.y, BLACK);
	drawline(cube->P6.x, cube->P6.y, cube->P3.x, cube->P3.y, BLACK);
	drawline(cube->P6.x, cube->P6.y, cube->P7.x, cube->P7.y, BLACK);
	drawline(cube->P7.x, cube->P7.y, cube->P4.x, cube->P4.y, BLACK);
	drawline(cube->P7.x, cube->P7.y, cube->P8.x, cube->P8.y, BLACK);
	drawline(cube->P1.x, cube->P1.y, cube->P8.x, cube->P8.y, BLACK);
	drawline(cube->P5.x, cube->P5.y, cube->P8.x, cube->P8.y, BLACK);

}

void PointOf_Light_Source(struct cube_t *cube, float xs, float xy, float xz)
{
	struct coordinates_2D pt_src; float lambda = 2.2;

	//find lamda
	//n = (0, 0, 1), a = (0, 0, 0) ---> lamda = n* (a-Ps)/ n * (Ps - Pi)
	float numer = (0 + 0 + 1*(0 - xz)), denom = (0 + 0 + 1*(xz - cube->P2_3D.z));
	lambda = numer / denom;
	printf("Value of Lamda is: %f\n", lambda);

	pt_src = Transformation_pipeline(xs, xy, xz);

	//Line equation for vertices P2, Point source and shadow point
	cube->Sh1_3D.x = xs + (lambda * (xs - cube->P2_3D.x));
	cube->Sh1_3D.y = xy + (lambda * (xy - cube->P2_3D.y));
	cube->Sh1_3D.z = xz + (lambda * (xz - cube->P2_3D.z));
	cube->Sh1 = Transformation_pipeline(cube->Sh1_3D.x, cube->Sh1_3D.y, cube->Sh1_3D.z);
	//drawline( pt_src.x, pt_src.y, cube->Sh1.x, cube->Sh1.y, BLACK);
	//printf("Value of Sh1_3D: (%d, %d, %d)\n", cube->Sh1_3D.x, cube->Sh1_3D.y, cube->Sh1_3D.z);

	//Line equation for vertices P3, Point source and shadow point
	cube->Sh2_3D.x = xs + (lambda * (xs - cube->P3_3D.x )) ;
	cube->Sh2_3D.y = xy + (lambda * (xy - cube->P3_3D.y)) ;
	cube->Sh2_3D.z = xz + (lambda * (xz - cube->P3_3D.z)) ;
	cube->Sh2 = Transformation_pipeline(cube->Sh2_3D.x, cube->Sh2_3D.y, cube->Sh2_3D.z);
	//drawline(pt_src.x, pt_src.y, cube->Sh2.x, cube->Sh2.y, BLACK);
	//printf("Value of Sh2_3D: (%d, %d, %d)\n", cube->Sh2_3D.x, cube->Sh2_3D.y, cube->Sh2_3D.z);

	//Line equation for vertices P4, Point source and shadow point
	cube->Sh3_3D.x = xs + (lambda * (xs - cube->P4_3D.x )) ;
	cube->Sh3_3D.y = xy + (lambda * (xy - cube->P4_3D.y)) ;
	cube->Sh3_3D.z = xz + (lambda * (xz - cube->P4_3D.z)) ;
	cube->Sh3 = Transformation_pipeline(cube->Sh3_3D.x, cube->Sh3_3D.y, cube->Sh3_3D.z);
	//drawline(pt_src.x, pt_src.y, cube->Sh3.x, cube->Sh3.y, BLACK);
	//printf("Value of Sh3_3D: (%d, %d, %d)\n", cube->Sh3_3D.x, cube->Sh3_3D.y, cube->Sh3_3D.z);

	//Line equation for vertices P1, Point source and shadow point
	cube->Sh4_3D.x = xs + (lambda * (xs - cube->P1_3D.x));
	cube->Sh4_3D.y = xy + (lambda * (xy - cube->P1_3D.y));
	cube->Sh4_3D.z = xz + (lambda * (xz - cube->P1_3D.z));
	cube->Sh4 = Transformation_pipeline(cube->Sh4_3D.x, cube->Sh4_3D.y, cube->Sh4_3D.z);
	//drawline(pt_src.x, pt_src.y, cube->Sh4.x, cube->Sh4.y, BLACK);
	//printf("Value of Sh4_3D: (%d, %d, %d)\n", cube->Sh4_3D.x, cube->Sh4_3D.y, cube->Sh4_3D.z);
}

void draw_shadow(struct cube_t *cube, int cube_size)
{
	int x, y, x2, y2;
	//int temp_x, temp_y, temp_x2, temp_y2;
	struct coordinates_2D temp1, temp2;
	float i,j;
	struct coordinates_3D t1, t2;
	struct coordinates_2D m1, m2;

	//Join shadow coordinates
	drawline(cube->Sh1.x, cube->Sh1.y, cube->Sh2.x, cube->Sh2.y, BLACK);
	drawline(cube->Sh2.x, cube->Sh2.y, cube->Sh3.x, cube->Sh3.y, BLACK);
	drawline(cube->Sh3.x, cube->Sh3.y, cube->Sh4.x, cube->Sh4.y, BLACK);
	drawline(cube->Sh4.x, cube->Sh4.y, cube->Sh1.x, cube->Sh1.y, BLACK);

	//Color the shadow
	for (i = cube->Sh1_3D.x, j = cube->Sh2_3D.x; i > cube->Sh4_3D.x && j > cube->Sh3_3D.x; i--, j--)
	{
		t1.x = i;
		t1.y = (((cube->Sh4_3D.y - cube->Sh1_3D.y)/ (cube->Sh4_3D.x - cube->Sh1_3D.x)) * (t1.x - cube->Sh1_3D.x)) + cube->Sh1_3D.y;
		t1.z = cube->Sh1_3D.z;
		//printf("Value of P1: (%f, %f, %f)\n", t1.x, t1.y, t1.z);

		t2.x = j;
		t2.y = (((cube->Sh3_3D.y - cube->Sh2_3D.y)/ (cube->Sh3_3D.x - cube->Sh2_3D.x)) * (t1.x - cube->Sh2_3D.x)) + cube->Sh2_3D.y;
		t2.z = t1.z;
		//printf("Value of P2: (%f, %f, %f)\n\n", t2.x, t2.y, t2.z);

		m1 = Transformation_pipeline(t1.x, t1.y, t1.z);
		m2 = Transformation_pipeline(t2.x, t2.y, t2.z);
		drawline(m1.x, m1.y, m2.x, m2.y, BLACK);
	}

	// decorate cube
	//decorate3dcube1(point, cube_size);
	fillcube(cube);

	/********************Draw Square and Tree Pattern*******************************/
	squarePattern(cube->P2.x+2, cube->P2.y+5, cube->P3.x-5, cube->P3.y-5, cube->P6.x-2, cube->P6.y-2, cube->P5.x+2, cube->P5.y+5);
	/****************************Draw Square and Tree Pattern*********************/
	treePattern(cube->P3.x+10,cube->P3.y+10,5.23,4,4,GREEN); // draws with right branch (angle = 5.23 rad/300 deg)
	treePattern(cube->P3.x+10,cube->P3.y+10,4.18,4,4,GREEN); // draws with left branch (angle = 4.18 rad/240 deg)
	treePattern(cube->P3.x+10,cube->P3.y+10,4.71,4,4,GREEN); // draws with center branch (angle = 4.71 rad/0 deg)

	/***************************Draw Initial*****************************************/
	draw_Initial(cube, cube_size);
}

/*void draw_arrow(int cube_size)
{
	struct arrow_coordinates Arr_Cord;
	int x, y, x2, y2;
	struct coordinates_2D temp1, temp2;

	Arr_Cord.A1_2D = Transformation_pipeline(Arr_Cord.A1.x = cube_size/2 - 10,
			Arr_Cord.A1.y =  0, Arr_Cord.A1.z = cube_size + 10);

	Arr_Cord.A2_2D = Transformation_pipeline(Arr_Cord.A2.x = cube_size/2 - 10,
			Arr_Cord.A2.y = cube_size/2, Arr_Cord.A2.z = cube_size + 10);

	Arr_Cord.A3_2D = Transformation_pipeline(Arr_Cord.A3.x = 5,
			Arr_Cord.A3.y = cube_size/2, Arr_Cord.A3.z = cube_size + 10);

	Arr_Cord.A4_2D = Transformation_pipeline(Arr_Cord.A4.x = cube_size/2,
			Arr_Cord.A4.y = cube_size - 5, Arr_Cord.A4.z = cube_size + 10);

	Arr_Cord.A5_2D = Transformation_pipeline(Arr_Cord.A5.x = cube_size - 5,
			Arr_Cord.A5.y = cube_size/2, Arr_Cord.A5.z = cube_size + 10);

	Arr_Cord.A6_2D = Transformation_pipeline(Arr_Cord.A6.x = cube_size/2 + 10,
			Arr_Cord.A6.y = cube_size/2, Arr_Cord.A6.z = cube_size + 10);

	Arr_Cord.A7_2D = Transformation_pipeline(Arr_Cord.A7.x = cube_size/2 + 10,
			Arr_Cord.A7.y =  0, Arr_Cord.A7.z = cube_size + 10);

	drawline(Arr_Cord.A1_2D.x, Arr_Cord.A1_2D.y, Arr_Cord.A2_2D.x, Arr_Cord.A2_2D.y, BLACK);
	drawline(Arr_Cord.A2_2D.x, Arr_Cord.A2_2D.y, Arr_Cord.A3_2D.x, Arr_Cord.A3_2D.y, BLACK);
	drawline(Arr_Cord.A3_2D.x, Arr_Cord.A3_2D.y, Arr_Cord.A4_2D.x, Arr_Cord.A4_2D.y, BLACK);
	drawline(Arr_Cord.A4_2D.x, Arr_Cord.A4_2D.y, Arr_Cord.A5_2D.x, Arr_Cord.A5_2D.y, BLACK);
	drawline(Arr_Cord.A5_2D.x, Arr_Cord.A5_2D.y, Arr_Cord.A6_2D.x, Arr_Cord.A6_2D.y, BLACK);
	drawline(Arr_Cord.A6_2D.x, Arr_Cord.A6_2D.y, Arr_Cord.A7_2D.x, Arr_Cord.A7_2D.y, BLACK);
	drawline(Arr_Cord.A1_2D.x, Arr_Cord.A1_2D.y, Arr_Cord.A7_2D.x, Arr_Cord.A7_2D.y, BLACK);

	//decorate arrow
	//Fill arrow tail
	for(y=1; y < Arr_Cord.A2.y ; y++)
	{
		temp1 = Transformation_pipeline(Arr_Cord.A1.x = cube_size/2 - 10 + 1,
				Arr_Cord.A1.y =  y, Arr_Cord.A1.z);
		temp2 = Transformation_pipeline(Arr_Cord.A7.x = cube_size/2 + 10 - 1,
				Arr_Cord.A7.y =  y, Arr_Cord.A7.z);
		drawline(temp1.x, temp1.y, temp2.x, temp2.y, YELLOW);
	}
	//Fill arrow head
	for(x = (Arr_Cord.A3.x)-1, y = (Arr_Cord.A3.y)+1, x2 = (Arr_Cord.A5.x)-1, y2 = (Arr_Cord.A5.y)+1;
			((x< Arr_Cord.A4.x) && (y< Arr_Cord.A4.y) && (x2> Arr_Cord.A4.x) && (y2< Arr_Cord.A4.y));
			x++, y++, x2--, y2++)
	{
		temp1 = Transformation_pipeline(x, y, Arr_Cord.A1.z);
		temp2 = Transformation_pipeline(x2, y2, Arr_Cord.A7.z);
		drawline(temp1.x, temp1.y, temp2.x, temp2.y, YELLOW);
	}
}*/

void rotateCube(struct cube_t *cube1, struct cube_t *cube2, float alpha, float theta_deg)
{
	float theta = (theta_deg * PI)/180.00;
	//float alpha = (alpha_deg * PI)/180.00;
	printf("theta %f\n", theta);
	printf("alpha %f\n", alpha);
	float add = 5;
	float cos_theta = cos(theta);
	float sin_theta = sin(theta);
	printf("cos %f\n", cos_theta);
		printf("sin %f\n", sin_theta);

	//Compute new rotated coordiantes for P1
		cube2->P1_3D.x = alpha * ((cube1->P1_3D.x * cos_theta) - (cube1->P1_3D.y * sin_theta) - (200* (cos_theta - sin_theta - 1))) + add;
		cube2->P1_3D.y = alpha * ((cube1->P1_3D.x * sin_theta) + (cube1->P1_3D.y * cos_theta) - (200 * (sin_theta + cos_theta - 1))) + add;
		cube2->P1_3D.z = alpha * cube1->P1_3D.z;
		printf("\nP1 for 2nd Cube: (%f, %f, %f,)\n", cube2->P1_3D.x, cube2->P1_3D.y, cube2->P1_3D.z);

		//Compute new rotated coordiantes for P2
		cube2->P2_3D.x = alpha * ((cube1->P2_3D.x * cos_theta) - (cube1->P2_3D.y * sin_theta) - (200 * (cos_theta - sin_theta - 1))) + add;
		cube2->P2_3D.y = alpha * ((cube1->P2_3D.x * sin_theta) + (cube1->P2_3D.y * cos_theta) - (200 * (sin_theta + cos_theta - 1))) + add;
		cube2->P2_3D.z = alpha * cube1->P2_3D.z;
		printf("P2 for 2nd Cube: (%d, %d, %d,)\n", cube2->P2_3D.x, cube2->P2_3D.y, cube2->P2_3D.z);

		//Compute new rotated coordiantes for P3
		cube2->P3_3D.x = alpha * ((cube1->P3_3D.x * cos_theta) - (cube1->P3_3D.y * sin_theta) - (200 * (cos_theta - sin_theta - 1))) + add;
		cube2->P3_3D.y = alpha * ((cube1->P3_3D.x * sin_theta) + (cube1->P3_3D.y * cos_theta) - (200 * (sin_theta + cos_theta - 1))) + add;
		cube2->P3_3D.z = alpha * cube1->P3_3D.z;
		printf("P3 for 2nd Cube: (%d, %d, %d,)\n", cube2->P3_3D.x, cube2->P3_3D.y, cube2->P3_3D.z);

		//Compute new rotated coordiantes for P4
		cube2->P4_3D.x = alpha * ((cube1->P4_3D.x * cos_theta) - (cube1->P4_3D.y * sin_theta) - (200 * (cos_theta - sin_theta - 1))) + add;
		cube2->P4_3D.y = alpha * ((cube1->P4_3D.x * sin_theta) + (cube1->P4_3D.y * cos_theta) - (200 * (sin_theta + cos_theta - 1))) + add;
		cube2->P4_3D.z = alpha * cube1->P4_3D.z;
		printf("P4 for 2nd Cube: (%d, %d, %d,)\n", cube2->P4_3D.x, cube2->P4_3D.y, cube2->P4_3D.z);

		//Compute new rotated coordiantes for P5
		cube2->P5_3D.x = alpha * ((cube1->P5_3D.x * cos_theta) - (cube1->P5_3D.y * sin_theta) - (200 * (cos_theta - sin_theta - 1))) + add;
		cube2->P5_3D.y = alpha * ((cube1->P5_3D.x * sin_theta) + (cube1->P5_3D.y * cos_theta) - (200 * (sin_theta + cos_theta - 1))) + add;
		cube2->P5_3D.z = alpha * cube1->P5_3D.z;
		printf("P5 for 2nd Cube: (%d, %d, %d,)\n", cube2->P5_3D.x, cube2->P5_3D.y, cube2->P5_3D.z);

		//Compute new rotated coordiantes for P6
		cube2->P6_3D.x = alpha * ((cube1->P6_3D.x * cos_theta) - (cube1->P6_3D.y * sin_theta) - (200 * (cos_theta - sin_theta - 1))) + add;
		cube2->P6_3D.y = alpha * ((cube1->P6_3D.x * sin_theta) + (cube1->P6_3D.y * cos_theta) - (200 * (sin_theta + cos_theta - 1))) + add;
		cube2->P6_3D.z = alpha * cube1->P6_3D.z;
		printf("P6 for 2nd Cube: (%d, %d, %d,)\n", cube2->P6_3D.x, cube2->P6_3D.y, cube2->P6_3D.z);

		//Compute new rotated coordiantes for P7
		cube2->P7_3D.x = alpha * ((cube1->P7_3D.x * cos_theta) - (cube1->P7_3D.y * sin_theta) - (200 * (cos_theta - sin_theta - 1))) + add;
		cube2->P7_3D.y = alpha * ((cube1->P7_3D.x * sin_theta) + (cube1->P7_3D.y * cos_theta) - (200 * (sin_theta + cos_theta - 1))) + add;
		cube2->P7_3D.z = alpha * cube1->P7_3D.z;
		printf("P7 for 2nd Cube: (%d, %d, %d,)\n", cube2->P7_3D.x, cube2->P7_3D.y, cube2->P7_3D.z);

		//Compute new rotated coordiantes for P8
		cube2->P8_3D.x = alpha * ((cube1->P8_3D.x * cos_theta) - (cube1->P8_3D.y * sin_theta) - (200 * (cos_theta - sin_theta - 1))) + add;
		cube2->P8_3D.y = alpha * ((cube1->P8_3D.x * sin_theta) + (cube1->P8_3D.y * cos_theta) - (200 * (sin_theta + cos_theta - 1))) + add;
		cube2->P8_3D.z = alpha * cube1->P8_3D.z;
		printf("P8 for 2nd Cube: (%d, %d, %d,)\n", cube2->P8_3D.x, cube2->P8_3D.y, cube2->P8_3D.z);
}

void squarePattern(int x1, int y1, int x2,int y2,int x3,int y3,int x4,int y4)
{
	int x1_1=0, x2_1=0, x3_1=0, x4_1=0, y1_1=0,y2_1=0, y3_1=0, y4_1=0;
	float lambda=0.8;

	// Draw 10 levels of squares in each pattern
	for(int i=1;i<=5;i++){
		lcddelay(100);
		// Use the equation given in class to calculate the 4 vertices' coordinates of the recursive squares
		x1_1 = (x2+(lambda*(x1-x2)));
		y1_1 = (y2+(lambda*(y1-y2)));
		x2_1 = (x3+(lambda*(x2-x3)));
		y2_1 = (y3+(lambda*(y2-y3)));
		x3_1 = (x4+(lambda*(x3-x4)));
		y3_1 = (y4+(lambda*(y3-y4)));
		x4_1 = (x1+(lambda*(x4-x1)));
		y4_1 = (y1+(lambda*(y4-y1)));

		// Draw a square (4 drawline() for 4 sides)
		drawline(x1_1,y1_1,x2_1,y2_1,RED);
		drawline(x2_1,y2_1,x3_1,y3_1,RED);
		drawline(x4_1,y4_1,x3_1,y3_1,RED);
		drawline(x1_1,y1_1,x4_1,y4_1,RED);

	  	// Initiate the original vertices' values with the new calculated vertices' values
		x1 = x1_1;
		x2 = x2_1;
		x3 = x3_1;
		x4 = x4_1;

		y1 = y1_1;
		y2 = y2_1;
		y3 = y3_1;
		y4 = y4_1;
	}
}

void treePattern(int x, int y, float angle, int length, int level, int color)
{

	int x1,y1,len;
	float ang;

	if(level>0){
		// To calculate the x,y vertices for the branch after rotation
		x1 = x+length*cos(angle);
	    y1 = y+length*sin(angle);

	    // To draw the tree branch
	    drawline(x,y,x1,y1,color);

	    // Add 30 degree to angle to rotate it to right
	    ang = angle + 0.52;
	    // To calculate 80% of the line length
	    len = 0.8 * length;

	    // Call drawTree2d function recursively to draw tree pattern
	    treePattern(x1,y1,ang,len,level-1,color);

	    // Subtract 30 degree from the angle to rotate it to right
	    ang = angle - 0.52;
	    len = 0.8 * length;

	   treePattern(x1,y1,ang,len,level-1,color);

	    // Draw the next level
	    ang = angle;
	    len = 0.8 * length;
	    treePattern(x1,y1,ang,len,level-1,color);
	}
}

void draw_Initial(struct cube_t *cube, float cube_size)
{
	struct arrow_coordinates Intial_Cord;
	int x, y, x2, y2;
	struct coordinates_2D temp1, temp2;

	Intial_Cord.A1_2D = Transformation_pipeline(Intial_Cord.A1.x = cube->P1_3D.x + 10,
			Intial_Cord.A1.y = cube->P1_3D.y + 10, Intial_Cord.A1.z = cube_size);

	Intial_Cord.A2_2D = Transformation_pipeline(Intial_Cord.A2.x = cube->P1_3D.x + 10,
			Intial_Cord.A2.y = cube->P1_3D.y + 30 , Intial_Cord.A2.z = cube_size);

	Intial_Cord.A3_2D = Transformation_pipeline(Intial_Cord.A3.x = cube->P1_3D.x + 15,
			Intial_Cord.A3.y = cube->P1_3D.y + 30, Intial_Cord.A3.z = cube_size);

	Intial_Cord.A4_2D = Transformation_pipeline(Intial_Cord.A4.x = cube->P1_3D.x + 15,
			Intial_Cord.A4.y = cube->P1_3D.y + 12, Intial_Cord.A4.z = cube_size);

	Intial_Cord.A5_2D = Transformation_pipeline(Intial_Cord.A5.x = cube->P1_3D.x + 30,
			Intial_Cord.A5.y = cube->P1_3D.y + 12, Intial_Cord.A5.z = cube_size);

	Intial_Cord.A6_2D = Transformation_pipeline(Intial_Cord.A6.x = cube->P1_3D.x + 30,
			Intial_Cord.A6.y = cube->P1_3D.y + 10 , Intial_Cord.A6.z = cube_size);

	drawline(Intial_Cord.A1_2D.x, Intial_Cord.A1_2D.y, Intial_Cord.A2_2D.x, Intial_Cord.A2_2D.y, BLACK);
	drawline(Intial_Cord.A2_2D.x, Intial_Cord.A2_2D.y, Intial_Cord.A3_2D.x, Intial_Cord.A3_2D.y, BLACK);
	drawline(Intial_Cord.A3_2D.x, Intial_Cord.A3_2D.y, Intial_Cord.A4_2D.x, Intial_Cord.A4_2D.y, BLACK);
	drawline(Intial_Cord.A4_2D.x, Intial_Cord.A4_2D.y, Intial_Cord.A5_2D.x, Intial_Cord.A5_2D.y, BLACK);
	drawline(Intial_Cord.A5_2D.x, Intial_Cord.A5_2D.y, Intial_Cord.A6_2D.x, Intial_Cord.A6_2D.y, BLACK);
	drawline(Intial_Cord.A1_2D.x, Intial_Cord.A1_2D.y, Intial_Cord.A6_2D.x, Intial_Cord.A6_2D.y, BLACK);

}
