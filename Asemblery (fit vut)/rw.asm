; Read Write procedures, version 20100915
;

        %macro  prologue 0        
   	  mov ax,DATA
	  mov ds,ax
          mov ax,stack 
          mov ss,ax 
          mov sp,stacktop
        %endmacro

        %macro epilogue 0
 	  mov ah,4ch
	  int 21h
	%endmacro

        segment code
; Read
Read_Char:
        pushf
        pusha
        mov dx,mess_ch
        mov ah,9
        int 21h
	mov ah,0ch      ; Clear the Standard Input 
  	mov al,1        ; and Read Character
	int 21h         ;
        or al,al
        jnz rchcont     ; AL=ASCII_Character, AH=Scan_Code 
        mov ah,1        ; Extended Char
        int 21h
        mov ah,al
        xor al,al       ; AL=0, AH=Extended_ASCII_keystroke
rchcont: 
        mov [aux],ax
	popa
	mov ax,[aux]
	popf
	ret

Read_Byte:
        pushf
        pusha
        jmp rbcyc1
rbcont1:
        call WriteLn
rbcyc1: mov dx,mess_rb
        mov ah,9
        int 21h
        mov [ok],byte 0
        mov bl,10
	mov ax,0
rbcyc2:
	push ax
	mov ah,0ch       
  	mov al,1        
	int 21h 
	pop cx
	cmp al,13
	jne rbcont2
	cmp [ok],byte 0
	je rbcont1
	jmp rbcont3
rbcont2:
        cmp al,'0'
        jb rbcont1      
        cmp al,'9'
        ja rbcont1
        mov [ok],byte 1
        sub al,'0'
        mov ah,0
        xchg ax,cx
        mul bl
        add ax,cx
        cmp ax,255
        ja rbcont1
        jmp rbcyc2
rbcont3:
        mov [aux],cx
        popa
        mov al,[aux]
        popf
	ret

Read_ShortInt:
        pushf
        pusha
        jmp rscyc1
rscont1:
        call WriteLn
rscyc1: mov dx,mess_rs
        mov ah,9
        int 21h
        mov [ok],byte 0
        mov [sign],byte 0	; nebylo zadano
        mov bl,10
	mov ax,0
rscyc2:
	push ax
	mov ah,0ch       
  	mov al,1        
	int 21h 
	pop cx
	cmp al,13
	jne rscont2
	cmp [ok],byte 0
	je rscont1
	jmp rscont5
rscont2:
        cmp al,'+'
        jne rscont3
        cmp [ok], byte 0
        jne rscont1
	cmp [sign],byte 0
	jne rscont1
        mov [sign],byte 1	; +
        mov ax,cx
        jmp rscyc2
rscont3:
        cmp al,'-'
        jne rscont4
        cmp [ok], byte 0
        jne rscont1
	cmp [sign],byte 0
	jne rscont1
        mov [sign],byte -1	; -
        mov ax,cx
        jmp rscyc2
rscont4:
        cmp al,'0'
        jb near rscont1      
        cmp al,'9'
        ja near rscont1
        mov [ok],byte 1
        sub al,'0'
        mov ah,0
        xchg ax,cx
        mul bl
        add ax,cx
        cmp ax,128
        ja near rscont1
        cmp [sign],byte -1
        je rscyc2
        cmp ax,127
        ja near rscont1
        jmp rscyc2
rscont5:
        cmp cx,128
        je rscont6
        cmp [sign],byte -1
        jne rscont6
        neg cx
rscont6:
        mov [aux],cx
        popa
        mov al,[aux]
        popf
        ret

Read_Word:
        pushf
        pusha
        jmp rwcyc1
rwcont1:
        call WriteLn
rwcyc1: mov dx,mess_rw
        mov ah,9
        int 21h
        mov [ok],byte 0
        mov bx,10
	mov ax,0
rwcyc2:
	push ax
	mov ah,0ch       
  	mov al,1        
	int 21h 
	pop cx
	cmp al,13
	jne rwcont2
	cmp [ok],byte 0
	je rwcont1
	jmp rwcont3
rwcont2:
        cmp al,'0'
        jb rwcont1      
        cmp al,'9'
        ja rwcont1
        mov [ok],byte 1
        sub al,'0'
        mov ah,0
        xchg ax,cx
        cmp ax,6553
        ja rwcont1
        mul bx
        add ax,cx
        jc rwcont1
        jmp rwcyc2
rwcont3:
        mov [aux],cx
        popa
        mov ax,[aux]
	popf
        ret

Read_Integer:
	pushf
        pusha
        jmp ricyc1
ricont1:
        call WriteLn
ricyc1: mov dx,mess_ri
        mov ah,9
        int 21h
        mov [ok],byte 0
        mov [sign],byte 0	; nebylo zadano
        mov bx,10
	mov ax,0
ricyc2:
	push ax
	mov ah,0ch       
  	mov al,1        
	int 21h 
	pop cx
	cmp al,13
	jne ricont2
	cmp [ok],byte 0
	je ricont1
	jmp ricont5
ricont2:
        cmp al,'+'
        jne ricont3
        cmp [ok], byte 0
        jne ricont1
	cmp [sign],byte 0
	jne ricont1
        mov [sign],byte 1	; +
        mov ax,cx
        jmp ricyc2
ricont3:
        cmp al,'-'
        jne ricont4
        cmp [ok], byte 0
        jne ricont1
	cmp [sign],byte 0
	jne ricont1
        mov [sign],byte -1	; -
        mov ax,cx
        jmp ricyc2
