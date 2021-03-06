/* ���: usbdrvasm165.inc
 * ������: AVR USB driver
 * �����: Christian Starkjohann
 * �������: microsin.ru 
 * ���� ��������: 2007-04-22
 * ���������: 4
 * Copyright: (c) 2007 by OBJECTIVE DEVELOPMENT Software GmbH
 * ��������: GNU GPL v2 (��. License.txt) ��� ������������� (CommercialLicense.txt)
 * �������: $Id: usbdrvasm165.inc 607 2008-05-13 15:57:28Z cs $
 */

/* �� �������� ���� ����! ������ ����� �������� usbdrvasm.S, ������� ������ include
 *  ����� � ������ ����������� (��������� �� �������� �������)!
 */

/*
�������� ��������:
���� ���� �������� 16 ��� ������� ������������ ����� �������� USB. �� ������������
��� ATTiny45 � �������� �����������������, ���������� �� ������� 16.5 ��� �����������
RC-����������. ��� ������ �������� ������� �������������� ������� � ��������� 
���������, ����� ���������� � ���������� �������� ������� +/- 1%.

��. usbdrv.h ��� ������ �������� ��������.

��������� ����� ���� ��� �������� �� ������� ����������, �� ������� ���, ���� ��
�� ������������� ���������, ��� �������! ��������� ����� �� ������ ������� ������������
��������� ������ CPU, �� � ������ ���������� ������!
*/

;���� ������������ ��������� ������. ������� ����� �������! �� �������, ���� �� ������ ��������� �������!
;����� ������ ����������: 4 ����� + insn ������ = 7 max ���� ���������� ���������
;max ���������� �������� ����������: 59 ������ -> max 52 ������ ������� ����������
;max ������������� �����: [ret(2), r0, SREG, YL, YH, shift, x1, x2, x3, x4, cnt] = 12 ����
;����������� �������: 16.5 ��� -> 11 ������ �� ���
; 16.3125 ��� < F_CPU < 16.6875 ��� (+/- 1.1%)
; ������ � ������� - ����� ����������� �� ������ ���������� ���� sync
; ����� ���������� ��������


USB_INTR_VECTOR:
;������� ���������� ���������: YL, SREG [sofError], r0, YH, shift, x1, x2, x3, x4, cnt
    push    YL                  ;[-23] push ������ ���� ����� �����. �� ������ ASAP
    in      YL, SREG            ;[-21]
    push    YL                  ;[-20]
;----------------------------------------------------------------------------
; ������������� � ��������� sync:
;----------------------------------------------------------------------------
;������� sync-����� (D-) �� �������� (LSb) �� �������� (MSb) ����: 01010100 [1 = �������� (idle) = J, 0 = K]
;sync �� J �� ����� K �� ����� �������� sync -- ������������� ����� ������� ������
;������ ����� �� ����� ��������, ��������� ��� ������� IDLE ��� SE1 (== ���������)
waitForJ:
    sbis    USBIN, USBMINUS     ;[-18] ������� D- == 1
    rjmp    waitForJ
waitForK:
;��������� ��� �������� � ���� ������� < 1/4 ����, ������� ������������� ������������.
    sbis    USBIN, USBMINUS     ;[-15]
    rjmp    foundK              ;[-14]
    sbis    USBIN, USBMINUS
    rjmp    foundK
    sbis    USBIN, USBMINUS
    rjmp    foundK
    sbis    USBIN, USBMINUS
    rjmp    foundK
    sbis    USBIN, USBMINUS
    rjmp    foundK
    sbis    USBIN, USBMINUS
    rjmp    foundK
#if USB_COUNT_SOF
    lds     YL, usbSofCount
    inc     YL
    sts     usbSofCount, YL
#endif  /* USB_COUNT_SOF */
    rjmp    sofError
foundK:                         ;[-12]
;{3, 5} ����� ����� �� D- ������� ��������: 4 ����� [��� ����� 5 ��� ������������� �������]
; �������. ����� � ������� - ����� �� ������ ������� sync (������� K) ���� 
; ����� ����������
    push    r0                  ;[-12]
;   [---]                       ;[-11]
    push    YH                  ;[-10]
;   [---]                       ;[-9]
    lds     YL, usbInputBufOffset;[-8]
