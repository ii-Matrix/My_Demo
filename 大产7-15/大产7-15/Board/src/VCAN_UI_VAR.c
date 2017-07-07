#include "common.h"
#include "VCAN_LCD.h"
#include "VCAN_UI_VAR.h"
#include "VCAN_NRF24L0_MSG.h"
#include "include.h"

#define VAR_VALUE(var_tab)      num_info[var_tab].val       //ָ����ŵı�����ֵ
#define VAR_OLDVALUE(var_tab)   num_info[var_tab].oldval    //ָ����ŵı��������ȷ��ֵ
#define VAR_MAXVALUE(var_tab)   num_info[var_tab].maxval
#define VAR_MINVALUE(var_tab)   num_info[var_tab].minval
#define VAR_SITE(var_tab)       num_info[var_tab].site


uint32 *var_addr[VAR_MAX] = {(uint32*)&VarReceive[0],(uint32 *)&VarReceive[1], (uint32 *)&VarReceive[2], (uint32 *)&VarReceive[3], (uint32 *)&VarReceive[4], (uint32 *)&VarReceive[5], (uint32 *)&VarReceive[6], (uint32 *)&VarReceive[7], (uint32 *)&VarReceive[8], (uint32 *)&VarReceive[9], (uint32 *)&VarReceive[10], (uint32 *)&VarReceive[11], (uint32 *)&VarReceive[12], (uint32 *)&VarReceive[13], (uint32 *)&VarReceive[14], (uint32 *)&VarReceive[15], (uint32 *)&VarReceive[16], (uint32 *)&VarReceive[17], (uint32 *)&VarReceive[18], (uint32 *)&VarReceive[19], (uint32 *)&VarReceive[20], (uint32 *)&VarReceive[21], (uint32 *)&VarReceive[22], (uint32 *)&VarReceive[23]};

ui_var_info_t  num_info[VAR_MAX] =
{
    //  {val,oldval,minval,maxval,{x,y}}
    //val,oldval,���ڵ���key_event_init��ʱ������Ӧ�����︳ֵ���������������ֵ��������д
    //��Ҫ����minval,maxval,{x,y}
    //���ע����Сֵ��Ҫ�������ֵ
    {0, 0, 0, 255, {88,0}},      
    {0, 0, 0, 65535, {88, 16}},                             
    {0, 0, 0, 65535, {88, 32}},              
    {0, 0, 0, 65535, {88, 48}},               
    {0, 0, 0, 65535, {88, 64}},             
    {0, 0, 0, 65535, {88, 80}},
    {0, 0, 0, 65535, {88, 96}},
    {0, 0, 0, 65535, {88, 112}},
    
    {0, 0, 0, 65535, {88,0}},      
    {0, 0, 0, 65535, {88, 16}},                             
    {0, 0, 0, 65535, {88, 32}},              
    {0, 0, 0, 65535, {88, 48}},               
    {0, 0, 0, 65535, {88, 64}},             
    {0, 0, 0, 65535, {88, 80}},
    {0, 0, 0, 65535, {88, 96}},
    {0, 0, 0, 65535, {88, 112}}, 
    
     {0, 0, 0, 65535, {88,0}},      
    {0, 0, 0, 65535, {88, 16}},                             
    {0, 0, 0, 65535, {88, 32}},              
    {0, 0, 0, 65535, {88, 48}},               
    {0, 0, 0, 65535, {88, 64}},             
    {0, 0, 0, 65535, {88, 80}},
    {0, 0, 0, 65535, {88, 96}},
    {0, 0, 0, 65535, {88, 112}}, 
};

uint8   new_tab = 0;        //��ǰѡ��ı������
uint32  last_tab;           //�����յ��ı������



//ͬ��ָ����ֵ��tab Ϊ VAR_NUM ʱ��ʾȫ��ͬ����С����ͬ����Ӧ��
//������ͬ������ʾȫ������Ϊ�п���ͬ��ʧ�ܡ�
//static uint8 var_syn(uint8 tab);         //ͬ�����ݣ�1��ʾ�ɹ���0��ʾʧ��

