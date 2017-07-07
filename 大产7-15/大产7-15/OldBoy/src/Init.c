/***********************************************
老男孩之            初呀么初始化
***********************************************/
#include "common.h"
#include "include.h"
#include  "Init.h"
extern DMA_InitTypeDef DMA_InitStruct,DMA_InitStruct2;
extern uint8 TX_ADDRESS[5];   // 定义一个静态发送地址
extern uint8 RX_ADDRESS[5];

/******************
作用：所有的初始化
参数：无
返回：无
******************/
void InitAll()
{
  //  电机初始化
  ftm_pwm_init(FTM0, MotorCH2,10*1000,0);
  ftm_pwm_init(FTM0, MotorCH1,10*1000,0);         //10KHZ

  //舵机初始化
  ftm_pwm_init(FTM2, FTM_CH0,300,Dj_Mid);         //300HZ

//  //LED初始化
  led_init(LED_MAX);
  ////拨码开关初始化
  Boma_init();
  if(gpio_get (Boma_5) == 0)
    CarType = Front;              //定义该车为前车
  else
    CarType = Later;              //定义该车为后车
  if(gpio_get (Boma_4) == 0)
  {
    IMG_TX = OPEN;              //传图
    for(uint8 i = 0;i<5;i++)
    {
      TX_ADDRESS[i] = 0x66;
      RX_ADDRESS[i] = 0x66;
    }
  }
  else
  {
    IMG_TX = OFF;              //不传图
     for(uint8 i = 0;i<5;i++)
    {
      TX_ADDRESS[i] = 0x22;
      RX_ADDRESS[i] = 0x22;
    }
  }
//  //液晶初始化 
  LCD_init();
  LCD_Printf(0,0,"Duoji,Motor...OK");
  LCD_Printf(0,15,"Boma...OK");
  //梯形校正表及相关参数初始化
  jiaozheng_init();
  //ad采集初始化
  adc_init (ADC0_SE18 );
 // Voltage = (adc_once(ADC0_SE18, ADC_16bit)*2.84*3300/((1<<16)-1));      //电池电压采集
 // LCD_Printf(0,30,"Voltage:%d",(int)Voltage);
  // flash初始化
  flash_init();  
  //flash_erase_sector(SECTOR_NUM);
  //初始化flash
//  flash_erase_sector(SECTOR_NUM-1);
//  flash_erase_sector(SECTOR_NUM-2);                     //擦除扇区
//  flash_erase_sector(SECTOR_NUM-3);
  Flash_Read();                                     //为参数赋值
  LCD_Printf(0,45,"flash...OK");
  Var_Init();
  //按键初始化
  key_event_init(); 
  LCD_Printf(0,60,"Key...OK");    
  //SD卡初始化
  //SD_Init();
  ////PIT初始化,1ms                                                                                       
  pit_init_ms(PIT1, 1);                                
  set_vector_handler(PIT1_VECTORn ,PIT1_IRQHandler);     
  enable_irq (PIT1_IRQn); 

  
 //DMA串口初始化
   DMA_InitStruct.ch = DMA_CH1;                        //DMA通道号,共16个
  DMA_UART_Send_Init(UART5,9600,&DMA_InitStruct);
    set_vector_handler(DMA1_VECTORn ,DMA1_IRQHandler);    //设置DMA1的中断服务函数为 dma_ch0_handler
    enable_irq (DMA1_IRQn);    
    set_vector_handler(UART5_RX_TX_VECTORn,uart5_handler);   // 设置中断服务函数到中断向量表里
    uart_rx_irq_en (UART5);                                 //开串口接收中断

    DMA_InitStruct2.ch = DMA_CH2;                      //DMA通道号,共16个
    DMA_UART_Send_Init(UART0,9600,&DMA_InitStruct2);
    set_vector_handler(DMA2_VECTORn ,DMA2_IRQHandler);    //设置DMA1的中断服务函数为 dma_ch0_handler
    enable_irq (DMA2_IRQn);
    set_vector_handler(UART0_RX_TX_VECTORn,uart0_handler);   // 设置中断服务函数到中断向量表里
    uart_rx_irq_en (UART0);   
    
  //测速初始化
  ftm_quad_init(FTM1);         //脉冲     
  // gpio_init (PTA13, GPI,0);   
    LCD_Printf(0,75,"CeSu...OK");
  //梯形校正参数初始化 
     jiaozheng_init();
  //摄像头初始化
  camera_init(ImgNow);                                   //摄像头初始化，把图像采集到 imgbuff 地址
  //配置中断服务函数
  set_vector_handler(PORTC_VECTORn ,PORTC_IRQHandler);    //设置 PORTA 的中断服务函数为 PORTA_IRQHandler
  set_vector_handler(DMA0_VECTORn ,DMA0_IRQHandler);      //设置 DMA0 的中断服务函数为 DMA0_IRQHandler
  LCD_Printf(0,105,"CAMERA...OK"); 
    
    NVIC_SetPriorityGrouping(4);            //设置优先级分组,4bit 抢占优先级,没有亚优先级
    
    NVIC_SetPriority(PORTD_IRQn,0);
    NVIC_SetPriority(PORTC_IRQn,1);         
    NVIC_SetPriority(DMA0_IRQn,2); 
    NVIC_SetPriority(PIT1_IRQn,3);  
    NVIC_SetPriority(UART0_RX_TX_IRQn,4);
    NVIC_SetPriority(DMA2_IRQn,5);
    NVIC_SetPriority(UART5_RX_TX_IRQn,6);
    NVIC_SetPriority(DMA1_IRQn,7);    
  //蜂鸣器初始化
  ftm_pwm_init(FTM3, FTM_CH1,4*1000,0);
 // Beep_Welcome();                  //开机声音，兼具延时功能，使液晶画面维持一定时间
  RoadScan_Init();                      //扫描屏幕结构体初始化                    
  LCD_clear(RED);                                        //清屏
  CarState = CarStop;
  Sonic_Init();
  set_vector_handler(PORTD_VECTORn ,PORTD_IRQHandler);
  enable_irq(PORTD_IRQn); 
}
