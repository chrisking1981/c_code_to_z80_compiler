#include <stdint.h>

// External variables
extern uint8_t wWhichAnimationOffsets;
extern uint8_t wUpdateSpritesEnabled;
extern uint8_t rOBP1;
extern uint8_t wSpritePlayerStateData1FacingDirection;
extern uint8_t wCoordAdjustmentAmount;
extern uint8_t wShadowOAMSprite36;
extern uint8_t vChars1;

// Constants
#define BANK(x) 0

// External functions
extern void WriteCutOrBoulderDustAnimationOAMBlock(void);
extern void Delay3(void);
extern void LoadPlayerSpriteGraphics(void);
extern void CopyVideoData(uint8_t* dest, const uint8_t* src, uint8_t bank, uint8_t count);
extern void AdjustOAMBlockYPos(void);
extern void AdjustOAMBlockXPos(void);

// Function pointer type for dust adjustment functions
typedef void (*DustAdjustFunc)(void);

void AnimateBoulderDust(void) {
    uint8_t a;
    uint8_t c;
    uint8_t* bc_ptr;
    
    // ld a, $1
    a = 0x1;
    // ld [wWhichAnimationOffsets], a ; select the boulder dust offsets
    wWhichAnimationOffsets = a;
    // ld a, [wUpdateSpritesEnabled]
    a = wUpdateSpritesEnabled;
    // push af
    // (save wUpdateSpritesEnabled on stack)
    uint8_t saved_update_sprites = a;
    // ld a, $ff
    a = 0xff;
    // ld [wUpdateSpritesEnabled], a
    wUpdateSpritesEnabled = a;
    // ld a, %11100100
    a = 0xe4;
    // ldh [rOBP1], a
    rOBP1 = a;
    // call LoadSmokeTileFourTimes
    LoadSmokeTileFourTimes();
    // farcall WriteCutOrBoulderDustAnimationOAMBlock
    WriteCutOrBoulderDustAnimationOAMBlock(); /* farcall */
    // ld c, 8 ; number of steps in animation
    c = 8;

loop:
    // push bc
    // (save c on stack)
    uint8_t saved_c = c;
    // call GetMoveBoulderDustFunctionPointer
    GetMoveBoulderDustFunctionPointer();
    // ld bc, .returnAddress
    // push bc
    // ld c, 4
    c = 4;
    // jp hl
    // Call the function that was set up by GetMoveBoulderDustFunctionPointer

returnAddress:
    // ldh a, [rOBP1]
    a = rOBP1;
    // xor %01100100
    a ^= 0x64;
    // ldh [rOBP1], a
    rOBP1 = a;
    // call Delay3
    Delay3();
    // pop bc
    c = saved_c;
    // dec c
    c--;
    // jr nz, .loop
    if (c != 0) goto loop;
    
    // pop af
    // ld [wUpdateSpritesEnabled], a
    wUpdateSpritesEnabled = saved_update_sprites;
    // jp LoadPlayerSpriteGraphics
    LoadPlayerSpriteGraphics(); /* jp */
}

void GetMoveBoulderDustFunctionPointer(void) {
    uint8_t* hl;
    uint8_t a, c, b;
    uint8_t* de;
    uint8_t e, d;
    
    // ld a, [wSpritePlayerStateData1FacingDirection]
    a = wSpritePlayerStateData1FacingDirection;
    // ld hl, MoveBoulderDustFunctionPointerTable
    hl = (uint8_t*)MoveBoulderDustFunctionPointerTable;
    // ld c, a
    c = a;
    // ld b, $0
    b = 0;
    // add hl, bc
    hl += c; /* add hl, bc */
    // ld a, [hli]
    a = *hl++;
    // ld [wCoordAdjustmentAmount], a
    wCoordAdjustmentAmount = a;
    // ld a, [hli]
    a = *hl++;
    // ld e, a
    e = a;
    // ld a, [hli]
    a = *hl++;
    // ld h, [hl]
    // ld l, a
    // (This loads a function pointer from the table)
    // push hl
    // (The function pointer is now set up for calling)
    
    // ld hl, wShadowOAMSprite36
    hl = &wShadowOAMSprite36;
    // ld d, $0
    d = 0;
    // add hl, de
    hl += e; /* add hl, de */
    // ld e, l
    e = (uint8_t)(uintptr_t)hl;
    // ld d, h
    d = (uint8_t)((uintptr_t)hl >> 8);
    // pop hl
    // ret
    return;
}

// Boulder dust adjustment macro data structure
typedef struct {
    int8_t coord1;    // \1
    int8_t coord2;    // \2
    DustAdjustFunc func; // \3
} BoulderDustAdjust;

const BoulderDustAdjust MoveBoulderDustFunctionPointerTable[] = {
    // boulder_dust_adjust -1, 0, AdjustOAMBlockYPos ; down
    {-1, 0, AdjustOAMBlockYPos},
    // boulder_dust_adjust  1, 0, AdjustOAMBlockYPos ; up  
    {1, 0, AdjustOAMBlockYPos},
    // boulder_dust_adjust  1, 1, AdjustOAMBlockXPos ; left
    {1, 1, AdjustOAMBlockXPos},
    // boulder_dust_adjust -1, 1, AdjustOAMBlockXPos ; right
    {-1, 1, AdjustOAMBlockXPos}
};

void LoadSmokeTileFourTimes(void) {
    uint8_t* hl;
    uint8_t c;
    uint8_t* bc_ptr;
    
    // ld hl, vChars1 tile $7c
    hl = (uint8_t*)&vChars1;
    hl += 0x7c; /* tile $7c */
    // ld c, 4
    c = 4;

loop:
    // push bc
    uint8_t saved_c = c;
    // push hl
    uint8_t* saved_hl = hl;
    // call LoadSmokeTile
    LoadSmokeTile(hl);
    // pop hl
    hl = saved_hl;
    // ld bc, 1 tiles
    // add hl, bc
    hl += 16; /* 1 tile = 16 bytes */
    // pop bc
    c = saved_c;
    // dec c
    c--;
    // jr nz, .loop
    if (c != 0) goto loop;
    // ret
    return;
}

void LoadSmokeTile(uint8_t* hl) {
    uint8_t* de;
    uint8_t b, c;
    
    // ld de, SSAnneSmokePuffTile
    de = (uint8_t*)SSAnneSmokePuffTile;
    // lb bc, BANK(SSAnneSmokePuffTile), (SSAnneSmokePuffTileEnd - SSAnneSmokePuffTile) / $10
    b = BANK(SSAnneSmokePuffTile);
    c = (SSAnneSmokePuffTileEnd - SSAnneSmokePuffTile) / 0x10;
    // jp CopyVideoData
    CopyVideoData(hl, de, b, c); /* jp */
}

// Data arrays
extern const uint8_t SSAnneSmokePuffTile[];
extern const uint8_t SSAnneSmokePuffTileEnd[];

// Placeholder for the actual smoke graphics data
const uint8_t SSAnneSmokePuffTile[] = {
    // INCBIN "gfx/overworld/smoke.2bpp"
    // Placeholder for the actual graphics data
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t SSAnneSmokePuffTileEnd[] = {
    // End marker
}; 