ricont4:
        cmp al,'0'
        jb near ricont1      
        cmp al,'9'
        ja near ricont1
        mov [ok],byte 1
        sub al,'0'
        mov ah,0
        xchg ax,cx
        cmp ax,3276
        ja near ricont1
        mul bx
        add ax,cx
        cmp ax,32768
        ja near ricont1
        cmp [sign],byte -1
        je near ricyc2
        cmp ax,32767
        jna near ricyc2 
        jmp ricont1
ricont5:
        cmp ax,32768
        je ricont6
        cmp [sign],byte -1
        jne ricont6
        neg cx
ricont6:
        mov [aux],cx
        popa
        mov ax,[aux]
	popf
        ret

Read_String:
   	pushf
	pusha
	pusha
        mov dx,mess_st
        mov ah,9
        int 21h
        popa
	mov di,dx
	mov si,di
	mov [si],byte 0
rstrcyc:
	cmp [si],byte 255
	je rstrcont1
	mov ah,0ch       
  	mov al,1        
	int 21h
        cmp al,13
        je rstrcont1
        inc byte [si]
        inc di
        mov [di],al
        jmp rstrcyc
rstrcont1:	
	popa
	popf
	ret

; Write
Write_Char:             ; AL=Character
        pushf
        pusha
        mov dl,al
        mov ah,2        ; DL=Character
	int 21h
	popa
	popf
	ret

Write_Byte:             ; AL=Byte
        pushf
    	pusha
    	mov si,2
    	mov bl,10
wbcyc1: mov ah,0
        div bl
    	add ah,'0'
	mov [z_b+si],ah
        dec si
        cmp si,0
        jnz wbcyc1
        add al,'0'
	mov [z_b],al
    	mov dx,z_b
    	mov si,0
wbcyc2: cmp [z_b+si],byte '0'
    	jne wbcont
    	inc dx
    	inc si
    	cmp si,2
    	jne wbcyc2
wbcont: call Print_Str
	popa
	popf
	ret

Write_ShortInt:		; AL=ShortInt
	pushf
        pusha
    	cmp al,128
    	jb wscont2
    	mov dl,al
    	mov al,'-'
    	call Write_Char
    	mov al,dl
    	cmp al,128
    	jne wscont1
    	mov dx,z_bmax
    	call Print_Str
	jmp wscont3
wscont1:
	neg al
wscont2:	
	call Write_Byte
wscont3:	
	popa
	popf	
	ret

Write_Word:             ; AX=Byte
	pushf
    	pusha
    	mov si,4
    	mov bx,10
wwcyc1: mov dx,0
        div bx
    	add dl,'0'
	mov [z_w+si],dl
        dec si
        cmp si,0
        jnz wwcyc1
        add al,'0'
	mov [z_w],al
    	mov dx,z_w
    	mov si,0
wwcyc2: cmp [z_w+si],byte '0'
    	jne wwcont
    	inc dx
    	inc si
    	cmp si,4
    	jne wwcyc2
wwcont: call Print_Str
	popa
	popf
	ret

Write_Integer:		; AX=Integer
        pushf
        pusha
    	cmp ax,32768
    	jb wicont2
    	mov dx,ax
    	mov al,'-'
    	call Write_Char
    	mov ax,dx
    	cmp ax,32768
    	jne wicont1
    	mov dx,z_wmax
    	call Print_Str
	jmp wicont3
wicont1:
	neg ax
wicont2:	
	call Write_Word
wicont3:	
	popa
	popf
	ret

Write_String:
        pushf
        pusha
        mov si,dx
        mov di,string
        mov cl,[si]
        mov ch,0
        jcxz wstrcont
wstrcyc:
        inc si
        mov al,[si]
        mov [di],al
        inc di
        loop wstrcyc
        mov [di],byte '$'
        mov dx,string
        call Print_Str
wstrcont:
        popa
        popf
        ret

Write_Flags:
        pushf
        pusha
        pushf
        pop ax
        mov cx,16
        mov si,0
wfcyc:  shl ax,1
        mov [flags+si],byte '0'
        jnc wfcont
        mov [flags+si],byte '1'
wfcont: inc si
        loop wfcyc
        mov dx,mess_fl
        mov ah,9
        int 21h
        popa
        popf
        ret

WriteLn:
        pushf
	pusha
        mov dl,13       ; CR
	mov ah,2        
	int 21h
        mov dl,10       ; LF
	mov ah,2        
	int 21h
	popa
	popf
	ret

Print_Str:              ; DX=String_offset, String='???....???','$'
        pushf
        pusha
        mov ah,9
        int 21h
        popa
        popf
        ret

        segment DATA
z_w     resb 2
z_b     resb 3
        db '$'
z_bmax  db '128$'
z_wmax  db '32768$'
ok      resb 1
sign    resb 1
aux     resw 1

mess_ch db 'Zadejte znak: $'
mess_rb db 'Zadejte Byte (0,..,255): $'
mess_rs	db 'Zadejte ShortInt (-128,..,127): $'
mess_rw db 'Zadejte Word (0,..,65535): $'
mess_ri	db 'Zadejte Integer (-32768,..,32767): $'
mess_st db 'Zadejte ýetØz znak…: $'
mess_fl	db 'Flags: xxxxODITSZxAxPxC',13,10,
        db '       '
flags   resb 16
	db '$'
string  resb 256

        segment stack stack 
        resb 1000 
stacktop:
