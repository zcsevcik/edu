;----------------------------------------------------------------------------;
; Pomocí služeb DOS, naprogramujte následující:                              ;
;  0. doplòte kód pro prologue a epilogue                                    ;
;  1a. procedura W_Char: vytiskne znak v registru al                         ;
;  1b. procedura W_Ln:   provede odøádkování                                 ;
;  1c. procedura W_Hex: vytiskne èíslo v registru al v hexadecimálním tvaru  ;
;        (výstupem jsou tedy vždy dvì hexa-èíslovky) (1b)                    ;
;  2.  procedura F_Convert: obsah vstupního souboru pøepí‘e v hexa-tvaru     ;
;        do výstupního souboru (využijte vhodnì modivfikovanou W_Hex),       ;
;        na výstup bude indikován úspìch/neúspìch operace (2b)               ;
;----------------------------------------------------------------------------;
; Coded by Sevcik Radek

    bits 16
    segment code

..start:
  ; prologue
  mov ax, DATA
  mov ds, ax
  mov ax, stack 
  mov ss, ax 
  mov sp, stacktop

;---------------
  ;mov al, 88
  mov al, 'o'
  call W_Char
  call W_Ln
  call W_Hex
  call W_Ln

  call W_Ln
  call F_Convert
;---------------

  ; epilogue
  mov ah, 4ch
  int 21h

; Vase funkce

; ------------------------------------------------------------------- ;
W_Char:
  pushf
  pusha
  
  mov dl, al
  mov ah, 02h
  int 21h
  
  popa
  popf
  ret

; ------------------------------------------------------------------- ;
W_Ln:
  pushf
  pusha
  
  mov dl, 0dh
  mov ah, 02h
  int 21h

  mov dl, 0ah
  mov ah, 02h
  int 21h
  
  popa
  popf
  ret

; ------------------------------------------------------------------- ;
W_Hex:
  pushf
  push ax

  mov bh, al
  and bh, 0xf0
  shr bh, 4
  cmp bh, 0ah
  jnb .high_char
  add bh, '0'
  jmp .next_char
.high_char:
  sub bh, 0ah
  add bh, 'A'
  
.next_char:
  mov bl, al
  and bl, 0x0f
  cmp bl, 0ah
  jnb .high_2nd_char
  add bl, '0'
  jmp .end_2nd
.high_2nd_char:
  sub bl, 0ah
  add bl, 'A'
.end_2nd:
  
  mov al, bh
  call W_Char

  mov al, bl
  call W_Char

  pop ax
  popf
  ret

; ------------------------------------------------------------------- ;
F_Convert:
  pusha
  
; inicializace
  mov word [num_read], 0h
  mov word [num_write], 0h
  mov word si, 0h
  mov word di, 0h
  
; otevreni
  mov al, 0h     ;read
  mov ah, 3dh    ;openFile
  mov dx, zdroj  ;ds:dx fileName
  int 21h
  jc near .read_open_err
  mov [h_read], ax

  mov al, 1h
  mov ah, 3dh
  mov dx, cil
  int 21h
  jc near .write_open_err
  mov [h_write], ax

; smycka
.read_loop:
  mov ah, 3fh
  mov bx, [h_read]
  mov cx, len_read
  mov dx, buf_read
  mov si, 0h
  int 21h
  jc near .read_err
  mov [num_read], ax

  cmp word [num_read], 0h ; kontrola EOF
  je .close_write

.write_loop:
  cmp si, [num_read]
  je .read_loop  

  mov al, [buf_read+si]
  inc si
  call W_Hex
  mov [buf_write+di], bh
  inc di
  mov [buf_write+di], bl
  inc di
  mov byte [buf_write+di], 20h
  inc di

  mov ah, 40h
  mov bx, [h_write]
  mov cx, len_write
  mov dx, buf_write
  mov di, 0h
  int 21h
  jc near .write_err
  mov [num_write], ax

  jmp .write_loop

; uzavreni
.close_write:
  mov bx, [h_write]
  mov ah, 3eh
  int 21h
  jc .write_close_err

.close_read:
  mov bx, [h_read]
  mov ah, 3eh    ;closeFile
  int 21h
  jc .read_close_err
  jmp .end

; chyba otevreni
.write_open_err:
  ror ax, 4
  call W_Hex
  rol ax, 4
  call W_Hex
  mov al, 'Q'
  call W_Char
  jmp .close_read

.read_open_err:
  ror ax, 4
  call W_Hex
  rol ax, 4
  call W_Hex
  mov al, 'W'
  call W_Char
  jmp .end
  
; chyba uzavreni
.write_close_err:
  ror ax, 4
  call W_Hex
  rol ax, 4
  call W_Hex
  mov al, 'E'
  call W_Char
  jmp .close_read  

.read_close_err:
  ror ax, 4
  call W_Hex
  rol ax, 4
  call W_Hex
  mov al, 'R'
  call W_Char
  jmp .end
  
; chyba cteni/zapisu
.read_err:
  ror ax, 4
  call W_Hex
  rol ax, 4
  call W_Hex
  mov al, 'T'
  call W_Char
  jmp .close_write
  
.write_err:
  ror ax, 4
  call W_Hex
  rol ax, 4
  call W_Hex
  mov al, 'Y'
  call W_Char
  jmp .close_write
  
; konec
.end:
  mov word [h_read], 0h
  mov word [h_write], 0h        
  
  popa
  ret


;---------------
    segment DATA          ; datovy segment

    ;  sem ukladejte pomocna data (retezce, apod.)

zdroj      db    "vstup.txt", 0
cil        db    "vystup.txt", 0
len_read   equ   32
len_write  equ   3
buf_read   resb  len_read
buf_write  resb  len_write
num_read   resb  2
num_write  resb  2
h_read     resb  2
h_write    resb  2

;---------------
    segment stack stack   ; zasobnikovy segment

resb 1000
stacktop:
