#include "em_device.h"

#include <math.h>
#include <stdlib.h>

#include "ray_cast.h"
#include "trig_tables.h"

#define DEGTOINTERNAL(degrees) ((degrees*512)/360)

#define MIN(X, Y) (((X) <= (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) >= (Y)) ? (X) : (Y))

#define CHECK_INTERSECTION(intersection, map) \
	( map[intersection.y >> 6][intersection.x >> 6] )

#define INTERSECTION_NEXT(intersection, delta) {\
	intersection.x += delta.x; \
	intersection.x = (MIN(MAX(intersection.x, 0), 2048)); \
	intersection.y += delta.y; \
	intersection.y = (MIN(MAX(intersection.y, 0), 2048));}

#define FISHEYE_CORRECTION 1

void init_canvas(int32_t height, int32_t width, canvas_t *c){
	c->height = height;
	c->width = width;
	c->canvas = (uint8_t*)malloc(sizeof(uint32_t)*height*(width >> 5));
}

void init_viewport(int32_t height, int32_t width, uint32_t fov, viewport_t* vp){
	vp->height = height;
	vp->width = width;
	uint32_t tan_val;
	GET_TAN_VAL(tan_val, (fov >> 1));
	int32_t dist_to_vp = TRIG_DIV((width >> 1), tan_val);
	vp->wall_x_vp = WALL_HEIGHT * dist_to_vp;
	vp->fov = fov;
}

void render_viewport(player_t* p, viewport_t* vp, canvas_t* c, map_t map){
	uint32_t col;
	uint32_t angle_inc = (vp->fov << 16) / vp->width;
	uint32_t angle =  ((p->direction << 16) + ((vp->fov << 16) / 2)) & 0x3FFFFFF;
	for(col = 0; col < vp->width; ++col){
		render_column(col, (angle >> 16), p, vp, c, map);
		angle = (angle - angle_inc) & 0x3FFFFFF;
	}
	return;
}

