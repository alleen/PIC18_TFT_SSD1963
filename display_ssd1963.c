/*
 * File:   display_ssd1963.c
 * Author: Alleen Wang
 *
 * Created on 2014/12/23, PM 3:41
 */

#include <stdio.h>
#include <stdlib.h>
#include <htc.h>
#include <xc.h>
#include <string.h>

#include "common.h"

#define DISPLAY_TRIS_RS  TRISDbits.TRISD7
#define DISPLAY_TRIS_WR  TRISDbits.TRISD6
#define DISPLAY_TRIS_RD  TRISDbits.TRISD5
#define DISPLAY_TRIS_DB0 TRISDbits.TRISD4
#define DISPLAY_TRIS_DB1 TRISDbits.TRISD3
#define DISPLAY_TRIS_DB2 TRISDbits.TRISD2
#define DISPLAY_TRIS_DB3 TRISDbits.TRISD1
#define DISPLAY_TRIS_DB4 TRISDbits.TRISD0

//#define DISPLAY_TRIS_DB5 TRISEbits.TRISE7
#define DISPLAY_TRIS_DB5 TRISEbits.TRISE0

#define DISPLAY_TRIS_DB6 TRISEbits.TRISE6
#define DISPLAY_TRIS_DB7 TRISEbits.TRISE5
#define DISPLAY_TRIS_CS  TRISEbits.TRISE4
#define DISPLAY_TRIS_RST TRISEbits.TRISE3
#define DISPLAY_TRIS_BL  TRISEbits.TRISE2

#define DISPLAY_RS  LATDbits.LATD7
#define DISPLAY_WR  LATDbits.LATD6
#define DISPLAY_RD  LATDbits.LATD5

#define DISPLAY_DB0 LATDbits.LATD4
#define DISPLAY_DB1 LATDbits.LATD3
#define DISPLAY_DB2 LATDbits.LATD2
#define DISPLAY_DB3 LATDbits.LATD1
#define DISPLAY_DB4 LATDbits.LATD0

//#define DISPLAY_DB5 LATEbits.LATE7
#define DISPLAY_DB5 LATEbits.LATE0

#define DISPLAY_DB6 LATEbits.LATE6
#define DISPLAY_DB7 LATEbits.LATE5
#define DISPLAY_CS  LATEbits.LATE4
#define DISPLAY_RST LATEbits.LATE3
#define DISPLAY_BL  LATEbits.LATE2

#define DISPLAY_RS_DTAT_MODE 1
#define DISPLAY_RS_CONTROL_MODE 0

#define DataPort(x) \
            DISPLAY_DB0 = ( x >> 0) & 0b00000001; \
            DISPLAY_DB1 = ( x >> 1) & 0b00000001; \
            DISPLAY_DB2 = ( x >> 2) & 0b00000001; \
            DISPLAY_DB3 = ( x >> 3) & 0b00000001; \
            DISPLAY_DB4 = ( x >> 4) & 0b00000001; \
            DISPLAY_DB5 = ( x >> 5) & 0b00000001; \
            DISPLAY_DB6 = ( x >> 6) & 0b00000001; \
            DISPLAY_DB7 = ( x >> 7) & 0b00000001;

#define bit_test(x,n) (x & (0x01<<n))

short TFT_DISP_WIDTH, TFT_DISP_HEIGHT;
short X_Ini = 0;
short X_Fin = 0;
short Y_Ini = 0;
short Y_Fin = 0;

void TFT_Init_SSD1963_8bit(short display_width, short display_height)
{
    TFT_DISP_WIDTH = display_width;
    TFT_DISP_HEIGHT = display_height;
    X_Fin = display_width;
    Y_Fin = display_height;
    
    DISPLAY_TRIS_DB0 = 0;
    DISPLAY_TRIS_DB1 = 0;
    DISPLAY_TRIS_DB2 = 0;
    DISPLAY_TRIS_DB3 = 0;
    DISPLAY_TRIS_DB4 = 0;
    DISPLAY_TRIS_DB5 = 0;
    DISPLAY_TRIS_DB6 = 0;
    DISPLAY_TRIS_DB7 = 0;

    DISPLAY_TRIS_RS = 0;
    DISPLAY_TRIS_WR = 0;
    DISPLAY_TRIS_RD = 0;
    DISPLAY_TRIS_CS = 0;
    DISPLAY_TRIS_RST = 0;
    DISPLAY_TRIS_BL = 0;
    
    // Reset device
    TFT_Reset_SSD1963_8bit();
}

