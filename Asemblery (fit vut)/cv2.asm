; vytvorit 2 posl. bitu nezn. cisel ukoncene 0
; vypsat jejich delky
; z 1. vypsat max
; z delsi vypsat sudost/lichost
  bits 16
	%include "rw.asm"

	segment code
..start:
	  prologue
	  
	  ; spocita delku 1. pole
	  mov ah, 0
    mov bx, posl1
  @loop_1st:
    mov al, [bx]
    inc bx
    inc ah

    ; zjisti jestli je cislo vetsi nez v max    
    cmp al, [max]
    jnae @loop_1st_continue
    mov [max], al

  @loop_1st_continue
    cmp al, NULL
    jnz @loop_1st
    dec ah
    mov [len1], ah 
     
    mov dx, msg_len1
	  call Print_Str
	  mov al, ah
	  call Write_Byte
	  call WriteLn

    ; spocita delku 1. pole
	  mov ah, 0
    mov bx, posl2
  @loop_2nd:
    mov al, [bx]
    inc bx
    inc ah
    cmp al, NULL
    jnz @loop_2nd
    dec ah
    mov [len2], ah 
     
    mov dx, msg_len2
	  call Print_Str
	  mov al, ah
	  call Write_Byte
	  call WriteLn
  	
  	; vypise max prvek 1. pole
  	mov al, [max]
  	call Write_Byte
  	call WriteLn

    ; zjisti ktera posloupnost je vetsi
    mov al, [len1]
    cmp al, [len2]
    jae @prvni_vetsi
  ;@druha_vetsi:
    mov al, [len2]
  @prvni_vetsi:
    mov [len_max], al
        
  @vetsi_end:
  	; vypise sudost/lichost
    test byte [len_max], 1h
    jnz @je_lichy
  ;@je_sudy:
    mov dx, msg_suda
    jmp @end    
  @je_lichy:
    mov dx, msg_licha
  @end:  	
    call Print_Str
  
  	epilogue			

	segment DATA
    posl1     db    94, 0h
    posl2     db    12, 64, 0h
    max       db    0
    len1      resb  1
    len2      resb  1
    len_max   resb  1
    msg_len1  db    "Delka 1. pole: ",'$'
    msg_len2  db    "Delka 2. pole: ",'$'
    msg_suda  db    "Delsi posloupnost ma sudy pocet prvku", 0Dh, 0Ah, '$'
    msg_licha db    "Delsi posloupnost ma lichy pocet prvku", 0Dh, 0Ah, '$'
    NULL      equ   0h