void render_column(uint32_t col,uint32_t angle, player_t* p, viewport_t* vp, canvas_t* c, map_t map){
	coordinate_t y_intersection;
	coordinate_t x_intersection;
	coordinate_t y_delta;
	coordinate_t x_delta;
	int32_t tan_val;
	int32_t cos_val;
	int32_t sin_val;
	GET_TAN_VAL(tan_val, angle);
	GET_COS_VAL(cos_val, angle);
	GET_SIN_VAL(sin_val, angle);
	uint32_t x_iterations;
	uint32_t y_iterations;
	uint32_t i;

	/*Find map intersection and deltas in y-direction (up/down)*/
	if( angle < MAX_ANGLE/2 ){
		y_intersection.y = (p->position.y & 0xFFFFFF80) - 1;
		y_delta.y = -64;
	} else {
		y_intersection.y = (p->position.y & 0xFFFFFF80) + 64;
		y_delta.y = 64;
	}
	y_intersection.x = p->position.x + TRIG_DIV( (p->position.y - y_intersection.y), tan_val);
	y_intersection.x = MIN(MAX(y_intersection.x, 0), 2047);
	y_delta.x = TRIG_DIV(64, tan_val);
	y_iterations = (abs(y_delta.x) >> 6) + 1;

	/*Find map intersection and deltas in x-direction*/
	if( ((angle >> ANGLE_QUADRANT_SHIFT) == 0x01) || ((angle >> ANGLE_QUADRANT_SHIFT) == 0x02) ){
		x_intersection.x = (p->position.x & 0xFFFFFF80) - 1;
		x_delta.x = -64;
	} else {
		x_intersection.x = (p->position.x & 0xFFFFFF80) + 64;
		x_delta.x = 64;
	}
	x_intersection.y = p->position.y + TRIG_MULT( (p->position.x - x_intersection.x), tan_val);
	x_intersection.y = MIN(MAX(x_intersection.y, 0), 2047);
	x_delta.y = TRIG_MULT(64, tan_val);
	x_iterations = (abs(x_delta.y) >> 6) + 1;

//	/*Finding initial pointing direction*/
//	direction_t dir;
//	if (angle < 128 || angle > 896){
//		dir = DIR_RIGHT;
//	} else if (angle < 384) {
//		dir = DIR_UP;
//	} else if (angle < 640) {
//		dir = DIR_LEFT;
//	} else {
//		dir = DIR_DOWN;
//	}
//
//	uint8_t intersection_found = 0;
//	coordinate_t intersection;
//	switch(dir){
//	case (DIR_UP):
//	case (DIR_DOWN):
//		for(i = 0; i < y_iterations; ++i){
//		//while(y_intersection.y <= x_intersection.y){
//			if (CHECK_INTERSECTION(y_intersection, map)){
//				intersection_found = 1;
//				intersection = y_intersection;
//				break;
//			} else {
//				INTERSECTION_NEXT(y_intersection, y_delta);
//			}
//		}
//		if (intersection_found) break;
//		/*Supposed to fall through*/
//	case (DIR_LEFT):
//	case (DIR_RIGHT):
//		for(i = 0; i < x_iterations; ++i){
//		//while(x_intersection.x <= y_intersection.x){
//			if (CHECK_INTERSECTION(x_intersection, map)){
//				intersection_found = 1;
//				intersection = x_intersection;
//				break;
//			} else {
//				INTERSECTION_NEXT(x_intersection, x_delta);
//			}
//		}
//	default:
//		break;
//	}
//
//	while (!intersection_found){
//		//while(y_intersection.y <= x_intersection.y){
//		for(i = 0; i < y_iterations; ++i){
//			if (CHECK_INTERSECTION(y_intersection, map)){
//				intersection_found = 1;
//				intersection = y_intersection;
//				break;
//			} else {
//				INTERSECTION_NEXT(y_intersection, y_delta);
//			}
//		}
//		if (intersection_found) break;
//		//while(x_intersection.x <= y_intersection.x){
//		for(i = 0; i < x_iterations; ++i){
//			if (CHECK_INTERSECTION(x_intersection, map)){
//				intersection_found = 1;
//				intersection = x_intersection;
//				break;
//			} else {
//				INTERSECTION_NEXT(x_intersection, x_delta);
//			}
//		}
//	}
//
//	int distance_x = (p->position.x - intersection.x);
//	int distance_y = (p->position.y - intersection.y);
//	int distance;
//	if( abs(distance_x) > abs(distance_y) ){
//		distance = abs(TRIG_DIV( (p->position.x - intersection.x), cos_val));
//	} else {
//		distance = abs(TRIG_DIV( (p->position.y - intersection.y), sin_val));
//	}

	/*Finding initial pointing direction*/
	direction_t dir;
	if (angle < 128 || angle > 896){
		dir = DIR_RIGHT;
	} else if (angle < 384) {
		dir = DIR_UP;
	} else if (angle < 640) {
		dir = DIR_LEFT;
	} else {
		dir = DIR_DOWN;
	}

	uint32_t x_distance;
	uint32_t y_distance;
	while(1){
		if (CHECK_INTERSECTION(y_intersection, map)){
			switch(dir){
			case (DIR_UP):
			case (DIR_DOWN):
				y_distance = abs(TRIG_DIV( (p->position.y - y_intersection.y), sin_val));
				break;
			case (DIR_RIGHT):
			case (DIR_LEFT):
				y_distance = abs(TRIG_DIV( (p->position.x - y_intersection.x), cos_val));
				break;
			default:
				y_distance = UINT32_MAX;
				break;
			}
			break;
		} else {
			INTERSECTION_NEXT(y_intersection, y_delta);
		}
	}
	while(1){
		if (CHECK_INTERSECTION(x_intersection, map)){
			switch(dir){
			case (DIR_UP):
			case (DIR_DOWN):
				x_distance = abs(TRIG_DIV( (p->position.y - x_intersection.y), sin_val));
				break;
			case (DIR_RIGHT):
			case (DIR_LEFT):
				x_distance = abs(TRIG_DIV( (p->position.x - x_intersection.x), cos_val));
				break;
			default:
				x_distance = UINT32_MAX;
				break;
			}
			break;
		} else {
			INTERSECTION_NEXT(x_intersection, x_delta);
		}
	}

	uint32_t distance = MIN(x_distance, y_distance);

#if FISHEYE_CORRECTION
	int correction_angle =  abs(p->direction - angle);
	int cos_correction;
	GET_COS_VAL(cos_correction, correction_angle);
	distance = TRIG_MULT(distance, cos_correction);
#endif
	int draw_height = vp->wall_x_vp / distance;
	int draw_start = MAX( ((c->height - draw_height) >> 1), 0);
	int draw_end = MIN( ((c->height + draw_height) >> 1), 127);
	for (i = 0; i < vp->height; ++i){
		if (i >= draw_start && i <= draw_end){
			c->canvas[(i<<2) + (col >> 5)] = c->canvas[(i<<2) + (col >> 5)] | (0x1 << (col & 0x1F));
		} else {
			c->canvas[(i<<2) + (col >> 5)] = c->canvas[(i<<2) + (col >> 5)] & ~(0x1 << (col & 0x1F));
		}
	}
}
