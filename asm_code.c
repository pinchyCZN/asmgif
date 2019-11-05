
char buffer[640*480*4];
char buf2[640*480*4];


#define RESX 640
#define RESY 480

BYTE plasma[0x10000]={0};

int test_asm()
{
__asm{

; Brains canyon - 256b intro for the last Riverwash
; code - TomCat/Abaddon


	lea edi,buf2
	lea esi,buffer

 MOV    BX,13FH
texture:
 MOV    DI,512+RESX*4   ; generating plasma texture
l1:
 MOV    CL,0
 PUSH   0301H
 PUSH   0402H
 PUSH   0207H
l2:
 POP    DX
 IMUL   AX,DX,127
lX:
 MOV    AH,DH
 ;AAD   243
 __emit(0xd5)
 __emit(0xf3)

lY:
 MOV    [eDI],AX
 FLDPI
 FIMUL  [edi]
 FIDIV  [eSI]
 FSIN
 FIMUL  [eSI]
 FISTP  WORD [eDI]
 ADD    CL,[eDI]
 TEST   SP,SP
 JPE    l2
 CMP    CL,BL
 SALC
 STOSB                          ; value 255 or zero
 INC    BYTE [SI-256+lX-1]
 JNZ    l1
 DEC    BYTE [SI-256+lY-1]
 JNZ    l1

;640x480x32bits video mode:
; #13Fh - VMware
; #121h - ATI/AMD
; #112h - nVidia/Intel/DOSBox
setvideo:
 MOV    AX,4F02H
 INT    10H
 SHR    BL,1
 INC    BX
 INC    BX
 CMP    AH,BH
 JE     setvideo
 PUSH   0A000H
 POP    ES

nextframe:
 SUB    DI,DI
 MOV    CL,RESY/2+1
nextline:
 PUSH   CX
 SUB    BP,BP
nextpixel:
 MOV    SI,515
 MOV    CL,[FS:46CH]

 MOV    [SI],BP
 FLDPI
 FIMUL  WORD [SI]
 FIDIV  WORD [SI-515+_div-2]
 FSIN                             ; sin(0...PI/2)
 FIMUL  WORD [SI-515+_mul-2]      ; multiplication by 128
 FISTP  WORD [SI]
 MOV    BX,[SI]
 SUB    BL,CL
 SUB    BL,CL
 SUB    BH,[0FFFCH]

 AND    CH,0
_flip:
 CMP    BYTE [BX],DH
 JZ     @F
 MOV    CH,[SI]
 SUB    CL,[SI]
@@:
 MOV    AL,BH
 SUB    AL,120
 IMUL   AL
 MOV    BX,AX
 MOV    AX,BP
 SHR    AX,3
 MUL    AL
 ADD    AX,BX
 MOV    BX,9500+314H
_step:
 TEST   BX,BX
 JS     @F
 CMP    AX,BX
 JNB    @F
 MOV    CH,0
@@:

 MOV    BX,RESX*4/2-4-1
 SUB    BX,BP

 PUSH   DX                      ; color phases (~0, 120 and 240 degree)
 PUSH   -92
 PUSH   79
nextcolor:
 POP    AX
 ADD    AL,CL
 CBW
 XOR    AL,AH
 MUL    CH
 SHR    AX,6

 ADD    AL,[SI+BX]              ; vertical blur
 RCR    AL,1
 MOV    [SI+BX],AL
 MOV    [SI+BP+RESX*4/2-1],AL   ; horizontal mirror
 DEC    SI
 JPO    nextcolor               ; 3 color components (RGB)

 SUB    BP,SP
 INC    BX
 JNZ    nextpixel

twice:
 PUSH   SI
nextchunk:
 TEST   DI,DI                   ; 2 lines x 5 chunks = 5120 bytes
 JNZ    nobank
 MOV    AX,4F05H
 INT    10H
 INC    DX
nobank:
 MOV    CX,128
_mul:
 REP    MOVSD                   ; twice as fast than REP MOVSW
 CMP    SI,2560+1
_div:
 JB     nextchunk
 POP    SI
 DEC    BP
 JPE    twice                   ; duplicate scanline

 POP    CX
 DEC    CX
 JNZ    nextline
 SUB    WORD [SI-512+_step-2],DX
 JNZ    @F
 NOT    BYTE [SI-512+_flip-1]
@@:
 CWD

 IN     AL,60H                  ; check keypress
 DEC    AX
 JNZ    nextframe

 MOV    AL,3                    ; restore text mode
 INT    10H
RETN
*/
}

}