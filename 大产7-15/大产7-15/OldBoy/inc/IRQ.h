#ifndef _OLDBOY_IRQ_H_
#define _OLDBOY_IRQ_H_
void PIT1_IRQHandler();         //��ʱ���ж� 
void PORTC_IRQHandler();        //����ͷ���ж�
void PORTE_IRQHandler();        //nrf�ж�
void DMA0_IRQHandler();         //����ͷDMA�ж�
void DMA1_IRQHandler();         //���ڷ���DMA�ж�
void DMA2_IRQHandler();         //˫������DMA�ж�
extern void uart5_handler(void);
void uart0_handler();           //˫�����ڽ����ж�
void PORTD_IRQHandler();
#endif 

