/*
 *  Copyright(C) 2023 by Yukiya Ishioka
 */

#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

#define TFT_CS      5
#define TFT_RST    48
#define TFT_DC      4
#define TFT_SCLK    7
#define TFT_MOSI    6
#define TFT_CTRL   45

#define ADC_SW_PIN  1

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);


#include "app_param.h"

/* フォント配列の設定 */
extern  const unsigned char  ILMH32XB_FNT[];
extern  const unsigned char  ILMZ32XB_FNT[];
#define DEF_FONT_A_VAR       (unsigned char *)ILMH32XB_FNT
#define DEF_FONT_K_VAR       (unsigned char *)ILMZ32XB_FNT

/* 熟語データ */
extern  const unsigned char  yoji_data[];
extern  const unsigned char  yoji_data_eki[];
extern  const unsigned char  niji_data[];

/* 変数、配列 */
unsigned char  yoji_buffer1[ 4 ][ DEF_FONT_HIGH ][DEF_FONT_WIDTH ];
unsigned char  yoji_buffer2[ 4 ][ DEF_FONT_HIGH ][DEF_FONT_WIDTH ];
unsigned char  tmp_buffer[ DEF_FONT_HIGH ][ DEF_FONT_WIDTH ];
unsigned char  tmp_ank_buffer[ DEF_FONT_HIGH ][ DEF_FONT_WIDTH/2 ];

int  yoji_list[ DEF_YOJI_LST_MAX ];
int  char_list_1st[ DEF_NIJI_LST_MAX ];
int  char_list_2nd[ DEF_NIJI_LST_MAX ];

int  lcd_char_x;
int  lcd_char_y;
int  lcd_output_flag;
int  lcd_output_color;

int  disp_color;
int  disp_color_bak;
int  yoji_item_num;
int  yoji_item_pos;
int  yoji_item_pos_bef1;
int  yoji_item_pos_bef2;

int  yoji_pos;
int  yoji_dir;
int  yoji_bef_pos;
int  yoji_bef_dir;
unsigned int  yoji_code;

unsigned char  *yoji_data_pnt;
unsigned char  *niji_data_pnt;

int  x_item_all_cnt;
int  x_item_1st_cnt;
int  x_item_2nd_cnt;

int  x_demo_number;

unsigned char  kigou_quest[2] = { 0x81, 0x48 };
unsigned char  title_yoji[8] = { 0x8E,0x6C,0x8E,0x9A,0x8F,0x6E,0x8C,0xEA };
unsigned char  title_eki[8] = { 0x8E,0x6C,0x8E,0x9A,0x89,0x77,0x96,0xBC };
unsigned char  title_niji[8] = { 0x93,0xF1,0x8E,0x9A,0x8F,0x6E,0x8C,0xEA };


