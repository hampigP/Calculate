#include "LED_Display.h"

void LED_Display(unsigned char num){
    const char seg[16] = {0x3f,  // 0
                          0x06,  // 1
                          0x5b,  // 2
                          0x4f,  // 3                
                          0x66,  // 4
                          0x6d,  // 5
                          0x7d,  // 6
                          0x07,  // 7              
                          0x7f,  // 8
                          0x6f,  // 9
                          0x63,  // +
                          0x5c,  // -
                          0x73,  // *
                          0x08,  // / 
                          0x40,  // 負號
                          0x00               
    };
    const char eng[] = {
        0x79,  // E
        0x50,  // r
        0x5c,  // o
    };
    if(num == 'E') P2 = eng[0];
    else if(num == 'r') P2 = eng[1];
    else if(num == 'o') P2 = eng[2];
    else if(num == '--') P2 = seg[14];
    else P2 = seg[num];
}

