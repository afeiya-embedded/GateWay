#include "lcd.h"
#include "stdlib.h"
#include "font.h"
#include "stdio.h"


#include <rtthread.h>
#include <board.h>

#define DBG_TAG "lcd"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>  // 头文件一定要在 #define DBG_LVL DBG_LOG 之后 ， 否则不输出


//LCD的画笔颜色和背景色
uint32_t POINT_COLOR=0xFF000000;		//画笔颜色
uint32_t BACK_COLOR =0xFFFFFFFF;  	//背景色

//管理LCD重要参数
//默认为竖屏
_lcd_dev lcddev;

//MCU频率为168MHz时的us延时
void delay_us(uint32_t nus)
{
    uint32_t Delay = nus * 168/4;
    do
    {
        __NOP();
    }
    while (Delay --);
}

//写寄存器函数
//regval:寄存器值
void LCD_WR_REG(volatile uint16_t regval)
{
    regval=regval;							//使用-O2优化的时候,必须插入的延时
    LCD->LCD_REG=regval;				//写入要写的寄存器序号
}

//写LCD数据
//data:要写入的值
void LCD_WR_DATA(volatile uint16_t data)
{
    data=data;									//使用-O2优化的时候,必须插入的延时
    LCD->LCD_RAM=data;
}

//读LCD数据
//返回值:读到的值
uint16_t LCD_RD_DATA(void)
{
    volatile uint16_t ram;			//防止被优化
    ram=LCD->LCD_RAM;
    return ram;
}

//写寄存器
//LCD_Reg:寄存器地址
//LCD_RegValue:要写入的数据
void LCD_WriteReg(uint16_t LCD_Reg,uint16_t LCD_RegValue)
{
    LCD->LCD_REG = LCD_Reg;			//写入要写的寄存器序号
    LCD->LCD_RAM = LCD_RegValue;//写入数据
}

//读寄存器
//LCD_Reg:寄存器地址
//返回值:读到的数据
uint16_t LCD_ReadReg(uint16_t LCD_Reg)
{
    LCD_WR_REG(LCD_Reg);				//写入要读的寄存器序号
    delay_us(5);
    return LCD_RD_DATA();				//返回读到的值
}

//开始写GRAM
void LCD_WriteRAM_Prepare(void)
{
    LCD->LCD_REG=lcddev.wramcmd;
}

//LCD写GRAM
//RGB_Code:颜色值
void LCD_WriteRAM(uint16_t RGB_Code)
{
    LCD->LCD_RAM = RGB_Code;		//写十六位GRAM
}

//从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式。
//通过该函数转换
//c:GBR格式的颜色值
//返回值：RGB格式的颜色值
uint16_t LCD_BGR2RGB(uint16_t c)
{
    uint16_t  r,g,b,rgb;
    b=(c>>0)&0x1f;
    g=(c>>5)&0x3f;
    r=(c>>11)&0x1f;
    rgb=(b<<11)+(g<<5)+(r<<0);
    return(rgb);
}

//当mdk -O1时间优化时需要设置
//延时i
void opt_delay(uint8_t i)
{
    while(i--);
}

//读取个某点的颜色值
//x,y:坐标
//返回值:此点的颜色
uint32_t LCD_ReadPoint(uint16_t x,uint16_t y)
{
    uint16_t r=0,g=0,b=0;
    if(x>=lcddev.width||y>=lcddev.height)return 0;			//超过了范围,直接返回
    LCD_SetCursor(x,y);
    if (lcddev.id == 0X5510)    												//5510 发送读GRAM指令
    {
        LCD_WR_REG(0X2E00);
    }
    else                        												//其他IC(9341/5310/1963/7789)发送读GRAM指令
    {
        LCD_WR_REG(0X2E);
    }
    r=LCD_RD_DATA();																		//dummy Read
    if (lcddev.id == 0X1963)    												//对1963来说,是真读
    {
        return r;               												//1963直接读就可以
    }
    opt_delay(2);
    r = LCD_RD_DATA();          												//实际坐标颜色

    //9341/5310/5510/7789 要分2次读出
    opt_delay(2);
    b = LCD_RD_DATA();
    g = r & 0XFF;               												//对于9341/5310/5510/7789, 第一次读取的是RG的值,R在前,G在后,各占8位
    g <<= 8;																						//9341/5310/5510/7789 需要公式转换一下
    return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11));
}

//LCD开启显示
void LCD_DisplayOn(void)
{
    if (lcddev.id == 0X5510)    //5510开启显示指令
    {
        LCD_WR_REG(0X2900);     //开启显示
    }
    else                        //9341/5310/1963/7789 等发送开启显示指令
    {
        LCD_WR_REG(0X29);       //开启显示
    }
}

//LCD关闭显示
void LCD_DisplayOff(void)
{
    if (lcddev.id == 0X5510)    //5510关闭显示指令
    {
        LCD_WR_REG(0X2800);     //关闭显示
    }
    else                        //9341/5310/1963/7789 等发送关闭显示指令
    {
        LCD_WR_REG(0X28);       //关闭显示
    }
}

//设置光标位置
//Xpos:横坐标
//Ypos:纵坐标
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
    if (lcddev.id == 0X1963)
    {
        if (lcddev.dir == 0)   //x坐标需要变换
        {
            Xpos = lcddev.width - 1 - Xpos;
            LCD_WR_REG(lcddev.setxcmd);
            LCD_WR_DATA(0);
            LCD_WR_DATA(0);
            LCD_WR_DATA(Xpos >> 8);
            LCD_WR_DATA(Xpos & 0XFF);
        }
        else
        {
            LCD_WR_REG(lcddev.setxcmd);
            LCD_WR_DATA(Xpos >> 8);
            LCD_WR_DATA(Xpos & 0XFF);
            LCD_WR_DATA((lcddev.width - 1) >> 8);
            LCD_WR_DATA((lcddev.width - 1) & 0XFF);
        }

        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(Ypos >> 8);
        LCD_WR_DATA(Ypos & 0XFF);
        LCD_WR_DATA((lcddev.height - 1) >> 8);
        LCD_WR_DATA((lcddev.height - 1) & 0XFF);

    }
    else if (lcddev.id == 0X5510)
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(Xpos >> 8);
        LCD_WR_REG(lcddev.setxcmd + 1);
        LCD_WR_DATA(Xpos & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(Ypos >> 8);
        LCD_WR_REG(lcddev.setycmd + 1);
        LCD_WR_DATA(Ypos & 0XFF);
    }
    else     //9341/5310/7789等设置坐标
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(Xpos >> 8);
        LCD_WR_DATA(Xpos & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(Ypos >> 8);
        LCD_WR_DATA(Ypos & 0XFF);
    }
}

//设置LCD的自动扫描方向
//dir:0~7,代表8个方向(具体定义见lcd.h)
//9341/5310/5510/1963/7789等IC已经实际测试
//注意:其他函数可能会受到此函数设置的影响(尤其是9341),
//所以,一般设置为L2R_U2D即可,如果设置为其他扫描方式,可能导致显示不正常.
void LCD_Scan_Dir(uint8_t dir)
{
    uint16_t regval=0;
    uint16_t dirreg=0;
    uint16_t temp;
    //横屏时，对1963不改变扫描方向, 其他IC改变扫描方向！竖屏时1963改变方向, 其他IC不改变扫描方向
    if ((lcddev.dir == 1 && lcddev.id != 0X1963) || (lcddev.dir == 0 && lcddev.id == 0X1963))
    {
        switch (dir)   //方向转换
        {
        case 0:
            dir = 6;
            break;

        case 1:
            dir = 7;
            break;

        case 2:
            dir = 4;
            break;

        case 3:
            dir = 5;
            break;

        case 4:
            dir = 1;
            break;

        case 5:
            dir = 0;
            break;

        case 6:
            dir = 3;
            break;

        case 7:
            dir = 2;
            break;
        }
    }

    switch (dir)
    {
    case L2R_U2D://从左到右,从上到下
        regval |= (0 << 7) | (0 << 6) | (0 << 5);
        break;

    case L2R_D2U://从左到右,从下到上
        regval |= (1 << 7) | (0 << 6) | (0 << 5);
        break;

    case R2L_U2D://从右到左,从上到下
        regval |= (0 << 7) | (1 << 6) | (0 << 5);
        break;

    case R2L_D2U://从右到左,从下到上
        regval |= (1 << 7) | (1 << 6) | (0 << 5);
        break;

    case U2D_L2R://从上到下,从左到右
        regval |= (0 << 7) | (0 << 6) | (1 << 5);
        break;

    case U2D_R2L://从上到下,从右到左
        regval |= (0 << 7) | (1 << 6) | (1 << 5);
        break;

    case D2U_L2R://从下到上,从左到右
        regval |= (1 << 7) | (0 << 6) | (1 << 5);
        break;

    case D2U_R2L://从下到上,从右到左
        regval |= (1 << 7) | (1 << 6) | (1 << 5);
        break;
    }

    if (lcddev.id == 0X5510)dirreg = 0X3600;
    else dirreg = 0X36;

    if (lcddev.id == 0X9341 || lcddev.id == 0X7789)   //9341 & 7789 要设置BGR位
    {
        regval |= 0X08;
    }

    LCD_WriteReg(dirreg, regval);

    if (lcddev.id != 0X1963)   												//1963不做坐标处理
    {
        if (regval & 0X20)
        {
            if (lcddev.width < lcddev.height)   			//交换X,Y
            {
                temp = lcddev.width;
                lcddev.width = lcddev.height;
                lcddev.height = temp;
            }
        }
        else
        {
            if (lcddev.width > lcddev.height)   			//交换X,Y
            {	
                temp = lcddev.width;
                lcddev.width = lcddev.height;
                lcddev.height = temp;
            }
        }
    }

    //设置显示区域(开窗)大小
    if (lcddev.id == 0X5510)
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(0);
        LCD_WR_REG(lcddev.setxcmd + 1);
        LCD_WR_DATA(0);
        LCD_WR_REG(lcddev.setxcmd + 2);
        LCD_WR_DATA((lcddev.width - 1) >> 8);
        LCD_WR_REG(lcddev.setxcmd + 3);
        LCD_WR_DATA((lcddev.width - 1) & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(0);
        LCD_WR_REG(lcddev.setycmd + 1);
        LCD_WR_DATA(0);
        LCD_WR_REG(lcddev.setycmd + 2);
        LCD_WR_DATA((lcddev.height - 1) >> 8);
        LCD_WR_REG(lcddev.setycmd + 3);
        LCD_WR_DATA((lcddev.height - 1) & 0XFF);
    }
    else
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(0);
        LCD_WR_DATA(0);
        LCD_WR_DATA((lcddev.width - 1) >> 8);
        LCD_WR_DATA((lcddev.width - 1) & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(0);
        LCD_WR_DATA(0);
        LCD_WR_DATA((lcddev.height - 1) >> 8);
        LCD_WR_DATA((lcddev.height - 1) & 0XFF);
    }
}

//画点
//x,y:坐标
//POINT_COLOR:此点的颜色
void LCD_DrawPoint(uint16_t x,uint16_t y)
{
    LCD_SetCursor(x,y);		//设置光标位置
    LCD_WriteRAM_Prepare();	//开始写入GRAM
    LCD->LCD_RAM=POINT_COLOR;
}

