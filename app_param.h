/*
 * Copyright (C) 2022-2023 by Y.Ishioka
 */

#ifndef  _APP_PARAM_H_
#define  _APP_PARAM_H_

/* LCD display size */
#define DEF_LCD_WIDTH        240
#define DEF_LCD_HIGH         320

#if 0
#define DEF_FONT_HIGH   16
#define DEF_FONT_WIDTH  16
#else
#define DEF_FONT_HIGH   32
#define DEF_FONT_WIDTH  32
#endif

#define DEF_FONT_OUT_HIGH   64
#define DEF_FONT_OUT_WIDTH  64

#define DEF_YOJI_MOJI        4
#define DEF_YOJI_LST_MAX   200

#define DEF_NIJI_LST_MAX   500

#define DEF_TIM_YOJI_DLY   500
#define DEF_TIM_SET_DLY    500
#define DEF_TIM_GRAD_DLY    50
#define DEF_TIM_CLR_DLY    500

#define DEF_TIM_NIJI_GRAD_DLY    50
#define DEF_TIM_ANS_DISP  5000
#define DEF_TIM_NEXT_DLY  3000

#define DEF_DP_X_OFFSET     40

#define RGB565_BLACK        ST77XX_BLACK
#define RGB565_RED          ST77XX_RED
#define RGB565_GREEN        ST77XX_GREEN
#define RGB565_BLUE         ST77XX_BLUE
#define RGB565_MAGENTA      ST77XX_MAGENTA
#define RGB565_CYAN         ST77XX_CYAN
#define RGB565_YELLOW       ST77XX_YELLOW
#define RGB565_ORANGE       ST77XX_ORANGE
#define RGB565_WHITE        ST77XX_WHITE
#define RGB565_PINK         0xFE19
#define RGB565_SKYBLUE      0x867D
#define RGB565_NAVY         0x000F
#define RGB565_GREENYELLOW  0xB7E0
#define RGB565_NEON         0x37E2
#define RGB565_BUTTERMILK   0xEE73
#define RGB565_BTOMATO      0xFB08

/* Item number for SD card data */
#define DEF_TEXT_BUFF   (1024)

#endif /* _APP_PARAM_H_ */