;   [---]                       ;[-7]
    clr     YH                  ;[-6]
    subi    YL, lo8(-(usbRxBuf));[-5] [������������� ����� rx]
    sbci    YH, hi8(-(usbRxBuf));[-4] [������������� ����� rx]
    mov     r0, x2              ;[-3] [������������� ����� rx]
    sbis    USBIN, USBMINUS     ;[-2] ��� ����� 2 ���� K (������� 2 ����� ������� ������)
    rjmp    haveTwoBitsK        ;[-1]
    pop     YH                  ;[0] ������ ����������� push
    pop     r0                  ;[2]
    rjmp    waitForK            ;[4] ��� �� ��� ����� sync, ������� ��� ���
; ���� ������� �� waitForK �� rjmp waitForK ���� �� ������ ��������� ��� ���� �������
;  (= 22 �����).

;----------------------------------------------------------------------------
; ��������� �������� � �������������� ����������, ����� �� �������� ������ ����:
;----------------------------------------------------------------------------
haveTwoBitsK:               ;[1]
    push    shift           ;[1]
    push    x1              ;[3]
    push    x2              ;[5]
    push    x3              ;[7]
    ldi     shift, 0xff     ;[9] [������������� ����� rx]
    ori     x3, 0xff        ;[10] [������������� ����� rx] == ��������� x3, ������� ����� ����

    in      x1, USBIN       ;[11] <-- ������� ���� 0
    bst     x1, USBMINUS    ;[12]
    bld     shift, 0        ;[13]
    push    x4              ;[14] == ����
;   [---]                   ;[15]
    push    cnt             ;[16]
;   [---]                   ;[17]
    ldi     phase, 0        ;[18] [������������� ����� rx]
    ldi     cnt, USB_BUFSIZE;[19] [������������� ����� rx]
    rjmp    rxbit1          ;[20]
;   [---]                   ;[21]

;----------------------------------------------------------------------------
; ���� ��������� (����� � ������� ���������� ����� ������ ����� ����� ����������)
;----------------------------------------------------------------------------
/*
����-��������������� �������� �� ����� �����:
bit 0: ���������� ������
bit 1: �������� SE0
bit 2: �������� ������������
bit 3: �������
bit 4: rjmp ��� ���������� �������������� ��������� ��������
bit 5: PLL (Phase Locked Loop, ���� - ������� �������������� �������)
bit 6: �������
bit 7: �������, �������� bitstuff
; 87 [+ 2] �����
------------------------------------------------------------------
*/
continueWithBit5:
    in      x2, USBIN       ;[055] <-- ��� 5
    eor     r0, x2          ;[056]
    or      phase, r0       ;[057]
    sbrc    phase, USBMINUS ;[058]
    lpm                     ;[059] �������������� nop3; ������ r0
    in      phase, USBIN    ;[060] <-- ����
    eor     x1, x2          ;[061]
    bst     x1, USBMINUS    ;[062]
    bld     shift, 5        ;[063]
    andi    shift, 0x3f     ;[064]
    in      x1, USBIN       ;[065] <-- ��� 6
    breq    unstuff5        ;[066] *** unstuff �����
    eor     phase, x1       ;[067]
    eor     x2, x1          ;[068]
    bst     x2, USBMINUS    ;[069]
    bld     shift, 6        ;[070]
didUnstuff6:                ;[   ]
    in      r0, USBIN       ;[071] <-- ����
    cpi     shift, 0x02     ;[072]
    brlo    unstuff6        ;[073] *** unstuff �����
didUnstuff5:                ;[   ]
    nop2                    ;[074]
;   [---]                   ;[075]
    in      x2, USBIN       ;[076] <-- ��� 7
    eor     x1, x2          ;[077]
    bst     x1, USBMINUS    ;[078]
    bld     shift, 7        ;[079]
didUnstuff7:                ;[   ]
    eor     r0, x2          ;[080]
    or      phase, r0       ;[081]
    in      r0, USBIN       ;[082] <-- ����
    cpi     shift, 0x04     ;[083]
    brsh    rxLoop          ;[084]
;   [---]                   ;[085]
unstuff7:                   ;[   ]
    andi    x3, ~0x80       ;[085]
    ori     shift, 0x80     ;[086]
    in      x2, USBIN       ;[087] <-- ������� stuffed ���� 7
    nop                     ;[088]
    rjmp    didUnstuff7     ;[089]
