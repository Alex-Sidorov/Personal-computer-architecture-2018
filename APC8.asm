.386P
.model large

struct_deskriptor  struc                           ;��������� ����������� �����������
    limit  dw 0                    ;����� �������� (15:00)    
    base_low  dw 0                    ;����� ����, ������� ����� (15:0)
    base_medium  db 0                    ;����� ����, ������� ����� (23:16)
    access  db 0                    ;���� ������� 
    attribs     db 0                    ;����� �������� (19:16) � ��������
    base_hight   db 0                    ;����� ����, ������� �����
struct_deskriptor  ends     
   
struct_interrupt_deskriptor  struc                           ;��������� ����������� ������� ����������
    offset_low      dw 0                    ;����� ����������� (0:15)
    selectors        dw 0                    ;�������� ����, ����������� ��� �����������
    parametrs   db 0                    ;���������
    access      db 0                    ;������� �������
    offset_hight      dw 0                    ;����� ����������� (31:16)
struct_interrupt_deskriptor  ends   
     
struct_idtr struc                           ;��������� idtr
    limit       dw 0                         ;16-bit table limit
    idtr_low       dw 0                    ;�������� ���� (0-15)
    idtr_hight       dw 0                    ;�������� ���� (31-16)
struct_idtr  ends

;����� ������� ������� ���������	;76543210 : 7--(1-��� ������),
					; 6,5--(����������),
					; 4,���� ���� ��� ����������, �� ���������� ���������� ������� ���� ��� ������, � ���� �������, �� ��������� ������ (��������, ������� ��������� ������, ��������� ������������� �������, ����).
					;3--(1,1-��� ����, 1,0-��� ������),
					; 2--(0-���� ����� ����� ��� �����,��� ����-��� ����������)
					; 1--(0-��������� ������ ���������,������ ��������� ��� ����, 1-������ � ������ ��� ������)
					; 0--(��� ��������� � ��������)
;���� ��������� ��������� � �������� ��� ������ ��� ������ ������ ��� ��� ���������� ����, ����������� � ��, �� ��� A ����� ���������� (����� 1), ����� - ������� (0). 

access_read        EQU 00000010B           ;��� ������, ����������� ������ �� ������� �������� --------------------------
access_code        EQU 10011000B		    ;AR �������� ����: �����������, �������, ��� ��������
access_data 	    EQU 10010010B		    ;AR �������� ������: �����������, �������, ������/������
access_stack	    EQU 10010010B		    ;AR �������� �����
access_idt         EQU 10010010B           ;AR ������� idt    
access_interrupt         EQU 10001110B		    ;AR ������� INT
access_trap        EQU 10001111B
access_dpl_three       EQU 01100000B           ;X<DPL,DPL>XXXXX - ���������� �������, ������ ����� �������� ����� ���

;������� ���� ��������� ������       
code_for_real_mode segment para use16
code_for_real_mode_begin   = $
    assume cs:code_for_real_mode,DS:data,ES:data   ;������������� ��������� 
	
start:
	mov ax, 03 ; ������� ������
	int 10h

    mov ax,data                         ;������������� ���������� ���������
    mov ds,ax                                   
    mov es,ax                          
    lea dx,message_for_get_in_protected_mode
    mov ah,9h
    int 21h
    call input                          ;����� ������� ��� ����� ������

;��������� ����� A20 - �������� ������� ��� ���������� ������ c 32 ������ �������(��� ����������� ������)
    in  al,92h                                                                              
    or  al,2                            ;��������� 1-�� ���� � �������                                                   
    out 92h,al                                
	
	
;��������� ����� ����������     
    in      al,21h
    mov     mask_master,al			    ;�������                 
    in      al,0A1h
    mov     mask_slave,al			    ;�������
	
;������ ����������� � ������������� ����������      
  
    cli                                 ;������ ����������� ����������
    in  al,70h	
	or	al,10000000b                    ;���������� 7 ��� � 1 ��� ������� ������������� ����������
	out	70h,al
	
	
;��������� ���������� ������� ������������            
    mov ax,data				            ;���������� ����� ������ �������� ������ � dl
    mov dl,ah				            ;��������� � dl:ax ���������� �����, ��������������� ����������� ������ data
    xor dh,dh
    shl ax,4				            ;����� ax ����� 
    shr dx,4				            ;����� dx ������
	
    mov si,ax		;��������� ��� ����,����� ������ ����� ������ ��������
    mov di,dx		;��������� ��� ����,����� ������ ����� ������ ��������
	