void save_var2buff(var_tab_e var_num, uint8 *sendbuf);              //����Ҫ���͵ı���������д�뵽������

void var_init()
{
    uint8   var_num;
    uint32  vartemp;

    for(var_num = 0; var_num < VAR_MAX; var_num++)
    {
        get_var((var_tab_e)var_num, &vartemp);
        num_info[var_num].val       = vartemp;
        num_info[var_num].oldval    = vartemp;

        //�����Сֵ�����ֵ
        ASSERT(num_info[var_num].maxval  >=  num_info[var_num].minval );
    }
}

void save_var(var_tab_e var_tal, uint32 var_data)
{
//    if(var_tal <= VAR_8BIT)
//    {
//        *((uint8 *)(var_addr[var_tal])) = var_data;
//    }
//    else if(var_tal <= VAR_16BIT)
//    {
//        *((uint16 *)(var_addr[var_tal])) = var_data;
//    }
    if(var_tal <= VAR_32BIT)
    {
        *((uint32 *)(var_addr[var_tal])) = var_data;
    }

    VAR_VALUE(var_tal) = var_data;
    VAR_OLDVALUE(var_tal) = var_data;
}

void get_var(var_tab_e var_tal, uint32 *var_data)
{
//    if(var_tal <= VAR_8BIT)
//    {
//        *var_data = (uint32) * ((uint8 *)(var_addr[var_tal]));
//    }
//    else if(var_tal <= VAR_16BIT)
//    {
//        *var_data = (uint32) * ((uint16 *)(var_addr[var_tal])) ;
//    }
     if(var_tal <= VAR_32BIT)
    {
        *var_data = (uint32) * ((uint32 *)(var_addr[var_tal]));
    }
}


void updata_var(var_tab_e var_tal)
{
    uint32 vartemp;

    get_var(var_tal, &vartemp);

    VAR_VALUE(var_tal) = vartemp;
}

//�Ա����ļӼ����д���
void var_value(ui_var_event_e ctrl)
{
    ASSERT(new_tab < VAR_MAX);


    //�޸ĵ�ǰ������ֵ
    switch(ctrl)
    {
    case VAR_ADD:
        if(VAR_VALUE(new_tab) < VAR_MAXVALUE(new_tab))
        {
            VAR_VALUE(new_tab)++;
        }
        else
        {
            VAR_VALUE(new_tab) = VAR_MINVALUE(new_tab);
        }
        break;

    case VAR_SUB:
        if(VAR_VALUE(new_tab) > VAR_MINVALUE(new_tab))
        {
            VAR_VALUE(new_tab)--;
        }
        else
        {
            VAR_VALUE(new_tab) = VAR_MAXVALUE(new_tab) ;//��Сֵ��һΪ���ֵ
        }
        break;

    case VAR_ADD_HOLD:
        if(   (VAR_MAXVALUE(new_tab) - VAR_VALUE(new_tab))  >  VAR_VALUE_HOLE_OFFSET )
        {
            VAR_VALUE(new_tab) += VAR_VALUE_HOLE_OFFSET;
        }
        else
        {
            VAR_VALUE(new_tab) = VAR_MINVALUE(new_tab);
        }
        break;

    case VAR_SUB_HOLD:
        if( ( VAR_VALUE(new_tab) - VAR_MINVALUE(new_tab)) > VAR_VALUE_HOLE_OFFSET  )
        {
            VAR_VALUE(new_tab) -= VAR_VALUE_HOLE_OFFSET;
        }
        else
        {
            VAR_VALUE(new_tab) = VAR_MAXVALUE(new_tab) ;//��Сֵ��һΪ���ֵ
        }
        break;

    default:                        //��Чѡ�񣬲���Ҫ�л�
        break;
    }

  //  var_display(new_tab);
}

