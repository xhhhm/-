#ifndef __LCD_INIT_H
#define __LCD_INIT_H

//#include <rtthread.h>
//#include <rtdevice.h>
#include <board.h>

#define USE_HORIZONTAL 2  //���ú�������������ʾ 0��1Ϊ���� 2��3Ϊ����


#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 80
#define LCD_H 160

#else
#define LCD_W 160
#define LCD_H 80
#endif

#define LCD_RES     GET_PIN(E, 7)
#define LCD_DC      GET_PIN(E, 8)
#define LCD_CS      GET_PIN(E, 9)
#define LCD_BLK     GET_PIN(E, 10)

#define LCD_SCLK    GET_PIN(E, 11)
#define LCD_MOSI    GET_PIN(E, 12)

//-----------------LCD�˿ڶ���---------------- 

#define LCD_SCLK_Clr() rt_pin_write(LCD_SCLK, PIN_LOW)    //SCL=SCLK  rt_pin_write(LED2_PIN, PIN_HIGH);
#define LCD_SCLK_Set() rt_pin_write(LCD_SCLK, PIN_HIGH)

#define LCD_MOSI_Clr() rt_pin_write(LCD_MOSI, PIN_LOW)    //SDA=MOSI
#define LCD_MOSI_Set() rt_pin_write(LCD_MOSI, PIN_HIGH)

#define LCD_RES_Clr()  rt_pin_write(LCD_RES, PIN_LOW)     //RES
#define LCD_RES_Set()  rt_pin_write(LCD_RES, PIN_HIGH)

#define LCD_DC_Clr()   rt_pin_write(LCD_DC, PIN_LOW)    //DC
#define LCD_DC_Set()   rt_pin_write(LCD_DC, PIN_HIGH)
 		     
#define LCD_CS_Clr()   rt_pin_write(LCD_CS, PIN_LOW)     //CS
#define LCD_CS_Set()   rt_pin_write(LCD_CS, PIN_HIGH)

#define LCD_BLK_Clr()  rt_pin_write(LCD_BLK, PIN_LOW)     //BLK
#define LCD_BLK_Set()  rt_pin_write(LCD_BLK, PIN_HIGH)




void LCD_GPIO_Init(void);//��ʼ��GPIO
void LCD_Writ_Bus(u8 dat);//ģ��SPIʱ��
void LCD_WR_DATA8(u8 dat);//д��һ���ֽ�
void LCD_WR_DATA(u16 dat);//д�������ֽ�
void LCD_WR_REG(u8 dat);//д��һ��ָ��
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2);//�������꺯��
void LCD_Init(void);//LCD��ʼ��
#endif