void TFT_Reset_SSD1963_8bit()
{
    unsigned int  HT = 525;
    unsigned int  HPS = 43;
    unsigned int  LPS = 1;
    unsigned char HPW = 42;

    unsigned int  VT = 288;
    unsigned int  VPS = 12;
    unsigned int  FPS = 1;
    unsigned char VPW = 11;
    
    DISPLAY_RST = 0;
    DISPLAY_TRIS_RST = 0;

    DISPLAY_RS = 1;
    DISPLAY_TRIS_RS = 0;

    DISPLAY_CS = 1;
    DISPLAY_TRIS_CS = 0;

    DISPLAY_TRIS_RD = 0;
    DISPLAY_TRIS_WR = 0;
    DISPLAY_RD = 1;
    DISPLAY_WR = 1;

    // Release from reset
    usleep(100);
    DISPLAY_RST = 1;
    msleep(200);

    TFT_SSD1963_8bit_Set_Index(0xe2);
    TFT_SSD1963YT_8bit_Write_Command(0x2c);
    TFT_SSD1963YT_8bit_Write_Command(0x02);
    TFT_SSD1963YT_8bit_Write_Command(0x04);

    TFT_SSD1963_8bit_Set_Index(0xe0);
    TFT_SSD1963YT_8bit_Write_Command(0x01);
    msleep(1);

    TFT_SSD1963_8bit_Set_Index(0xe0);
    TFT_SSD1963YT_8bit_Write_Command(0x03);
    msleep(5);

    TFT_SSD1963_8bit_Set_Index(0x01);
    msleep(10);

    //Set LSHIFT freq, i.e. the DCLK with PLL freq 120MHz set previously
    //Typical DCLK for is 33.3MHz(datasheet), experiment shows 30MHz gives a stable result
    //30MHz = 120MHz*(LCDC_FPR+1)/2^20
    //LCDC_FPR = 262143 (0x3FFFF)
    //Time per line = (DISP_HOR_RESOLUTION+DISP_HOR_PULSE_WIDTH+DISP_HOR_BACK_PORCH+DISP_HOR_FRONT_PORCH)/30 us = 1056/30 = 35.2us
    //9MHz = 120MHz*(LCDC_FPR+1)/2^20
    //LCDC_FPR = 78642 (0x13332)
    TFT_SSD1963_8bit_Set_Index(0xE6);
    TFT_SSD1963YT_8bit_Write_Command(0x01);//0x03
    TFT_SSD1963YT_8bit_Write_Command(0x33);
    TFT_SSD1963YT_8bit_Write_Command(0x32);
 
    //Set LCD mode, varies from individual manufacturer
    TFT_SSD1963_8bit_Set_Index(0xB0);
    TFT_SSD1963YT_8bit_Write_Command(0b00100000);//0x10    // set 24-bit
    TFT_SSD1963YT_8bit_Write_Command(0b10000000);    // set TTL mode
    TFT_SSD1963YT_8bit_Write_Command(((480-1)>>8) & 0x00ff);   // Set LCD panel size H
    TFT_SSD1963YT_8bit_Write_Command((480-1)&0X00FF);  // Set LCD panel size L
    TFT_SSD1963YT_8bit_Write_Command(((272-1)>>8) & 0x00ff);  // Set LCD panel size H
    TFT_SSD1963YT_8bit_Write_Command((272-1) & 0x00ff);  // Set LCD panel size L
    TFT_SSD1963YT_8bit_Write_Command(0x00);//0x00    // RGB sequence

    //Set horizontal period
    TFT_SSD1963_8bit_Set_Index(0xB4);
    TFT_SSD1963YT_8bit_Write_Command((HT>>8)&0X00FF);  //Set HT
    TFT_SSD1963YT_8bit_Write_Command(HT&0X00FF);
    TFT_SSD1963YT_8bit_Write_Command((HPS>>8)&0X00FF);  //Set HPS
    TFT_SSD1963YT_8bit_Write_Command(HPS&0X00FF);
    TFT_SSD1963YT_8bit_Write_Command(HPW);      //Set HPW
    TFT_SSD1963YT_8bit_Write_Command((LPS>>8)&0X00FF);  //Set HPS
    TFT_SSD1963YT_8bit_Write_Command(LPS&0X00FF);
    TFT_SSD1963YT_8bit_Write_Command(0x0000); 
 
    //Set vertical period
    TFT_SSD1963_8bit_Set_Index(0xB6);
    TFT_SSD1963YT_8bit_Write_Command((VT>>8)&0X00FF);   //Set VT
    TFT_SSD1963YT_8bit_Write_Command(VT&0X00FF);
    TFT_SSD1963YT_8bit_Write_Command((VPS>>8)&0X00FF);  //Set VPS
    TFT_SSD1963YT_8bit_Write_Command(VPS&0X00FF);
    TFT_SSD1963YT_8bit_Write_Command(VPW);      //Set VPW
    TFT_SSD1963YT_8bit_Write_Command((FPS>>8)&0X00FF);  //Set FPS
    TFT_SSD1963YT_8bit_Write_Command(FPS&0X00FF);

    TFT_SSD1963_8bit_Set_Index(0x0036); //rotation
    TFT_SSD1963YT_8bit_Write_Command(0x0000);
    TFT_SSD1963_8bit_Set_Index(0x3A); //Set pixel format, i.e. the bpp
    TFT_SSD1963YT_8bit_Write_Command(0x70);  //set 16bpp
 
    TFT_SSD1963_8bit_Set_Index(0xF0); //Set pixel data interface
    TFT_SSD1963YT_8bit_Write_Command(0x00); //
 
    //unsigned char contrast   = 0x80;
    //unsigned char brightness = 0x80;
    //unsigned char saturation = 0x80;
 
    //TFT_SSD1963_8bit_Set_Index(0xBC);
    //TFT_SSD1963YT_8bit_Write_Command(contrast);//????
    //TFT_SSD1963YT_8bit_Write_Command(brightness);//???
    //TFT_SSD1963YT_8bit_Write_Command(saturation);//????
    //TFT_SSD1963YT_8bit_Write_Command(0x01);//?????
    msleep(10);

    TFT_SSD1963_8bit_Set_Index(0x29); // Turn on display; show the image on display
 /*
    //set PWM
    TFT_SSD1963_8bit_Set_Index(0xBE); //
    TFT_SSD1963YT_8bit_Write_Command(0x0E); //PLL(120M)/(256*Value)/256=120Hz
    TFT_SSD1963YT_8bit_Write_Command(0xFF); //duty=X/256
    TFT_SSD1963YT_8bit_Write_Command(0x09); //DBC enable PWM enable
    TFT_SSD1963YT_8bit_Write_Command(0xFF); //DBC manual brightness
    TFT_SSD1963YT_8bit_Write_Command(0x00); //DBC minimum brightness
    TFT_SSD1963YT_8bit_Write_Command(0x00); //Brightness prescaler :off(system frequency / Divcode / 32768)
 
    TFT_SSD1963_8bit_Set_Index(0xD4); //
    //TH1 = display width * display height * 3 * 0.1 /16
    //480*272 * 3 * 0.1 /16 =990H
    TFT_SSD1963YT_8bit_Write_Command(0x00); //
    TFT_SSD1963YT_8bit_Write_Command(0x09); //
    TFT_SSD1963YT_8bit_Write_Command(0x90); //
    //TH2 = display width * display height * 3 * 0.25 /16
    //480*272 * 3 * 0.25 /16 =17E8H
    TFT_SSD1963YT_8bit_Write_Command(0x00); //
    TFT_SSD1963YT_8bit_Write_Command(0x17); //
    TFT_SSD1963YT_8bit_Write_Command(0xE8); //
    //TH3 = display width * display height * 3 * 0.6 /16
    //480*272 * 3 * 0.6 /16  =3960H
    TFT_SSD1963YT_8bit_Write_Command(0x00); //
    TFT_SSD1963YT_8bit_Write_Command(0x39); //
    TFT_SSD1963YT_8bit_Write_Command(0x60); //

    TFT_SSD1963_8bit_Set_Index(0xD0); //
    TFT_SSD1963YT_8bit_Write_Command(0x0D); //PLL(120M)/(256*Value)/256=120Hz
    //end of PWM
*/
    DISPLAY_BL = 1;

    msleep(100);
}

