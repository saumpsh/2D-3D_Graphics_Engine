/*
 * 3D-Cube_v1.h
 *
 *  Created on: Nov 18, 2018
 *      Author: SaumPC
 */

#ifndef Three_D_cube_h_
#define Three_D_cube_h_

#include <stdint.h>
#define PI 3.142
#define D 300 //100

struct coordinates_2D
{
	int16_t x;
	int16_t y;
};
struct coordinates_3D
{
	float x;
	float y;
	float z;
};

struct cube_t
{
	struct coordinates_2D P1, P2, P3, P4, P5, P6, P7, P8;
	struct coordinates_3D P1_3D, P2_3D, P3_3D, P4_3D, P5_3D, P6_3D, P7_3D, P8_3D;

	struct coordinates_2D Sh1, Sh2, Sh3, Sh4;
	struct coordinates_3D Sh1_3D, Sh2_3D, Sh3_3D, Sh4_3D;
};

struct arrow_coordinates
{

	/*
	 * A7
	 * ------- A6
	 * 		 |
	 * 		 \ A5
	 * 		  \
	 * 		   \ A4
	 * 		   /
	 * 		  /
	 * 		 / A3
	 * 	A1	 |
	 * 	------ A2
	 *
	 */
	struct coordinates_3D A1, A2, A3, A4, A5, A6, A7;
	struct coordinates_2D A1_2D, A2_2D, A3_2D, A4_2D, A5_2D, A6_2D, A7_2D;
};

#define pgm_read_byte(addr) (*(const unsigned char *)(addr))

// To draw a 3D cube
//void draw3dcube1(struct cube_t *, int point, int cube_size);
void draw3dcube1(struct cube_t *cube, float cube_size, float x_base, float y_base, float z_base);
// To draw the world 3D coordinate system - x,y,z
void drawCoordinate();
// To fill the background with given color and dimensions
void background(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color);

void rotateCube(struct cube_t *cube1, struct cube_t *cube2, float alpha, float theta_deg);

void PointOf_Light_Source(struct cube_t *, float xs, float xy, float xz);

void draw_shadow(struct cube_t *, int cube_size);

void draw_arrow(int cube_size);

void compute_transformation_parameter();

void squarePattern(int x1, int y1, int x2,int y2,int x3,int y3,int x4,int y4);

void treePattern(int x, int y, float angle, int length, int level, int color);

void draw_Initial(struct cube_t *cube, float cube_size);

#endif /* 3D_CUBE_V1_H_ */
