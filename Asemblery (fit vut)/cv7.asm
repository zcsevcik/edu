; ---------------------------------------------------------------- ;
; Funkcni ukazka printf v linuxu (1b)                              ;
; Ukol: nacist dve cisla (polomer a vysku) a vypocet objemu valce  ;
; 1. Nacteni 2 cisel z klavesnice pomoci scanf a                   ;
;    jejich formatovany vypis ulozeni do pameti (1b)               ;
; 2. Vypoctete delku prepony (1b)                                  ;
; ---------------------------------------------------------------- ;

    global main
    extern scanf, printf

;---------------
    section .text

main:
    push dword printf_vyzva
    call printf
    add esp, 4

    push dword fp_odvesna2
    push dword fp_odvesna1
    push dword scanf_fmt
    call scanf
    add esp, 12

    cmp eax, 2
    jne .chyba

    push dword [fp_odvesna2+4]
    push dword [fp_odvesna2]
    push dword [fp_odvesna1+4]
    push dword [fp_odvesna1]
    push dword printf_fp
    call printf
    add esp, 20

; vypocet prepony
    fld qword [fp_odvesna1]
    fmul st0

    fld qword [fp_odvesna2]
    fmul st0

    fadd st1
    fsqrt
    fst qword [fp_prepona]

    push dword [fp_prepona+4]
    push dword [fp_prepona]
    push dword printf_prepona
    call printf
    add esp, 12

    jmp .konec

.chyba:
    push dword scanf_err
    call printf
    add esp, 4

.konec:
    mov eax, 0

    ret

;---------------
    section .data
scanf_fmt	db	'%lf %lf',0
scanf_err	db	'Chyba pri nacteni cisla pomoci scanf!',0ah,0
printf_fp	db	'Nacteny odvesny %f a %f',0ah,0
printf_vyzva	db	'Zadejte delku 2 odvesen: ',0
printf_prepona	db	'Delka prepony je %f',0ah,0
fp_odvesna1	dq	0.0
fp_odvesna2	dq	0.0
fp_prepona	dq	0.0
