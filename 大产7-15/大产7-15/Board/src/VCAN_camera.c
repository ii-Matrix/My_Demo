#include "common.h"
#include "include.h"

//ѹ����ֵ��ͼ���ѹ���ռ� �� ʱ�� ��ѹ��
//srclen �Ƕ�ֵ��ͼ���ռ�ÿռ��С
//��ӥ�۽�ѹ��ӥ��ͼ���ѹ��תΪ ��ά���� - ���ܳ������� - ɽ����̳ http://vcan123.com/forum.php?mod=viewthread&tid=17&ctid=6
//��ѹ��ʱ�������и����飬���úڡ��׶�Ӧ��ֵ�Ƕ��١�
void img_extract(void *dst, void *src, uint32_t srclen)
{
  uint8_t colour[2] = {ImgWhite, ImgBlack}; //0 �� 1 �ֱ��Ӧ����ɫ
  uint8_t * mdst = dst;
  uint8_t * msrc = src;
  //ע��ɽ�������ͷ 0 ��ʾ ��ɫ��1��ʾ ��ɫ
  uint8_t tmpsrc;
      while(srclen --)
      {
          tmpsrc = *msrc++;
          *mdst++ = colour[ (tmpsrc >> 7 ) & 0x01 ];
          *mdst++ = colour[ (tmpsrc >> 6 ) & 0x01 ];
          *mdst++ = colour[ (tmpsrc >> 5 ) & 0x01 ];
          *mdst++ = colour[ (tmpsrc >> 4 ) & 0x01 ];
          *mdst++ = colour[ (tmpsrc >> 3 ) & 0x01 ];
          *mdst++ = colour[ (tmpsrc >> 2 ) & 0x01 ];
          *mdst++ = colour[ (tmpsrc >> 1 ) & 0x01 ];
          *mdst++ = colour[ (tmpsrc >> 0 ) & 0x01 ];
      }
//  for (int y = 0; y < 120; y++) {
//    for(int x=0;x<160;x+=8)
//    {
//      int i =0;
//      Img[y][x+i]=colour[((ImgBufferAgo[y*20+x/8]>>7 )& 0x01)]; i++;
//      Img[y][x+i]=colour[((ImgBufferAgo[y*20+x/8]>>6 )& 0x01)]; i++;
//      Img[y][x+i]=colour[((ImgBufferAgo[y*20+x/8]>>5 )& 0x01)]; i++;
//      Img[y][x+i]=colour[((ImgBufferAgo[y*20+x/8]>>4 )& 0x01)]; i++;
//      Img[y][x+i]=colour[((ImgBufferAgo[y*20+x/8]>>3 )& 0x01)]; i++;
//      Img[y][x+i]=colour[((ImgBufferAgo[y*20+x/8]>>2 )& 0x01)]; i++;
//      Img[y][x+i]=colour[((ImgBufferAgo[y*20+x/8]>>1 )& 0x01)]; i++;
//      Img[y][x+i]=colour[((ImgBufferAgo[y*20+x/8]>>0 )& 0x01)]; 
//    }
//  }
}