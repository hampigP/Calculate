#include "Keypad4x4.h"

char KeyPad4x4(void){
    char r, c;
    for (r = 0; r < 4; r++) {             	// Scan each row of the keypad
        P0 = ~(0x01 << r);                	// Activate one row at a time，設置當前行為低電位，其他行為高電位
        if ((P0 & 0xf0) != 0xf0) {        	// Check if any key in the activated row is pressed，檢查按鍵是否有被按下
            for (c = 0; c < 4; c++) {     	// Scan each column of the activated row
                if (!(P0 & (0x10 << c))) { 	// Check if a key in the column is pressed，檢查按鍵是否有被按下
                    return c * 4 + r;     	// Calculate and return the key's value
                }
            }
        }
    }
    return 16;                            	// Return 16 if no key is pressed
}