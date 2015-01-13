/* 
 * File:   display_ssd1963.h
 * Author: Alleen Wang
 *
 * Created on 2014/12/23, PM 3:42
 */

#ifndef DISPLAY_SSD1963_H
#define	DISPLAY_SSD1963_H

#ifdef	__cplusplus
extern "C" {
#endif

void init_display_1963();

void TFT_Init_SSD1963_8bit(short display_width, short display_height);
void TFT_Reset_SSD1963_8bit();
void TFT_SSD1963_8bit_Set_Index(short index);
void TFT_SSD1963YT_8bit_Write_Command(char command);
void TFT_Convert_Color565(short Color565);
void TFT_Set_Address_SSD1963II(short x1, short y1, short x2, short y2);
void TFT_Set_Address_SSD1963(short x, short y);
void TFT_SSD1963YT_16bit_Write_Command(short command);
//void RGB888 (int PaintColor);
void DataColor(char R, char G, char B);

char Is_TFT_Rotated_180();
////////////////////////////////////////////////////////////////////////////////
void TFT_FillRGB(unsigned char R, unsigned char G, unsigned char B);
void TFT_Fill(unsigned long Color);
void TFT_Pixel(short x, short y, unsigned long Color);
void TFT_Line(short x1, short y1, short x2, short y2, unsigned long Color);
void TFT_Circle(short x, short y, short radius, char Fill, unsigned long Color);
void TFT_Rect(short x1, short y1, short x2, short y2, char fill, unsigned long Color);
void TFT_Text(short x, short y, char* textptr, char size, unsigned long Color);
void TFT_Pixel565(short x, short y, short ColorPaint565);

#ifdef	__cplusplus
}
#endif

#endif	/* DISPLAY_SSD1963_H */