;��������� ���������� gdt
    lea bx,gdt_gdt
    add ax,offset gdt ; ������� �������� ��� ���������� ��������� ��� ������
    adc dx,0 ; cf=1
    mov [bx][struct_deskriptor.base_low],ax ;����� 32 ������ �� ����� ��� ��� base � ������ ������ ���������
    mov [bx][struct_deskriptor.base_medium],dl
    mov [bx][struct_deskriptor.base_hight],dh
	
 ;��������� ���������� �������� ���� ��������� ������
    lea bx,gdt_code_for_real_mode
    mov ax,cs
    xor dh,dh
    mov dl,ah
    shl ax,4
    shr dx,4
    mov [bx][struct_deskriptor.base_low],ax
    mov [bx][struct_deskriptor.base_medium],dl
    mov [bx][struct_deskriptor.base_hight],dh
	
 ;��������� ���������� �������� ������
    lea bx,gdt_data
    mov ax,si
    mov dx,di
    mov [bx][struct_deskriptor.base_low],ax
    mov [bx][struct_deskriptor.base_medium],dl
    mov [bx][struct_deskriptor.base_hight],dh
	
 ;��������� ���������� �������� �����
    lea bx, gdt_stack
    mov ax,ss
    xor dh,dh
    mov dl,ah
    shl ax,4
    shr dx,4
    mov [bx][struct_deskriptor.base_low],ax
    mov [bx][struct_deskriptor.base_medium],dl
    mov [bx][struct_deskriptor.base_hight],dh
	
  ;��������� ���������� ���� ����������� ������
    lea bx,gdt_code_for_protected_mode
    mov ax,code_for_protected_mode
    xor dh,dh
    mov dl,ah
    shl ax,4
    shr dx,4
    mov [bx][struct_deskriptor.base_low],ax
    mov [bx][struct_deskriptor.base_medium],dl
    mov [bx][struct_deskriptor.base_hight],dh        
    or  [bx][struct_deskriptor.attribs],40h	 ;40h - 10000000 (������ � ���. �������) tasm �� ��������� ������ � bin
	
;��������� ���������� idt
    lea bx,gdt_idt
    mov ax,si
    mov dx,di
    add ax,offset idt
    adc dx,0
    mov [bx][struct_deskriptor.base_low],ax
    mov [bx][struct_deskriptor.base_medium],dl
    mov [bx][struct_deskriptor.base_hight],dh        
    mov idtr.idtr_low,ax
    mov idtr.idtr_hight,dx

;��������� ������� ������������ ������ ����������
    irpc    index, 0                         ;��������� ������ 20 ���������
        lea eax,irg_master_dummy		 ;�������� ��� ���������� ���������� �������� ����������� 
        mov idt_2&index.offset_low, ax;offs_l  � offd_h ������ ��������� �.� ��� �� ����� � ���������
        shr eax,16
        mov idt_2&index.offset_hight, ax
    endm
	
	lea eax, interrupt_for_keyboard            ;��������� ���������� ���������� ���������� �� 21 ����
    mov idt_keyboard.offset_low,ax
    shr eax, 16
    mov idt_keyboard.offset_hight,ax
	
    irpc    index, 345678        			 ;�������� ��� ��������
        lea eax,irg_master_dummy
        mov idt_2&index.offset_low, ax
        shr eax,16
        mov idt_2&index.offset_hight, ax
    endm           
    
    irpc    index, 9ABCDEF              	 ;�������� ��� ��������
        lea eax,irg_slave_dummy
        mov idt_2&index.offset_low,ax
        shr eax,16
        mov idt_2&index.offset_hight,ax
    endm
	
    lgdt fword ptr gdt_gdt               ;��������� ������� gdtr
    lidt fword ptr idtr                  ;��������� ������� idtr
    mov eax,cr0                          ;�������� ����������� ������� ������ ���������� ������� cr0
    or  al,00000001b                     ;���������� ��� pe � 1 - ���������� ������ ������
    mov cr0,eax                          ;�������� ���������� cr0 � ��� ����� �������� ���������� �����
	
;far jmp
    db  0eah	;��� �������� jmp
    dw  $+4		;����� ��������� �������
    dw  code_for_real_mode_descriptor        
	
