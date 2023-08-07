INCLUDE "hardware.inc/hardware.inc"
HeaderLogo equ $00A8
HeaderTitle equ $0134
HeaderMenufacturer equ $013F
HeaderCGBCompat equ $0143
HeaderNewLicensee equ $0144
HeaderSGBFlag equ $0146
HeaderCartType equ $0147
HeaderROMSize equ $0148
HeaderRAMSize equ $0149
HeaderRegionCode equ $014A
HeaderOldLicensee equ $014B
HeaderROMVersion equ $014C
HeaderChecksum equ $014D
HeaderGlobalChecksum equ $014E



SECTION "Boot ROM", ROM0[$000]

EntryPoint:
    ld sp, hStackBottom

    xor a
    ld hl, $9FFF
.clearVRAM
    ld [hld], a
    bit 7, h
    jr nz, .clearVRAM

    ld hl, rNR52
    ld c, LOW(rNR11) ; CH1 length
    ; Enable APU
    ; This sets (roughly) all audio registers to 0
    ld a, $80
    ld [hld], a
    ; hl = rNR51
    ; Set CH1 duty cycle to 25%
    ldh [c], a
    inc c ; ld c, LOW(rNR11) ; CH1 envelope
    ld a, $F3 ; Initial volume 15, 3 decreasing sweep
    ldh [c], a
    ; Route all channels to left speaker, CH2 and CH1 to right speaker
    ld [hld], a
    ; hl = rNR50
    ; Set volume on both speakers to 7, disable VIN on both speakers
    ld a, $77
    ld [hl], a

    ld a, $FC
    ldh [rBGP], a

IF DEF(dmg0)
    ld hl, HeaderLogo
    push hl
    ld de, Logo
.checkLogo
    ld a, [de]
    inc de
    cp [hl]
    jr nz, Lockup
    inc hl
    ld a, l
    cp LOW(HeaderTitle)
    jr nz, .checkLogo
    ld b, HeaderChecksum - HeaderTitle
    ld a, b
.computeChecksum
    add a, [hl]
    inc hl
    dec b
    jr nz, .computeChecksum
    add a, [hl]
    jr nz, Lockup
    pop de ; ld de, HeaderLogo
ELSE
    ld de, HeaderLogo
ENDC
    ld hl, vLogoTiles
.decompressLogo
    ld a, [de]
    call DecompressFirstNibble
    call DecompressSecondNibble
    inc de
    ld a, e
    cp LOW(HeaderTitle)
    jr nz, .decompressLogo

IF !DEF(dmg0)
    ; ld hl, vRTile
    ld de, RTile
    ld b, 8
.copyRTile
    ld a, [de]
    inc de
    ld [hli], a
    inc hl ; Skip every other byte
    dec b
    jr nz, .copyRTile
    ld a, $19
    ld [vMainTilemap + SCRN_VX_B * 8 + 16], a ; 8 rows down, 16 across
ELSE
    ld a, $18
ENDC

    ld hl, vMainTilemap + SCRN_VX_B * 9 + 15
.writeTilemapRow
    ld c, 12
.writeTilemapByte
IF DEF(dmg0)
    ld [hld], a
ENDC
    dec a
    jr z, ScrollLogo
IF !DEF(dmg0)
    ld [hld], a
ENDC
    dec c
    jr nz, .writeTilemapByte
IF DEF(dmg0)
    ; Go to previous row
    ld de, -(SCRN_VX_B - 12)
    add hl, de
ELSE
    ld l, LOW(vMainTilemap + SCRN_VX_B * 8 + 15)
ENDC
    jr .writeTilemapRow


ScrollLogo:
    ; a = 0
    ld h, a ; ld h, 0
    ld a, $64
    ld d, a
    ldh [rSCY], a
    ld a, LCDCF_ON | LCDCF_BLK01 | LCDCF_BGON
    ldh [rLCDC], a
    inc b ; ld b, 1

    ; h = Number of times the logo was scrolled up
    ; d = How many frames before exiting the loop
    ; b = Whether to scroll the logo

.loop
    ld e, 2
IF DEF(dmg0)
    call DelayFrames
ELSE
.delayFrames
    ld c, 12
.waitVBlank
    ldh a, [rLY]
    cp SCRN_Y
    jr nz, .waitVBlank
    dec c
    jr nz, .waitVBlank
    dec e
    jr nz, .delayFrames
ENDC

    ld c, LOW(rNR13) ; CH1 frequency low byte
    inc h
    ld a, h
    ld e, $83
    cp $62
    jr z, .playSound
    ld e, $C1
    cp $64
    jr nz, .dontPlaySound
.playSound
    ld a, e
    ldh [c], a
    inc c ; ld c, LOW(rNR14) ; CH1 frequency high byte
    ; Set frequency to $7XX and restart channel
    ld a, $87
    ldh [c], a
