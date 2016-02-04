	bits 16
	%include "rw.asm"

	segment code

@printf:
  push bp
  mov bp, sp
  sub sp, 4
  ; [bp-2] offset na data
  ; [bp-4] znak z formatu

  mov word [bp-2], 6

.read_next:
  mov bx, [bp+4]
  mov ax, [bx]
  mov [bp-4], ax
  
  cmp byte [bp-4], 0
  je .konec
  cmp byte [bp-4], '#'
  je .crim
  cmp byte [bp-4], '$'
  je .string
  jmp .char
  
.crim:
  mov bx, bp
  add bx, [bp-2]
  mov ax, [ss:bx]
  push word ax 
  call @print_hex
  add sp, 2
  add word [bp-2], 2
  jmp .do_repeat  

.string:
  mov bx, bp
  add bx, [bp-2]
  mov dx, [ss:bx]
  call Print_Str
  add word [bp-2], 2
  jmp .do_repeat

.char:
  mov ax, [bp-4]
  call Write_Char
  jmp .do_repeat
    
.do_repeat:
  inc word [bp+4]
  jmp .read_next

.konec:
  mov sp, bp
  pop bp
  ret

@print_hex:
  push bp
  mov bp, sp
  
  mov ax, '0'
  call Write_Char
  mov ax, 'x'
  call Write_Char
  
  mov bx, [bp+4]
  and bx, 0xF000
  shr bx, 12
  mov ax, [bx+hexa]       
  call Write_Char

  mov bx, [bp+4]
  and bx, 0x0F00
  shr bx, 8
  mov ax, [bx+hexa]       
  call Write_Char

  mov bx, [bp+4]
  and bx, 0x00F0
  shr bx, 4 
  mov ax, [bx+hexa]       
  call Write_Char

  mov bx, [bp+4]
  and bx, 0x000F
  mov ax, [bx+hexa]       
  call Write_Char
  
  mov sp, bp
  pop bp
  ret

..start:
	prologue

  push word szMsg2
  push word 38492
  push word 9126
  push word szMsg
  push word szFormat
  call @printf
  add sp, 10
  call WriteLn	
	
	epilogue			

	segment DATA
szFormat    db    "$ je # a # == $",0
szMsg       db    "desitkove 9126",'$'
szMsg2      db    "38492",'$'
hexa        db    "0123456789ABCDEF"