//char Is_TFT_Rotated_180()
//{
//  return TFT_Rotated_180;
//}

void TFT_SSD1963_8bit_Set_Index(short index)
{
   //usleep(1);
   //usleep(1);
   DISPLAY_CS = 0;
   DISPLAY_RD = 1;
   DISPLAY_RS = 0;
   DataPort(index);
   DISPLAY_WR = 0;
   DISPLAY_WR = 1;
   DISPLAY_CS = 1;
}

void TFT_SSD1963YT_8bit_Write_Command(char command)
 {
   DISPLAY_CS = 0;
   DISPLAY_RD = 1;
   DISPLAY_RS = 1;
   DataPort(command);
   DISPLAY_WR = 0;
   DISPLAY_WR = 1;
   DISPLAY_CS = 1;
   //usleep(1);
   //usleep(1);
}

void TFT_Convert_Color565(short Color565)
{
    char temp;
    short ColorPaint565;
    
    ColorPaint565 = Color565;
    
    temp = (ColorPaint565>>11);
    temp = (temp<<3);

    if ((temp>>7) == 1)
    {
        temp += 7;
    }

    TFT_SSD1963YT_8bit_Write_Command(temp);
    temp = (ColorPaint565>>5);
    temp = (temp<<2);
    
    if ((temp>>7) == 1)
    {
        temp += 3;
    }

    TFT_SSD1963YT_8bit_Write_Command(temp);
    temp = (ColorPaint565<<3);

    if ((temp>>7) == 1)
    {
        temp += 7;
    }

    TFT_SSD1963YT_8bit_Write_Command(temp);
}

