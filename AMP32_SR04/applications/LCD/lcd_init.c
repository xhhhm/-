#include "lcd_init.h"
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include "drv_spi.h"


struct rt_spi_device *spi_dev_lcd;

void LCD_GPIO_Init(void)
{

//    rt_hw_spi_init();
//    rt_spi_take_bus(spi_dev_lcd);
////    spi_dev_lcd = (struct rt_spi_device *)rt_device_find("spi1");
//    if(spi_dev_lcd==RT_NULL)
//    {
//        rt_kprintf("spi1_device_not_find\r\n");
//    }
////    rt_hw_spi
//
////    rt_spi_bus_attach_device()
//
//    if (rt_spi_transfer(spi_dev_lcd, 'a', RT_NULL, 1) == 1)
//    {
//        ;
//    }
//    else
//    {
//        rt_kprintf("spi_transfer_err\r\n");
//    }
//    rt_device_init(spi_dev_lcd);
//    rt_spi_take_bus (device);

    rt_pin_mode(LCD_CS, PIN_MODE_OUTPUT);
//
//    rt_spi_bus_attach_device(spi_dev_lcd,"spi10","spi1",(void *) LCD_CS);

    rt_pin_mode(LCD_RES, PIN_MODE_OUTPUT);
    rt_pin_mode(LCD_DC, PIN_MODE_OUTPUT);

    rt_pin_mode(LCD_BLK, PIN_MODE_OUTPUT);

    rt_pin_mode(LCD_SCLK, PIN_MODE_OUTPUT);
    rt_pin_mode(LCD_MOSI, PIN_MODE_OUTPUT);
}


/******************************************************************************
      ����˵����LCD��������д�뺯��
      ������ݣ�dat  Ҫд��Ĵ�������
      ����ֵ��  ��
******************************************************************************/
void LCD_Writ_Bus(u8 dat) 
{
	u8 i;
	LCD_CS_Clr();
	for(i=0;i<8;i++)
	{
		LCD_SCLK_Clr();
		if(dat&0x80)
		{
		   LCD_MOSI_Set();
		}
		else
		{
		   LCD_MOSI_Clr();
		}
		LCD_SCLK_Set();
		dat<<=1;
	}
//   rt_spi_send(spi_dev_lcd,&dat,1);
  LCD_CS_Set();
}


/******************************************************************************
      ����˵����LCDд������
      ������ݣ�dat д�������
      ����ֵ��  ��
******************************************************************************/
void LCD_WR_DATA8(u8 dat)
{
	LCD_Writ_Bus(dat);
}


/******************************************************************************
      ����˵����LCDд������
      ������ݣ�dat д�������
      ����ֵ��  ��
******************************************************************************/
void LCD_WR_DATA(u16 dat)
{
	LCD_Writ_Bus(dat>>8);
	LCD_Writ_Bus(dat);
}


/******************************************************************************
      ����˵����LCDд������
      ������ݣ�dat д�������
      ����ֵ��  ��
******************************************************************************/
void LCD_WR_REG(u8 dat)
{
	LCD_DC_Clr();//д����
	LCD_Writ_Bus(dat);
	LCD_DC_Set();//д����
}


