/*
*�ļ�����              DMA.c
*˵����                DMA���ڿ⺯��
*����оƬ�ͺţ�        MK60FX512VQL15
*����ʱ�䣺            2015-12-27
*���ߣ�                ��������ѧС����
*/
#include "common.h"
#include "include.h"
#include  "DMA.h"
#include "stdbool.h"
DMA_InitTypeDef DMA_InitStruct,DMA_InitStruct2;

extern UART_MemMapPtr UARTN[UART_MAX];
//���ں�ѡ��
static const uint32 UART_SendDMATriggerSourceTable[] = 
{
    DMA_UART0_Tx,
    DMA_UART1_Tx,
    DMA_UART2_Tx,
    DMA_UART3_Tx,
    DMA_UART4_Tx,
    DMA_UART5_Tx,
};
//���ڷ��ͼĴ�����ַ
static const void* UART_DataPortAddrTable[] = 
{
    (void*)&UART_D_REG(UART0_BASE_PTR),
    (void*)&UART_D_REG(UART1_BASE_PTR),
    (void*)&UART_D_REG(UART2_BASE_PTR),
    (void*)&UART_D_REG(UART3_BASE_PTR),
    (void*)&UART_D_REG(UART4_BASE_PTR),
    (void*)&UART_D_REG(UART5_BASE_PTR),   
   
};
/*************************************************
���ã�DMA���ڷ��ͳ�ʼ��
���������ں�
      ������
      DMA��ʼ���ṹ��
���أ���
**************************************************/
void DMA_UART_Send_Init(UARTn_e uartInstnace,uint32 baud,DMA_InitTypeDef *DMA_InitStruct)
{
  uart_init (uartInstnace, baud);                            //���ڳ�ʼ��
  UART_ITDMAConfig(uartInstnace, kUART_DMA_Tx, true);          //����������ΪDMAģʽ                   
  DMA_InitStruct->chlTriggerSource = UART_SendDMATriggerSourceTable[uartInstnace];//���ڴ����Ͻ���ôô�յĴ���
  DMA_InitStruct->triggerSourceMode = kDMA_TriggerSource_Normal;                //��ͨģʽ���������ڴ���ģʽ
  DMA_InitStruct->minorLoopByteCnt = 1;
  DMA_InitStruct->majorLoopCnt =  0;
  
  DMA_InitStruct->sAddr = (uint32)UART_SendDMATriggerSourceTable[uartInstnace];//Դ��ַ
  DMA_InitStruct->sLastAddrAdj = 0;                                           //������ѭ��ִ�����Դ��ַƫ����,��Ϊ��һֱ���ͣ�����Դ��ַҪƫ�ƻ�ȥ
  DMA_InitStruct->sAddrOffset = 1;                                            //ÿ�ζ�ȡԴ��ַ��ĵ�ַƫ��
  DMA_InitStruct->sDataWidth = kDMA_DataWidthBit_8;                           //ÿ�δ����λ��
  DMA_InitStruct->sMod = kDMA_ModuloDisable;                                  //��ֹModuloģʽ
  
  DMA_InitStruct->dAddr = (uint32)UART_DataPortAddrTable[uartInstnace];
  DMA_InitStruct->dLastAddrAdj = 0;
  DMA_InitStruct->dAddrOffset = 0;
  DMA_InitStruct->dDataWidth = kDMA_DataWidthBit_8;
  DMA_InitStruct->dMod = kDMA_ModuloDisable;
  DMA_InitStruct->cfg = DADDR_Recover;
  DMA_Init(DMA_InitStruct);
}
/*************************************************
���ã�DMA��ʼ���Ĵ�������
������DMA��ʼ���ṹ��
���أ���
**************************************************/
void DMA_Init(DMA_InitTypeDef *DMA_InitStruct)
{

    ASSERT(DMA_InitStruct->majorLoopCnt < 0x8000); //���ԣ����ֻ֧��0x7FFF

    //DMA �Ĵ��� ����

    /* ����ʱ�� */
    SIM_SCGC7 |= SIM_SCGC7_DMA_MASK;                        //��DMAģ��ʱ��

#if defined(MK60DZ10)
    SIM_SCGC6 |= SIM_SCGC6_DMAMUX_MASK;                     //��DMA��·������ʱ��
#elif defined(MK60F15)
    SIM_SCGC6 |= SIM_SCGC6_DMAMUX0_MASK;                    //��DMA��·������ʱ��
#endif

    /* ���� DMA ͨ�� �� ������ƿ� TCD ( Transfer Control Descriptor ) */
    DMA_SADDR(DMA_InitStruct->ch) =    (uint32)DMA_InitStruct->sAddr;                      // ����  Դ��ַ
    DMA_DADDR(DMA_InitStruct->ch) =    (uint32)DMA_InitStruct->dAddr;                      // ����Ŀ�ĵ�ַ
    DMA_SOFF(DMA_InitStruct->ch)  =    0x01u;                      // ����Դ��ַƫ�� 
    DMA_DOFF(DMA_InitStruct->ch)  =    DMA_InitStruct->dAddrOffset;                      // ÿ�δ����Ŀ�ĵ�ַ��ƫ����
    DMA_ATTR(DMA_InitStruct->ch)  =    (0
                         | DMA_ATTR_SMOD(DMA_InitStruct->sMod)               // Դ��ַģ����ֹ  Source address modulo feature is disabled
                         | DMA_ATTR_SSIZE(DMA_InitStruct->sDataWidth)            // Դ����λ��   ��     0 -> 8-bit 1 -> 16-bit , 2 -> 32-bit �� 4 -> 16-byte
                         | DMA_ATTR_DMOD(DMA_InitStruct->dMod)               // Ŀ���ַģ����ֹ
                         | DMA_ATTR_DSIZE(DMA_InitStruct->dDataWidth)            // Ŀ������λ��   ��  ����ͬ��
                        );

    DMA_CITER_ELINKNO(DMA_InitStruct->ch)  = DMA_CITER_ELINKNO_CITER(DMA_InitStruct->majorLoopCnt);   //��ǰ��ѭ������
    DMA_BITER_ELINKNO(DMA_InitStruct->ch)  = DMA_BITER_ELINKNO_BITER(DMA_InitStruct->majorLoopCnt);   //��ʼ��ѭ������

    DMA_CR &= ~DMA_CR_EMLM_MASK;                                // CR[EMLM] = 0

    //��CR[EMLM] = 0 ʱ:
    DMA_NBYTES_MLNO(DMA_InitStruct->ch) =   DMA_NBYTES_MLNO_NBYTES(DMA_InitStruct->minorLoopByteCnt); // ��ѭ��ÿ�δ����ֽ�����ע��ֵΪ0��ʾ����4GB */

    /* ���� DMA ���������Ĳ��� */
    DMA_SLAST(DMA_InitStruct->ch)      =   0;                              //����  Դ��ַ�ĸ���ֵ,��ѭ��������ָ�  Դ��ַ
    DMA_DLAST_SGA(DMA_InitStruct->ch)  =   (uint32)( (DMA_InitStruct->cfg & DADDR_KeepOn ) == 0 ? (-DMA_InitStruct->minorLoopByteCnt*DMA_InitStruct->majorLoopCnt)  : 0 ); //����Ŀ�ĵ�ַ�ĸ���ֵ,��ѭ��������ָ�Ŀ�ĵ�ַ���߱��ֵ�ַ
    DMA_CSR(DMA_InitStruct->ch)        =   (0
                             | DMA_CSR_BWC(3)               //�������,ÿ��һ�Σ�eDMA ����ֹͣ 8 �����ڣ�0��ֹͣ��1������2ֹͣ4���ڣ�3ֹͣ8���ڣ�
                             | DMA_CSR_DREQ_MASK            //��ѭ��������ֹͣӲ������
                             | DMA_CSR_INTMAJOR_MASK        //��ѭ������������ж�
                            );

    /* ���� DMA ����Դ */
#if defined(MK60DZ10)
    DMAMUX_CHCFG_REG(DMAMUX_BASE_PTR, DMA_InitStruct->ch) = (0
#elif defined(MK60F15)
    DMAMUX_CHCFG_REG(DMAMUX0_BASE_PTR, DMA_InitStruct->ch) = (0
#endif
            | DMAMUX_CHCFG_ENBL_MASK                        /* Enable routing of DMA request */
            //| DMAMUX_CHCFG_TRIG_MASK                        /* Trigger Mode: Periodic   PIT���ڴ�������ģʽ   ͨ��1��ӦPIT1������ʹ��PIT1����������Ӧ��PIT��ʱ���� */
            | DMAMUX_CHCFG_SOURCE( DMA_InitStruct->chlTriggerSource) /* ͨ����������Դ:     */
                                             );

    DMA_DIS(DMA_InitStruct->ch);                                    //��ֹͨ��DMAͨ�� Ӳ������
    DMA_IRQ_CLEAN(DMA_InitStruct->ch);                              //�������ͨ����־λ
}                                                        
/*************************************************
���ã�����DMAģ��ָ��ͨ����Ŀ���ַ
������DMAͨ����
      32λ��Ŀ�����ݵ�ַ
���أ���
**************************************************/
void DMA_SetSourceAddress(uint8 ch, uint32 address)
{
    DMA_SADDR(ch) = address;
}
                                               /**
/*************************************************
���ã�����DMA_SetMajorLoopCounter����ֵ
������DMAͨ����
      ֵ
���أ���
**************************************************/
void DMA_SetMajorLoopCounter(uint8 ch, uint32 val)
{
    DMA_CITER_ELINKNO(ch) &= ~DMA_CITER_ELINKNO_CITER_MASK;
    DMA_CITER_ELINKNO(ch) |= DMA_CITER_ELINKNO_CITER(val);
}
/*************************************************
���ã����ô���ΪDMAģʽ
���������ں�
      ����ģʽѡ��
      ����
���أ���
**************************************************/
void UART_ITDMAConfig(UARTn_e uartn, UART_ITDMAConfig_Type config, bool status)
{
   switch(config)
    {

        case kUART_DMA_Tx:
            (status)?
            (UART_C2_REG(UARTN[uartn]) |= UART_C2_TIE_MASK):
            (UART_C2_REG(UARTN[uartn]) &= ~UART_C2_TIE_MASK);
            (status)?
            (UART_C5_REG(UARTN[uartn]) |= UART_C5_TDMAS_MASK):
            (UART_C5_REG(UARTN[uartn]) &= ~UART_C5_TDMAS_MASK);
            break;
        case kUART_DMA_Rx:
            (status)?
            (UART_C2_REG(UARTN[uartn]) |= UART_C2_RIE_MASK):
            (UART_C2_REG(UARTN[uartn]) &= ~UART_C2_RIE_MASK);
            (status)?
            (UART_C5_REG(UARTN[uartn]) |= UART_C5_RDMAS_MASK):
            (UART_C5_REG(UARTN[uartn]) &= ~UART_C5_RDMAS_MASK);
            break;
        default:
            break;
    }
}
/*************************************************
���ã�DMA���ڷ���
������DMA��ʼ���ṹ��
      �����׵�ַ
      �������ݳ���
���أ���
**************************************************/
void UART_SendWithDMA(DMA_InitTypeDef *DMA_InitStruct, uint8 *buf,uint32 length)
{
    //û�д�����ɵĻ��Ͳ�ִ���������
  //  if(!DMA_IsMajorLoopComplete(DMA_InitStruct))
    {
      DMA_SetSourceAddress(DMA_InitStruct->ch, (uint32)buf);
      DMA_CITER_ELINKNO(DMA_InitStruct->ch)  = length;   //��ǰ��ѭ������
      DMA_BITER_ELINKNO(DMA_InitStruct->ch)  = length;   //��ʼ��ѭ������

    //��㱻ע�͵��ˣ�������Щ���⣬�ͳ��˵ķ�����ͬ����������һ�ַ�ʽʵ�֣�
    //Ҳ������DMA��ʼ����ֱ��������ѭ������������DMA�ж����dma_repeat������ѭ������
 //  DMA_SetMajorLoopCounter(DMA_InitStruct->ch, size);
    //����DMA����
      DMA_EN(DMA_InitStruct->ch);                        //ʹ��DMA Ӳ������
    }
}
/*************************************************
���ã����DMA�����Ƿ����
������DMA��ʼ���ṹ��
���أ���
**************************************************/                                                             
uint8 DMA_IsMajorLoopComplete(DMA_InitTypeDef *DMA_InitStruct)
{
    if(DMA_ERQ & (1 << DMA_InitStruct->ch))
    {
        if(DMA_CSR(DMA_InitStruct->ch) & DMA_CSR_DONE_MASK)
        {
            /* clear this bit */
            DMA_CDNE = DMA_CDNE_CDNE(DMA_InitStruct->ch);
            return 0;
        }
        else
        {
            return 1;
        }
    }
    /* this chl is idle, so return 0 and clear DONE bit anyway; */
    DMA_CDNE = DMA_CDNE_CDNE(DMA_InitStruct->ch);
    return 0;
}
/*************************************************
���ã�DMA �������ã�������Ϻ�һЩ������ı䣬��Ҫ���¸�ֵ
������DMA��ʼ���ṹ��
���أ���
**************************************************/
void DMA_UART_Repeat(DMA_InitTypeDef *DMA_InitStruct)
{
    DMA_IRQ_CLEAN(DMA_InitStruct->ch);   //���ͨ�������жϱ�־λ
    DMA_CITER_ELINKNO(DMA_InitStruct->ch)  = DMA_CITER_ELINKNO_CITER(DMA_InitStruct->majorLoopCnt);   //��ǰ��ѭ������
    DMA_BITER_ELINKNO(DMA_InitStruct->ch)  = DMA_BITER_ELINKNO_BITER(DMA_InitStruct->majorLoopCnt);   //��ʼ��ѭ������

    DMA_SADDR(DMA_InitStruct->ch) =    (uint32)DMA_InitStruct->sAddr;                      // ����  Դ��ַ
    DMA_DADDR(DMA_InitStruct->ch) =    (uint32)DMA_InitStruct->dAddr;                      // ����Ŀ�ĵ�ַ
}