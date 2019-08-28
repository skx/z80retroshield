;==================================================================================
; Contents of this file are copyright Grant Searle
;
; You have permission to use this for NON COMMERCIAL USE ONLY
; If you wish to use it elsewhere, please include an acknowledgement to myself.
;
; http://searle.hostei.com/grant/index.html
;
; eMail: home.micros01@btinternet.com
;
; If the above don't work, please perform an Internet search to see if I have
; updated the web page hosting service.
;
;==================================================================================

; Minimum 6850 ACIA interrupt driven serial I/O to run modified NASCOM Basic 4.7
; Full input buffering with incoming data hardware handshaking
; Handshake shows full before the buffer is totally filled to allow run-on from the sender

SER_BUFSIZE     EQU     3FH
SER_FULLSIZE    EQU     30H
SER_EMPTYSIZE   EQU     5

serBuf          EQU     02000H
serInPtr        EQU     serBuf+SER_BUFSIZE
serRdPtr        EQU     serInPtr+2
serBufUsed      EQU     serRdPtr+2
basicStarted    EQU     serBufUsed+1
TEMPSTACK       EQU     020EDH ; Top of BASIC line input buffer so is "free ram" when BASIC resets

CR              EQU     0DH
LF              EQU     0AH
CS              EQU     0CH             ; Clear screen

                ORG 0000
;------------------------------------------------------------------------------
; Reset

RST00           DI                       ;Disable interrupts
                JP       INIT            ;Initialize Hardware and go

                db  (0008h-$) dup (0)

;------------------------------------------------------------------------------
; TX a character over RS232

                ORG     0008H
RST08           JP      TXA

                db  (0010h-$) dup (0)

;------------------------------------------------------------------------------
; RX a character over RS232 Channel A [Console], hold here until char ready.

                ORG 0010H
RST10           JP      RXA

                db (0018h - $) dup (0)

;------------------------------------------------------------------------------
; Check serial status

                ORG 0018H
RST18           JP      CKINCHAR

                db (0038h - $) dup (0)

;------------------------------------------------------------------------------
; RST 38 - INTERRUPT VECTOR [ for IM 1 ]

                ORG     0038H
RST38            JR      serialInt

;------------------------------------------------------------------------------
serialInt:       PUSH     AF
                PUSH     HL
        	IN A,(00h)   ;RECEIVE CHAR

                PUSH     AF
                LD       A,(serBufUsed)
                CP       SER_BUFSIZE     ; If full then ignore
                JR       NZ,notFull
                POP      AF
                JR       rts0

notFull:        LD       HL,(serInPtr)
                INC      HL
                LD       A,L             ; Only need to check low byte becasuse buffer<256 bytes
                CP       (serBuf+SER_BUFSIZE) & 0FFH
                JR       NZ, notWrap
                LD       HL,serBuf
notWrap:        LD       (serInPtr),HL
                POP      AF
                LD       (HL),A
                LD       A,(serBufUsed)
                INC      A
                LD       (serBufUsed),A
                CP       SER_FULLSIZE
                JR       C,rts0
rts0:           POP      HL
                POP      AF
                EI
                RETI

;------------------------------------------------------------------------------
RXA:
waitForChar:    LD       A,(serBufUsed)
                CP       00H
                JR       Z, waitForChar
                PUSH     HL
                LD       HL,(serRdPtr)
                INC      HL
                LD       A,L             ; Only need to check low byte becasuse buffer<256 bytes
                CP       (serBuf+SER_BUFSIZE) & 0FFH
                JR       NZ, notRdWrap
                LD       HL,serBuf
notRdWrap:      DI
                LD       (serRdPtr),HL
                LD       A,(serBufUsed)
                DEC      A
                LD       (serBufUsed),A
                CP       SER_EMPTYSIZE
                JR       NC,rts1
rts1:
                LD       A,(HL)
                EI
                POP      HL
                RET                      ; Char ready in A

;------------------------------------------------------------------------------
TXA:            OUT (00h),A
		RET


;------------------------------------------------------------------------------
CKINCHAR        LD       A,(serBufUsed)
                CP       0
                RET

PRINT:          LD       A,(HL)          ; Get character
                OR       A               ; Is it $00 ?
                RET      Z               ; Then RETurn on terminator
                RST      08H             ; Print it
                INC      HL              ; Next Character
                JR       PRINT           ; Continue until $00
                RET
;------------------------------------------------------------------------------
INIT:
               LD        HL,TEMPSTACK    ; Temp stack
               LD        SP,HL           ; Set up a temporary stack
               LD        HL,serBuf
               LD        (serInPtr),HL
               LD        (serRdPtr),HL
               XOR       A               ;0 to accumulator
               LD        (serBufUsed),A

		LD A,4Dh            ; ERTURK changed from 4E to 4D
	NOP
        NOP
                NOP
                NOP
		LD A,37h
	NOP
        NOP

               IM        1
               EI
               LD        HL,SIGNON1      ; Sign-on message
               CALL      PRINT           ; Output string
               LD        A,(basicStarted); Check the BASIC STARTED flag
               CP        'Y'             ; to see if this is power-up
               JR        NZ,COLDSTART    ; If not BASIC started then always do cold start
               LD        HL,SIGNON2      ; Cold/warm message
               CALL      PRINT           ; Output string
CORW:
               CALL      RXA
               AND       11011111B       ; lower to uppercase
               CP        'C'
               JR        NZ, CHECKWARM
               RST       08H
               LD        A,0DH
               RST       08H
               LD        A,0AH
               RST       08H
COLDSTART:     LD        A,'Y'           ; Set the BASIC STARTED flag
               LD        (basicStarted),A
               JP        0150H           ; Start BASIC COLD
CHECKWARM:
               CP        'W'
               JR        NZ, CORW
               RST       08H
               LD        A,0DH
               RST       08H
               LD        A,0AH
               RST       08H
               JP        0153H           ; Start BASIC WARM

SIGNON1:       DB     CS
               DB     "Z80 SBC By Grant Searle",CR,LF,0
SIGNON2:       DB     CR,LF
               DB     "Cold or warm start (C or W)? ",0

                DB (00150H - $) dup (0)

.END