//�Ա�������ѡ��
void var_select(ui_var_event_e  ctrl)
{
    ASSERT(new_tab < VAR_MAX);

    uint8 old_tab = new_tab;       //�ȱ��ݵ�ǰ�������

    //�л�����һ������
    switch(ctrl)
    {
    case VAR_NEXT:                      //��һ��
        new_tab++;
        if(new_tab >= (VAR_MAX) )
        {
            new_tab = 0;               //��ͷ��ʼ
        }
        break;

    case VAR_PREV:                      //��һ��
        new_tab--;
        if(new_tab >= (VAR_MAX) )
        {
            new_tab = VAR_MAX - 1;     //��β��ʼ
        }
        break;

    case VAR_NEXT_HOLD:                 //����
        new_tab += VAR_SELECT_HOLD_OFFSET;
        if(new_tab >= (VAR_MAX) )
        {
            new_tab = 0;               //��ͷ��ʼ
        }
        break;

    case VAR_PREV_HOLD:                 //����
        new_tab -= VAR_SELECT_HOLD_OFFSET;
        if(new_tab >= (VAR_MAX) )
        {
            new_tab = VAR_MAX - 1;     //��β��ʼ
        }
        break;

    default:                        //��Чѡ�񣬲���Ҫ�л�
        return;
    }

  //  var_display(old_tab);               //������һ������

  //  var_display(new_tab);              //����ǰ������

}


//ȷ�ϵ�ǰѡ���
void var_ok()
{
    ASSERT(new_tab < VAR_MAX);

    //�Ƚ��Ƿ��иı�ֵ
  //  if(VAR_VALUE(new_tab) != VAR_OLDVALUE(new_tab))   //ֵ�ı��ˣ�����Ҫ����
    {
        var_syn(new_tab);          //ͬ���µ�ֵ
    }
    //flash_write(SECTOR_NUM,(new_tab)*8,VAR_VALUE(new_tab));

 //   var_display(new_tab);
}

//ȡ����ǰѡ���ֵ  OK
void val_cancel()
{
    ASSERT(new_tab < VAR_MAX);

    //ֱ�ӻ�ԭ��ǰֵ
    VAR_VALUE(new_tab) = VAR_OLDVALUE(new_tab);

  //  var_display(new_tab);
}



//��ʾָ����ֵ��tab Ϊ VAR_MAX ʱ��ʾȫ����ʾ��С������ʾ��Ӧ��
//tabΪ1ʱ����ʾ��һҳ��Ϊ2ʱ����ʾ�ڶ�ҳ��
void var_display(uint8 tab)
{
  LastPage=page;
#if UI_VAR_USE_LCD

    //���屳����ʱ
#define SELECT_NO_CHANGE_BG         WHITE   //��ǰѡ�У�����û�иı�
#define SELECT_CHANGE_BG            WHITE   //��ǰѡ�У����Ҹı���
#define NO_SELECT_NO_CHANGE_BG      RED     //û��ѡ�У�����û�иı䣨��ͨ�ľ���������
#define NO_SELECT_CHANGE_BG         RED     //û��ѡ�У����Ҹı���

    //����������ɫ
#define SELECT_NO_CHANGE            BLUE    //��ǰѡ�У�����û�иı�
#define SELECT_CHANGE               GREEN   //��ǰѡ�У����Ҹı���
#define NO_SELECT_NO_CHANGE         BLUE    //û��ѡ�У�����û�иı䣨��ͨ�ľ���������
#define NO_SELECT_CHANGE            GREEN   //û��ѡ�У����Ҹı���

    uint8  i = 0;
    uint16 bkColor;
    uint16 Color;

    ASSERT((new_tab < VAR_MAX) && (tab <= VAR_MAX));

    if(new_tab==0 || new_tab == 7)
    {
      page = 1;
    }
    if(new_tab==8 || new_tab == 15)
    {
      page = 2;
    }
    if(new_tab==16 || new_tab == 23)
    {
      page = 3;
    }
//    if(new_tab==16 || new_tab == 23)
//      page = 3;
//    if(new_tab==24 || new_tab == 31)
//      page = 4;
    i = 7;    //ѭ���Ĵ���
    if(tab == 1)      //��ʾ���Ʋ�����һҳ
    {
        tab = 0;
    }
    if(tab == 2)      //��ʾ���Ʋ����ڶ�ҳ
    {
        tab = 8;
    }
    if(tab == 3)      //��ʾ���Ʋ����ڶ�ҳ
    {
        tab = 16;
    }
//    if(tab == 3)      //��ʾ���Ʋ�������ҳ
//    {
//        tab = 16;
//    }
//    if(tab == 4)      //��ʾ���Ʋ�������ҳ
//    {
//        tab = 24;
//    }
    do
    {
        if(tab == new_tab)
        {
            //��ʾ��ǰ��ֵ���ж�ֵ�Ƿ�ı�
            if(VAR_VALUE(tab) == VAR_OLDVALUE(tab)) //ֵû�ı䣬����Ҫ����
            {
                Color   =  SELECT_NO_CHANGE;
                bkColor =  SELECT_NO_CHANGE_BG;
            }
            else
            {
                Color   =  SELECT_CHANGE;
                bkColor =  SELECT_CHANGE_BG;
            }
        }
        else
        {
            //��ʾ�ǵ�ǰ��ֵ
            if(VAR_VALUE(tab) == VAR_OLDVALUE(tab)) //ֵû�ı䣬����Ҫ����
            {
                Color   =  NO_SELECT_NO_CHANGE;
                bkColor =  NO_SELECT_NO_CHANGE_BG;

            }
            else
            {
                Color   =  NO_SELECT_CHANGE;
                bkColor =  NO_SELECT_CHANGE_BG;
            }
        }

        //��ʾ����
        LCD_num_C(VAR_SITE(tab), VAR_VALUE(tab), Color, bkColor);

        tab++;
    }
    while(i--);         //tab != VAR_MAX ��ʱ��ִ��һ�ξ�����
#else
    tab = tab;          //�������뾯��
#endif
}

