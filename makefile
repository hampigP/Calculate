main.hex: main.ihx
	packihx main.ihx > main.hex

main.ihx: main.c mylib.lib
	sdcc main.c mylib.lib
 
mylib.lib: Keypad4x4.rel LED_Display.rel
	sdar -rs mylib.lib Keypad4x4.rel LED_Display.rel
	sdranlib mylib.lib

Keypad4x4.rel: Keypad4x4.c LED_Display.rel
	sdcc -c Keypad4x4.c LED_Display.rel

LED_Display.rel: LED_Display.c
	sdcc -c LED_Display.c

clean:
	del *.hex *.ihx *.map *.asm *.lk *lst *.mem *.rel *.rst *sym *.lib