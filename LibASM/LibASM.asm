
; Project:   Application responsible for changing color saturation in images.
;            Program uses stb_image library to load and save images and Qt library to implement user interface.
;
; Algorithm: Algorithm is based on the HSV color model. For each pixel of the loaded image, saturation is changed by adding to it its value multiplied
;            by a given factor which is calculated based on the slider value representing the percentage of change. Slider value is then clamped to the range of 0 to 1
;            (for increasing the saturation) or to the range of -1 to 0 if a decrease in saturation was selected. The new pixel saturation value can be a maximum of 255 and a minimum of 0.
;            Modified pixels by the given saturation value are then converted to the RGB model and saved to a new image.
;
; Author:     Wiktoria Marczyk
; Version:    1.0
;
; sem. 5, 2022/2023


.code

; void changeSaturation(hsv * pixelsBuffer, int bufferSize, float saturationLvl);
; x64 calling convention
; arg 0 - RCX  - pointer to HSV buffer
; arg 1 - RDX  - buffer size
; arg 2 - xmm2 - saturation level as float: (-1;0) for desaturation and (0;1) for saturation

const_divisor dd 255.0

changeSaturation proc

pxor xmm0, xmm0                     ; xmm0 = 0
comiss xmm2, xmm0                   ; check if saturation level is 0
jz end_procedure                    ; if saturation level is 0, return
comiss xmm2, xmm0                   ; check if saturation level is negative
jb loop_desaturation                ; if saturation level is negative, desaturate

loop_saturation:

xor rax, rax                        ; rax = 0

mov al, byte ptr  [rcx+1]           ; get saturation value 1
shl rax, 32                         ; s << 32
mov al, byte ptr  [rcx+5]           ; get saturation value 2
movq mm0, rax                       ; mmx0 = s
CVTPI2PS xmm7, mm0                  ; convert to float
movlhps xmm7, xmm7                  ; shift values from low qword to high qword

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
movlhps xmm7, xmm7                  ; shift values from low qword to high qword

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


end_procedure:

ret                                 ; return


changeSaturation endp

end