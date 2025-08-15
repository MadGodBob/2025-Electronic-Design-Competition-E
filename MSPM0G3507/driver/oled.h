#ifndef __OLED_H
#define __OLED_H






void OLED_WrCmd(unsigned char IIC_Data);
void OLED_WrDat(unsigned char IIC_Data);
void LCD_Set_Pos(unsigned char x, unsigned char y);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void LCD_P6x8Char(unsigned char x,unsigned char  y,unsigned char ch);
void LCD_P6x8Str(unsigned char x,unsigned char  y,unsigned char ch[]);
void write_6_8_number(unsigned char x,unsigned char  y,float number);
void LCD_P8x16Char(unsigned char x,unsigned char  y,unsigned char ch);
void LCD_P8x16Str(unsigned char x,unsigned char  y,unsigned char ch[]);
void write_8_16_number(unsigned char x,unsigned char  y,float number);
void LCD_clear_L(unsigned char x,unsigned char y);
void OLED_Init(void);
void Draw_Logo(void);
void LCD_CLS(void);
void write_6_8_number_f1(unsigned char x,unsigned char y, float number);


void display_6_8_number(unsigned char x,unsigned char y, float number);
void display_6_8_string(unsigned char x,unsigned char  y,char ch[]);
	
	
void display_6_8_number_pro(unsigned char x,unsigned char y, float number);
#endif



//#define lcd_read_sda()  ((DL_GPIO_readPins(PORTB_PORT,  PORTB_LCD_SDA_PIN))?:1,0)
//#define lcd_sda_high()  DL_GPIO_setPins(PORTB_PORT,   PORTB_LCD_SDA_PIN)
//#define lcd_sda_high()   DL_GPIO_clearPins(PORTB_PORT, PORTB_LCD_SDA_PIN)
//#define lcd_scl_high()  DL_GPIO_setPins(PORTA_PORT,   PORTA_LCD_SCL_PIN)
//#define lcd_scl_low()   DL_GPIO_clearPins(PORTA_PORT, PORTA_LCD_SCL_PIN)
//#define lcd_dc_high()   DL_GPIO_setPins(PORTB_PORT, PORTB_LCD_DC_PIN)
//#define lcd_dc_low()    DL_GPIO_clearPins(PORTB_PORT, PORTB_LCD_DC_PIN)



