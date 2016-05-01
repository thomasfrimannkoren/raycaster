/***************************************************************************//**
 * @file displayconfigapp.h
 * @brief Display application specific configuration file.
 * @version 4.1.0
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef _DISPLAY_CONFIG_APP_H_
#define _DISPLAY_CONFIG_APP_H_

/* We must include TEXTDISPLAY support because this example is a
   TEXTDISPLAY demo. */
#define INCLUDE_TEXTDISPLAY_SUPPORT


/* Include escape sequence support: */
#define INCLUDE_VIDEO_TERMINAL_ESCAPE_SEQUENCE_SUPPORT


/* Select one of the fonts listed below:
   TEXTDISPLAY_FONT_6x8
   TEXTDISPLAY_FONT_8x8
*/
#define  TEXTDISPLAY_FONT_6x8


/* Enable or disable scroll mode on the text display. */
#define RETARGETTEXTDISPLAY_SCROLL_MODE  (false)


/* Enable or disable adding Carriage Return (CR) to Line Feed (LF) characters
   on the text display. */
#define RETARGETTEXTDISPLAY_LINE_FEED_MODE  (true)


#endif /* _DISPLAY_CONFIG_APP_H_ */