void TFT_Set_Address_SSD1963II(short x1, short y1, short x2, short y2)
{
    short s_col, e_col, s_page, e_page;

    s_col = x1;
    e_col = x2;
    s_page = y1;
    e_page = y2;

    TFT_SSD1963_8bit_Set_Index(0x2a);             // SET column address
    TFT_SSD1963YT_8bit_Write_Command((char)(s_col >> 8));
    TFT_SSD1963YT_8bit_Write_Command(s_col);
    TFT_SSD1963YT_8bit_Write_Command((char)(e_col >> 8));
    TFT_SSD1963YT_8bit_Write_Command(e_col);

    TFT_SSD1963_8bit_Set_Index(0x2b);             // SET page address
    TFT_SSD1963YT_8bit_Write_Command((char)(s_page >> 8));
    TFT_SSD1963YT_8bit_Write_Command(s_page);
    TFT_SSD1963YT_8bit_Write_Command((char)(e_page >> 8));
    TFT_SSD1963YT_8bit_Write_Command(e_page);
    TFT_SSD1963_8bit_Set_Index(0x2C);
    TFT_SSD1963_8bit_Set_Index(0x3C);
}

void TFT_Set_Address_SSD1963(short x, short y)
{
    TFT_SSD1963_8bit_Set_Index(0x2a);           // SET column address
    TFT_SSD1963YT_8bit_Write_Command((x)>>8);     // SET start column address=0
    TFT_SSD1963YT_8bit_Write_Command(x);

    TFT_SSD1963YT_8bit_Write_Command((480-1)>>8);   // SET end column address=479 (799)
    TFT_SSD1963YT_8bit_Write_Command((480-1));


    TFT_SSD1963_8bit_Set_Index(0x2b);           // SET page address
    TFT_SSD1963YT_8bit_Write_Command((y)>>8);     // SET start page address=0
    TFT_SSD1963YT_8bit_Write_Command(y);

    TFT_SSD1963YT_8bit_Write_Command(272>>8);   // SET end page address=271
    TFT_SSD1963YT_8bit_Write_Command(272);

    TFT_SSD1963_8bit_Set_Index(0x2c);
}