.dontPlaySound
    ldh a, [rSCY]
    sub b
    ldh [rSCY], a
    dec d
    jr nz, .loop

    dec b
IF DEF(dmg0)
    jr nz, Done
ELSE
    jr nz, CheckLogo
ENDC
    ld d, $20
    jr .loop


IF DEF(dmg0)
Lockup:
    ld a, LCDCF_ON | LCDCF_BLK01 | LCDCF_BGON
    ldh [rLCDC], a
.loop
    ld e, 20
    call DelayFrames
    ldh a, [rBGP]
    xor a, $FF
    ldh [rBGP], a
    jr .loop
ENDC


DecompressFirstNibble:
    ld c, a
DecompressSecondNibble:
    ld b, 8 / 2 ; Set all 8 bits of a, "consuming" 4 bits of c
.loop
    push bc
    rl c ; Extract MSB of c
    rla ; Into LSB of a
    pop bc
    rl c ; Extract that same bit
    rla ; So that bit is inserted twice in a (= horizontally doubled)
    dec b
    jr nz, .loop
    ld [hli], a
    inc hl ; Skip second plane
    ld [hli], a ; Also double vertically
    inc hl
    ret


IF DEF(dmg0)
DelayFrames:
    ld c, 12
.loop
    ldh a, [rLY]
    cp SCRN_Y
    jr nz, .loop
    dec c
    jr nz, .loop
    dec e
    jr nz, DelayFrames
    ret
ENDC


; Each tile is encoded using 2 (!) bytes
; How to read: the logo is split into two halves (top and bottom), each half being encoded
;              separately. Each half must be read in columns.
;              So, the first byte is `db %XX.._XXX.`, then `db %XXX._XX.X`, matching the
;              `db $CE, $ED` found in many places. And so on! :)
MACRO logo_row_gfx
    ASSERT _NARG % 4 == 0
    PUSHO
    OPT b.X
    FOR N1, 1, _NARG / 4 + 1 ; N1, N2, N3, and N4 iterate through the 4 equally-sized rows
        DEF N2 = N1 + _NARG / 4
        DEF N3 = N2 + _NARG / 4
        DEF N4 = N3 + _NARG / 4
        db %\<N1>\<N2>, %\<N3>\<N4>
    ENDR
    POPO
ENDM

; Whitespace is not stripped after line continuations until RGBDS v0.6.0, so rows are not indented
    Logo:  logo_row_gfx \
XX.., .XX., ...., ...., ...., ..XX, ...., .XXX, XX.., ...., ...., ...., \
XXX., .XX., ...., ...., ...., ..XX, ...., .XX., .XX., ...., ...., ...., \
XXX., .XX., ...., ...., ...., ..XX, ...., .XX., .XX., ...., ...., ...., \
XX.X, .XX., .XXX, X..., XXXX, ..XX, XXX., .XXX, XX.., .XXX, X..X, X.XX
           logo_row_gfx \                           
XX.X, .XX., XX.., XX.X, X..X, X.XX, ..XX, .XX., .XX., XX.., XX.X, X.XX, \
XX.., XXX., XX.., XX.X, X..X, X.XX, ..XX, .XX., .XX., XX.., XX.., XXXX, \
XX.., XXX., XX.., XX.X, X..X, X.XX, ..XX, .XX., .XX., XX.., XX.., .XX., \
XX.., .XX., .XXX, X..., XXXX, ..XX, XXX., .XXX, XX.., .XXX, X..., XX..


IF !DEF(dmg0)
RTile:
    PUSHO
    OPT b..
    db %........
    db %........
    db %........
    db %........
    db %........
    db %........
    db %........
    db %........
    POPO


CheckLogo:
    ld hl, HeaderLogo
    ld de, Logo
.compare
    ld a, [de]
    inc de
    cp [hl]
.logoFailure
    jr nz, .logoFailure
    inc hl
    ld a, l
    cp LOW(HeaderTitle)
    nop
    nop

    ld b, HeaderChecksum - HeaderTitle
    ld a, b
.computeChecksum
    add a, [hl]
    inc hl
    dec b
    jr nz, .computeChecksum
    add a, [hl]
.checksumFailure
    nop
    nop

    IF DEF(mgb)
    ld a, $FF
    ELSE
    ld a, 1
    ENDC

ELSE
    ds 2
Done:
    inc a
ENDC
    ldh [$FF50], a
    assert @ == $100 ; Execution now falls through to the cartridge's header



SECTION "VRAM tiles", VRAM[$8000],BANK[0]

vBlankTile:
    ds $10
vLogoTiles:
    ds $10 * (HeaderTitle - HeaderLogo) / 2
vRTile:
    ds $10

SECTION "VRAM tilemap", VRAM[$9800],BANK[0]

vMainTilemap:
    ds SCRN_VX_B * SCRN_VY_B


SECTION "HRAM", HRAM[$FFEE]

    ds $10
hStackBottom:
