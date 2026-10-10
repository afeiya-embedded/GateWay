#ifndef __XPT2046_H__
#define __XPT2046_H__

#include "stm32f4xx_hal.h"


extern uint8_t  CMD_RDX;
extern uint8_t  CMD_RDY;


//电阻屏芯片连接引脚	  


#define PEN  		HAL_GPIO_ReadPin(T_PEN_GPIO_Port,T_PEN_Pin)  		//T_PEN
#define DOUT 		HAL_GPIO_ReadPin(T_MISO_GPIO_Port,T_MISO_Pin) 		//T_MISO
#define TDIN(x) 	HAL_GPIO_WritePin(T_MOSI_GPIO_Port,T_MOSI_Pin,x)    //T_MOSI
#define TCLK(x) 	HAL_GPIO_WritePin(T_CLK_GPIO_Port,T_CLK_Pin,x)  	//T_CLK
#define TCS(x) 		HAL_GPIO_WritePin(T_CS_GPIO_Port,T_CS_Pin,x)  		//T_CS

//#define PEN  		PFin(11)  	//T_PEN
//#define DOUT 		PBin(2)   	//T_MISO
//#define TDIN 		PBout(1)  	//T_MOSI
//#define TCLK 		PAout(5)  	//T_CLK
//#define TCS  		PBout(0)  	//T_CS 
 
//触摸屏控制器参数	
extern	uint16_t Xdown; 		 
extern	uint16_t Ydown;	   //触摸屏被按下就返回的的坐标值
extern  uint16_t Xup;
extern  uint16_t Yup;       //触摸屏被按下之后抬起返回的的坐标值  

extern float xFactor;	 //触摸屏校准参数(电容屏不需要校准) 
extern float yFactor;
extern short xOffset;  
extern short yOffset;

   
//电阻屏函数
/*********电阻屏SPI通信与读取触摸点AD值*********************/
void SPI_Write_Byte(uint8_t num);				  //向控制芯片写入一个数据
uint16_t SPI_Read_AD(uint8_t CMD);							//读取AD转换值
uint16_t RTouch_Read_XorY(uint8_t xy);							//带滤波的坐标读取(X/Y)
uint8_t RTouch_Read_XY(uint16_t *x,uint16_t *y);				//双方向读取(X+Y)
uint8_t RTouch_Read_XY2(uint16_t *x,uint16_t *y);			//两次读取的双方向坐标读取

/*********电阻屏初始化函数*********************/
void XPT2046_Init(void);						 	//初始化

/*********电阻屏扫描按键函数*********************/
void XPT2046_Scan(uint8_t tp);	        //tp:0,屏幕坐标;1,物理坐标	


void Clear_Screen(void);
void Draw_Point(uint16_t x,uint16_t y,uint16_t color);
void R_Touch_test(void);
#endif

