;������������������ ��������� ���������� �������� �� �����������
    mov ax,data_descriptor
    mov ds,ax                         
    mov es,ax                         
    mov ax,stack_descriptor
    mov ss,ax                         
    xor ax,ax                             
    lldt ax                              ;�������� ������� LDTR - �� ������������ ������� ��������� ������������
	
    push cs                              ;������� ����
    push offset  back_to_real_mode               ;�������� ����� ��������
    lea  edi,to_protected_mode                    ;�������� ����� ����� � ���������� �����
    mov  eax,code_for_protected_mode_descriptor                ;�������� ���������� ���� ����������� ������
    push eax                                
    push edi                                    
	
;������������������ ���������� ���������� 
    mov al,00010000b                     ;ICW1 - ����������������� ����������� ����������
    out 20h,al                    
    out 0A0h,al           
    mov al,20h                           ;ICW2 - ����� �������� ������� ����������
    out 21h,al                           ;�������� �����������
    mov al,28h                          
    out 0A1h,al                          ;�������� �����������
    mov al,04h                           ;ICW3 - ������� ���������� ��������� � 3 �����
    out 21h,al       
    mov al,02h                           ;ICW3 - ������� ���������� ��������� � 3 �����
    out 0A1h,al      
    mov al,11h                           ;ICW4 - ����� ����������� ������ ����������� ��� �������� �����������
    out 21h,al       ;11h-00010001 ��������� ��� -8086 4-master
    mov al,01h                           ;ICW4 - ����� ������� ������ ����������� ��� �������� �����������
    out 0A1h,al       
    mov al, 0                            ;�������������� ����������
    out 21h,al                                 
    out 0A1h,al         
	
;�������� ����������� � ������������� ����������
    in  al,70h	
	and	al,01111111b                     ;���������� 7 ��� � 0 ��� ������� ������������� ����������
	out	70h,al
    sti                                  ;��������� ����������� ����������
	
 ;��������� � �������� ���� ����������� ������
    db 66h                                      
    retf ;push ����� ����� � ���������� �����
	
 back_to_real_mode: ;����� �������� � �������� �����
    cli                                  ;������ ����������
    in  al,70h	                               
	or	al,10000000b                     ;���������� ������������� ����������        
	out	70h,al
	
;������������������ ���������� ����������                                 
    mov al,00010000b                            
    out 20h,al                                  
    out 0A0h,al                                 
    mov al,8h                                   
    out 21h,al                                  
    mov al,70h                                 
    out 0A1h,al     
    mov al,04h      
    out 21h,al       
    mov al,02h           
    out 0A1h,al      
    mov al,11h             
    out 21h,al        
    mov al,01h                   
    out 0A1h,al
	
 ;���������� ���������� �������� ��� �������� � �������� �����          
    mov gdt_code_for_real_mode.limit,0ffffh           ;��������� ������ �������� ����
    mov gdt_data.limit,0ffffh              ;��������� ������ �������� ������
    mov gdt_stack.limit,0ffffh             ;��������� ������ �������� �����
	
    db  0eah                               ;������������� ������� cs
    dw  $+4
    dw  code_for_real_mode_descriptor                       ;�� ������� ���� ��������� ������
	
    mov ax,data_descriptor                       ;�������� ���������� �������� ������������ �������� ������
    mov ds,ax                                   
    mov es,ax                                                                   
    mov ax,stack_descriptor
    mov ss,ax                              ;�������� ������� ����� ������������ �����
	
  ;������� �������� �����
    mov eax,cr0
    and al,11111110b                       ;������� 0 ��� �������� cr0(feh)
    mov cr0,eax 
	
    db  0eah
    dw  $+4;4 �������� ����� ��������
    dw  code_for_real_mode                            ;������������ ������� ����
	
    mov ax,segment_stack
    mov ss,ax                      
    mov ax,data
    mov ds,ax                      
    mov es,ax
    xor ax,ax
    mov idtr.limit, 3ffh              ;������ ������� �������� ����������  
    mov dword ptr  idtr+2, 0            
    lidt fword ptr idtr                 
	
 ;����������� ����� ����������
    mov al,mask_master
    out 21h,al                             ;�������     
    mov al,mask_slave
    out 0A1h,al                            ;�������
	
;�������� ����������� � ������������� ����������
    in  al,70h	
	and	al,01111111b                       ;���������� 7 ��� � 0 ��� ���������� ������������� ����������
	out	70h,al
   ; nop
    sti                                    ;��������� ����������� ����������
	
 ;������� ������� A20
    in  al,92h
    and al,11111101b                       ;�������� 1 ��� - ��������� ����� A20
    out 92h, al
	
