; zvoli se mod pismenkem co se bude delat
; podle modu se nactou cisla, zalezi na modu kolik
; vypise se delka nactene sekvence
; vypocet podle modu
; bude se pokracovat odznovu

; 0 konec zadavani (50 znaku max)

; mody:
; D vyber kazdeho druheho (cyklus for)
; P pocet cisel v intervalu <5;10>
; K konec

	bits 16
	%include "rw.asm"
	
	segment code
..start:

	prologue

@switch_start:
  call Read_Char
  call WriteLn
  cmp al, 'k'
  jne @switch_d
  jmp @konec
  
@switch_d:
  cmp al, 'd'
  jne @switch_p
  
  mov bx, posl
  mov cx, 0h
@d_start:
  call Read_Byte
  call WriteLn
  cmp al, 0h
  jz @d_konec
  inc cx            ; pocet prvku
  mov [bx], al
  inc bx            ; posune posloupnost 
  jmp @d_start    
@d_konec:
  mov [len_posl], cx
  jcxz @preskocit_vypis
  cmp cx, 1h
  je @preskocit_vypis
  mov bx, posl
  shr cx, 1h
@vypis_posl:  
  mov al, [bx+1]
  call Write_Byte
  mov al, 20h
  call Write_Char
  add bx, 2h
  loop @vypis_posl
  call WriteLn
@preskocit_vypis:
; pocet cisel v intervalu
  mov dx, msg_pocet
  call Print_Str
  mov al, [len_posl]
  call Write_Byte
  call WriteLn
          
  jmp @switch_konec
  
@switch_p:
  cmp al, 'p'
  jne @switch_default
  
  mov cx, 0
  mov bx, posl

@p_start:
  call Read_Byte
  call WriteLn
  cmp al, 0h          ; test 0
  je @p_konec
  cmp al, int_left    ; test al >= int_left
  jb @p_start
  cmp al, int_right   ; test al <= int_right
  ja @p_start
  inc cx              ; pricteni
  jmp @p_start
@p_konec:
  mov [len_posl], cx
; pocet cisel v intervalu
  mov dx, msg_pocet
  call Print_Str
  mov al, [len_posl]
  call Write_Byte
  call WriteLn
  
  jmp @switch_konec
  
@switch_default:
  mov dx, msg_bad_mode
  call Print_Str 
  jmp @switch_konec
  
@switch_konec:
  jmp @switch_start

@konec:
	epilogue
  			
	segment DATA
	  len_posl      db      0
	  posl          resb    50
	  int_left      equ     5
	  int_right     equ     15
	  msg_bad_mode  db      "Byl zvolen spatny mod, opakujte svou volbu...",0Ah,0Dh,'$'
	  msg_pocet     db      "Pocet cisel v posloupnosti: ",'$'