;   [---]                   ;[090]
                            ;[080]

unstuff5:                   ;[067]
    eor     phase, x1       ;[068]
    andi    x3, ~0x20       ;[069]
    ori     shift, 0x20     ;[070]
    in      r0, USBIN       ;[071] <-- ����
    mov     x2, x1          ;[072]
    nop                     ;[073]
    nop2                    ;[074]
;   [---]                   ;[075]
    in      x1, USBIN       ;[076] <-- ��� 6
    eor     r0, x1          ;[077]
    or      phase, r0       ;[078]
    eor     x2, x1          ;[079]
    bst     x2, USBMINUS    ;[080]
    bld     shift, 6        ;[081] �� ����� ���������, ������ ��� ��� ����
    in      r0, USBIN       ;[082] <-- phase
    rjmp    didUnstuff5     ;[083]
;   [---]                   ;[084]
                            ;[074]

unstuff6:                   ;[074]
    andi    x3, ~0x40       ;[075]
    in      x1, USBIN       ;[076] <-- ����� ��� 6
    ori     shift, 0x40     ;[077]
    nop2                    ;[078]
;   [---]                   ;[079]
    rjmp    didUnstuff6     ;[080]
;   [---]                   ;[081]
                            ;[071]

unstuff0:                   ;[013]
    eor     r0, x2          ;[014]
    or      phase, r0       ;[015]
    andi    x2, USBMASK     ;[016] �������� SE0
    in      r0, USBIN       ;[017] <-- ����
    breq    didUnstuff0     ;[018] ������ ������� � se0 ��� �� ������� �������
    andi    x3, ~0x01       ;[019]
    ori     shift, 0x01     ;[020]
    mov     x1, x2          ;[021] ���������� ��������� �������
    in      x2, USBIN       ;[022] <-- ����� ��� 1
    rjmp    didUnstuff0     ;[023]
;   [---]                   ;[024]
                            ;[014]

unstuff1:                   ;[024]
    eor     r0, x1          ;[025]
    or      phase, r0       ;[026]
    andi    x3, ~0x02       ;[027]
    in      r0, USBIN       ;[028] <-- ����
    ori     shift, 0x02     ;[029]
    mov     x2, x1          ;[030]
    rjmp    didUnstuff1     ;[031]
;   [---]                   ;[032]
                            ;[022]

unstuff2:                   ;[035]
    eor     r0, x2          ;[036]
    or      phase, r0       ;[037]
    andi    x3, ~0x04       ;[038]
    in      r0, USBIN       ;[039] <-- ����
    ori     shift, 0x04     ;[040]
    mov     x1, x2          ;[041]
    rjmp    didUnstuff2     ;[042]
;   [---]                   ;[043]
                            ;[033]

unstuff3:                   ;[043]
    in      x2, USBIN       ;[044] <-- ����� ��� 3
    eor     r0, x2          ;[045]
    or      phase, r0       ;[046]
    andi    x3, ~0x08       ;[047]
    ori     shift, 0x08     ;[048]
    nop                     ;[049]
    in      r0, USBIN       ;[050] <-- ����
    rjmp    didUnstuff3     ;[051]
;   [---]                   ;[052]
                            ;[042]

unstuff4:                   ;[053]
    andi    x3, ~0x10       ;[054]
    in      x1, USBIN       ;[055] <-- ����� ��� 4
    ori     shift, 0x10     ;[056]
    rjmp    didUnstuff4     ;[057]
;   [---]                   ;[058]
                            ;[048]

rxLoop:                     ;[085]
    eor     x3, shift       ;[086] �������������: x3 0 � ����������������� ����, ������� �� ��������, 1 ��� ������
    in      x1, USBIN       ;[000] <-- ��� 0
    st      y+, x3          ;[001]
;   [---]                   ;[002]
    eor     r0, x1          ;[003]
    or      phase, r0       ;[004]
    eor     x2, x1          ;[005]
    in      r0, USBIN       ;[006] <-- ����
    ser     x3              ;[007]
    bst     x2, USBMINUS    ;[008]
    bld     shift, 0        ;[009]
    andi    shift, 0xf9     ;[010]
