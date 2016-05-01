#ifndef _raycast_h
#define _raycast_h

#define MAP_HEIGHT	32
#define MAP_WIDTH	32
#define ANGLE_QUADRANT_SHIFT	8
#define MAX_ANGLE	1024
#define WALL_HEIGHT 64

typedef struct _viewport_t {
	int32_t height;
	int32_t width;
	int32_t wall_x_vp;
	uint32_t fov;
} viewport_t;

typedef struct _coordinate_t {
	int32_t x;
	int32_t y;
} coordinate_t;

typedef struct _player_t {
	coordinate_t position;
	uint32_t direction;
} player_t;

typedef struct _canvas_t {
	int32_t height;
	int32_t width;
	uint32_t* canvas;
} canvas_t;

typedef uint8_t map_t[32][32];

typedef enum{DIR_RIGHT, DIR_UP, DIR_LEFT, DIR_DOWN} direction_t;

void init_canvas(int32_t height, int32_t width, canvas_t *c);
void init_viewport(int32_t height, int32_t width, uint32_t fov, viewport_t* vp);
void render_viewport(player_t* p, viewport_t* vp, canvas_t* c, map_t map);
void render_column(uint32_t col,uint32_t angle, player_t* p, viewport_t* vp, canvas_t* c, map_t map);

#endif //_raycast_h