;������� �� ���������
    mov ax,03h
    int 10h                              ;������� �����
	
    lea dx,message_for_real_mode
    mov ah,09h
    int 21h                        
    mov ax,4c00h                           ;����� �� ���������
    int 21h               
	
input proc near                            ;��������� ������ ������� ��� ������������ �������
    mov ah, 00h							   ;���� �������
	xor al, al	
	int 16h
	mov input_key, ah

	lea dx,message_for_getch
   	mov ah,9h
   	int 21h
	
	mov ah, 01h							   ;��� ������ ������ ����� ����� ������� �������
	xor al, al						   
    int 21h
return:
    	ret
input endp


size_code_for_real_mode    = $ - code_for_real_mode_begin    ;����� �������� ����
code_for_real_mode ends

;������� ���� ����������� ������
code_for_protected_mode  segment para use32
code_for_protected_mode_begin   = $
    assume cs:code_for_protected_mode,ds:data,es:data      ;�������� ��������� ��� ����������
	
to_protected_mode:                                  ;����� ����� � ���������� �����
    call cls                              
	xor  edi,edi                           ;� edi �������� �� ������
    lea  esi,message_for_protected_mode                  ;� esi ����� ������
    call output         

    add  edi,160                               
    lea  esi,message_for_input
    call output                     ;������� ���� ��� ������ ����-���� ����������
    mov edi,320                
	
while:             
    jmp  while                       ;������� ���������� ����������       
	
exit_from_iterrupt:                       ;����� ������ ��� ������ �������� �� ����������� ����������
    popad
    pop es
    pop ds
    pop eax                                ;����� �� ����� ������ EIP
    pop eax                                ;CS  
    pop eax                                ;� EFLAGS
    sti                                    ;��������� ����������
    db 66H
    retf                                   ;������� � 16-������ ������� ����    

full_value_to_hex proc near                      ;��������� �������� ����� � ���������������� ���
    push ax
    mov ah,al             
    shr al,4              
    call dec_to_hex     
    mov [di],al           
    inc di                
    mov al,ah             
    and al,0Fh            
    call dec_to_hex     
    mov [di],al           
    inc di                
    pop ax
    ret    
full_value_to_hex endp


dec_to_hex proc near                     ;��������� �������� ����� � ���������������� ���
    add al,'0'            
    cmp al,'9'            
    jle end_dec_to_hex           
    add al,7              
end_dec_to_hex:
    ret        
dec_to_hex endp

irg_master_dummy proc near                 ;�������� ��� ���������� �������� �����������
    push eax
    mov  al,20h
    out  20h,al
    pop  eax
    iretd
irg_master_dummy endp

irg_slave_dummy  proc near                 ;�������� ��� ���������� �������� �����������
    push eax
    mov  al,20h
    out  20h,al
    out  0A0h,al
    pop  eax
    iretd
irg_slave_dummy  endp

interrupt_for_keyboard proc near                 ;���������� ���������� �� ����������
    push ds
    push es
    pushad   
	
    in   al, 60h                           ;������� ���� ��� ��������� ������� �������    
	cmp  al, input_key      			   ;���������� ��� ������� � ���, ������� ���� ������ �������               
    je   exit_interrupt                     ;���� ����� �� ����� �� ����������� ������   
    mov  ds:[scan_code],al               
    lea  edi,ds:[buffer_for_scan_code]
    mov  al,ds:[scan_code]
    xor  ah,ah
    call full_value_to_hex                         
    mov  edi,172            ;������ ��� ������ ���������
    lea  esi,buffer_for_scan_code                   
    call output                    
    jmp  return_interrupt 
	
exit_interrupt:
    mov  al,20h
    out  20h,al
    db 0eah
    dd offset exit_from_iterrupt 
    dw code_for_protected_mode_descriptor  
	
return_interrupt:
    mov  al,20h
    out  20h,al                                
    popad 
    pop es
    pop ds
    iretd                              
interrupt_for_keyboard endp

cls  proc near                          ;��������� ������� �������
    push es
    pushad
    mov  ax,text_gdt    
    mov  es,ax
    xor  edi,edi
    mov  ecx,2000                        ;������ ������
    mov  ax,1700h	                       ;������ ������� � ��������� ������ ����� ah-���� al-������
    rep  stosw	                           ;��������� ���� �� ������� �� ���� ��������� ������
    popad
    pop  es
    ret
cls  endp