void save_var2buff(var_tab_e var_num, uint8 *sendbuf)
{
    uint32 temp;
    get_var(var_num, &temp);
    *((uint32 *)&sendbuf[COM_LEN]) = (uint32)var_num;
    *((uint32 *)&sendbuf[COM_LEN + sizeof(uint32)]) = temp;
}

//ͬ��ָ����ֵ��tab Ϊ VAR_MAX ʱ��ʾȫ��ͬ����С����ͬ����Ӧ��
uint8 var_syn(uint8 tab)
{
    ASSERT((new_tab < VAR_MAX) && (tab <= VAR_MAX));

    uint8  i = 0;
    uint8 tempbuff[DATA_PACKET];
    uint32 oldvalue;

    if(tab == VAR_MAX)
    {
        i = VAR_MAX - 1;
        tab = 0;
    }

    do
    {
        oldvalue = VAR_OLDVALUE(tab);                   //���ݾɵ�ֵ

        //��ֵ���Ƶ���Ӧ�ı���
        save_var((var_tab_e)tab, VAR_VALUE(tab));

        //�����µ�ֵ
        save_var2buff((var_tab_e)tab, tempbuff);        //�ѱ���д�� tempbuff ��
//        nrf_msg_tx(COM_VAR, tempbuff);                //��������
//
//        while(nrf_tx_state() == NRF_TXING);             //�ȴ��������
//
//        if(NRF_TX_ERROR == nrf_tx_state())             //����ʧ��
//        {
//            VAR_OLDVALUE(tab) = oldvalue;               //��ԭ�ɵ�ֵ
//
//            //����ͬ��
//            return 0;
//        }
        tab++;
    }
    while(i--);

    return 1;
}

uint8    car_ctrl_get()
{
    return  CarState;
}


//��״̬����
uint8 car_ctrl_syn(CAR_CTRL_MODE_e mode)
{
    uint8 ret;
    ASSERT(mode < CAR_CTRL_MAX);

    VAR_VALUE(CAR_CTRL) =  mode;

    ret =  var_syn(CAR_CTRL);

   // var_display(CAR_CTRL);

    return ret;
};



