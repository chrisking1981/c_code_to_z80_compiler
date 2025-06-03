#include <stdint.h>

// Extern declarations for variables/functions
extern uint8_t wWhichAnimationOffsets;
extern uint8_t wUpdateSpritesEnabled;
extern uint8_t rOBP1;
extern uint8_t wCoordAdjustmentAmount;
extern uint8_t* wShadowOAMSprite36;
extern void LoadSmokeTileFourTimes(void);
extern void WriteCutOrBoulderDustAnimationOAMBlock(void);
extern void Delay3(void);
extern void LoadPlayerSpriteGraphics(void);
extern void AdjustOAMBlockYPos(void);
extern void AdjustOAMBlockXPos(void);
extern void LoadSmokeTile(void);
extern void CopyVideoData(void);

// Constants
#define BIT_SOME_FLAG 2

// Function pointer table
const struct {
    int8_t x;
    int8_t y;
    void (*function)(void);
} MoveBoulderDustFunctionPointerTable[] = {
    {-1, 0, AdjustOAMBlockYPos},  // down
    {1, 0, AdjustOAMBlockYPos},   // up
    {1, 1, AdjustOAMBlockXPos},   // left
    {-1, 1, AdjustOAMBlockXPos},  // right
    {-1, -1, NULL} // end marker
};

// Graphics section
const uint8_t SSAnneSmokePuffTile[] = { /* gfx/overworld/smoke.2bpp */ };

// Function implementations
void AnimateBoulderDust(void) {
    // ld a, $1
    uint8_t a = 0x1;
    // ld [wWhichAnimationOffsets], a
    wWhichAnimationOffsets = a;
    // ld a, [wUpdateSpritesEnabled]
    a = wUpdateSpritesEnabled;
    // push af
    uint8_t saved_af = a;
    // ld a, $ff
    a = 0xFF;
    // ld [wUpdateSpritesEnabled], a
    wUpdateSpritesEnabled = a;
    // ld a, %11100100
    a = 0b11100100;
    // ldh [rOBP1], a
    rOBP1 = a;
    // call LoadSmokeTileFourTimes
    LoadSmokeTileFourTimes();
    // farcall WriteCutOrBoulderDustAnimationOAMBlock
    WriteCutOrBoulderDustAnimationOAMBlock(); /* farcall */
    // ld c, 8
    uint8_t c = 8;
.loop:
    // push bc
    uint8_t saved_bc = c;
    // call GetMoveBoulderDustFunctionPointer
    GetMoveBoulderDustFunctionPointer();
    // ld bc, .returnAddress
    uint16_t returnAddress = (uint16_t) &&.returnAddress;
    // push bc
    // ld c, 4
    c = 4;
    // jp hl
    goto *hl;
.returnAddress:
    // ldh a, [rOBP1]
    a = rOBP1;
    // xor %01100100
    a ^= 0b01100100;
    // ldh [rOBP1], a
    rOBP1 = a;
    // call Delay3
    Delay3();
    // pop bc
    c = saved_bc;
    // dec c
    c--;
    // jr nz, .loop
    if (c != 0) goto .loop;
    // pop af
    a = saved_af;
    // ld [wUpdateSpritesEnabled], a
    wUpdateSpritesEnabled = a;
    // jp LoadPlayerSpriteGraphics
    LoadPlayerSpriteGraphics(); /* jp */
}

void GetMoveBoulderDustFunctionPointer(void) {
    // ld a, [wSpritePlayerStateData1FacingDirection]
    uint8_t a = wSpritePlayerStateData1FacingDirection;
    // ld hl, MoveBoulderDustFunctionPointerTable
    const struct {
        int8_t x;
        int8_t y;
        void (*function)(void);
    }* hl = MoveBoulderDustFunctionPointerTable;
    // ld c, a
    uint8_t c = a;
    // ld b, $0
    uint8_t b = 0;
    // add hl, bc
    hl += (b << 8) | c; // Pointer arithmetic
    // ld a, [hli]
    a = *hl;
    // ld [wCoordAdjustmentAmount], a
    wCoordAdjustmentAmount = a;
    // ld a, [hli]
    a = *hl++;
    // ld e, a
    uint8_t e = a;
    // ld a, [hli]
    a = *hl++;
    // ld h, [hl]
    uint8_t h = *hl;
    // ld l, a
    uint8_t l = a;
    // push hl
    // ld hl, wShadowOAMSprite36
    hl = wShadowOAMSprite36;
    // ld d, $0
    uint8_t d = 0;
    // add hl, de
    hl += (d << 8) | e; // Pointer arithmetic
    // ld e, l
    e = l;
    // ld d, h
    d = h;
    // pop hl
    // ret
    return;
}

void LoadSmokeTileFourTimes(void) {
    // ld hl, vChars1 tile $7c
    uint8_t* hl = (uint8_t*)&vChars1 + 0x7C; // Pointer arithmetic
    // ld c, 4
    uint8_t c = 4;
.loop:
    // push bc
    uint8_t saved_bc = c;
    // push hl
    uint8_t* saved_hl = hl;
    // call LoadSmokeTile
    LoadSmokeTile();
    // pop hl
    hl = saved_hl;
    // ld bc, 1 tiles
    // add hl, bc
    hl += 1; // Pointer arithmetic
    // pop bc
    c = saved_bc;
    // dec c
    c--;
    // jr nz, .loop
    if (c != 0) goto .loop;
    // ret
    return;
}