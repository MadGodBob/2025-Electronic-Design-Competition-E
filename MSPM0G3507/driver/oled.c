#include "ti_msp_dl_config.h"
#include "string.h"
#include "oled.h"
#include "oledfont.h"
#include "ssd1306.h"
#include "system.h"

#define ABS(X)  (((X)>0)?(X):-(X))

#define XLevelL  	0x00
#define XLevelH  	0x10
#define XLevel    ((XLevelH&0x0F)*16+XLevelL)
#define Max_Column 	128
#define Max_Row  	64
#define Brightness 	0xCF
#define X_WIDTH 	128
#define Y_WIDTH 	64

#define  OLED_WORK_MODE_I2C//OLED使用I2C模式驱动，当采用SPI模式时，注释本行即可



/*********************************************************************************/
#define lcd_read_sda()  ((PORTB_PORT->DIN31_0 & PORTB_LCD_SDA_PIN)?:1,0)
#define lcd_sda_high()    PORTB_PORT->DOUTSET31_0 = PORTB_LCD_SDA_PIN
#define lcd_sda_low()     PORTB_PORT->DOUTCLR31_0 = PORTB_LCD_SDA_PIN
#define lcd_scl_high()    PORTB_PORT->DOUTSET31_0 = PORTB_LCD_SCL_PIN
#define lcd_scl_low()     PORTB_PORT->DOUTCLR31_0 = PORTB_LCD_SCL_PIN
#define lcd_dc_high()     PORTB_PORT->DOUTSET31_0 = PORTB_LCD_DC_PIN
#define lcd_dc_low()      PORTB_PORT->DOUTCLR31_0 = PORTB_LCD_DC_PIN

	
#ifdef OLED_WORK_MODE_I2C	

void bsp_analog_i2c_start(void);
void bsp_analog_i2c_stop(void);

#define analog_i2c_delay_cnt 1//5
static void analog_i2c_delay(void)
{
  volatile uint8_t i=0;
  for (i = 0; i < analog_i2c_delay_cnt; i++)	{;}
}

//#define analog_i2c_delay() {__NOP();__NOP();__NOP();__NOP();__NOP();}

void bsp_analog_i2c_init(void)
{
  delay_ms(10);
  bsp_analog_i2c_stop();
}




void i2c_sda_out(void)
{
  DL_GPIO_initDigitalOutput(PORTB_LCD_SDA_IOMUX);

  //DL_GPIO_setPins(USER_GPIO_PORT, OLED_PORTB_SDA_PIN);
  DL_GPIO_enableOutput(PORTB_PORT, PORTB_LCD_SDA_PIN);
}