//快速画点
//x,y:坐标
//color:颜色
void LCD_Fast_DrawPoint(uint16_t x,uint16_t y,uint32_t color)
{
    if (lcddev.id == 0X5510)
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(x >> 8);
        LCD_WR_REG(lcddev.setxcmd + 1);
        LCD_WR_DATA(x & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(y >> 8);
        LCD_WR_REG(lcddev.setycmd + 1);
        LCD_WR_DATA(y & 0XFF);
    }
    else if (lcddev.id == 0X1963)
    {
        if (lcddev.dir == 0)x = lcddev.width - 1 - x;

        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(x >> 8);
        LCD_WR_DATA(x & 0XFF);
        LCD_WR_DATA(x >> 8);
        LCD_WR_DATA(x & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(y >> 8);
        LCD_WR_DATA(y & 0XFF);
        LCD_WR_DATA(y >> 8);
        LCD_WR_DATA(y & 0XFF);
    }
    else     															//9341/5310/7789等设置坐标
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(x >> 8);
        LCD_WR_DATA(x & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(y >> 8);
        LCD_WR_DATA(y & 0XFF);
    }

    LCD->LCD_REG=lcddev.wramcmd;
    LCD->LCD_RAM=color;
}

//SSD1963 背光设置
//pwm:背光等级,0~100.越大越亮.
void LCD_SSD_BackLightSet(uint8_t pwm)
{
    LCD_WR_REG(0xBE);												//配置PWM输出
    LCD_WR_DATA(0x05);											//1设置PWM频率
    LCD_WR_DATA(pwm*2.55);									//2设置PWM占空比
    LCD_WR_DATA(0x01);											//3设置C
    LCD_WR_DATA(0xFF);											//4设置D
    LCD_WR_DATA(0x00);											//5设置E
    LCD_WR_DATA(0x00);											//6设置F
}

//设置LCD显示方向
//dir:0,竖屏；1,横屏
void LCD_Display_Dir(uint8_t dir)
{
    lcddev.dir = dir;       								//竖屏/横屏

    if (dir == 0)           								//竖屏
    {
        lcddev.width = 240;
        lcddev.height = 320;

        if (lcddev.id == 0x5510)
        {
            lcddev.wramcmd = 0X2C00;
            lcddev.setxcmd = 0X2A00;
            lcddev.setycmd = 0X2B00;
            lcddev.width = 480;
            lcddev.height = 800;
        }
        else if (lcddev.id == 0X1963)
        {
            lcddev.wramcmd = 0X2C;  				//设置写入GRAM的指令
            lcddev.setxcmd = 0X2B;  				//设置写X坐标指令
            lcddev.setycmd = 0X2A;  				//设置写Y坐标指令
            lcddev.width = 480;     				//设置宽度480
            lcddev.height = 800;    				//设置高度800
        }
        else                        				//其他IC, 包括: 9341 / 5310 / 7789等IC
        {
            lcddev.wramcmd = 0X2C;
            lcddev.setxcmd = 0X2A;
            lcddev.setycmd = 0X2B;
        }

        if (lcddev.id == 0X5310)    				//如果是5310 则表示是 320*480分辨率
        {
            lcddev.width = 320;
            lcddev.height = 480;
        }
    }
    else     																//横屏
    {
        lcddev.width = 320;
        lcddev.height = 240;

        if (lcddev.id == 0x5510)
        {
            lcddev.wramcmd = 0X2C00;
            lcddev.setxcmd = 0X2A00;
            lcddev.setycmd = 0X2B00;
            lcddev.width = 800;
            lcddev.height = 480;
        }
        else if (lcddev.id == 0X1963)
        {
            lcddev.wramcmd = 0X2C;  				//设置写入GRAM的指令
            lcddev.setxcmd = 0X2A;  				//设置写X坐标指令
            lcddev.setycmd = 0X2B;  				//设置写Y坐标指令
            lcddev.width = 800;     				//设置宽度800
            lcddev.height = 480;    				//设置高度480
        }
        else                        				//其他IC, 包括: 9341 / 5310 / 7789等IC
        {
            lcddev.wramcmd = 0X2C;
            lcddev.setxcmd = 0X2A;
            lcddev.setycmd = 0X2B;
        }

        if (lcddev.id == 0X5310)    				//如果是5310 则表示是 320*480分辨率
        {
            lcddev.width = 480;
            lcddev.height = 320;
        }
    }

    LCD_Scan_Dir(DFT_SCAN_DIR);     				//默认扫描方向
}

//设置窗口,并自动设置画点坐标到窗口左上角(sx,sy).
//sx,sy:窗口起始坐标(左上角)
//width,height:窗口宽度和高度,必须大于0!!
//窗体大小:width*height.
void LCD_Set_Window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height)
{
    uint16_t twidth, theight;
    twidth = sx + width - 1;
    theight = sy + height - 1;

    if (lcddev.id == 0X1963 && lcddev.dir != 1)     //1963竖屏特殊处理
    {
        sx = lcddev.width - width - sx;
        height = sy + height - 1;
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(sx >> 8);
        LCD_WR_DATA(sx & 0XFF);
        LCD_WR_DATA((sx + width - 1) >> 8);
        LCD_WR_DATA((sx + width - 1) & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(sy >> 8);
        LCD_WR_DATA(sy & 0XFF);
        LCD_WR_DATA(height >> 8);
        LCD_WR_DATA(height & 0XFF);
    }
    else if (lcddev.id == 0X5510)
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(sx >> 8);
        LCD_WR_REG(lcddev.setxcmd + 1);
        LCD_WR_DATA(sx & 0XFF);
        LCD_WR_REG(lcddev.setxcmd + 2);
        LCD_WR_DATA(twidth >> 8);
        LCD_WR_REG(lcddev.setxcmd + 3);
        LCD_WR_DATA(twidth & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(sy >> 8);
        LCD_WR_REG(lcddev.setycmd + 1);
        LCD_WR_DATA(sy & 0XFF);
        LCD_WR_REG(lcddev.setycmd + 2);
        LCD_WR_DATA(theight >> 8);
        LCD_WR_REG(lcddev.setycmd + 3);
        LCD_WR_DATA(theight & 0XFF);
    }
    else     //9341/5310/7789/1963横屏 等 设置窗口
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(sx >> 8);
        LCD_WR_DATA(sx & 0XFF);
        LCD_WR_DATA(twidth >> 8);
        LCD_WR_DATA(twidth & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(sy >> 8);
        LCD_WR_DATA(sy & 0XFF);
        LCD_WR_DATA(theight >> 8);
        LCD_WR_DATA(theight & 0XFF);
    }
}



//清屏函数
//color:要清屏的填充色
void LCD_Clear(uint32_t color)
{
    uint32_t index=0;
    uint32_t totalpoint=lcddev.width;
    totalpoint*=lcddev.height;          //得到总点数
    LCD_SetCursor(0x00,0x0000);         //设置光标位置
    LCD_WriteRAM_Prepare();             //开始写入GRAM
    for(index=0; index<totalpoint; index++)
    {
        LCD->LCD_RAM=color;
    }
}


//初始化lcd
//该初始化函数可以初始化各种ILI93XX液晶,但是其他函数是基于ILI9320的!!!
//在其他型号的驱动芯片上没有测试!
void LCD_Init(void)
{
    //尝试9341 ID的读取
    LCD_WR_REG(0XD3);
    lcddev.id=LCD_RD_DATA();	//dummy read
    lcddev.id=LCD_RD_DATA();	//读到0X00
    lcddev.id=LCD_RD_DATA();   	//读取93
    lcddev.id<<=8;
    lcddev.id|=LCD_RD_DATA();  	//读取41
    LOG_I("LCD ID:%x",lcddev.id); //打印LCD ID
    if(lcddev.id==0X9341)	//9341初始化
    {
        LCD_WR_REG(0xCF);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xC1);
        LCD_WR_DATA(0X30);
        LCD_WR_REG(0xED);
        LCD_WR_DATA(0x64);
        LCD_WR_DATA(0x03);
        LCD_WR_DATA(0X12);
        LCD_WR_DATA(0X81);
        LCD_WR_REG(0xE8);
        LCD_WR_DATA(0x85);
        LCD_WR_DATA(0x10);
        LCD_WR_DATA(0x7A);
        LCD_WR_REG(0xCB);
        LCD_WR_DATA(0x39);
        LCD_WR_DATA(0x2C);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x34);
        LCD_WR_DATA(0x02);
        LCD_WR_REG(0xF7);
        LCD_WR_DATA(0x20);
        LCD_WR_REG(0xEA);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_REG(0xC0);    //Power control
        LCD_WR_DATA(0x1B);   //VRH[5:0]
        LCD_WR_REG(0xC1);    //Power control
        LCD_WR_DATA(0x01);   //SAP[2:0];BT[3:0]
        LCD_WR_REG(0xC5);    //VCM control
        LCD_WR_DATA(0x30); 	 //3F
        LCD_WR_DATA(0x30); 	 //3C
        LCD_WR_REG(0xC7);    //VCM control2
        LCD_WR_DATA(0XB7);
        LCD_WR_REG(0x36);    // Memory Access Control
        LCD_WR_DATA(0x48);
        LCD_WR_REG(0x3A);
        LCD_WR_DATA(0x55);
        LCD_WR_REG(0xB1);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x1A);
        LCD_WR_REG(0xB6);    // Display Function Control
        LCD_WR_DATA(0x0A);
        LCD_WR_DATA(0xA2);
        LCD_WR_REG(0xF2);    // 3Gamma Function Disable
        LCD_WR_DATA(0x00);
        LCD_WR_REG(0x26);    //Gamma curve selected
        LCD_WR_DATA(0x01);
        LCD_WR_REG(0xE0);    //Set Gamma
        LCD_WR_DATA(0x0F);
        LCD_WR_DATA(0x2A);
        LCD_WR_DATA(0x28);
        LCD_WR_DATA(0x08);
        LCD_WR_DATA(0x0E);
        LCD_WR_DATA(0x08);
        LCD_WR_DATA(0x54);
        LCD_WR_DATA(0XA9);
        LCD_WR_DATA(0x43);
        LCD_WR_DATA(0x0A);
        LCD_WR_DATA(0x0F);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_REG(0XE1);    //Set Gamma
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x15);
        LCD_WR_DATA(0x17);
        LCD_WR_DATA(0x07);
        LCD_WR_DATA(0x11);
        LCD_WR_DATA(0x06);
        LCD_WR_DATA(0x2B);
        LCD_WR_DATA(0x56);
        LCD_WR_DATA(0x3C);
        LCD_WR_DATA(0x05);
        LCD_WR_DATA(0x10);
        LCD_WR_DATA(0x0F);
        LCD_WR_DATA(0x3F);
        LCD_WR_DATA(0x3F);
        LCD_WR_DATA(0x0F);
        LCD_WR_REG(0x2B);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x01);
        LCD_WR_DATA(0x3f);
        LCD_WR_REG(0x2A);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xef);
        LCD_WR_REG(0x11); //Exit Sleep
        HAL_Delay(120);
        LCD_WR_REG(0x29); //display on
    }
    //初始化完成以后,提速
    if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X5510||lcddev.id==0X1963)//如果是这几个IC,则设置WR时序为最快
    {
        //重新配置写时序控制寄存器的时序
        FSMC_Bank1E->BWTR[6]&=~(0XF<<0);//地址建立时间(ADDSET)清零
        FSMC_Bank1E->BWTR[6]&=~(0XF<<8);//数据保存时间清零
        FSMC_Bank1E->BWTR[6]|=3<<0;		//地址建立时间(ADDSET)为3个HCLK =18ns
        FSMC_Bank1E->BWTR[6]|=2<<8; 	//数据保存时间(DATAST)为6ns*3个HCLK=18ns
    }
    LCD_Display_Dir(1);		//默认为竖屏
    HAL_GPIO_WritePin(LCD_BLK_GPIO_Port, LCD_BLK_Pin, GPIO_PIN_SET);						//点亮背光
    LCD_Clear(WHITE);
}