output proc near                    ;��������� ������ ���������� ������, ��������������� 0
    push es
    pushad
    mov  ax,text_gdt                      ;��������� � es �������� ������
    mov  es,ax
show:                               ;���� �� ������ ������
    lodsb                                       
    cmp al,00h
    je   exit_output                       ;���� ����� �� 0, �� ����� ������
    stosb
    inc  edi
    jmp  show
exit_output:                               ;����� �� ��������� ������
    popad
    pop  es
    ret
output  endp

size_code_for_protected_mode     =       $ - code_for_protected_mode_begin
code_for_protected_mode  ends

;������� ������ ���������/����������� ������
	data    segment para use16                 ;������� ������ ���������/����������� ������ - ���������� ��������� ������, ����....��� ������� � ��� � ���������� ������
	data_begin      = 	$
    ;gdt - ���������� ������� ������������
    gdt_begin  = 	$
    gdt label   word                       ;����� ������ gdt
    gdt_0       struct_deskriptor <0,0,0,0,0,0>            ;��� ��������� ���������� ����������                  
    gdt_gdt     struct_deskriptor <gdt_size-1,,,access_data,0,>             ;����� ��������,,,���� �������, ��������   
    gdt_code_for_real_mode struct_deskriptor <size_code_for_real_mode-1,,,access_code,0,>             ;������� ����(�������� �����)
    gdt_data    struct_deskriptor <size_data-1,,,access_data+access_dpl_three,0,>      
    gdt_stack   struct_deskriptor <1000h-1,,,access_data,0,>                    
    gdt_text    struct_deskriptor <2000h-1,8000h,0Bh,access_data+access_dpl_three,0,0> 
    gdt_code_for_protected_mode struct_deskriptor <size_code_for_protected_mode-1,,,access_code+access_read,0,>    ;������� ����(���������� �����)
    gdt_idt     struct_deskriptor <size_idt-1,,,access_idt,0,>                  
    gdt_size    = ($ - gdt_begin)               ;������ gdt
    
	;���������� ���������
    code_for_real_mode_descriptor = (gdt_code_for_real_mode - gdt_0)
    data_descriptor    = (gdt_data - gdt_0)      
    stack_descriptor   = (gdt_stack - gdt_0)
    text_gdt    = (gdt_text - gdt_0)  
    code_for_protected_mode_descriptor = (gdt_code_for_protected_mode - gdt_0)
    
	;idt - ������� ������������ ����������
    idtr    struct_idtr  <size_idt,0,0>              ;������ �������� ITDR  (�����, ��. � ��. ����� ��������)
    idt     label   word                            ;����� ������ idt
    idt_begin   = $
	
    irpc    index, 0123456789ABCDEF					;������� ������ �� �������� struct_interrupt_deskriptor
        idt_0&index struct_interrupt_deskriptor <0, code_for_protected_mode_descriptor, 0, access_trap, 0>        ; 00...0F   (1--����� �����������,  2--�������� ����, 3--���������, 4--������� �������, 5--����� �����������)
    endm
    irpc    index, 0123456789ABCDEF
        idt_1&index struct_interrupt_deskriptor <0, code_for_protected_mode_descriptor, 0, access_trap, 0>     ; 10...1F
    endm 
    idt_keyboard struct_interrupt_deskriptor <0,code_for_protected_mode_descriptor,0,access_interrupt,0>    	
    irpc    index, 03456789ABCDEF
        idt_2&index        struct_interrupt_deskriptor <0, code_for_protected_mode_descriptor, 0, access_interrupt, 0>  ; 22...2F
    endm	
    size_idt  =  $ - idt_begin
	
    message_for_real_mode            db "Real mode$"
    message_for_protected_mode        db "Protected mode",0
    message_for_input        db "input:",0
    message_for_get_in_protected_mode           db "Press special key for get in protected mode:$"
    message_for_getch       	db 0dh,0ah,"Press key for get in protected mode:$"
	
    mask_master          db  1 dup(?)            ;�������� �������� ����� �������� �����������
    mask_slave          db  1 dup(?)            ;�������� �������� ����� �������� �����������
    scan_code       db  1 dup(?)       
    buffer_for_scan_code    db  2 dup(' '),0      

    input_key           db  1 dup(?)  
	size_data   = $ - data_begin             ;������ �������� ������
data    ends

;������� ����� ���������/����������� ������
segment_stack segment para stack
    db  1000h dup(?)
segment_stack  ends

end start