void DataColor(char R, char G, char B)
{
    //TFT_SSD1963YT_8bit_Write_Command(R);    //RED
    //TFT_SSD1963YT_8bit_Write_Command(G);    //GREEN
    //TFT_SSD1963YT_8bit_Write_Command(B);    //BLUE

    DISPLAY_CS=1;
    DISPLAY_RD=1;
    DISPLAY_CS=0;
    DISPLAY_RS=1;

    DataPort(R);

    DISPLAY_WR=0;
    DISPLAY_WR=1;

    DataPort(G);

    DISPLAY_WR=0;
    DISPLAY_WR=1;

    DataPort(B);
    
    DISPLAY_WR=0;
    DISPLAY_WR=1;
    DISPLAY_CS=1;
}

void TFT_Fill(unsigned long Color)
{
    short i,j;
    unsigned char r=0,g=0,b=0;

    r = (Color>>16) & 0xff;
    g = (Color>>8) & 0xff;
    b = (Color) & 0xff;

    TFT_Set_Address_SSD1963II(X_Ini,Y_Ini,X_Fin,Y_Fin);

    for(i = 0; i <= X_Fin; i++)
    {
        for(j = 0; j <= Y_Fin; j++)
        {
            DataColor(r,g,b);
        }
    }
}

void TFT_FillRGB(unsigned char R, unsigned char G, unsigned char B)
{
    short i,j;
    TFT_Set_Address_SSD1963II(X_Ini,Y_Ini,X_Fin,Y_Fin);
    for(i = 0; i <= X_Fin; i++)
    {
        for(j = 0; j <= Y_Fin; j++)
        {
            DataColor(R,G,B);
        }
    }
}

void TFT_Pixel(short x, short y, unsigned long Color)
{
    unsigned char r=0,g=0,b=0;

    r = (Color>>16) & 0xff;
    g = (Color>>8) & 0xff;
    b = (Color) & 0xff;

    TFT_Set_Address_SSD1963II(x,y,x,y);
    DataColor(r,g,b);
}

void TFT_Line(short x1, short y1, short x2, short y2, unsigned long Color)
{
    short        dy = 0, dx = 0, i = 0;
    signed char  addx=1, addy=1;
    signed short P, diff;
    
    dx = abs(x2 - x1);
    dy = abs(y2 - y1);
    
    if(x1 > x2)
        addx = -1;
    if(y1 > y2)
        addy = -1;

    if(dx >= dy)
    {
        dy *= 2;
        P = dy - dx;
        diff = P - dx;

        for(; i<=dx; ++i)
        {
            TFT_Pixel(x1, y1, Color);
            if(P < 0)
            {
                P  += dy;
                x1 += addx;
            }
            else
            {
                P  += diff;
                x1 += addx;
                y1 += addy;
            }
        }
    }
    else
    {
        dx *= 2;
        P = dx - dy;
        diff = P - dy;
        
        for(; i<=dy; ++i)
        {
            TFT_Pixel(x1, y1, Color);

            if(P < 0)
            {
                P  += dx;
                y1 += addy;
            }
            else
            {
                P  += diff;
                x1 += addx;
                y1 += addy;
            }
        }
    }
}

void TFT_Rect(short x1, short y1, short x2, short y2, char fill, unsigned long Color)
{
    short i, xmin, xmax, ymin, ymax;
    
    if(fill)
    {
        
        if(x1 < x2)                            //  Find x min and max
        {
            xmin = x1;
            xmax = x2;
        }
        else
        {
            xmin = x2;
            xmax = x1;
        }
        
        if(y1 < y2)                            // Find the y min and max
        {
            ymin = y1;
            ymax = y2;
        }
        else
        {
            ymin = y2;
            ymax = y1;
        }

        for(; xmin <= xmax; ++xmin)
        {
            for(i=ymin; i<=ymax; ++i)
            {
                TFT_Pixel(xmin, i, Color);
            }
        }
    }
    else
    {
        TFT_Line(x1, y1, x2, y1, Color);      // Draw the 4 sides
        TFT_Line(x1, y2, x2, y2, Color);
        TFT_Line(x1, y1, x1, y2, Color);
        TFT_Line(x2, y1, x2, y2, Color);
    }
}