void i2c_sda_in(void)
{
  //DL_GPIO_initDigitalInput(OLED_PORTB_SDA_IOMUX);

  DL_GPIO_initDigitalInputFeatures(PORTB_LCD_SDA_IOMUX, DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP, DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
}


void bsp_analog_i2c_start(void)
{
  /*    _____
   *SDA      \_____________
   *    __________
   *SCL           \________
   */
  i2c_sda_out();
  lcd_sda_high();
  lcd_scl_high();
  analog_i2c_delay();
  lcd_sda_low();
  analog_i2c_delay();
  lcd_scl_low();
  analog_i2c_delay();
}


void bsp_analog_i2c_stop(void)
{
  /*               _______
   *SDA __________/
   *          ____________
   *SCL _____/
   */
  i2c_sda_out();
  lcd_sda_low();
  lcd_scl_high();
  analog_i2c_delay();
  lcd_sda_high();
  analog_i2c_delay();
}


uint8_t bsp_analog_i2c_wait_ack(void)
{
  uint32_t timeout = 0;
  lcd_sda_high();
  i2c_sda_in();
  analog_i2c_delay();
  lcd_scl_high();
  analog_i2c_delay();

  while(lcd_read_sda())
  {
    timeout++;

    if(timeout > 100)//2000
    {
      bsp_analog_i2c_stop();
      return 1;
    }
  }

  lcd_scl_low();
  analog_i2c_delay();
  return 0;
}


void bsp_analog_i2c_ack(void)
{
  /*           ____
   *SCL ______/    \______
   *    ____         _____
   *SDA     \_______/
   */
  lcd_scl_low();
  i2c_sda_out();
  lcd_sda_high();
  analog_i2c_delay();
  lcd_sda_low();
  analog_i2c_delay();
  lcd_scl_high();
  analog_i2c_delay();
  lcd_scl_low();
  analog_i2c_delay();
  lcd_sda_high();
}



void bsp_analog_i2c_nack(void)
{
  /*           ____
   *SCL ______/    \______
   *    __________________
   *SDA
   */
  lcd_scl_low();
  i2c_sda_out();
  lcd_sda_high();
  analog_i2c_delay();
  lcd_scl_high();
  analog_i2c_delay();
  lcd_scl_low();
  analog_i2c_delay();
}


void bsp_analog_i2c_send_byte(uint8_t data)
{
  uint8_t i;
  i2c_sda_out();
  lcd_scl_low();

  for(i = 0; i < 8; i++)
  {
    if((data & 0x80) >> 7)
    {
      lcd_sda_high();
    }
    else
    {
      lcd_sda_low();
    }

    data <<= 1;
    analog_i2c_delay();
    lcd_scl_high();
    analog_i2c_delay();
    lcd_scl_low();
    analog_i2c_delay();
  }

  bsp_analog_i2c_wait_ack();
}


uint8_t bsp_analog_i2c_read_byte(void)
{
  uint8_t i, data = 0;

  for(i = 0; i < 8; i++ )
  {
    data <<= 1;
    lcd_scl_high();
    analog_i2c_delay();

    if(lcd_read_sda())
    {
      data++;
    }

    lcd_scl_low();
    analog_i2c_delay();
  }

  return data;
}

#endif

/*********************OLED???************************************/ 
void OLED_WrDat(unsigned char IIC_Data)
{
#ifdef OLED_WORK_MODE_I2C	
  bsp_analog_i2c_start();
  bsp_analog_i2c_send_byte(0x78);
  bsp_analog_i2c_send_byte(0x40);			//write data
  bsp_analog_i2c_send_byte(IIC_Data);
  bsp_analog_i2c_stop();
#else	
  unsigned char i=0;
  lcd_dc_high();
  for(i=0;i<8;i++)				//传送8位数据：时钟线拉低有效
  {
    lcd_scl_low();
    if(IIC_Data&0x80)	lcd_sda_high();
    else	lcd_sda_low();
    lcd_scl_high();
    IIC_Data<<=1;
  }	
#endif
}



/*********************OLED???************************************/
void OLED_WrCmd(unsigned char IIC_Command)
{
#ifdef OLED_WORK_MODE_I2C	
  bsp_analog_i2c_start();
  bsp_analog_i2c_send_byte(0x78);            //Slave address,SA0=0
  bsp_analog_i2c_send_byte(0x00);			//write command
  bsp_analog_i2c_send_byte(IIC_Command);
  bsp_analog_i2c_stop();
#else		
  unsigned char i=0;
  lcd_dc_low();
  for(i=0;i<8;i++)			//传送8位数据：时钟线拉低有效
  {
    lcd_scl_low();
    if(IIC_Command&0x80)	lcd_sda_high();
    else lcd_sda_low();
    lcd_scl_high();
    IIC_Command<<=1;
  }
  lcd_dc_high();
#endif
}


/***************************************
函数名:	void OLED_Set_Pos(unsigned char x, unsigned char y) 
说明:	OLED 设置坐标
入口:	unsigned char x-横轴坐标x,
			unsigned char y-纵轴坐标y
出口:	无
备注:	无
作者:	无名创新
***************************************/
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
  OLED_WrCmd(0xb0+y);
  OLED_WrCmd(((x&0xf0)>>4)|0x10);
  OLED_WrCmd((x&0x0f)|0x01);
} 

/***************************************
函数名:	void OLED_Fill(unsigned char bmp_dat)  
说明:	OLED全屏写入0或1
入口:	unsigned char bmp_dat-待写入数据
出口:	无
备注:	无
作者:	无名创新
***************************************/
void OLED_Fill(unsigned char bmp_dat) 
{
  unsigned char y,x;
  for(y=0;y<8;y++)
  {
    OLED_WrCmd(0xb0+y);
    OLED_WrCmd(0x01);
    OLED_WrCmd(0x10);
    for(x=0;x<X_WIDTH;x++)
      OLED_WrDat(bmp_dat);
  }
}

