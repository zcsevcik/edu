	bits 16
	%include "rw.asm"

	segment code
@to_lower:
  push bp
  mov bp, sp
  mov bx, [bp+4]  ; v bx adresa retezce
  mov ch, 0h
  mov cl, [bx]
.next_char:
  inc bx
  mov al, [bx]
  cmp al, 'A'
  jb .not_big
  cmp al, 'Z'
  ja .not_big
  ;je velky pismeno
  add al, 20h
  mov [bx], al
.not_big:
  loop .next_char
  mov sp, bp
  pop bp
  ret

@is_blank:
  push bp
  mov bp, sp
  mov ah, [bp+4]
  cmp ah, 20h
  je .return_1
  cmp ah, 09h
  je .return_1
  mov al, 0h
  jmp .return
.return_1:
  mov al, 1h
.return:
  mov sp, bp
  pop bp
  ret

..start:
	prologue

.start:
  ;switch
  call WriteLn
  call Read_Char
  call WriteLn
  
  cmp al, 'k'
  je .switch_k
  cmp al, 'l'
  je .switch_l
  cmp al, 'b'
  je .switch_b
  ;default:
  mov dx, msg_spatna_volba
  call Print_Str
  jmp .start

.switch_l:
  mov dx, msg_to_lower
  call Print_Str
  
  mov dx, msg_string
  call Read_String
  call WriteLn
  
  push word msg_string
  call @to_lower
  add sp, 2
  
  mov dx, msg_string
  call Write_String
  call WriteLn
  jmp .start

.switch_b:
  mov dx, msg_is_blank
  call Print_Str
  
  call Read_Char
  call WriteLn
  
  push ax
  call @is_blank
  add sp, 2
  
  cmp al, 1h
  jne .not_blank
  mov dx, msg_blank_true
  jmp .print_blank
.not_blank:  
  mov dx, msg_blank_false
.print_blank:  
  call Print_Str
  jmp .start
  
.switch_k:
	epilogue			

	segment DATA
msg_spatna_volba  db  'Spatna volba',0Dh,0Ah,'$'
msg_to_lower      db  'Vybrali jste funkci void to_lower(char*)',0Dh,0Ah,'$'
msg_is_blank      db  'Vybrali jste funkci bool is_blank(char)',0Dh,0Ah,'$'
msg_blank_true    db  'Zadali jste mezeru nebo tabulator',0Dh,0Ah,'$'
msg_blank_false   db  'Zadali jste tisknutelny znak',0Dh,0Ah,'$'
msg_string        resb  256