//在指定区域内填充指定颜色块
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
//color:要填充的颜色
void LCD_Color_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t *color)
{
    uint16_t height,width;
    uint16_t i,j;
    width=ex-sx+1; 			//得到填充的宽度
    height=ey-sy+1;			//高度
    for(i=0; i<height; i++)
    {
        LCD_SetCursor(sx,sy+i);   	//设置光标位置
        LCD_WriteRAM_Prepare();     //开始写入GRAM
        for (j = 0; j < width; j++)
        {
            LCD->LCD_RAM=color[i * width + j];  //写入数据
        }
    }
}

/* FSMC initialization function */
extern SRAM_HandleTypeDef hsram4;
static void FSMC_Init(void)
{

  /* USER CODE BEGIN FSMC_Init 0 */

  /* USER CODE END FSMC_Init 0 */

  FSMC_NORSRAM_TimingTypeDef Timing = {0};
  FSMC_NORSRAM_TimingTypeDef ExtTiming = {0};

  /* USER CODE BEGIN FSMC_Init 1 */

  /* USER CODE END FSMC_Init 1 */

  /** Perform the SRAM4 memory initialization sequence
  */
  hsram4.Instance = FSMC_NORSRAM_DEVICE;
  hsram4.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;
  /* hsram4.Init */
  hsram4.Init.NSBank = FSMC_NORSRAM_BANK4;
  hsram4.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;
  hsram4.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM;
  hsram4.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;
  hsram4.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;
  hsram4.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
  hsram4.Init.WrapMode = FSMC_WRAP_MODE_DISABLE;
  hsram4.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;
  hsram4.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;
  hsram4.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;
  hsram4.Init.ExtendedMode = FSMC_EXTENDED_MODE_ENABLE;
  hsram4.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
  hsram4.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;
  hsram4.Init.PageSize = FSMC_PAGE_SIZE_NONE;
  /* Timing */
  Timing.AddressSetupTime = 0;
  Timing.AddressHoldTime = 15;
  Timing.DataSetupTime = 4;
  Timing.BusTurnAroundDuration = 0;
  Timing.CLKDivision = 16;
  Timing.DataLatency = 17;
  Timing.AccessMode = FSMC_ACCESS_MODE_A;
  /* ExtTiming */
  ExtTiming.AddressSetupTime = 0;
  ExtTiming.AddressHoldTime = 15;
  ExtTiming.DataSetupTime = 4;
  ExtTiming.BusTurnAroundDuration = 0;
  ExtTiming.CLKDivision = 16;
  ExtTiming.DataLatency = 17;
  ExtTiming.AccessMode = FSMC_ACCESS_MODE_A;

  if (HAL_SRAM_Init(&hsram4, &Timing, &ExtTiming) != HAL_OK)
  {
    Error_Handler( );
  }

  /* USER CODE BEGIN FSMC_Init 2 */

  /* USER CODE END FSMC_Init 2 */
}


void LCD_BLK_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* USER CODE BEGIN MX_GPIO_Init_1 */

    /* USER CODE END MX_GPIO_Init_1 */

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(LCD_BLK_GPIO_Port, LCD_BLK_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin : LCD_BLK_Pin */
    GPIO_InitStruct.Pin = LCD_BLK_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LCD_BLK_GPIO_Port, &GPIO_InitStruct);

    /* USER CODE BEGIN MX_GPIO_Init_2 */

    /* USER CODE END MX_GPIO_Init_2 */
}


int LCD_Device_Init(void)
{
    LOG_I("LCD_Device_Init");
    FSMC_Init();
    LCD_BLK_Init();
    LCD_Init(); // lcd 初始化
    return RT_EOK ;
}
//INIT_APP_EXPORT(LCD_Device_Init);