/***************************************
函数名:	void OLED_CLS(void)  
说明:	OLED复位
入口:	无
出口:	无
备注:	无
作者:	无名创新
***************************************/
void OLED_CLS(void)
{
  unsigned char y,x;
  for(y=0;y<8;y++)
  {
    OLED_WrCmd(0xb0+y);
    OLED_WrCmd(0x01);
    OLED_WrCmd(0x10);
    for(x=0;x<X_WIDTH;x++)
      OLED_WrDat(0);
  }
}

/***************************************
函数名:	void OLED_Init_I2C(void)  
说明:	OLED内部初始化
入口:	无
出口:	无
备注:	无
作者:	无名创新
***************************************/
void OLED_Init_I2C(void)
{
  //Delay_Ms(100);//初始化之前的延时很重要！
  OLED_WrCmd(0xae);//--turn off oled panel
  OLED_WrCmd(0x00);//---set low column address
  OLED_WrCmd(0x10);//---set high column address
  OLED_WrCmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
  OLED_WrCmd(0x81);//--set contrast control register
  OLED_WrCmd(Brightness); // Set SEG Output Current Brightness
  OLED_WrCmd(0xa0);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
  OLED_WrCmd(0xc0);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
  OLED_WrCmd(0xa6);//--set normal display
  OLED_WrCmd(0xa8);//--set multiplex ratio(1 to 64)
  OLED_WrCmd(0x3f);//--1/64 duty
  OLED_WrCmd(0xd3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
  OLED_WrCmd(0x00);//-not offset
  OLED_WrCmd(0xd5);//--set display clock divide ratio/oscillator frequency
  OLED_WrCmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
  OLED_WrCmd(0xd9);//--set pre-charge period
  OLED_WrCmd(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
  OLED_WrCmd(0xda);//--set com pins hardware configuration
  OLED_WrCmd(0x12);
  OLED_WrCmd(0xdb);//--set vcomh
  OLED_WrCmd(0x40);//Set VCOM Deselect Level
  OLED_WrCmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
  OLED_WrCmd(0x02);//
  OLED_WrCmd(0x8d);//--set Charge Pump enable/disable
  OLED_WrCmd(0x14);//--set(0x10) disable
  OLED_WrCmd(0xa4);// Disable Entire Display On (0xa4/0xa5)
  OLED_WrCmd(0xa6);// Disable Inverse Display On (0xa6/a7) 
  OLED_WrCmd(0xaf);//--turn on oled panel
  OLED_Fill(0x00); //初始清屏
  OLED_Set_Pos(0,0);
} 


/*----------------设置坐标------------------*/
void LCD_Set_Pos(unsigned char x, unsigned char y)
{
  OLED_WrCmd(0xb0+y);
  OLED_WrCmd(((x&0xf0)>>4)|0x10);
  OLED_WrCmd((x&0x0f)|0x01);
}
/*----------------全屏显示-----------------*/
void LCD_Fill(unsigned char bmp_dat)
{
  unsigned char y,x;
  for(y=0;y<8;y++)
  {
    OLED_WrCmd(0xb0+y);
    OLED_WrCmd(0x01);
    OLED_WrCmd(0x10);
    for(x=0;x<X_WIDTH;x++)
    {
      OLED_WrDat(bmp_dat);
    }
  }
}
/*---------------LCD复位-----------------*/
void LCD_CLS(void)
{
  unsigned char y,x;
  for(y=0;y<8;y++)
  {
    OLED_WrCmd(0xb0+y);
    OLED_WrCmd(0x01);
    OLED_WrCmd(0x10);
    for(x=0;x<X_WIDTH;x++)
      OLED_WrDat(0); 
  }
  
}
/*------显示6X8一组标准的ASCII字符串，显示坐标为（x，y）------*/
void LCD_P6x8Str(unsigned char x,unsigned char  y,unsigned char ch[])
{
  unsigned char c=0,i=0,j=0;
  while (ch[j]!='\0')
  {
    c =ch[j]-32;
    if(x>126){x=0;y++;}
    OLED_Set_Pos(x,y);
    for(i=0;i<6;i++)
      OLED_WrDat(F6x8[c][i]);
    x+=6;
    j++;
  }
}
//显示一个6X8的字符
void LCD_P6x8Char(unsigned char x,unsigned char  y,unsigned char ucData)
{
  unsigned char i, ucDataTmp;
  ucDataTmp = ucData-32;
  if(x > 126)
  {
    x= 0;
    y++;
  }
  OLED_Set_Pos(x, y);
  for(i = 0; i < 6; i++)
  {
    OLED_WrDat(F6x8[ucDataTmp][i]);
  }
}
/*--------------显示6X8的浮点数--------------*/
void write_6_8_number(unsigned char x,unsigned char y, float number)
{
  unsigned char i=0;
  unsigned char temp[16];
  unsigned char *point=temp;
  float decimal;
  int data;
  if(number<0)
  {
    temp[0]='-';
    LCD_P6x8Char(x,y,temp[0]);
    x+=6;
    number=-number;
  }
  data=(int)number;
  decimal=number-data;					//小数部分
  
  if(data>=1000000000)           //是否能被10^9整除
  {
    temp[i]=48+data/1000000000;
    data=data%1000000000;
    i++;
  }
  if(data>=100000000)           //是否能被10^8整除
  {
    temp[i]=48+data/100000000;
    data=data%100000000;
    i++;
  }
  else
    if(data<100000000&&i!=0)
    {
      temp[i]=0+48;
      i++;
    }
  if(data>=10000000)           //是否能被10^7整除
  {
    temp[i]=48+data/10000000;
    data=data%10000000;
    i++;
  }
  else
    if(data<10000000&&i!=0)
    {
      temp[i]=0+48;
      i++;
    }
  if(data>=1000000)           //是否能被10^6整除
  {
    temp[i]=48+data/1000000;
    data=data%1000000;
    i++;
  }
  else
    if(data<1000000&&i!=0)
    {
      temp[i]=0+48;
      i++;
    }
  if(data>=100000)           //是否能被100000整除
  {
    temp[i]=48+data/100000;
    data=data%100000;
    i++;
  }
  else
    if(data<100000&&i!=0)
    {
      temp[i]=0+48;
      i++;
    }
  if(data>=10000)           //是否能被10000整除
  {
    temp[i]=48+data/10000;
    data=data%10000;
    i++;
  }
  else
    if(data<10000&&i!=0)
    {
      temp[i]=0+48;
      i++;
    }
  if(data>=1000)           //是否能被1000整除
  {
    temp[i]=48+data/1000;
    data=data%1000;
    i++;
  }
  else
    if(data<1000&&i!=0)
    {
      temp[i]=0+48;
      i++;
    }
  if(data>=100)              //是否能被100整除
  {
    temp[i]=48+data/100;
    data=data%100;
    i++;
  }
  else
    if(data<100&&i!=0)
    {
      temp[i]=0+48;
      i++;
    }
  if(data>=10)                  //是否能被10整除
  {
    temp[i]=48+data/10;
    data=data%10;
    i++;
  }
  else
    if(data<10&&i!=0)
    {
      temp[i]=48;
      i++;
    }
  temp[i]=48+data;
  if(decimal>=0.0001f)           //判断是否有小数部分
  {
    i++;
    temp[i]='.';                //显示小数点
    i++;
    data=(int)(decimal*1000);
    temp[i]=48+data/100;
    data=data%100;
    i++;
    if(data>0)
    {
      temp[i]=48+data/10;
      data=data%10;
    }
    if(data>=0)
    {
      i++;
      temp[i]=data+48;
    }
  }
  i++;
  temp[i]='\0';
  LCD_P6x8Str(x,y,point);
}

/*--------------显示6X8的浮点数--------------*/
void write_6_8_number_f1(unsigned char x,unsigned char y, float number)
{
  unsigned char i=0;
  unsigned char temp[16];
  unsigned char *point=temp;
  float decimal;
  int data;
  if(number<0)
  {
    temp[0]='-';
    LCD_P6x8Char(x,y,temp[0]);
    x+=6;
    number=-number;
  }
  data=(int)number;
  decimal=number-data;					//小数部分
  
  if(data>=1000000000)           //是否能被10^9整除
  {
    temp[i]=48+data/1000000000;
    data=data%1000000000;
    i++;
  }
  if(data>=100000000)           //是否能被10^8整除
  {
    temp[i]=48+data/100000000;
    data=data%100000000;
    i++;
  }
  else
    if(data<100000000&&i!=0)
    {
      temp[i]=0+48;
      i++;
    }
  if(data>=10000000)           //是否能被10^7整除
  {
    temp[i]=48+data/10000000;
    data=data%10000000;
    i++;
  }
  else
    if(data<10000000&&i!=0)
    {
      temp[i]=0+48;
      i++;
    }
  if(data>=1000000)           //是否能被10^6整除
  {
    temp[i]=48+data/1000000;
    data=data%1000000;
    i++;
  }
  else
    if(data<1000000&&i!=0)
    {
      temp[i]=0+48;
      i++;
    }
  if(data>=100000)           //是否能被100000整除
  {
    temp[i]=48+data/100000;
    data=data%100000;
    i++;
  }
  else
    if(data<100000&&i!=0)
    {
      temp[i]=0+48;
      i++;
    }
  if(data>=10000)           //是否能被10000整除
  {
    temp[i]=48+data/10000;
    data=data%10000;
    i++;
  }
  else
    if(data<10000&&i!=0)
    {
      temp[i]=0+48;
      i++;
    }
  if(data>=1000)           //是否能被1000整除
  {
    temp[i]=48+data/1000;
    data=data%1000;
    i++;
  }
  else
    if(data<1000&&i!=0)
    {
      temp[i]=0+48;
      i++;
    }
  if(data>=100)              //是否能被100整除
  {
    temp[i]=48+data/100;
    data=data%100;
    i++;
  }
  else
    if(data<100&&i!=0)
    {
      temp[i]=0+48;
      i++;
    }
  if(data>=10)                  //是否能被10整除
  {
    temp[i]=48+data/10;
    data=data%10;
    i++;
  }
  else
    if(data<10&&i!=0)
    {
      temp[i]=48;
      i++;
    }
  temp[i]=48+data;
  if(decimal>=0.1f)           //判断是否有小数部分
  {
    i++;
    temp[i]='.';                //显示小数点
    i++;
    data=(int)(decimal*10);
    temp[i]=48+data;
  }
  i++;
  temp[i]='\0';
  LCD_P6x8Str(x,y,point);
}


/*------显示8X16一组标准的ASCII字符串，显示坐标为（x，y）------*/
void LCD_P8x16Str(unsigned char x,unsigned char  y,unsigned char ch[])
{
  unsigned char c=0,i=0,j=0;
  while (ch[j]!='\0')
  {
    c =ch[j]-32;
    if(x>120){x=0;y++;}
    OLED_Set_Pos(x,y);
    for(i=0;i<8;i++)
      OLED_WrDat(F8X16[c*16+i]);
    OLED_Set_Pos(x,y+1);
    for(i=0;i<8;i++)
      OLED_WrDat(F8X16[c*16+i+8]);  
    x+=8;
    j++;
  }
}
//显示一个8X16的字符
void LCD_P8x16Char(unsigned char x,unsigned char  y,unsigned char ch)
{
  unsigned char c=0,i=0,j=0;
  c =ch-32;
  if(x>120){x=0;y++;}
  OLED_Set_Pos(x,y);
  for(i=0;i<8;i++)
    OLED_WrDat(F8X16[c*16+i]);
  OLED_Set_Pos(x,y+1);
  for(i=0;i<8;i++)
    OLED_WrDat(F8X16[c*16+i+8]); 
  x+=8;
  j++;
}
/*---------------------显示8X16的浮点数--------------------*/
void write_8_16_number(unsigned char x,unsigned char y, float number)
{
  unsigned char i=0;
  unsigned char temp[16];
  unsigned char *point=temp;
  float decimal;
  int data;
  
  if(number<0)
  {
    temp[0]='-';
    LCD_P8x16Char(x,y,temp[0]);
    x+=1;
    number=-number;
  }
  data=(int)number;
  decimal=number-data;     //小数部分
  if(data>=1000)           //是否可被1000整除
  {
    temp[i]=48+data/1000;
    data=data%1000;
    i++;
  }
  if(data>=100)              //可否被100整除
  {
    temp[i]=48+data/100;
    data=data%100;
    i++;
  }
  else
    if(data<100&&i!=0)
    {
      temp[i]=0+48;
      i++;
    }
  if(data>=10)                  //可否被10整除
  {
    temp[i]=48+data/10;
    data=data%10;
    i++;
  }
  else
    if(data<10&&i!=0)
    {
      temp[i]=48;
      i++;
    }
  temp[i]=48+data;
  if(decimal>=0.0001f)           //判断是够为小数
  {
    i++;
    temp[i]='.';                //显示小数点
    i++;
    data=(int)(decimal*1000);
    temp[i]=48+data/100;
    data=data%100;
    i++;
    if(data>0)
    {
      temp[i]=48+data/10;
      data=data%10;
    }
    if(data>=0)
    {
      i++;
      temp[i]=data+48;
    }
  }
  i++;
  temp[i]='\0';
  LCD_P8x16Str(x,y,point);
}

//--------------------------------------------------------------
// Prototype      : void write_16_16_CN(unsigned char x, unsigned char y, unsigned char N)
// Calls          : 
// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); N:汉字在codetab.h中的索引
// Description    : 显示codetab.h中的汉字,16*16点阵
//--------------------------------------------------------------
void write_16_16_CN(unsigned char x, unsigned char y, unsigned char N)
{
  unsigned char wm=0;
  unsigned int  adder=32*N;
  OLED_Set_Pos(x , y);
  for(wm = 0;wm < 16;wm++)
  {
//    OLED_WrDat(F16x16[adder]);
    adder += 1;
  }
  OLED_Set_Pos(x,y + 1);
  for(wm = 0;wm < 16;wm++)
  {
//    OLED_WrDat(F16x16[adder]);
    adder += 1;
  } 		
}

void LCD_clear_L(unsigned char x,unsigned char y)
{
		OLED_WrCmd(0xb0+y);
		OLED_WrCmd(0x01);
		OLED_WrCmd(0x10);
		OLED_Set_Pos(x,y);
		for(;x<X_WIDTH;x++)
		{
			OLED_WrDat(0);
		}
}


void Draw_Logo(void)
{
  unsigned int ii=0;
  unsigned char x,y;
  for(y=0;y<8;y++)
  {
    OLED_Set_Pos(0,y);
    for(x=0;x<128;x++)
    {
      OLED_WrDat(NC_Logo[ii++]);
      Delay_Ms(1);
    }
  }
  Delay_Ms(200);
  LCD_CLS();
}



/***************************************
函数名:	void OLED_Init(void)
说明: OLED显示屏初始化
入口:	无
出口:	无
备注:	上电初始化，运行一次
作者:	无名创新
***************************************/
void OLED_Init(void)
{
	DL_GPIO_clearPins(PORTB_PORT, PORTB_LCD_CS_PIN);
	DL_GPIO_clearPins(PORTB_PORT, PORTB_LCD_RST_PIN);
	delay_ms(100);
  DL_GPIO_setPins(PORTB_PORT,   PORTB_LCD_RST_PIN);
	
	
  ssd1306_begin(SSD1306_SWITCHCAPVCC);   
 // Draw_Logo();//LOGO显示
  LCD_CLS();//清屏
}






void display_6_8_number(unsigned char x,unsigned char y, float number)
{
		write_6_8_number(x,y,number);
}

void display_6_8_number_pro(unsigned char x,unsigned char y, float number)
{
	 if(number>=0)	LCD_P6x8Char(x,y,'+');
	  else LCD_P6x8Char(x,y,'-');
		write_6_8_number(x+6,y,ABS(number));
}


void display_6_8_string(unsigned char x,unsigned char  y, char ch[])
{
		LCD_P6x8Str(x,y,(unsigned char *)ch);
}