unsigned char  kigou_right2[ 16 ][ 16 ] = {
 /* 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 */
  { 0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0 },
  { 0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0 },
  { 0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0 },
  { 0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0 },
  { 0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0 },
  { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0 },
  { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0 },
  { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
  { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
  { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0 },
  { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0 },
  { 0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0 },
  { 0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0 },
  { 0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0 },
  { 0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0 },
  { 0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0 },
};

unsigned char  kigou_down2[ 16 ][ 16 ] = {
 /* 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 */
  { 0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0 },
  { 0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0 },
  { 0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0 },
  { 0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0 },
  { 0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0 },
  { 0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0 },
  { 0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0 },
  { 0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0 },
  { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
  { 0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0 },
  { 0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0 },
  { 0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0 },
  { 0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0 },
  { 0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0 },
  { 0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0 },
  { 0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0 },
};

uint16_t  lcd_color_tbl[] = {
  RGB565_RED,
  RGB565_GREEN,
  RGB565_CYAN,
  RGB565_YELLOW,
  RGB565_ORANGE,
  RGB565_WHITE,
  0x0000
};

#define  DEF_LCD_COLOR_NUM  6


#define DR_REG_RNG_BASE  0x3ff75144

/*************************************************
 *  DR_REG_RNG
 *************************************************/
uint32_t getESP32Random( void )
{
  delay( 2 );
  return  READ_PERI_REG(DR_REG_RNG_BASE);
}


/*************************************************
 *  Buttons_ADC
 *   return : 0   no push
 *            1   left
 *            2   center
 *            3   right
 *************************************************/
int  getButtonADC( void )
{
  int  vol;
  int  btn = 0;

  vol = analogRead( ADC_SW_PIN );
  if( vol < 1200 ) {         /* 3.3/(10+3.3) -> 1016 */
    btn = 3;
  } else if( vol < 2600 ) {  /* 15/(10+15) -> 2457 */
    btn = 2;
  } else if( vol < 3200 ) {  /* 27/(10+27) -> 2988 */
    btn = 1;
  }

  return  btn;
}


/*************************************************
 *  check demo
 *************************************************/
int  check_demo_mode( void )
{
    int  sw;
    int  ret = 0;

    sw = getButtonADC();
    if( sw == 1 ) {
        if( x_demo_number != 1 ) {
            ret = 1;
            x_demo_number = 1;
        }
    } else if( sw == 2 ) {
        if( x_demo_number != 2 ) {
            ret = 2;
            x_demo_number = 2;
        }
    } else if( sw == 3 ) {
        if( x_demo_number != 3 ) {
            ret = 3;
            x_demo_number = 3;
        }
    }

    return  ret;
}


/*************************************************
 *  
 *************************************************/
void  y_fillRect( int x, int y, int dx, int dy, uint16_t color )
{
  int  row;
  int  col;

  tft.startWrite();
  tft.setAddrWindow( DEF_LCD_WIDTH-1-y-dy, x, dy, dx );
  tft.endWrite();

  for( row=0 ; row<dx ; row++ ) {
    for( col=0 ; col<dy ; col++ ) {
      tft.pushColor( color );
    }
  }
}


/*************************************************
 *  
 *************************************************/
void  y_fillScreen( uint16_t color )
{
  int  row;
  int  col;

  tft.startWrite();
  tft.setAddrWindow(0, 0, DEF_LCD_WIDTH, DEF_LCD_HIGH);
  tft.endWrite();

  for( row=0 ; row<DEF_LCD_HIGH ; row++ ) {
    for( col=0 ; col<DEF_LCD_WIDTH ; col++ ) {
      tft.pushColor( color );
    }
  }
}


/*************************************************
 *  font size  ANK
 *************************************************/
void  y_setImage_a( uint16_t x, uint16_t y, uint16_t color, unsigned char *buff )
{
  int  row;
  int  col;
  int  wx = DEF_FONT_WIDTH/2;
  int  wy = DEF_FONT_HIGH;

  int  tx;
  int  ty;

  for( ty=0 ; ty<DEF_FONT_HIGH ; ty++ ) {
    for( tx=0 ; tx<DEF_FONT_WIDTH/2 ; tx++ ) {
      tmp_buffer[ tx ][ DEF_FONT_HIGH-1 - ty ] = *buff;
      buff++;
    }
  }
  buff = (unsigned char*)tmp_buffer;

  tft.startWrite();
  //tft.setAddrWindow( x, y, wx, wy );
  tft.setAddrWindow( DEF_LCD_WIDTH-1-y-wy, x, wy, wx );
  tft.endWrite();

  for( row=0 ; row<wy ; row++ ) {
    for( col=0 ; col<wx ; col++ ) {
      if( *buff != 0 ) {
        tft.pushColor( color );
      } else {
        tft.pushColor( RGB565_BLACK );
      }
      buff++;
    }
  }
}


/*************************************************
 *  font size for KANJI
 *************************************************/
void  y_setImage_k( uint16_t x, uint16_t y, uint16_t color, unsigned char *buff )
{
  int  row;
  int  col;
  int  wx = DEF_FONT_WIDTH;
  int  wy = DEF_FONT_HIGH;

  int  tx;
  int  ty;

  for( ty=0 ; ty<DEF_FONT_HIGH ; ty++ ) {
    for( tx=0 ; tx<DEF_FONT_WIDTH ; tx++ ) {
      tmp_buffer[ tx ][ DEF_FONT_HIGH-1 - ty ] = *buff;
      buff++;
    }
  }
  buff = (unsigned char*)tmp_buffer;

  tft.startWrite();
  //tft.setAddrWindow( x, y, wx, wy );
  tft.setAddrWindow( DEF_LCD_WIDTH-1-y-wy, x, wy, wx );
  tft.endWrite();

  for( row=0 ; row<wy ; row++ ) {
    for( col=0 ; col<wx ; col++ ) {
      if( *buff != 0 ) {
        tft.pushColor( color );
      } else {
        tft.pushColor( RGB565_BLACK );
      }
      buff++;
    }
  }
}


/*************************************************
 *  
 *************************************************/
void  y_output_char_a( unsigned char *buff )
{
  int  tmp_x;
  int  tmp_y;

  tmp_x = lcd_char_x + DEF_FONT_WIDTH/2;

  if( tmp_x >= DEF_LCD_WIDTH ) {
      lcd_char_x = 0;

      tmp_y = lcd_char_y + DEF_FONT_HIGH + 4;
      if( tmp_y >= DEF_LCD_HIGH ) {
          lcd_output_flag = 1;
          return;
      }
      lcd_char_y = tmp_y;
  }

  y_setImage_a( lcd_char_x, lcd_char_y, lcd_color_tbl[ lcd_output_color%DEF_LCD_COLOR_NUM ], buff );

  lcd_char_x += DEF_FONT_WIDTH/2;
}


/*************************************************
 *  
 *************************************************/
void  y_output_char_k( unsigned char *buff )
{
  int  tmp_x;
  int  tmp_y;

  tmp_x = lcd_char_x + DEF_FONT_WIDTH;

  if( tmp_x >= DEF_LCD_WIDTH ) {
      lcd_char_x = 0;

      tmp_y = lcd_char_y + DEF_FONT_HIGH + 4;
      if( tmp_y >= DEF_LCD_HIGH ) {
          lcd_output_flag = 1;
          return;
      }
      lcd_char_y = tmp_y;
  }

  y_setImage_k( lcd_char_x, lcd_char_y, lcd_color_tbl[ lcd_output_color%DEF_LCD_COLOR_NUM ], buff );

  lcd_char_x += DEF_FONT_WIDTH;
}


/*************************************************
 *  １文字分のフォントデータを表示データ配列の指定位置へセット
 *************************************************/
void  set_font( uint8_t *font, uint8_t *buff, int width )
{
    int  i, j, k;
    int  row;
    int  w = (width/8);   /* font width byte */
    uint8_t  pat;

    /* row */
    for( i=0 ; i<DEF_FONT_HIGH ; i++ ) {
        row = width * i;
        /* col */
        for( j=0 ; j<w ; j++ ) {
            pat = 0x80;
            for( k=0 ; k<8 ; k++ ) {
                if( (font[ i * w + j ] & pat) != 0 ) {
                    buff[ row + j*8 + k ] = 1;
                }
                pat >>= 1; /* bit shift */
            }
        }
    }
}


void  make_yoji_bitmap1( unsigned char  *yoji )
{
    unsigned int  code;
    int  i;
    unsigned char  *fontdata;

	memset( yoji_buffer1, 0x00, sizeof(yoji_buffer1) );

    for( i=0 ; i<DEF_YOJI_MOJI ; i++ ) {
        code = (unsigned int)(*yoji);
        code = (code<<8) + *(yoji+1);
        fontdata = read_fontx2_k( DEF_FONT_K_VAR, code );
        set_font( fontdata, (uint8_t *)yoji_buffer1[i], DEF_FONT_WIDTH );
        yoji += 2;
    }
}


void  make_yoji_bitmap2( unsigned char  *yoji )
{
    unsigned int  code;
    int  i;
    unsigned char  *fontdata;

	memset( yoji_buffer2, 0x00, sizeof(yoji_buffer2) );

    for( i=0 ; i<DEF_YOJI_MOJI ; i++ ) {
        code = (unsigned int)(*yoji);
        code = (code<<8) + *(yoji+1);
        fontdata = read_fontx2_k( DEF_FONT_K_VAR, code );
        set_font( fontdata, (uint8_t *)yoji_buffer2[i], DEF_FONT_WIDTH );
        yoji += 2;
    }
}


void  set_font_to_lcdimg( int x, int y, unsigned char *font, unsigned int color )
{
  int  loop;
  unsigned int  ctmp;
  unsigned int  r, g, b;

  ctmp = lcd_color_tbl[color%DEF_LCD_COLOR_NUM];

  for( loop=4 ; loop>=0 ; loop-- ) {
    r = ((ctmp & 0xf800) >> loop) & 0xf800;
    g = ((ctmp & 0x07e0) >> loop) & 0x07e0;
    b = ((ctmp & 0x001f) >> loop) & 0x001f;
    //lcd_set_image( x, y, 32, 32, font, r|g|b );
    y_setImage_k( x, y, r|g|b, font );
    delay( DEF_TIM_GRAD_DLY );
  }
}


void  clr_font_to_lcdimg( int x, int y, unsigned char *font, unsigned int color )
{
  int  loop;
  unsigned int  ctmp;
  unsigned int  r, g, b;

  ctmp = lcd_color_tbl[ color%DEF_LCD_COLOR_NUM ];

  for( loop=1 ; loop<=5 ; loop++ ) {
    r = ((ctmp & 0xf800) >> loop) & 0xf800;
    g = ((ctmp & 0x07e0) >> loop) & 0x07e0;
    b = ((ctmp & 0x001f) >> loop) & 0x001f;
    if( loop == 5 ) {
      g = 0;
    }
    //lcd_set_image( x, y, 32, 32, font, r|g|b );
    y_setImage_k( x, y, r|g|b, font );
    delay( DEF_TIM_GRAD_DLY );
  }
}


/*************************************************
 *  半角文字コードからフォントデータの先頭アドレス取得
 *************************************************/
unsigned char  *read_fontx2_a( unsigned char *font, unsigned int code )
{
    unsigned char  *address = NULL ;
    unsigned int  fontbyte ;

    fontbyte = (font[14] + 7) / 8 * font[15] ;
    address = &font[17] + fontbyte * code ;

    return  address ;
}


/*************************************************
 *  全角文字コードからフォントデータの先頭アドレス取得
 *************************************************/
unsigned char  *read_fontx2_k( unsigned char *font, unsigned int code )
{
    unsigned char  *address = NULL ;
    unsigned char  *tmp ;
    unsigned int  blknum, i, fontnum ;
    unsigned int  bstart, bend ;
    unsigned int  fontbyte ;

    fontbyte = (font[14] + 7) / 8 * font[15] ;
    fontnum = 0 ;

    blknum = (unsigned int)font[17] * 4 ;
    tmp = &font[18] ;
    for( i=0 ; i<blknum ; i+=4 ) {
        bstart = tmp[i]   + ((unsigned int)tmp[i+1] << 8) ;
        bend   = tmp[i+2] + ((unsigned int)tmp[i+3] << 8) ;
        if( code >= bstart && code <= bend ) {
            address = tmp + (fontnum + (code - bstart)) * fontbyte + blknum ;
            break ;
        }

        fontnum += (bend - bstart) + 1 ;
    }

    return  address ;
}


int  yoji_item_count( void )
{
    int  count = 0;
    unsigned char  *pnt = yoji_data_pnt;

    while( *pnt != 0x00 ) {
        count++;
        pnt += 10;
    }

    return  count;
}


int  yoji_comp_list( unsigned int code, int pos )
{
    int  num;
    int  i;
    unsigned char  *pnt = yoji_data_pnt;

    for( i=0 ; i<DEF_YOJI_LST_MAX ; i++ ) {
        yoji_list[i] = 0;
    }

    pnt += pos * 2;
    num = 0;
    for( i=0 ; i<yoji_item_num ; i++ ) {
        if( *pnt == ((code >> 8) & 0xff) && *(pnt+1) == (code & 0xff) ) {
            yoji_list[ num++ ] = i;
            if( num >= DEF_YOJI_LST_MAX ) {
                break;
            }
        }
        pnt += 10;
    }

    return  num;
}


int  pos_template_y[] = {  50,  86, 122, 158 };
int  pos_template_x[] = {  80, 116, 152, 188 };


void  task1( void )
{
    unsigned char  *yoji_table;
    int  num;
    int  i;
    int  match;
    int  pos;
    int  loop;
    int  tmp_yoji_item_pos;

    yoji_pos = 0;
    yoji_dir = 0;
    yoji_bef_pos = 0;
    yoji_bef_dir = 0;
    yoji_code = 0x0000;
    yoji_item_pos_bef1 = 99999;
    yoji_item_pos_bef2 = 99999;

    digitalWrite( TFT_CTRL, HIGH );
    y_fillScreen(RGB565_BLACK);
    digitalWrite( TFT_CTRL, LOW );

#if 0
    memset( tmp_ank_buffer, 0x00, sizeof(tmp_ank_buffer) );
    set_font( read_fontx2_a( DEF_FONT_A_VAR, (unsigned int)('0'+x_demo_number) ), (unsigned char *)tmp_ank_buffer, DEF_FONT_WIDTH/2 );
    set_lcd_image( (unsigned char *)tmp_ank_buffer, 0, 0, DEF_FONT_WIDTH/2, DEF_FONT_HIGH, RGB565_BTOMATO );
#else
   {
    unsigned char  *title_pnt;
    int  i;

    if( x_demo_number == 1 ) {
        title_pnt = (unsigned char *)title_yoji;
    } else {
        title_pnt = (unsigned char *)title_eki;
    }

    for( i=0 ; i<4 ; i++ ) {
        memset( tmp_buffer, 0x00, sizeof(tmp_buffer) );
        make_kanji_bitmap( title_pnt, (unsigned char  *)tmp_buffer );
        set_lcd_image( (unsigned char *)tmp_buffer, 0, 34*i, DEF_FONT_WIDTH, DEF_FONT_HIGH, RGB565_BLUE );
        title_pnt += 2;
    }
   }
#endif

    delay( DEF_TIM_YOJI_DLY );

    if( x_demo_number == 1 ) {
        yoji_data_pnt = (unsigned char *)yoji_data;
    } else {
        yoji_data_pnt = (unsigned char *)yoji_data_eki;
    }
    yoji_item_num = yoji_item_count();
    Serial.printf( "yoji_item_num=%d", yoji_item_num );
    yoji_item_pos = rand() % yoji_item_num;
    Serial.printf( "yoji_item_pos=%d", yoji_item_pos );

    disp_color = 0;
    disp_color_bak = 0;
    yoji_table = yoji_data_pnt + yoji_item_pos * 10;

    /* set yoji */
    make_yoji_bitmap1( yoji_table );
    set_font_to_lcdimg( pos_template_x[0], pos_template_y[yoji_pos], (unsigned char *)yoji_buffer1[0], disp_color );
    set_font_to_lcdimg( pos_template_x[1], pos_template_y[yoji_pos], (unsigned char *)yoji_buffer1[1], disp_color );
    set_font_to_lcdimg( pos_template_x[2], pos_template_y[yoji_pos], (unsigned char *)yoji_buffer1[2], disp_color );
    set_font_to_lcdimg( pos_template_x[3], pos_template_y[yoji_pos], (unsigned char *)yoji_buffer1[3], disp_color );

    delay( DEF_TIM_YOJI_DLY );

    while( true ) {
        disp_color_bak = disp_color;
        disp_color++;
        if( lcd_color_tbl[disp_color] == 0x0000 ) {
            disp_color = 0;
        }

        yoji_bef_pos = yoji_pos;
        yoji_pos = rand() % DEF_YOJI_MOJI;

        /* search yoji */
        for( match=0 ; match <= 1 ; match++ ) {
            for( i=0 ; i<DEF_YOJI_MOJI ; i++ ) {
                yoji_table = yoji_data_pnt + yoji_item_pos * 10 + yoji_pos * 2;
                yoji_code = (*yoji_table << 8) + *(yoji_table + 1);

                /* make list and get list-num */
                num = yoji_comp_list( yoji_code, yoji_bef_pos );

                if( num == 0 ) {
                    Serial.printf( "yoji_pos=%d (match=%d  num=%d)  skip search.", yoji_pos, match, num );
                    yoji_pos++;
                    if( yoji_pos >= DEF_YOJI_MOJI ) {
                        yoji_pos = 0;
                    }
                    continue;
                }
                Serial.printf( "yoji_pos=%d (match=%d  num=%d)", yoji_pos, match, num );

                /* duplication check for previous data */
                tmp_yoji_item_pos = yoji_item_pos;
                for( loop=0 ; loop<5 ; loop++ ) {
                    pos = rand() % num;
                    yoji_item_pos = yoji_list[pos];
                    //if( (match > 0 && num == 1) || 
                    if( (match > 0) || 
                        (yoji_item_pos != yoji_item_pos_bef1 && yoji_item_pos != yoji_item_pos_bef2) ) {
                        match = 2;
                        break;
                    }
                }
                if( match == 2 ) {
                    break;
                } else {
                    Serial.printf( "duplicated. new=%d  bef1=%d  bef2=%d", 
                           yoji_item_pos, yoji_item_pos_bef1, yoji_item_pos_bef2 );
                    yoji_item_pos = tmp_yoji_item_pos;
                    yoji_pos++;
                    if( yoji_pos >= DEF_YOJI_MOJI ) {
                        yoji_pos = 0;
                    }
                }
            }
        }
        Serial.printf( "yoji_pos=%d  yoji_item_pos=%d", yoji_pos, yoji_item_pos );

        {
            yoji_bef_dir = yoji_dir;
            if( yoji_dir == 0 ) {
                yoji_dir = 1;
            } else {
                yoji_dir = 0;
            }

            yoji_table = yoji_data_pnt + yoji_item_pos * 10;
            /* set yoji */

            if( yoji_dir == 0 ) {
                make_yoji_bitmap1( yoji_table );
                for( i=0 ; i<4 ; i++ ) {
                  set_font_to_lcdimg( pos_template_x[i], pos_template_y[yoji_pos], (unsigned char *)yoji_buffer1[i], disp_color );
                  if( check_demo_mode() != 0 ) { return; }
                }
            } else {
                make_yoji_bitmap2( yoji_table );
                for( i=0 ; i<4 ; i++ ) {
                  set_font_to_lcdimg( pos_template_x[yoji_pos], pos_template_y[i], (unsigned char *)yoji_buffer2[i], disp_color );
                  if( check_demo_mode() != 0 ) { return; }
                }
            }

            delay( DEF_TIM_SET_DLY );
            if( check_demo_mode() != 0 ) { return; }

            if( yoji_bef_dir == 0 ) {
                for( i=0 ; i<DEF_YOJI_MOJI ; i++ ) {
                    if( i != yoji_pos ) {
                        clr_font_to_lcdimg( pos_template_x[i], pos_template_y[yoji_bef_pos], (unsigned char *)yoji_buffer1[i], disp_color_bak );
                    }
                    if( check_demo_mode() != 0 ) { return; }
                }
            } else {
                for( i=0 ; i<DEF_YOJI_MOJI ; i++ ) {
                    if( i != yoji_pos ) {
                        clr_font_to_lcdimg( pos_template_x[yoji_bef_pos], pos_template_y[i], (unsigned char *)yoji_buffer2[i], disp_color_bak );
                    }
                    if( check_demo_mode() != 0 ) { return; }
                }
            }

            yoji_item_pos_bef2 = yoji_item_pos_bef1;
            yoji_item_pos_bef1 = yoji_item_pos;
        }

        delay( DEF_TIM_YOJI_DLY );

        if( check_demo_mode() != 0 ) { return; }
    }
}


void  lcd_pixel_set( int x, int y, unsigned int color )
{
  tft.startWrite();
  tft.setAddrWindow( DEF_LCD_WIDTH-1-y-1, x, 1, 1 );
  tft.endWrite();

  tft.pushColor( color );
}


void  set_lcd_image( unsigned char *src, int sx, int sy, int width, int hight, unsigned int color )
{
    int  x, y;
    int  mx, my;

    mx = sx + width;
    my = sy + hight;

    for( y=sy ; y<my ; y++ ) {
        for( x=sx ; x<mx ; x++ ) {
            if( *src != 0 ) {
                lcd_pixel_set( x, y, color );
            }
            src++;
        }
    }
}


void  set_lcd_image_x2( unsigned char *src, int sx, int sy, int width, int hight, unsigned int color )
{
  int  x, y;
  int  mx, my;

  int  loop;
  unsigned int  r, g, b;
  unsigned char *tmp;

  mx = sx + width * 2;
  my = sy + hight * 2;

  tmp = src;
  for( loop=4 ; loop>=0 ; loop-- ) {
    r = ((color & 0xf800) >> loop) & 0xf800;
    g = ((color & 0x07e0) >> loop) & 0x07e0;
    b = ((color & 0x001f) >> loop) & 0x001f;
    for( y=sy ; y<my ; y+=2 ) {
        for( x=sx ; x<mx ; x+=2 ) {
            if( *src != 0 ) {
                y_fillRect( x, y, 2, 2, r|g|b );
            }
            src++;
        }
    }
    delay( DEF_TIM_NIJI_GRAD_DLY );
    src = tmp;
  }
}


void  clr_lcd_image_x2( unsigned char *src, int sx, int sy, int width, int hight, unsigned int color )
{
  int  x, y;
  int  mx, my;

  int  loop;
  unsigned int  r, g, b;
  unsigned char *tmp;

  mx = sx + width * 2;
  my = sy + hight * 2;

  tmp = src;
  for( loop=1 ; loop<=5 ; loop++ ) {
    r = ((color & 0xf800) >> loop) & 0xf800;
    g = ((color & 0x07e0) >> loop) & 0x07e0;
    b = ((color & 0x001f) >> loop) & 0x001f;
    if( loop == 5 ) {
      g = 0;
    }
    for( y=sy ; y<my ; y+=2 ) {
        for( x=sx ; x<mx ; x+=2 ) {
            if( *src != 0 ) {
                y_fillRect( x, y, 2, 2, r|g|b );
            }
            src++;
        }
    }
    delay( DEF_TIM_NIJI_GRAD_DLY );
    src = tmp;
  }
}


void  make_kanji_bitmap( unsigned char *kanji, unsigned char *out )
{
    unsigned int  code;
    unsigned char  *fontdata;

    code = (unsigned int)(*kanji);
    code = (code<<8) + *(kanji+1);

    fontdata = read_fontx2_k( DEF_FONT_K_VAR, code );
    set_font( fontdata, (uint8_t *)out, DEF_FONT_WIDTH );
}


int  niji_item_count( void )
{
    int  count = 0;
    unsigned char  *pnt = (unsigned char *)niji_data_pnt;

    while( *pnt != 0x00 ) {
        count++;
        pnt += 6; /* k1k1 k2k2 crlf */
    }

    return  count;
}


unsigned char  *get_random_code( void )
{
    unsigned char  *code;
    int  niji_pnt;
    int  sel_char;

    niji_pnt = rand() % x_item_all_cnt;
    sel_char = rand() & 0x1;  /* 0 or 1 */
    code = (unsigned char *)niji_data_pnt + niji_pnt * 6 + sel_char * 2;

    return  code;
}


void task2( void )
{
    unsigned char  *code_pnt;
    unsigned char  *niji_pnt;
    unsigned char  code1, code2;
    unsigned char  niji_ans[2];
    unsigned char  niji_ans_msg[5];  /* with nul */
    unsigned char  niji_ans_tmp;
    int  niji_a;
    int  niji_b;
    int  niji_c;
    int  niji_d;
    int  num;
    int  x, y;
    int  countdown;

    disp_color = 0;
    disp_color_bak = 0;

    digitalWrite( TFT_CTRL, HIGH );
    y_fillScreen(RGB565_BLACK);
    digitalWrite( TFT_CTRL, LOW );

#if 0
    memset( tmp_ank_buffer, 0x00, sizeof(tmp_ank_buffer) );
    set_font( read_fontx2_a( DEF_FONT_A_VAR, (unsigned int)('0'+x_demo_number) ), (unsigned char *)tmp_ank_buffer, DEF_FONT_WIDTH/2 );
    set_lcd_image( (unsigned char *)tmp_ank_buffer, 0, 0, DEF_FONT_WIDTH/2, DEF_FONT_HIGH, RGB565_BTOMATO );
#else
   {
    unsigned char  *title_pnt;
    int  i;

    title_pnt = (unsigned char *)title_niji;

    for( i=0 ; i<4 ; i++ ) {
        memset( tmp_buffer, 0x00, sizeof(tmp_buffer) );
        make_kanji_bitmap( title_pnt, (unsigned char  *)tmp_buffer );
        set_lcd_image( (unsigned char *)tmp_buffer, 0, 34*i, DEF_FONT_WIDTH, DEF_FONT_HIGH, RGB565_BLUE );
        title_pnt += 2;
    }
   }
#endif

    delay( DEF_TIM_CLR_DLY );

    niji_data_pnt = (unsigned char *)niji_data;
    niji_pnt = (unsigned char  *)niji_data_pnt;
    x_item_all_cnt = niji_item_count();

    /* disp arrow */
    set_lcd_image( (unsigned char *)kigou_right2,  64+ 2+DEF_DP_X_OFFSET,  88+24, 16, 16, RGB565_GREEN );
    set_lcd_image( (unsigned char *)kigou_down2,   88+24+DEF_DP_X_OFFSET,  64+ 2, 16, 16, RGB565_GREEN );
    set_lcd_image( (unsigned char *)kigou_right2, 152+ 6+DEF_DP_X_OFFSET,  88+24, 16, 16, RGB565_GREEN );
    set_lcd_image( (unsigned char *)kigou_down2,   88+24+DEF_DP_X_OFFSET, 152+ 6, 16, 16, RGB565_GREEN );

    /* disp center rect */
    for( x=84+DEF_DP_X_OFFSET ; x<84+DEF_DP_X_OFFSET+64+8 ; x++ ) {
        lcd_pixel_set( x, 84,      RGB565_RED );
        lcd_pixel_set( x, 84+1,    RGB565_RED );
        lcd_pixel_set( x, 152+2,   RGB565_RED );
        lcd_pixel_set( x, 152+2+1, RGB565_RED );
    }
    for( y=84 ; y<84+64+8 ; y++ ) {
        lcd_pixel_set( 84+DEF_DP_X_OFFSET,      y, RGB565_RED );
        lcd_pixel_set( 84+1+DEF_DP_X_OFFSET,    y, RGB565_RED );
        lcd_pixel_set( 152+2+DEF_DP_X_OFFSET,   y, RGB565_RED );
        lcd_pixel_set( 152+2+1+DEF_DP_X_OFFSET, y, RGB565_RED );
    }
    delay( DEF_TIM_CLR_DLY );

    while( true ) {
        do {
            /* get kanji code from niji-zyukugo */
            code_pnt = get_random_code();
            code1 = *code_pnt;
            code2 = *(code_pnt+1);

            niji_pnt = (unsigned char *)niji_data_pnt;
            x_item_1st_cnt = 0;
            x_item_2nd_cnt = 0;
            for( num=0 ; num<x_item_all_cnt ; num++ ) {
                if( (*niji_pnt == code1) && (*(niji_pnt+1) == code2) ) {
                    char_list_1st[x_item_1st_cnt] = num;
                    x_item_1st_cnt++;
                    if( x_item_1st_cnt >= DEF_NIJI_LST_MAX ) {
                        break;
                    }
                } else if( (*(niji_pnt+2) == code1) && (*(niji_pnt+3) == code2) ) {
                    char_list_2nd[x_item_2nd_cnt] = num;
                    x_item_2nd_cnt++;
                    if( x_item_2nd_cnt >= DEF_NIJI_LST_MAX ) {
                        break;
                    }
                }
                niji_pnt += 6;
            }
        } while ( x_item_1st_cnt < 2 || x_item_2nd_cnt < 2 );

        niji_ans[0] = code1;
        niji_ans[1] = code2;

        do {
            int  tmp1, tmp2;
            tmp1 = rand() % x_item_2nd_cnt;
            tmp2 = rand() % x_item_2nd_cnt;
            niji_a = char_list_2nd[ tmp1 ];
            niji_b = char_list_2nd[ tmp2 ];
        } while( niji_a == niji_b );

        do {
            int  tmp1, tmp2;
            tmp1 = rand() % x_item_1st_cnt;
            tmp2 = rand() % x_item_1st_cnt;
            niji_c = char_list_1st[ tmp1 ];
            niji_d = char_list_1st[ tmp2 ];
        } while( niji_c == niji_d );

        memset( yoji_buffer1, 0x00, sizeof(yoji_buffer1) );

        make_kanji_bitmap( (unsigned char *)(niji_data_pnt + niji_a*6),   (unsigned char  *)yoji_buffer1[0] );
        make_kanji_bitmap( (unsigned char *)(niji_data_pnt + niji_b*6),   (unsigned char  *)yoji_buffer1[1] );
        make_kanji_bitmap( (unsigned char *)(niji_data_pnt + niji_c*6+2), (unsigned char  *)yoji_buffer1[2] );
        make_kanji_bitmap( (unsigned char *)(niji_data_pnt + niji_d*6+2), (unsigned char  *)yoji_buffer1[3] );
        if( check_demo_mode() != 0 ) { return; }

        set_lcd_image_x2( (unsigned char *)yoji_buffer1[0],   0+DEF_DP_X_OFFSET,  88, DEF_FONT_WIDTH, DEF_FONT_HIGH, RGB565_ORANGE );
        if( check_demo_mode() != 0 ) { return; }
        set_lcd_image_x2( (unsigned char *)yoji_buffer1[1],  88+DEF_DP_X_OFFSET,   0, DEF_FONT_WIDTH, DEF_FONT_HIGH, RGB565_CYAN );
        if( check_demo_mode() != 0 ) { return; }
        set_lcd_image_x2( (unsigned char *)yoji_buffer1[2], 176+DEF_DP_X_OFFSET,  88, DEF_FONT_WIDTH, DEF_FONT_HIGH, RGB565_NEON );
        if( check_demo_mode() != 0 ) { return; }
        set_lcd_image_x2( (unsigned char *)yoji_buffer1[3],  88+DEF_DP_X_OFFSET, 176, DEF_FONT_WIDTH, DEF_FONT_HIGH, RGB565_YELLOW );
        if( check_demo_mode() != 0 ) { return; }

        /* disp "?" */
        memset( tmp_buffer, 0x00, sizeof(tmp_buffer) );
        make_kanji_bitmap( kigou_quest, (unsigned char  *)tmp_buffer );
        set_lcd_image_x2( (unsigned char *)tmp_buffer, 88+DEF_DP_X_OFFSET, 88, DEF_FONT_WIDTH, DEF_FONT_HIGH, RGB565_WHITE );

        for( countdown=5 ; countdown >= 0 ; countdown-- ) {
            y_fillRect( 300, 200, DEF_FONT_WIDTH/2, DEF_FONT_HIGH, RGB565_BLACK );
            memset( tmp_ank_buffer, 0x00, sizeof(tmp_ank_buffer) );
            set_font( read_fontx2_a( DEF_FONT_A_VAR, (unsigned int)('0'+countdown) ), (unsigned char *)tmp_ank_buffer, DEF_FONT_WIDTH/2 );
            set_lcd_image( (unsigned char *)tmp_ank_buffer, 300, 200, DEF_FONT_WIDTH/2, DEF_FONT_HIGH, RGB565_GREENYELLOW );
            delay( 1000 );  /* wait 1sec */

            if( check_demo_mode() != 0 ) { return; }
        }

        clr_lcd_image_x2( (unsigned char *)tmp_buffer, 88+DEF_DP_X_OFFSET, 88, DEF_FONT_WIDTH, DEF_FONT_HIGH, RGB565_BLACK );

        niji_ans[0] = code1;
        niji_ans[1] = code2;
        memset( tmp_buffer, 0x00, sizeof(tmp_buffer) );
        make_kanji_bitmap( niji_ans, (unsigned char  *)tmp_buffer );
        set_lcd_image_x2( (unsigned char *)tmp_buffer, 88+DEF_DP_X_OFFSET, 88, DEF_FONT_WIDTH, DEF_FONT_HIGH, RGB565_WHITE );

        delay( DEF_TIM_NEXT_DLY );

        clr_lcd_image_x2( (unsigned char *)yoji_buffer1[0],   0+DEF_DP_X_OFFSET,  88, DEF_FONT_WIDTH, DEF_FONT_HIGH, RGB565_ORANGE );
        if( check_demo_mode() != 0 ) { return; }
        clr_lcd_image_x2( (unsigned char *)yoji_buffer1[1],  88+DEF_DP_X_OFFSET,   0, DEF_FONT_WIDTH, DEF_FONT_HIGH, RGB565_CYAN );
        if( check_demo_mode() != 0 ) { return; }
        clr_lcd_image_x2( (unsigned char *)yoji_buffer1[2], 176+DEF_DP_X_OFFSET,  88, DEF_FONT_WIDTH, DEF_FONT_HIGH, RGB565_NEON );
        if( check_demo_mode() != 0 ) { return; }
        clr_lcd_image_x2( (unsigned char *)yoji_buffer1[3],  88+DEF_DP_X_OFFSET, 176, DEF_FONT_WIDTH, DEF_FONT_HIGH, RGB565_YELLOW );
        if( check_demo_mode() != 0 ) { return; }
        clr_lcd_image_x2( (unsigned char *)tmp_buffer, 88+DEF_DP_X_OFFSET, 88, DEF_FONT_WIDTH, DEF_FONT_HIGH, RGB565_WHITE );
        if( check_demo_mode() != 0 ) { return; }
        y_fillRect( 300, 200, DEF_FONT_WIDTH/2, DEF_FONT_HIGH, RGB565_BLACK );
        if( check_demo_mode() != 0 ) { return; }

        delay( DEF_TIM_CLR_DLY );
    }
}


/*************************************************
 *  Arduinoの setup関数
 *************************************************/
void setup( void )
{
  /* シリアルの初期化 */
  Serial.begin(115200);
  Serial.println( "call setup()" );

  lcd_char_x = 0;
  lcd_char_y = 0;
  lcd_output_flag = 0;
  lcd_output_color = 0;
  x_demo_number = 3;

  pinMode( TFT_CTRL, OUTPUT );
  digitalWrite( TFT_CTRL, HIGH );

  tft.init( DEF_LCD_WIDTH, DEF_LCD_HIGH );
  y_fillScreen(RGB565_BLACK);
  digitalWrite( TFT_CTRL, LOW );
}


/*************************************************
 *  Arduinoの loop関数
 *************************************************/
void loop( void )
{
  Serial.println( "call mainTask()" );
  srand( esp_random() );

  if( x_demo_number != 3 ) {
    task1();
  } else {
    task2();
  }
  delay( 100 );
}