void TFT_Circle(short x, short y, short radius, char Fill, unsigned long Color)
{
    signed short CirA;
    signed short CirB;
    signed short CirP;

    CirA = 0;
    CirB = radius;
    CirP = 1 - radius;

    do
    {
        if(Fill)
        {
            TFT_Line(x-CirA, y+CirB, x+CirA, y+CirB, Color);
            TFT_Line(x-CirA, y-CirB, x+CirA, y-CirB, Color);
            TFT_Line(x-CirB, y+CirA, x+CirB, y+CirA, Color);
            TFT_Line(x-CirB, y-CirA, x+CirB, y-CirA, Color);
        }
        else
        {
            TFT_Pixel(CirA+x, CirB+y,Color);
            TFT_Pixel(CirB+x, CirA+y,Color);
            TFT_Pixel(x-CirA, CirB+y,Color);
            TFT_Pixel(x-CirB, CirA+y,Color);
            TFT_Pixel(CirB+x, y-CirA,Color);
            TFT_Pixel(CirA+x, y-CirB,Color);
            TFT_Pixel(x-CirA, y-CirB,Color);
            TFT_Pixel(x-CirB, y-CirA,Color);
        }

        if(CirP < 0)
            CirP += 3 + 2 * CirA++;
        else
            CirP += 5 + 2 * (CirA++ - CirB--);
    } while(CirA <= CirB);
}

void TFT_Text(short x, short y, char* textptr, char size, unsigned long Color)
{
    char j, k, l, m;                       // Loop counters
    char pixelData[5];                     // Stores character data

    for(; *textptr != '\0'; ++textptr, ++x)// Loop through the passed string
    {
        if(*textptr < 'S') // Checks if the letter is in the first font arra
        {
            memcpy(pixelData, FONT[*textptr - ' '], 5);
        }
        else if(*textptr <= '~') // Check if the letter is in the second font array
        {
            memcpy(pixelData, FONT2[*textptr - 'S'], 5);
        }
        else
        {
            memcpy(pixelData, FONT[0], 5);   // Default to space
        }

        switch(*textptr)
        {
            case '\n':
                y += 7*size + 1;
                continue;
                
            case '\r':
                x = 0;
                continue;
        }

        if(x+5*size >= TFT_DISP_WIDTH)          // Performs character wrapping
        {
            x = 0;                           // Set x at far left position
            y += 7*size + 1;                 // Set y at next position down
        }

        for(j=0; j<5; ++j, x+=size)         // Loop through character byte data
        {
            for(k=0; k < 7; ++k)             // Loop through the vertical pixels
            {
                if(bit_test(pixelData[j], k)) // Check if the pixel should be set
                {
                    for(l=0; l < size; ++l)    // These two loops change the
                    {                          // character's size
                        for(m=0; m < size; ++m)
                        {
                            TFT_Pixel(x+m, y+k*size+l, Color); // Draws the pixel
                        }
                    }
                }
            }
        }
    }
}
/*
void TFT_Image(int16 x, int16 y, int16 *img[][]) {
   int16 j,i;

   for(i = 0; i < sizeof(*img); i++) {
      for(j = 0; j < sizeof(*img[i]); j++) {
         TFT_Pixel(x+i,y+j,*img[j][i]);
      }
   }
}


void TFT_Imagen(int16 pos_x,int16 pos_y, int16 dimX, int16 Dimy, int16 *picture)
{
   int16 x, y, i;


    for(x=0;x<dimX;x++)
    {
    for(y=0;y<dimY;y++)
    {
    TFT_Set_Address_SSD1963II(x, y, x, y);
    TFT_SSD1963YT_8bit_Write_Command(picture++);
    }
    }
}

*/
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void TFT_Pixel565(short x, short y, short ColorPaint565)
{
    TFT_Set_Address_SSD1963II(x,y,x,y);
    TFT_Convert_Color565(ColorPaint565);
}
////////////////////////////////////////////////////////////////////////////////
