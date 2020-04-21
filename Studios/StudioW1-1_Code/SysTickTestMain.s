; SysTickTestMain.s
; Runs on MSP432E401Y
; Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu

        IMPORT   PLL_Init

        AREA    |.text|, CODE, READONLY, ALIGN=2
        THUMB
        EXPORT  Start

;Addresses of the three SysTick Timer Registers
NVIC_ST_CTRL_R        EQU 0xE000E010  ;The STCTRL    Regsister
NVIC_ST_RELOAD_R      EQU 0xE000E014  ;The STRELOAD  Register 
NVIC_ST_CURRENT_R     EQU 0xE000E018  ;The STCURRENT Register 

;Counter value, maximum counter that can be loaded 
NVIC_ST_RELOAD_M      EQU 0x00FFFFFF  ; Counter load value ( maximum value that can be loaded in the counter)
NVIC_ST_CTRL_COUNT    EQU 0x00010000  ; Count flag

;Address of Port   Data Register 
GPIO_PORTX_DATA_R    EQU 0x400633FC  
GPIO_PORTX_DIR_R     EQU 0x40063400 
GPIO_PORTX_DEN_R     EQU 0x4006351C 

;Addresses of RCGCGPIO_R Register 
SYSCTL_RCGCGPIO_R    EQU 0x400FE608


;Function Defintions 
;------------SysTick_Init------------
SysTick_Init
    LDR R1, =NVIC_ST_CTRL_R     
    MOV R0, #0                      
    STR R0, [R1]                    
    LDR R1, =NVIC_ST_RELOAD_R       
    LDR R0, =NVIC_ST_RELOAD_M      
    STR R0, [R1]                    
    LDR R1, =NVIC_ST_CURRENT_R      
    MOV R0, #0                      
    STR R0, [R1]                    
    LDR R1, =NVIC_ST_CTRL_R         
    MOV R0, #0x05
    STR R0, [R1]                    
    BX  LR                          

;------------SysTick_Wait------------
; Input: R0  delay parameter in units of the core clock (units of 8.333 nsec for 120 MHz clock)
SysTick_Wait
    LDR R1, =NVIC_ST_CURRENT_R      ; R1 = &NVIC_ST_CURRENT_R, ; get the starting time (R2)
    LDR R2, [R1]                    ; R2 = [R1] = startTime
SysTick_Wait_loop
    LDR R3, [R1]                    ; R3 = [R1] = currentTime
    SUB R3, R2, R3                  ; R3 = R2 - R3 = startTime - currentTime
    AND R3, R3, #0x00FFFFFF         ; handle possible counter roll over by converting to 24-bit subtraction
    CMP R3, R0                      ; is elapsed time (R3) <= delay (R0)?
    BLS SysTick_Wait_loop
    BX  LR                          ; return

;------------SysTick_Wait10ms------------
; Assumes  120 MHz Clock
; Input: R0  number of times to wait 10 ms before returning
DELAY10MS             EQU 1200000   ; clock cycles in 10 ms (assumes 120 MHz clock)
SysTick_Wait10ms
    PUSH {R4, LR}                   ; save current value of R4 and LR
    MOVS R4, R0                     ; R4 = R0 = remainingWaits
    BEQ SysTick_Wait10ms_done       ; R4 == 0, done
SysTick_Wait10ms_loop
    LDR R0, =DELAY10MS              ; R0 = DELAY10MS
    BL  SysTick_Wait                ; wait 10 ms
    SUBS R4, R4, #1                 ; R4 = R4 - 1; remainingWaits--
    BHI SysTick_Wait10ms_loop       ; if(R4 > 0), wait another 10 ms
SysTick_Wait10ms_done
    POP {R4, LR}                    ; restore previous value of R4 and LR
    BX  LR                          ; return

PortX_Init
    LDR R1, =SYSCTL_RCGCGPIO_R      
    LDR R0, [R1]                    
    ORR R0, R0, #0x800
    STR R0, [R1]                    
    NOP                             ;allow time for clock to stabilize 
    NOP
    LDR R1, =GPIO_PORTX_DIR_R       ;set direction register
    LDR R0, [R1]                    
    ORR R0, R0, #0x02               
    STR R0, [R1]                    
    LDR R1, =GPIO_PORTX_DEN_R       ;set digital enable register
    LDR R0, [R1]                    
    ORR R0, R0, #0x02               
    STR R0, [R1]                    
    BX LR
    BX LR

Start
     BL  PLL_Init                    ; Set system clock to 120 MHz
     BL  SysTick_Init                ; Initalize SysTick Timer     
	 BL  PortX_Init 
     LDR R4, =GPIO_PORTX_DATA_R      
     LDR R5, [R4]                    
loop EOR R5, R5, #0x02               ;May need to change this depending on what pin that you use
     STR R5, [R4]                    
     MOV R0, #20                      ;Constant Determines how much times the SysTick_Wait10ms is called
     BL  SysTick_Wait10ms
     B   loop
	 ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