/******************************************************************************
      ����˵����������ʼ�ͽ�����ַ
      ������ݣ�x1,x2 �����е���ʼ�ͽ�����ַ
                y1,y2 �����е���ʼ�ͽ�����ַ
      ����ֵ��  ��
******************************************************************************/
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2)
{
	if(USE_HORIZONTAL==0)
	{
		LCD_WR_REG(0x2a);//�е�ַ����
		LCD_WR_DATA(x1+26);
		LCD_WR_DATA(x2+26);
		LCD_WR_REG(0x2b);//�е�ַ����
		LCD_WR_DATA(y1+1);
		LCD_WR_DATA(y2+1);
		LCD_WR_REG(0x2c);//������д
	}
	else if(USE_HORIZONTAL==1)
	{
		LCD_WR_REG(0x2a);//�е�ַ����
		LCD_WR_DATA(x1+26);
		LCD_WR_DATA(x2+26);
		LCD_WR_REG(0x2b);//�е�ַ����
		LCD_WR_DATA(y1+1);
		LCD_WR_DATA(y2+1);
		LCD_WR_REG(0x2c);//������д
	}
	else if(USE_HORIZONTAL==2)
	{
		LCD_WR_REG(0x2a);//�е�ַ����
		LCD_WR_DATA(x1+1);
		LCD_WR_DATA(x2+1);
		LCD_WR_REG(0x2b);//�е�ַ����
		LCD_WR_DATA(y1+26);
		LCD_WR_DATA(y2+26);
		LCD_WR_REG(0x2c);//������д
	}
	else
	{
		LCD_WR_REG(0x2a);//�е�ַ����
		LCD_WR_DATA(x1+1);
		LCD_WR_DATA(x2+1);
		LCD_WR_REG(0x2b);//�е�ַ����
		LCD_WR_DATA(y1+26);
		LCD_WR_DATA(y2+26);
		LCD_WR_REG(0x2c);//������д
	}
}

void LCD_Init(void)
{
	LCD_GPIO_Init();//��ʼ��GPIO


	LCD_RES_Clr();//��λ
	rt_thread_mdelay(50);
	LCD_RES_Set();
	rt_thread_mdelay(50);

	LCD_BLK_Set();//�򿪱���
	rt_thread_mdelay(50);

	LCD_WR_REG(0x11);     //Sleep out
	rt_thread_mdelay(50);                //Delay 120ms
	LCD_WR_REG(0xB1);     //Normal mode
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x3C);
	LCD_WR_REG(0xB2);     //Idle mode
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x3C);
	LCD_WR_REG(0xB3);     //Partial mode
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x3C);
	LCD_WR_REG(0xB4);     //Dot inversion
	LCD_WR_DATA8(0x03);
	LCD_WR_REG(0xC0);     //AVDD GVDD
	LCD_WR_DATA8(0xAB);
	LCD_WR_DATA8(0x0B);
	LCD_WR_DATA8(0x04);
	LCD_WR_REG(0xC1);     //VGH VGL
	LCD_WR_DATA8(0xC5);   //C0
	LCD_WR_REG(0xC2);     //Normal Mode
	LCD_WR_DATA8(0x0D);
	LCD_WR_DATA8(0x00);
	LCD_WR_REG(0xC3);     //Idle
	LCD_WR_DATA8(0x8D);
	LCD_WR_DATA8(0x6A);
	LCD_WR_REG(0xC4);     //Partial+Full
	LCD_WR_DATA8(0x8D);
	LCD_WR_DATA8(0xEE);
	LCD_WR_REG(0xC5);     //VCOM
	LCD_WR_DATA8(0x0F);
	LCD_WR_REG(0xE0);     //positive gamma
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x0E);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x02);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x0F);
	LCD_WR_DATA8(0x25);
	LCD_WR_DATA8(0x36);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x10);
	LCD_WR_REG(0xE1);     //negative gamma
	LCD_WR_DATA8(0x0A);
	LCD_WR_DATA8(0x0D);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x0F);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x02);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x0F);
	LCD_WR_DATA8(0x25);
	LCD_WR_DATA8(0x35);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x10);

	LCD_WR_REG(0xFC);
	LCD_WR_DATA8(0x80);

	LCD_WR_REG(0x3A);
	LCD_WR_DATA8(0x05);
	LCD_WR_REG(0x36);
	if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x08);
	else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC8);
	else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x78);
	else LCD_WR_DATA8(0xA8);
	LCD_WR_REG(0x21);     //Display inversion
	LCD_WR_REG(0x29);     //Display on
	LCD_WR_REG(0x2A);     //Set Column Address
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x1A);  //26
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x69);   //105
	LCD_WR_REG(0x2B);     //Set Page Address
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x01);    //1
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0xA0);    //160
	LCD_WR_REG(0x2C);
}