rxbit1:                     ;[   ]
    in      x2, USBIN       ;[011] <-- ��� 1
    breq    unstuff0        ;[012] *** unstuff �����
    andi    x2, USBMASK     ;[013] SE0 �������� ��� ���� 1
didUnstuff0:                ;[   ] Z ��������������� ������ ���� �� ������������� SE0 � bitstuff
    breq    se0             ;[014]
    eor     r0, x2          ;[015]
    or      phase, r0       ;[016]
    in      r0, USBIN       ;[017] <-- ����
    eor     x1, x2          ;[018]
    bst     x1, USBMINUS    ;[019]
    bld     shift, 1        ;[020]
    andi    shift, 0xf3     ;[021]
didUnstuff1:                ;[   ]
    in      x1, USBIN       ;[022] <-- ��� 2
    breq    unstuff1        ;[023] *** unstuff �����
    eor     r0, x1          ;[024]
    or      phase, r0       ;[025]
    subi    cnt, 1          ;[026] �������� ������������
    brcs    overflow        ;[027]
    in      r0, USBIN       ;[028] <-- ����
    eor     x2, x1          ;[029]
    bst     x2, USBMINUS    ;[030]
    bld     shift, 2        ;[031]
    andi    shift, 0xe7     ;[032]
didUnstuff2:                ;[   ]
    in      x2, USBIN       ;[033] <-- ��� 3
    breq    unstuff2        ;[034] *** unstuff �����
    eor     r0, x2          ;[035]
    or      phase, r0       ;[036]
    eor     x1, x2          ;[037]
    bst     x1, USBMINUS    ;[038]
    in      r0, USBIN       ;[039] <-- ����
    bld     shift, 3        ;[040]
    andi    shift, 0xcf     ;[041]
didUnstuff3:                ;[   ]
    breq    unstuff3        ;[042] *** unstuff �����
    nop                     ;[043]
    in      x1, USBIN       ;[044] <-- ��� 4
    eor     x2, x1          ;[045]
    bst     x2, USBMINUS    ;[046]
    bld     shift, 4        ;[047]
didUnstuff4:                ;[   ]
    eor     r0, x1          ;[048]
    or      phase, r0       ;[049]
    in      r0, USBIN       ;[050] <-- ����
    andi    shift, 0x9f     ;[051]
    breq    unstuff4        ;[052] *** unstuff �����
    rjmp    continueWithBit5;[053]
;   [---]                   ;[054]

macro POP_STANDARD ; 16 ������
    pop     cnt
    pop     x4
    pop     x3
    pop     x2
    pop     x1
    pop     shift
    pop     YH
    pop     r0
endm
macro POP_RETI     ; 5 ������
    pop     YL
    out     SREG, YL
    pop     YL
endm

#include "asmcommon.inc"


; ������������ USB �������:
; idle = J
; J = (D+ = 0), (D- = 1)
; K = (D+ = 1), (D- = 0)
; ������������ ��������� 7.5 ��� �� EOP �� SOP ��� �������

bitstuff7:
    eor     x1, x4          ;[4]
    ldi     x2, 0           ;[5]
    nop2                    ;[6] C ����� ���� (���������� brcc)
    rjmp    didStuff7       ;[8]

bitstuffN:
    eor     x1, x4          ;[5]
    ldi     x2, 0           ;[6]
    lpm                     ;[7] 3 ����� NOP, ������������ r0
    out     USBOUT, x1      ;[10] <-- �����
    rjmp    didStuffN       ;[0]

#define bitStatus   x3

sendNakAndReti:
    ldi     cnt, USBPID_NAK ;[-19]
    rjmp    sendCntAndReti  ;[-18]
sendAckAndReti:
    ldi     cnt, USBPID_ACK ;[-17]
sendCntAndReti:
    mov     r0, cnt         ;[-16]
    ldi     YL, 0           ;[-15] R0 ����� ����� 0
    ldi     YH, 0           ;[-14]
    ldi     cnt, 2          ;[-13]
;   rjmp    usbSendAndReti      �������������

;usbSend:
;��������� �� ������ � 'Y'
;���������� ���� � 'cnt' -- ������� ���� sync [�������� 2 ... 12]
;������������: x1...x4, shift, cnt, Y
;����� � ������� ���������� ����� � ��� ���, ��� ������ ��� ����������������� ������� (sync pattern) ������
usbSendAndReti:             ; 12 ������ ���� �� �������� SOP
    in      x2, USBDDR      ;[-12]
    ori     x2, USBMASK     ;[-11]
    sbi     USBOUT, USBMINUS;[-10] ���������� ��������� �������� (idle); D+ � D- ������ ���� � 0 (��� ����������� ���������� (pullups))
    in      x1, USBOUT      ;[-8] �������������� ����� ��� ����� �������� (tx)
    out     USBDDR, x2      ;[-7] <- ��������� ����
; �� ����� ���������������� x2 (������� bitstuff (������� ���������������� ���)), ��������� sync �������� � 0
    ldi     x4, USBMASK     ;[-6] ����� exor (����������� ���)
    ldi     shift, 0x80     ;[-5] ���� sync - ������ �� ������������ ����
    ldi     bitStatus, 0xff ;[-4] �������������� ������� �����/���, �������� ��� ���������� �� 12 ����
byteloop:
bitloop:
    sbrs    shift, 0        ;[8] [-3]
    eor     x1, x4          ;[9] [-2]
    out     USBOUT, x1      ;[10] [-1] <-- �����
    ror     shift           ;[0]
    ror     x2              ;[1]
didStuffN:
    cpi     x2, 0xfc        ;[2]
    brcc    bitstuffN       ;[3]
    nop                     ;[4]
    subi    bitStatus, 37   ;[5] 256 / 7 ~=~ 37
    brcc    bitloop         ;[6] ����� �� �������� ����, bitStatus ����� ����� ��������� ��������
    sbrs    shift, 0        ;[7]
    eor     x1, x4          ;[8]
    ror     shift           ;[9]
didStuff7:
    out     USBOUT, x1      ;[10] <-- �����
    ror     x2              ;[0]
    cpi     x2, 0xfc        ;[1]
    brcc    bitstuff7       ;[2]
    ld      shift, y+       ;[3]
    dec     cnt             ;[5]
    brne    byteloop        ;[6]
;make SE0:
    cbr     x1, USBMASK     ;[7] ���������� SE0 [������������ �������, ��� EOP ����� ���� �� 21 �� 25 ������]
    lds     x2, usbNewDeviceAddr;[8]
    lsl     x2              ;[10] �� ���������� � ����� ��������� �������
    out     USBOUT, x1      ;[11] <-- ����� SE0-- ������ �� 2 bits==20 ������ ���� ���� � ��������� �������� (idle)
;2006-03-06: ���������� �������� ������ ������ � usbDeviceAddr �� ���� C � ���������:
;��������� ������ ������ ����� ����, ��� ����� ������ ���������, �� ����� ����������� (handshake)
    subi    YL, 2           ;[0] ������ ��������� ����� �� ������ ������, ��� ACK/NAK � r0
    sbci    YH, 0           ;[1]
    breq    skipAddrAssign  ;[2]
    sts     usbDeviceAddr, x2; ���� �� ���������: SE0 �� ���� ���� �������
skipAddrAssign:
;����� �������� usbDeviceAddress
    ldi     x2, 1<<USB_INTR_PENDING_BIT;[4] int0 ��������� �� ����� TX -- ������� ����� �������� �������
    USB_STORE_PENDING(x2)   ;[5]
    ori     x1, USBIDLE     ;[6]
    in      x2, USBDDR      ;[7]
    cbr     x2, USBMASK     ;[8] ��������� ����� ����� �� ����
    mov     x3, x1          ;[9]
    cbr     x3, USBMASK     ;[10] ������������� ���������� ����������� ���������� (pullup) �� ����� ������
    ldi     x4, 4           ;[11]
se0Delay:
    dec     x4              ;[12] [15] [18] [21]
    brne    se0Delay        ;[13] [16] [19] [22]
    out     USBOUT, x1      ;[23] <-- ����� J (idle) -- ��������� SE0 (������ EOP)
    out     USBDDR, x2      ;[24] <-- ������ ������������ ����
    out     USBOUT, x3      ;[25] <-- �������� � ���, ��� ��� �������� ����������� (pull-up) ����������
    rjmp    doReturn
