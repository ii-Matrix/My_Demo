/***********************************************
���к�֮            ��ѽô��ʼ��
***********************************************/
#include "common.h"
#include "include.h"
#include  "Init.h"
extern DMA_InitTypeDef DMA_InitStruct,DMA_InitStruct2;
extern uint8 TX_ADDRESS[5];   // ����һ����̬���͵�ַ
extern uint8 RX_ADDRESS[5];

/******************
���ã����еĳ�ʼ��
��������
���أ���
******************/
void InitAll()
{
  //  �����ʼ��
  ftm_pwm_init(FTM0, MotorCH2,10*1000,0);
  ftm_pwm_init(FTM0, MotorCH1,10*1000,0);         //10KHZ

  //�����ʼ��
  ftm_pwm_init(FTM2, FTM_CH0,300,Dj_Mid);         //300HZ

//  //LED��ʼ��
  led_init(LED_MAX);
  ////���뿪�س�ʼ��
  Boma_init();
  if(gpio_get (Boma_5) == 0)
    CarType = Front;              //����ó�Ϊǰ��
  else
    CarType = Later;              //����ó�Ϊ��
  if(gpio_get (Boma_4) == 0)
  {
    IMG_TX = OPEN;              //��ͼ
    for(uint8 i = 0;i<5;i++)
    {
      TX_ADDRESS[i] = 0x66;
      RX_ADDRESS[i] = 0x66;
    }
  }
  else
  {
    IMG_TX = OFF;              //����ͼ
     for(uint8 i = 0;i<5;i++)
    {
      TX_ADDRESS[i] = 0x22;
      RX_ADDRESS[i] = 0x22;
    }
  }
//  //Һ����ʼ�� 
  LCD_init();
  LCD_Printf(0,0,"Duoji,Motor...OK");
  LCD_Printf(0,15,"Boma...OK");
  //����У������ز�����ʼ��
  jiaozheng_init();
  //ad�ɼ���ʼ��
  adc_init (ADC0_SE18 );
 // Voltage = (adc_once(ADC0_SE18, ADC_16bit)*2.84*3300/((1<<16)-1));      //��ص�ѹ�ɼ�
 // LCD_Printf(0,30,"Voltage:%d",(int)Voltage);
  // flash��ʼ��
  flash_init();  
  //flash_erase_sector(SECTOR_NUM);
  //��ʼ��flash
//  flash_erase_sector(SECTOR_NUM-1);
//  flash_erase_sector(SECTOR_NUM-2);                     //��������
//  flash_erase_sector(SECTOR_NUM-3);
  Flash_Read();                                     //Ϊ������ֵ
  LCD_Printf(0,45,"flash...OK");
  Var_Init();
  //������ʼ��
  key_event_init(); 
  LCD_Printf(0,60,"Key...OK");    
  //SD����ʼ��
  //SD_Init();
  ////PIT��ʼ��,1ms                                                                                       
  pit_init_ms(PIT1, 1);                                
  set_vector_handler(PIT1_VECTORn ,PIT1_IRQHandler);     
  enable_irq (PIT1_IRQn); 

  
 //DMA���ڳ�ʼ��
   DMA_InitStruct.ch = DMA_CH1;                        //DMAͨ����,��16��
  DMA_UART_Send_Init(UART5,9600,&DMA_InitStruct);
    set_vector_handler(DMA1_VECTORn ,DMA1_IRQHandler);    //����DMA1���жϷ�����Ϊ dma_ch0_handler
    enable_irq (DMA1_IRQn);    
    set_vector_handler(UART5_RX_TX_VECTORn,uart5_handler);   // �����жϷ��������ж���������
    uart_rx_irq_en (UART5);                                 //�����ڽ����ж�

    DMA_InitStruct2.ch = DMA_CH2;                      //DMAͨ����,��16��
    DMA_UART_Send_Init(UART0,9600,&DMA_InitStruct2);
    set_vector_handler(DMA2_VECTORn ,DMA2_IRQHandler);    //����DMA1���жϷ�����Ϊ dma_ch0_handler
    enable_irq (DMA2_IRQn);
    set_vector_handler(UART0_RX_TX_VECTORn,uart0_handler);   // �����жϷ��������ж���������
    uart_rx_irq_en (UART0);   
    
  //���ٳ�ʼ��
  ftm_quad_init(FTM1);         //����     
  // gpio_init (PTA13, GPI,0);   
    LCD_Printf(0,75,"CeSu...OK");
  //����У��������ʼ�� 
     jiaozheng_init();
  //����ͷ��ʼ��
  camera_init(ImgNow);                                   //����ͷ��ʼ������ͼ��ɼ��� imgbuff ��ַ
  //�����жϷ�����
  set_vector_handler(PORTC_VECTORn ,PORTC_IRQHandler);    //���� PORTA ���жϷ�����Ϊ PORTA_IRQHandler
  set_vector_handler(DMA0_VECTORn ,DMA0_IRQHandler);      //���� DMA0 ���жϷ�����Ϊ DMA0_IRQHandler
  LCD_Printf(0,105,"CAMERA...OK"); 
    
    NVIC_SetPriorityGrouping(4);            //�������ȼ�����,4bit ��ռ���ȼ�,û�������ȼ�
    
    NVIC_SetPriority(PORTD_IRQn,0);
    NVIC_SetPriority(PORTC_IRQn,1);         
    NVIC_SetPriority(DMA0_IRQn,2); 
    NVIC_SetPriority(PIT1_IRQn,3);  
    NVIC_SetPriority(UART0_RX_TX_IRQn,4);
    NVIC_SetPriority(DMA2_IRQn,5);
    NVIC_SetPriority(UART5_RX_TX_IRQn,6);
    NVIC_SetPriority(DMA1_IRQn,7);    
  //��������ʼ��
  ftm_pwm_init(FTM3, FTM_CH1,4*1000,0);
 // Beep_Welcome();                  //���������������ʱ���ܣ�ʹҺ������ά��һ��ʱ��
  RoadScan_Init();                      //ɨ����Ļ�ṹ���ʼ��                    
  LCD_clear(RED);                                        //����
  CarState = CarStop;
  Sonic_Init();
  set_vector_handler(PORTD_VECTORn ,PORTD_IRQHandler);
  enable_irq(PORTD_IRQn); 
}
