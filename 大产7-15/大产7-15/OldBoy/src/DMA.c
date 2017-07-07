/*
*文件名：              DMA.c
*说明：                DMA串口库函数
*所用芯片型号：        MK60FX512VQL15
*创建时间：            2015-12-27
*作者：                河南理工大学小焓焓
*/
#include "common.h"
#include "include.h"
#include  "DMA.h"
#include "stdbool.h"
DMA_InitTypeDef DMA_InitStruct,DMA_InitStruct2;

extern UART_MemMapPtr UARTN[UART_MAX];
//串口号选择
static const uint32 UART_SendDMATriggerSourceTable[] = 
{
    DMA_UART0_Tx,
    DMA_UART1_Tx,
    DMA_UART2_Tx,
    DMA_UART3_Tx,
    DMA_UART4_Tx,
    DMA_UART5_Tx,
};
//串口发送寄存器地址
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
作用：DMA串口发送初始化
参数：串口号
      波特率
      DMA初始化结构体
返回：无
**************************************************/
void DMA_UART_Send_Init(UARTn_e uartInstnace,uint32 baud,DMA_InitTypeDef *DMA_InitStruct)
{
  uart_init (uartInstnace, baud);                            //串口初始化
  UART_ITDMAConfig(uartInstnace, kUART_DMA_Tx, true);          //将串口配置为DMA模式                   
  DMA_InitStruct->chlTriggerSource = UART_SendDMATriggerSourceTable[uartInstnace];//挂在串口上进行么么哒的传输
  DMA_InitStruct->triggerSourceMode = kDMA_TriggerSource_Normal;                //普通模式，不是周期触发模式
  DMA_InitStruct->minorLoopByteCnt = 1;
  DMA_InitStruct->majorLoopCnt =  0;
  
  DMA_InitStruct->sAddr = (uint32)UART_SendDMATriggerSourceTable[uartInstnace];//源地址
  DMA_InitStruct->sLastAddrAdj = 0;                                           //所以主循环执行完后，源地址偏移量,因为是一直发送，所以源地址要偏移回去
  DMA_InitStruct->sAddrOffset = 1;                                            //每次读取源地址后的地址偏移
  DMA_InitStruct->sDataWidth = kDMA_DataWidthBit_8;                           //每次传输的位宽
  DMA_InitStruct->sMod = kDMA_ModuloDisable;                                  //禁止Modulo模式
  
  DMA_InitStruct->dAddr = (uint32)UART_DataPortAddrTable[uartInstnace];
  DMA_InitStruct->dLastAddrAdj = 0;
  DMA_InitStruct->dAddrOffset = 0;
  DMA_InitStruct->dDataWidth = kDMA_DataWidthBit_8;
  DMA_InitStruct->dMod = kDMA_ModuloDisable;
  DMA_InitStruct->cfg = DADDR_Recover;
  DMA_Init(DMA_InitStruct);
}
/*************************************************
作用：DMA初始化寄存器配置
参数：DMA初始化结构体
返回：无
**************************************************/
void DMA_Init(DMA_InitTypeDef *DMA_InitStruct)
{

    ASSERT(DMA_InitStruct->majorLoopCnt < 0x8000); //断言，最大只支持0x7FFF

    //DMA 寄存器 配置

    /* 开启时钟 */
    SIM_SCGC7 |= SIM_SCGC7_DMA_MASK;                        //打开DMA模块时钟

#if defined(MK60DZ10)
    SIM_SCGC6 |= SIM_SCGC6_DMAMUX_MASK;                     //打开DMA多路复用器时钟
#elif defined(MK60F15)
    SIM_SCGC6 |= SIM_SCGC6_DMAMUX0_MASK;                    //打开DMA多路复用器时钟
#endif

    /* 配置 DMA 通道 的 传输控制块 TCD ( Transfer Control Descriptor ) */
    DMA_SADDR(DMA_InitStruct->ch) =    (uint32)DMA_InitStruct->sAddr;                      // 设置  源地址
    DMA_DADDR(DMA_InitStruct->ch) =    (uint32)DMA_InitStruct->dAddr;                      // 设置目的地址
    DMA_SOFF(DMA_InitStruct->ch)  =    0x01u;                      // 设置源地址偏移 
    DMA_DOFF(DMA_InitStruct->ch)  =    DMA_InitStruct->dAddrOffset;                      // 每次传输后，目的地址的偏移量
    DMA_ATTR(DMA_InitStruct->ch)  =    (0
                         | DMA_ATTR_SMOD(DMA_InitStruct->sMod)               // 源地址模数禁止  Source address modulo feature is disabled
                         | DMA_ATTR_SSIZE(DMA_InitStruct->sDataWidth)            // 源数据位宽   。     0 -> 8-bit 1 -> 16-bit , 2 -> 32-bit ， 4 -> 16-byte
                         | DMA_ATTR_DMOD(DMA_InitStruct->dMod)               // 目标地址模数禁止
                         | DMA_ATTR_DSIZE(DMA_InitStruct->dDataWidth)            // 目标数据位宽   。  设置同上
                        );

    DMA_CITER_ELINKNO(DMA_InitStruct->ch)  = DMA_CITER_ELINKNO_CITER(DMA_InitStruct->majorLoopCnt);   //当前主循环次数
    DMA_BITER_ELINKNO(DMA_InitStruct->ch)  = DMA_BITER_ELINKNO_BITER(DMA_InitStruct->majorLoopCnt);   //起始主循环次数

    DMA_CR &= ~DMA_CR_EMLM_MASK;                                // CR[EMLM] = 0

    //当CR[EMLM] = 0 时:
    DMA_NBYTES_MLNO(DMA_InitStruct->ch) =   DMA_NBYTES_MLNO_NBYTES(DMA_InitStruct->minorLoopByteCnt); // 子循环每次传输字节数，注：值为0表示传输4GB */

    /* 配置 DMA 传输结束后的操作 */
    DMA_SLAST(DMA_InitStruct->ch)      =   0;                              //调整  源地址的附加值,主循环结束后恢复  源地址
    DMA_DLAST_SGA(DMA_InitStruct->ch)  =   (uint32)( (DMA_InitStruct->cfg & DADDR_KeepOn ) == 0 ? (-DMA_InitStruct->minorLoopByteCnt*DMA_InitStruct->majorLoopCnt)  : 0 ); //调整目的地址的附加值,主循环结束后恢复目的地址或者保持地址
    DMA_CSR(DMA_InitStruct->ch)        =   (0
                             | DMA_CSR_BWC(3)               //带宽控制,每读一次，eDMA 引擎停止 8 个周期（0不停止；1保留；2停止4周期；3停止8周期）
                             | DMA_CSR_DREQ_MASK            //主循环结束后停止硬件请求
                             | DMA_CSR_INTMAJOR_MASK        //主循环结束后产生中断
                            );

    /* 配置 DMA 触发源 */
#if defined(MK60DZ10)
    DMAMUX_CHCFG_REG(DMAMUX_BASE_PTR, DMA_InitStruct->ch) = (0
#elif defined(MK60F15)
    DMAMUX_CHCFG_REG(DMAMUX0_BASE_PTR, DMA_InitStruct->ch) = (0
#endif
            | DMAMUX_CHCFG_ENBL_MASK                        /* Enable routing of DMA request */
            //| DMAMUX_CHCFG_TRIG_MASK                        /* Trigger Mode: Periodic   PIT周期触发传输模式   通道1对应PIT1，必须使能PIT1，且配置相应的PIT定时触发 */
            | DMAMUX_CHCFG_SOURCE( DMA_InitStruct->chlTriggerSource) /* 通道触发传输源:     */
                                             );

    DMA_DIS(DMA_InitStruct->ch);                                    //禁止通道DMA通道 硬件请求
    DMA_IRQ_CLEAN(DMA_InitStruct->ch);                              //清除传输通道标志位
}                                                        
/*************************************************
作用：设置DMA模块指定通道的目标地址
参数：DMA通道号
      32位的目标数据地址
返回：无
**************************************************/
void DMA_SetSourceAddress(uint8 ch, uint32 address)
{
    DMA_SADDR(ch) = address;
}
                                               /**
/*************************************************
作用：设置DMA_SetMajorLoopCounter计数值
参数：DMA通道号
      值
返回：无
**************************************************/
void DMA_SetMajorLoopCounter(uint8 ch, uint32 val)
{
    DMA_CITER_ELINKNO(ch) &= ~DMA_CITER_ELINKNO_CITER_MASK;
    DMA_CITER_ELINKNO(ch) |= DMA_CITER_ELINKNO_CITER(val);
}
/*************************************************
作用：配置串口为DMA模式
参数：串口号
      工作模式选择
      开关
返回：无
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
作用：DMA串口发送
参数：DMA初始化结构体
      发送首地址
      发送数据长度
返回：无
**************************************************/
void UART_SendWithDMA(DMA_InitTypeDef *DMA_InitStruct, uint8 *buf,uint32 length)
{
    //没有传输完成的话就不执行这个函数
  //  if(!DMA_IsMajorLoopComplete(DMA_InitStruct))
    {
      DMA_SetSourceAddress(DMA_InitStruct->ch, (uint32)buf);
      DMA_CITER_ELINKNO(DMA_InitStruct->ch)  = length;   //当前主循环次数
      DMA_BITER_ELINKNO(DMA_InitStruct->ch)  = length;   //起始主循环次数

    //这点被注释掉了，用着有些问题，和超核的方法不同，采用了另一种方式实现，
    //也就是在DMA初始化里直接设置主循环次数，并在DMA中断里的dma_repeat重置主循环次数
 //  DMA_SetMajorLoopCounter(DMA_InitStruct->ch, size);
    //启动DMA传输
      DMA_EN(DMA_InitStruct->ch);                        //使能DMA 硬件请求
    }
}
/*************************************************
作用：检测DMA传输是否完成
参数：DMA初始化结构体
返回：无
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
作用：DMA 重新配置，传输完毕后，一些参数会改变，需要重新赋值
参数：DMA初始化结构体
返回：无
**************************************************/
void DMA_UART_Repeat(DMA_InitTypeDef *DMA_InitStruct)
{
    DMA_IRQ_CLEAN(DMA_InitStruct->ch);   //清除通道传输中断标志位
    DMA_CITER_ELINKNO(DMA_InitStruct->ch)  = DMA_CITER_ELINKNO_CITER(DMA_InitStruct->majorLoopCnt);   //当前主循环次数
    DMA_BITER_ELINKNO(DMA_InitStruct->ch)  = DMA_BITER_ELINKNO_BITER(DMA_InitStruct->majorLoopCnt);   //起始主循环次数

    DMA_SADDR(DMA_InitStruct->ch) =    (uint32)DMA_InitStruct->sAddr;                      // 设置  源地址
    DMA_DADDR(DMA_InitStruct->ch) =    (uint32)DMA_InitStruct->dAddr;                      // 设置目的地址
}