.code

;void changeSaturation(hsv * pixelsBuffer, int bufferSize, float saturationLvl);
; x64 calling convention
; arg 0 - RCX  - pointer to HSV buffer
; arg 1 - RDX  - buffer size
; arg 2 - xmm2 - saturation level as float

const_divisor dd 255.0

changeSaturation proc

pxor xmm0, xmm0                     ; xmm0 = 0
comiss xmm2, xmm0                   ; check if saturation level is negative
jb loop_desaturation                ; if saturation level is negative, desaturate

loop_saturation:

xor rax, rax                        ; rax = 0

mov al, byte ptr  [rcx+1]           ; get saturation value 1
shl rax, 32                         ; s << 32
mov al, byte ptr  [rcx+5]           ; get saturation value 2
movq mm0, rax                       ; mmx0 = s
CVTPI2PS xmm7, mm0                  ; convert to float
movlhps xmm7, xmm7                  ; shift values from low  qword to high qword

mov al, byte ptr  [rcx+9]           ; get saturation value 3
shl rax, 32                         ; s << 32
mov al, byte ptr  [rcx+13]          ; get saturation value 4
movq mm0, rax                       ; mmx0 = s
CVTPI2PS xmm7, mm0                  ; convert to float

VBROADCASTSS xmm6, const_divisor    ; broadcast 255.0 to all 4 qwords (AVX)
VBROADCASTSS xmm2, xmm2             ; broadcast saturation level to all 4 qwords (AVX)

movaps xmm0, xmm6                   ; xmm0 = 255.0
subps xmm0, xmm7                    ; xmm0 = 255.0 - s
mulps xmm0, xmm2                    ; xmm0 = (255.0 - s) * saturationLevel

movaps xmm3, xmm7                   ; xmm3 = s
divps xmm3, xmm6                    ; xmm3 = s / 255.0
mulps xmm0, xmm3                    ; xmm0 = (255.0 - s) * saturationLevel * (s / 255.0)
addps xmm0, xmm7                    ; xmm0 = (255.0 - s) * saturationLevel * (s / 255.0) + s

CVTTPS2PI mm0, xmm0                 ; convert half of the result to uint64
movq rax, mm0                       ; save it to rax
mov [rcx+13], al                    ; save it to buffer
shr rax, 32                         ; shift right to get the other half
mov [rcx+9], al                     ; save it to buffer

movhlps xmm0, xmm0                  ; shift values from high qword to low qword
CVTTPS2PI mm0, xmm0                 ; convert half of the result to uint64
movq rax, mm0                       ; save it to rax
mov [rcx+5], al                     ; save it to buffer
shr rax, 32                         ; shift right to get the other half
mov [rcx+1], al                     ; save it to buffer

add rcx, 16                         ; increment pointer - we processed 4 pixels (each pixel has 4 bytes)
sub rdx, 4                          ; decrement buffer size
jnz loop_saturation                 ; jump to LOOP if buffer size is not 0

ret                                 ; return


loop_desaturation:

xor rax, rax                        ; rax = 0

mov al, byte ptr  [rcx+1]           ; get saturation value 1
shl rax, 32                         ; s << 32
mov al, byte ptr  [rcx+5]           ; get saturation value 2
movq mm0, rax                       ; mmx0 = s
CVTPI2PS xmm7, mm0                  ; convert to float
movlhps xmm7, xmm7                  ; shift values from low  qword to high qword

mov al, byte ptr  [rcx+9]           ; get saturation value 3
shl rax, 32                         ; s << 32
mov al, byte ptr  [rcx+13]          ; get saturation value 4
movq mm0, rax                       ; mmx0 = s
CVTPI2PS xmm7, mm0                  ; convert to float

VBROADCASTSS xmm2, xmm2             ; broadcast saturation level to all 4 qwords (AVX)

movaps xmm0, xmm7                   ; xmm0 = s
mulps xmm0, xmm2                    ; xmm0 = s * saturationLevel
addps xmm0, xmm7                    ; xmm0 = s * saturationLevel + s

CVTTPS2PI mm0, xmm0                 ; convert half of the result to uint64
movq rax, mm0                       ; save it to rax
mov [rcx+13], al                    ; save it to buffer
shr rax, 32                         ; shift right to get the other half
mov [rcx+9], al                     ; save it to buffer

movhlps xmm0, xmm0                  ; shift values from high qword to low qword
CVTTPS2PI mm0, xmm0                 ; convert half of the result to uint64
movq rax, mm0                       ; save it to rax
mov [rcx+5], al                     ; save it to buffer
shr rax, 32                         ; shift right to get the other half
mov [rcx+1], al                     ; save it to buffer

add rcx, 16                         ; increment pointer - we processed 4 pixels (each pixel has 4 bytes)
sub rdx, 4                          ; decrement buffer size
jnz loop_desaturation               ; jump to LOOP if buffer size is not 0

ret                                 ; return

changeSaturation endp


end