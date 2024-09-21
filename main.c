#include <8051.h>
#include <stdlib.h>
#include "Keypad4x4.h"
#include "LED_Display.h"
#define Fosc 12000000
#define Fclk (Fosc/12)
#define Fint 400
#define Treload (65536-Fclk/Fint)
#define TH0_R (Treload >> 8)
#define TL0_R (Treload & 0xff)
unsigned char i, j = 0;
unsigned char display[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; // 存七段顯示器顯示的數字，初始化七段顯示器為空
unsigned char error[5] = {0x79, 0x50, 0x50, 0x5c, 0x50};
int a = 0, b = 0;
char operator = 0; // 儲存當前運算符號
long results[10]; // 保存歷史結果的數組
char result_index = 0; // 當前顯示的結果索引
char total_results = 0; // 保存的總結果數
char display_history = 0; // 標誌是否顯示歷史結果

void timer0_isr(void) __interrupt (1) __using(1)
{
    TH0 = TH0_R;     // 重新加載 TH0 的值 (存計數器高 8 位)
    TL0 = TL0_R;     // 重新加載 TL0 的值 (存計數器低 8 位)
    P1 = 0x00 + j;    // 顯示數字的位置
    LED_Display(display[j]);
    j++;
    if(j == 8) j = 0; // 顯示8位數字
}

int calculate(int a, int b, char operator)
{
    int result = 0;
    switch(operator){
        case 12: // '+'
            result = a + b;
            break;
        case 13: // '-'
            result = a - b;
            break;
        case 14: // '*'
            result = a * b;
            break;
        case 15: // '/'
            if(b != 0) result = a / b;
            break;
        default:
            break;
    }
    return result;
}

// 清除顯示函數，漿顯示器設置為全暗
void clear_display(void){
    for(i = 0; i < 8; i++){
        display[i] = 0xff;
    }
}

// 顯示結果函數，將結果顯示在數字顯示的位置
void display_result(int result) {
    clear_display(); // 清空顯示器
    if (result < 0) {
        int pos = 7;
        result = -result; // 取绝对值
        int temp1 = result;
        for (i = 7; temp1 > 0 && i >= 0; i--) { // 从右向左显示数字
            display[i] = temp1 % 10;
            temp1 /= 10;
            pos--;
        }
        if(pos >= 0) display[pos] = '--'; // 顯示負號
        
    } else if (result == 0) {
        display[7] = 0; // 显示数字 0
        return; // 如果是零直接返回，不需要显示多余的空位
    }else{
        // 显示数字部分
        int temp = result;
        for (i = 7; temp > 0 && i >= 0; i--) { // 从右向左显示数字
            display[i] = temp % 10;
            temp /= 10;
        }
    }

}

// 顯示錯誤函數，將錯誤信息顯示在七段顯示器上
void display_error(void) {
    clear_display(); // 清空顯示器
    
    display[3] = 'E'; // E
    display[4] = 'r'; // r
    display[5] = 'r'; // r
    display[6] = 'o'; // o
    display[7] = 'r'; // r
}

void main(void)
{
    char new_key = 16, old_key = 17; // 初始化按鍵變數
   
    signed int result = 0; // 計算結果
    char input_stage = 0; // 0 -> 輸入a； 1 -> 輸入b
   

    // Initialize Timer0
    TMOD = 0x01;     // 設置 Timer0, Mode 1 (16 位計時器)
    TH0 = TH0_R;     // 加載 TH0 的初始值(Fint==500)
    TL0 = TL0_R;     // 加載 TL0 的初始值
    ET0 = 1;         // 啟用 Timer0 中斷
    EA = 1;          // 啟動全局中斷
    TR0 = 1;         // 啟動 Timer0

   
    while(1) {
        new_key = KeyPad4x4(); // 獲取新的按鍵輸入
        if (new_key != 16 && new_key != old_key) { // 檢查按鍵輸入是否有效
            if(new_key < 10 && input_stage == 0){ // 輸入第一個操作數
                a = a * 10 + new_key; // 更新操作數a
                clear_display(); // 清空顯示器
                int temp = a;
                for(i = 7; temp > 0 && i >= 0; i--){ // 更新七段顯示器，顯示數字a
                    display[i] = temp % 10;
                    temp /= 10;
                }
                display_history = 0; // 重置歷史顯示標誌
            }
            else if((new_key >= 12 && new_key <= 15) && input_stage == 0){ // 輸入運算符號
                operator = new_key; // 設置運算符號
                input_stage = 1; // 進入下一個輸入階段
                display_history = 0; // 重置歷史顯示標誌
            }
            else if(new_key < 10 && input_stage == 1){ // 輸入第二個操作數
                b = b * 10 + new_key; // 更新操作數b
                clear_display(); // 清空顯示器
                int temp = b;
                for(i = 7; temp > 0 && i >= 0 ;i--){ // 更新七段顯示器，顯示數字b
                    display[i] = temp % 10;
                    temp /= 10;
                }
                display_history = 0; // 重置歷史顯示標誌
            }
            else if(new_key == 10 && input_stage == 1){ // 按下等於鍵計算結果
                if(operator == 15 && b == 0){
                    display_error();
                }else{
                    result = calculate(a, b, operator); // 計算結果
                    display_result(result); // 顯示結果
                    // 保存結果到歷史記錄
                    if(total_results < 10) {
                        results[total_results++] = result;
                    } else {
                        for(i = 1; i < 10; i++) {
                            results[i-1] = results[i];
                        }
                        results[9] = result;
                    }
                    result_index = total_results - 1; // 更新結果索引

                }
                
                
                a = b = 0; // 重置操作數
                input_stage = 0; // 重置輸入階段
                display_history = 0; // 重置歷史顯示標誌
            }
            else if(new_key == 11){ // 按下11號鍵顯示之前的結果
               if(total_results > 0) {
                    display_result(results[result_index]); // 顯示當前歷史結果
                    result_index = (result_index > 0) ? result_index - 1 : total_results - 1; // 更新結果索引
                }
                a = b = 0; // 重置操作數
                input_stage = 0; // 重置輸入階段
                operator = 0; // 重置運算符
                display_history = 1; // 設置顯示歷史結果標誌
            }
            old_key = new_key; // 更新上一次的按鍵
        }
    }   
}
