#ifndef _OLDBOY_IRQ_H_
#define _OLDBOY_IRQ_H_
void PIT1_IRQHandler();         //定时器中断 
void PORTC_IRQHandler();        //摄像头场中断
void PORTE_IRQHandler();        //nrf中断
void DMA0_IRQHandler();         //摄像头DMA中断
void DMA1_IRQHandler();         //串口发送DMA中断
void DMA2_IRQHandler();         //双车串口DMA中断
extern void uart5_handler(void);
void uart0_handler();           //双车串口接收中断
void PORTD_IRQHandler();
#endif 

