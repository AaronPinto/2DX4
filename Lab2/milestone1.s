; SysTickTestMain.s
; Runs on MSP432E401Y
; Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu

        AREA    |.text|, CODE, READONLY, ALIGN=2
        THUMB
        EXPORT  Start

;Address of Port M Data Register 
GPIO_PORTM_DATA_R    EQU  0x400633FC
GPIO_PORTM_DIR_R     EQU  0x40063400
GPIO_PORTM_DEN_R     EQU  0x4006351C
	
GPIO_PORTN_DATA_R    EQU  0x400643FC
GPIO_PORTN_DIR_R     EQU  0x40064400
GPIO_PORTN_DEN_R     EQU  0x4006451C

counter EQU 0x4

;Addresses of RCGCGPIO_R Register 
SYSCTL_RCGCGPIO_R     EQU 0x400FE608

;Function Defintions
PortM_Init 
	LDR R1, =SYSCTL_RCGCGPIO_R      
    LDR R0, [R1]                    
    ORR R0, R0, #0x800
    STR R0, [R1]                    
    NOP                             ;allow time for clock to stabilize 
	NOP
    LDR R1, =GPIO_PORTM_DIR_R       ;set direction register
    LDR R0, [R1]                    
    ORR R0, R0, #0xC0             
    STR R0, [R1]                    
    LDR R1, =GPIO_PORTM_DEN_R       ;set digital enable register
    LDR R0, [R1]                    
    ORR R0, R0, #0xFF              
    STR R0, [R1]                    
    BX LR
	
PortN_Init 
	LDR R1, =SYSCTL_RCGCGPIO_R      
    LDR R0, [R1]                    
    ORR R0, R0, #0x1000
    STR R0, [R1]                    
    NOP                             ;allow time for clock to stabilize 
	NOP
    LDR R1, =GPIO_PORTN_DIR_R       ;set direction register
    LDR R0, [R1]                    
    ORR R0, R0, #0xFF             
    STR R0, [R1]                    
    LDR R1, =GPIO_PORTN_DEN_R       ;set digital enable register
    LDR R0, [R1]                    
    ORR R0, R0, #0xFF              
    STR R0, [R1]                    
    BX LR

Start
	 BL  PortM_Init
	 BL  PortN_Init
	 LDR R1, =0x0
	 LDR R8, =GPIO_PORTN_DATA_R      
     LDR R9, [R8]
loop3 LDR R4, =GPIO_PORTM_DATA_R      
     LDR R5, [R4]
	 AND R5, R5, #0x0
	 ORR R5, R5, #0x80
	 STR R5, [R4]
loop4 LDR R2, =counter
	 AND R1, R1, #0x0
	 AND R9, R9, #0x0
	 STR R9, [R8]
loop LDR R4, =GPIO_PORTM_DATA_R      
     LDR R5, [R4]
	 AND R6, R5, #0x10
	 CMP R6, #0x0
	 BNE loop
loop2 LDR R4, =GPIO_PORTM_DATA_R      
     LDR R5, [R4]
	 AND R6, R5, #0x10
	 CMP R6, #0x10
	 BNE loop2
	 ;UPDATE STATE LEDs
	 ADD R9, R9, #0x1
	 STR R9, [R8]
	 ;LEFT SHIFT SHIT INTO R1
	 LSL R1, R1, #1
	 AND R7, R5, #0x1 
	 ADD R1, R1, R7
	 SUBS R2, R2, #1
	 BHI loop
	 ;CHECK IF EQUAL TO CODE
	 CMP R1, #0xC
	 BNE loop3
	 AND R1, R1, #0x0 ;Reset R1
	 AND R5, R5, #0x0
	 ORR R5, R5, #0x40
	 STR R5, [R4]
     B   loop4
	 ALIGN                           ; make sure the end of this section is aligned
     END   