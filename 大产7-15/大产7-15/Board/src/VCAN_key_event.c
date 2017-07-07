/*!
*     COPYRIGHT NOTICE
*     Copyright (c) 2013,ɽ��Ƽ�
*     All rights reserved.
*     �������ۣ�ɽ����̳ http://www.vcan123.com
*
*     ��ע�������⣬�����������ݰ�Ȩ����ɽ��Ƽ����У�δ����������������ҵ��;��
*     �޸�����ʱ���뱣��ɽ��Ƽ��İ�Ȩ������
*
* @file       VCAN_key_event.h
* @brief      KEY �¼�����ʵ��
* @author     ɽ��Ƽ�
* @version    v5.0
* @date       2014-01-04
*/

#include "VCAN_key_event.h"
#include "VCAN_UI_VAR.h"
#include "include.h"
//#include "NRF24L0.h"
//#include "NRF24L0_MSG.h"
extern uint8   new_tab ;        //��ǰѡ��ı������
extern reg_s ov7725_eagle_reg[];
void key_event_init()
{
  //��ʼ�� ȫ�� ����
  key_init(KEY_MAX);
  
  //��ʼ����ʱ����
  var_init();
  
  //ͬ��ȫ�����ݲ���ʾ
  //   var_syn(VAR_MAX);       //ͬ��ȫ�� ,������ͬ������ʾȫ������Ϊ�п���ͬ��ʧ�ܡ�
  //  var_display(VAR_MAX);   //��ʾȫ��
}

void deal_key_event()
{
  KEY_MSG_t keymsg;
  
  while(get_key_msg(& keymsg))     //��ð����ͽ��д���
  {
    if(keymsg.status == KEY_DOWN)
    {
      switch(keymsg.key)
      {
      case KEY_U:
        if(page == 0)
          Img_LCD_Loaction_Y=0;
        if(page == 1 || page ==2 || page ==3 || page ==4)
          var_value(VAR_ADD);
        break;
        
      case KEY_D:
        if(page == 0)
          Img_LCD_Loaction_Y=24;
        if(page == 1 || page ==2 || page ==3 || page ==4)
          var_value(VAR_SUB);
        break;
        
      case KEY_L:
        if(page == 1 || page ==2 || page ==3 || page ==4)
        {
       //   if(new_tab>=((page-1)*8-1) && new_tab<(page)*8)
            var_select(VAR_PREV);
        }
        if(page == 0)
        {
          Img_LCD_Loaction_X--;
          if(Img_LCD_Loaction_X<0)
            Img_LCD_Loaction_X=0;
        }
        break;
        
      case KEY_R:
        if(page == 1 || page ==2 || page ==3 || page ==4)
        {
     //     if(new_tab>=(page-1)*8 && new_tab<=(page)*8-1)
            var_select(VAR_NEXT);
        }
        if(page == 0)
        {
          Img_LCD_Loaction_X++;
          if(Img_LCD_Loaction_X>2)
            Img_LCD_Loaction_X=2;
        }
        break;
        
      case KEY_OK:
        var_ok();
        break;
        
      case KEY_STOP:
          FlagStart = 2;
        break;
        
      case KEY_START:
        FlagStart = 1;
        DELAY_MS(2000);
        break;
      case KEY_Flash:
          //var_ok(); 
        VarConfirm_Key();
          Flash_Write();
          ov7725_eagle_reg[45].val=VarReceive[17];
          camera_init(ImgAgo);
        break;
      case KEY_PAGE:
        page++;
        if(page>5)
          page=0;
        LCD_clear(RED);
        break;
      default:
        break;
      }
    }
    else if(keymsg.status == KEY_HOLD)
    {
      switch(keymsg.key)
      {
      case KEY_U:
        var_value(VAR_ADD_HOLD);
        break;
        
      case KEY_D:
        var_value(VAR_SUB_HOLD);
        break;
        
      case KEY_L:
        var_select(VAR_PREV_HOLD);
        break;
        
      case KEY_R:
        var_select(VAR_NEXT_HOLD);
        break;
        
      case KEY_OK:                //���� OK �� ͬ��ȫ�����ݲ���ʾ

        //       var_syn(VAR_MAX);       //ͬ��ȫ�� ,������ͬ������ʾȫ������Ϊ�п���ͬ��ʧ�ܡ�
        //        var_display(VAR_MAX);   //��ʾȫ��
        break;
        
      case KEY_START:

        break;
        
        
      default:            //����Ҫ���� cancel
        break;
      }
    }
    else
    {
      //KEY_UP ,�����д���
    }
  }
}





