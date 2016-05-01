/**************************************************************************//**
 * @file
 * @brief Empty Project
 * @author Energy Micro AS
 * @version 3.20.2
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silicon Labs Software License Agreement. See 
 * "http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt"  
 * for details. Before using this software for any purpose, you must agree to the 
 * terms of that agreement.
 *
 ******************************************************************************/
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"

#define INCLUDE_DISPLAY_SHARP_LS013B7DH03

#include "display.h"

#include "ray_cast.h"
#include "map.h"


#define ANGLE_ADD(angle, val) \
	( (angle + val) & 0x3FF )

#define ANGLE_SUB(angle, val) \
	( (angle + val) & 0x3FF )

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
	/* Chip errata */
	CHIP_Init();

	CMU_OscillatorEnable(cmuOsc_HFXO, true, true);
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
	CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);

	DISPLAY_Device_t display;

	DISPLAY_Ls013b7dh03Init();
	DISPLAY_Init();
	DISPLAY_DriverRefresh();
	DISPLAY_DeviceGet(0, &display);

	viewport_t vp;
	init_viewport(128,128,144,&vp);
	canvas_t c;
	init_canvas(128,128,&c);
	player_t p = {
		  .position = {
			  .x = 129,
			  .y = 129,
		  },
		  .direction = 256*1-1*100,
	};

//	for(int col = 0; col < c.width; ++col){
//		int i;
//		int draw_height = 127-col;
//		int draw_start = (c.height - draw_height) >> 1;
//		int draw_end = (c.height + draw_height) >> 1;
//		for (i = 0; i < c.height; ++i){
//			if (i >= draw_start && i <= draw_end){
//				c.canvas[(i<<2) + (col >> 5)] = c.canvas[(i<<2) + (col >> 5)] | (0x1 << (col & 0x1F));
//			} else {
//				c.canvas[(i<<2) + (col >> 5)] = c.canvas[(i<<2) + (col >> 5)] & !(0x1 << (col & 0x1F));
//			}
//		}
//	}

#define ROTATE 1
#if ROTATE == 0
	render_viewport(&p, &vp, &c, &map);
	display.pDisplayPowerOn(&display, 1);
	display.pPixelMatrixDraw(&display, (void*)(c.canvas), 0, 128, 0, 128);
	while (1) {
  }
#else
	while(true){
		render_viewport(&p, &vp, &c, &map);
		p.direction = ANGLE_ADD(p.direction, 1);
		display.pDisplayPowerOn(&display, 1);
		display.pPixelMatrixDraw(&display, (void*)(c.canvas), 0, 128, 0, 128);
	}
#endif

	/* Infinite loop */

}
