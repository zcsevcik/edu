	bits 16
	%include "rw.asm"

	segment code
..start:
	prologue
	call Read_Byte
	call WriteLn
	
	mov BL, AL ; v BL nactene cislo
	
	; obsah krychle
  mul BL
  mul BL 
	call Write_Word
	call WriteLn
	
	; obvod trojuhelnika
	mov AL, BL
  add AL, BL
  add AL, BL
  mov AH, AL  ; v AH 1. obvod 
	call Write_Byte
	call WriteLn
	
	; zbytecna jmp operace
	jmp .skip
	sub AL, BL
	.skip:
	
	; obvod 2. trojuhelnika
	call Read_Byte
	call WriteLn
	mov BL, AL ; v BL nactene 2. cislo
	add AL, BL
	add AL, BL
	call Write_Byte
	call WriteLn
	
	; rozdil obvodu
	; v AL 2. obvod
	; v AH 1. obvod
	call WriteLn
	sub AL, AH
	call Write_Byte
	call WriteLn
	
	epilogue			

	segment DATA
;  msg_hello_world		db	"Prvni program v asm!", 13, 10, '$'
