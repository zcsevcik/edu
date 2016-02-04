	bits 16
	%include "rw.asm"
	segment code
..start:
	prologue
	mov dx,msg_hello_world		; Vypis textu na obrazovku
	call Print_Str			; Print_Str tiskne text ukonceny '$'
	epilogue			
	segment DATA
msg_hello_world		db	"Hello World!",13,10,'$'
