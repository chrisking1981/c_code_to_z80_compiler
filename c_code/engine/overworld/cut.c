#include <stdint.h>

// External variables
extern uint8_t wActionResultOrTookBattleTurn;
extern uint8_t wCurMapTileset;
extern uint8_t wTileInFrontOfPlayer;
extern uint8_t wCutTile;
extern uint8_t wWhichPokemon;
extern uint8_t wPartyMonNicks;
extern uint8_t wStatusFlags5;
extern uint8_t hWY;
extern uint8_t wUpdateSpritesEnabled;
extern uint8_t wWhichAnimationOffsets;
extern uint8_t rOBP1;
extern uint8_t vChars1;
extern uint8_t wShadowOAMSprite36Attributes;
extern uint8_t wSpritePlayerStateData1YPixels;
extern uint8_t wCurMapWidth;
extern uint8_t wCurrentTileBlockMapViewPointer;
extern uint8_t wSpritePlayerStateData1FacingDirection;
extern uint8_t wXBlockCoord;
extern uint8_t wYBlockCoord;

// Constants
#define OVERWORLD 0
#define GYM 1
#define BIT_NO_TEXT_DELAY 0
#define SCREEN_HEIGHT_PX 144
#define SPRITE_FACING_UP 1
#define SPRITE_FACING_LEFT 2
#define OAM_OBP1 0x10
#define SFX_CUT 0x20
#define BANK(x) 0

// External functions
extern void PrintText(const char* text);
extern void GetPartyMonName(uint8_t* hl, uint8_t a);
extern void GBPalWhiteOutWithDelay3(void);
extern void ClearSprites(void);
extern void RestoreScreenTilesAndReloadTilePatterns(void);
extern void Delay3(void);
extern void LoadGBPal(void);
extern void LoadCurrentMapView(void);
extern void SaveScreenTilesToBuffer2(void);
extern void LoadScreenTilesFromBuffer2(void);
extern void RedrawMapView(void);
extern void AnimCut(void);
extern void PlaySound(uint8_t sound);
extern void UpdateSprites(void);
extern void CopyVideoData(uint8_t* dest, const uint8_t* src, uint8_t bank, uint8_t count);
extern void WriteOAMBlock(uint8_t count, const uint8_t* data);

void UsedCut(void) {
    uint8_t* hl;
    uint8_t a;
    
    // xor a
    a = 0;
    // ld [wActionResultOrTookBattleTurn], a ; initialise to failure value
    wActionResultOrTookBattleTurn = a;
    // ld a, [wCurMapTileset]
    a = wCurMapTileset;
    // and a ; OVERWORLD
    // jr z, .overworld
    if (a == 0) goto overworld;
    // cp GYM
    // jr nz, .nothingToCut
    if (a != GYM) goto nothingToCut;
    // ld a, [wTileInFrontOfPlayer]
    a = wTileInFrontOfPlayer;
    // cp $50 ; gym cut tree
    // jr nz, .nothingToCut
    if (a != 0x50) goto nothingToCut;
    // jr .canCut
    goto canCut;

overworld:
    // dec a
    a--;
    // ld a, [wTileInFrontOfPlayer]
    a = wTileInFrontOfPlayer;
    // cp $3d ; cut tree
    // jr z, .canCut
    if (a == 0x3d) goto canCut;
    // cp $52 ; grass
    // jr z, .canCut
    if (a == 0x52) goto canCut;

nothingToCut:
    // ld hl, .NothingToCutText
    hl = (uint8_t*)NothingToCutText;
    // jp PrintText
    PrintText((const char*)hl); /* jp */

canCut:
    // ld [wCutTile], a
    wCutTile = a;
    // ld a, 1
    a = 1;
    // ld [wActionResultOrTookBattleTurn], a ; used cut
    wActionResultOrTookBattleTurn = a;
    // ld a, [wWhichPokemon]
    a = wWhichPokemon;
    // ld hl, wPartyMonNicks
    hl = &wPartyMonNicks;
    // call GetPartyMonName
    GetPartyMonName(hl, a);
    // ld hl, wStatusFlags5
    hl = &wStatusFlags5;
    // set BIT_NO_TEXT_DELAY, [hl]
    *hl |= (1 << BIT_NO_TEXT_DELAY);
    // call GBPalWhiteOutWithDelay3
    GBPalWhiteOutWithDelay3();
    // call ClearSprites
    ClearSprites();
    // call RestoreScreenTilesAndReloadTilePatterns
    RestoreScreenTilesAndReloadTilePatterns();
    // ld a, SCREEN_HEIGHT_PX
    a = SCREEN_HEIGHT_PX;
    // ldh [hWY], a
    hWY = a;
    // call Delay3
    Delay3();
    // call LoadGBPal
    LoadGBPal();
    // call LoadCurrentMapView
    LoadCurrentMapView();
    // call SaveScreenTilesToBuffer2
    SaveScreenTilesToBuffer2();
    // call Delay3
    Delay3();
    // xor a
    a = 0;
    // ldh [hWY], a
    hWY = a;
    // ld hl, UsedCutText
    hl = (uint8_t*)UsedCutText;
    // call PrintText
    PrintText((const char*)hl); /* jp */
    // call LoadScreenTilesFromBuffer2
    LoadScreenTilesFromBuffer2();
    // ld hl, wStatusFlags5
    hl = &wStatusFlags5;
    // res BIT_NO_TEXT_DELAY, [hl]
    *hl &= ~(1 << BIT_NO_TEXT_DELAY);
    // ld a, $ff
    a = 0xff;
    // ld [wUpdateSpritesEnabled], a
    wUpdateSpritesEnabled = a;
    // call InitCutAnimOAM
    InitCutAnimOAM();
    // ld de, CutTreeBlockSwaps
    // call ReplaceTreeTileBlock
    ReplaceTreeTileBlock((uint8_t*)CutTreeBlockSwaps);
    // call RedrawMapView
    RedrawMapView(); /* jp */
    // farcall AnimCut
    AnimCut();
    // ld a, $1
    a = 0x1;
    // ld [wUpdateSpritesEnabled], a
    wUpdateSpritesEnabled = a;
    // ld a, SFX_CUT
    a = SFX_CUT;
    // call PlaySound
    PlaySound(a);
    // ld a, $90
    a = 0x90;
    // ldh [hWY], a
    hWY = a;
    // call UpdateSprites
    UpdateSprites();
    // jp RedrawMapView
    RedrawMapView(); /* jp */
}

void InitCutAnimOAM(void) {
    uint8_t* hl;
    uint8_t* de;
    uint8_t a, b, c;
    
    // xor a
    a = 0;
    // ld [wWhichAnimationOffsets], a
    wWhichAnimationOffsets = a;
    // ld a, %11100100
    a = 0xe4;
    // ldh [rOBP1], a
    rOBP1 = a;
    // ld a, [wCutTile]
    a = wCutTile;
    // cp $52
    // jr z, .grass
    if (a == 0x52) goto grass;
    
    // tree
    // ld de, Overworld_GFX tile $2d ; cuttable tree sprite top row
    de = (uint8_t*)Overworld_GFX;
    de += 0x2d;
    // ld hl, vChars1 tile $7c
    hl = (uint8_t*)&vChars1;
    hl += 0x7c;
    // lb bc, BANK(Overworld_GFX), 2
    b = BANK(Overworld_GFX);
    c = 2;
    // call CopyVideoData
    CopyVideoData(hl, de, b, c); /* jp */
    // ld de, Overworld_GFX tile $3d ; cuttable tree sprite bottom row
    de = (uint8_t*)Overworld_GFX;
    de += 0x3d;
    // ld hl, vChars1 tile $7e
    hl = (uint8_t*)&vChars1;
    hl += 0x7e;
    // lb bc, BANK(Overworld_GFX), 2
    b = BANK(Overworld_GFX);
    c = 2;
    // call CopyVideoData
    CopyVideoData(hl, de, b, c); /* jp */
    // jr WriteCutOrBoulderDustAnimationOAMBlock
    goto WriteCutOrBoulderDustAnimationOAMBlock;

grass:
    // ld hl, vChars1 tile $7c
    hl = (uint8_t*)&vChars1;
    hl += 0x7c;
    // call LoadCutGrassAnimationTilePattern
    LoadCutGrassAnimationTilePattern(hl);
    // ld hl, vChars1 tile $7d
    hl = (uint8_t*)&vChars1;
    hl += 0x7d;
    // call LoadCutGrassAnimationTilePattern
    LoadCutGrassAnimationTilePattern(hl);
    // ld hl, vChars1 tile $7e
    hl = (uint8_t*)&vChars1;
    hl += 0x7e;
    // call LoadCutGrassAnimationTilePattern
    LoadCutGrassAnimationTilePattern(hl);
    // ld hl, vChars1 tile $7f
    hl = (uint8_t*)&vChars1;
    hl += 0x7f;
    // call LoadCutGrassAnimationTilePattern
    LoadCutGrassAnimationTilePattern(hl);
    // call WriteCutOrBoulderDustAnimationOAMBlock
    WriteCutOrBoulderDustAnimationOAMBlock();
    // ld hl, wShadowOAMSprite36Attributes
    hl = &wShadowOAMSprite36Attributes;
    // ld de, 4
    de = (uint8_t*)4;
    // ld a, $30
    a = 0x30;
    // ld c, e
    c = 4; /* direct value since de is 4 */

loop:
    // ld [hl], a
    *hl = a;
    // add hl, de
    hl += 4; /* add hl, de */
    // xor $60
    a ^= 0x60;
    // dec c
    c--;
    // jr nz, .loop
    if (c != 0) goto loop;
    // ret
    return;

WriteCutOrBoulderDustAnimationOAMBlock:
    // call GetCutOrBoulderDustAnimationOffsets
    GetCutOrBoulderDustAnimationOffsets();
    // ld a, $9
    a = 0x9;
    // ld de, .OAMBlock
    de = (uint8_t*)OAMBlock;
    // jp WriteOAMBlock
    WriteOAMBlock(a, de); /* jp */
}

void LoadCutGrassAnimationTilePattern(uint8_t* hl) {
    uint8_t* de;
    uint8_t b, c;
    
    // ld de, MoveAnimationTiles1 tile 6 ; tile depicting a leaf
    de = (uint8_t*)MoveAnimationTiles1;
    de += 6; /* tile 6 */
    // lb bc, BANK(MoveAnimationTiles1), 1
    b = BANK(MoveAnimationTiles1);
    c = 1;
    // jp CopyVideoData
    CopyVideoData(hl, de, b, c); /* jp */
}

void GetCutOrBoulderDustAnimationOffsets(void) {
    uint8_t* hl;
    uint8_t a, b, c, d, e;
    
    // ld hl, wSpritePlayerStateData1YPixels
    hl = &wSpritePlayerStateData1YPixels;
    // ld a, [hli] ; player's sprite screen Y position
    a = *hl++;
    // ld b, a
    b = a;
    // inc hl
    hl++;
    // ld a, [hli] ; player's sprite screen X position
    a = *hl++;
    // ld c, a ; bc holds ypos/xpos of player's sprite
    c = a;
    // inc hl
    hl++;
    // inc hl
    hl++;
    // ld a, [hl] ; a holds direction of player (00: down, 04: up, 08: left, 0C: right)
    a = *hl;
    // srl a
    a >>= 1;
    // ld e, a
    e = a;
    // ld d, $0 ; de holds direction (00: down, 02: up, 04: left, 06: right)
    d = 0;
    // ld a, [wWhichAnimationOffsets]
    a = wWhichAnimationOffsets;
    // and a
    // ld hl, CutAnimationOffsets
    hl = (uint8_t*)CutAnimationOffsets;
    // jr z, .next
    if (a == 0) goto next;
    // ld hl, BoulderDustAnimationOffsets
    hl = (uint8_t*)BoulderDustAnimationOffsets;

next:
    // add hl, de
    hl += (uint8_t)e;
    // ld e, [hl]
    e = *hl;
    // inc hl
    hl++;
    // ld d, [hl]
    d = *hl;
    // ld a, b
    a = b;
    // add d
    a += d;
    // ld b, a
    b = a;
    // ld a, c
    a = c;
    // add e
    a += e;
    // ld c, a
    c = a;
    // ret
    return;
}

void ReplaceTreeTileBlock(uint8_t* de) {
    uint8_t* hl;
    uint8_t a, b, c, d, e;
    
    // push de
    // (saving de for later)
    uint8_t* saved_de = de;
    
    // ld a, [wCurMapWidth]
    a = wCurMapWidth;
    // add 6
    a += 6;
    // ld c, a
    c = a;
    // ld b, 0
    b = 0;
    // ld d, 0
    d = 0;
    // ld hl, wCurrentTileBlockMapViewPointer
    hl = &wCurrentTileBlockMapViewPointer;
    // ld a, [hli]
    a = *hl++;
    // ld h, [hl]
    // ld l, a
    hl = (uint8_t*)((*hl << 8) | a);
    // add hl, bc
    hl += (uint8_t)c;
    // ld a, [wSpritePlayerStateData1FacingDirection]
    a = wSpritePlayerStateData1FacingDirection;
    // and a
    // jr z, .down
    if (a == 0) goto down;
    // cp SPRITE_FACING_UP
    // jr z, .up
    if (a == SPRITE_FACING_UP) goto up;
    // cp SPRITE_FACING_LEFT
    // jr z, .left
    if (a == SPRITE_FACING_LEFT) goto left;
    
    // right
    // ld a, [wXBlockCoord]
    a = wXBlockCoord;
    // and a
    // jr z, .centerTileBlock
    if (a == 0) goto centerTileBlock;
    // jr .rightOfCenter
    goto rightOfCenter;

down:
    // ld a, [wYBlockCoord]
    a = wYBlockCoord;
    // and a
    // jr z, .centerTileBlock
    if (a == 0) goto centerTileBlock;
    // jr .belowCenter
    goto belowCenter;

up:
    // ld a, [wYBlockCoord]
    a = wYBlockCoord;
    // and a
    // jr z, .aboveCenter
    if (a == 0) goto aboveCenter;
    // jr .centerTileBlock
    goto centerTileBlock;

left:
    // ld a, [wXBlockCoord]
    a = wXBlockCoord;
    // and a
    // jr z, .leftOfCenter
    if (a == 0) goto leftOfCenter;
    // jr .centerTileBlock
    goto centerTileBlock;

belowCenter:
    // add hl, bc
    hl += (uint8_t)c;

centerTileBlock:
    // add hl, bc
    hl += (uint8_t)c;

aboveCenter:
    // ld e, $2
    e = 0x2;
    // add hl, de
    hl += (uint8_t)e;
    // jr .next
    goto next;

leftOfCenter:
    // ld e, $1
    e = 0x1;
    // add hl, bc
    hl += (uint8_t)c;
    // add hl, de
    hl += (uint8_t)e;
    // jr .next
    goto next;

rightOfCenter:
    // ld e, $3
    e = 0x3;
    // add hl, bc
    hl += (uint8_t)c;
    // add hl, de
    hl += (uint8_t)e;

next:
    // pop de
    de = saved_de;
    // ld a, [hl]
    a = *hl;
    // ld c, a
    c = a;

loop:
    // ld a, [de]
    a = *de;
    // inc de
    de++;
    // inc de
    de++;
    // cp $ff
    // ret z
    if (a == 0xff) return;
    // cp c
    // jr nz, .loop
    if (a != c) goto loop;
    // dec de
    de--;
    // ld a, [de] ; replacement tile block from matching array entry
    a = *de;
    // ld [hl], a
    *hl = a;
    // ret
    return;
}

// Data arrays
const char NothingToCutText[] = "Nothing to cut here!";
const char UsedCutText[] = "Used CUT!";

const uint8_t Overworld_GFX[] = {
    // Placeholder for graphics data
    0x00, 0x00, 0x00, 0x00
};

const uint8_t MoveAnimationTiles1[] = {
    // Placeholder for animation tiles
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t OAMBlock[] = {
    0xfc, OAM_OBP1,
    0xfd, OAM_OBP1,
    0xfe, OAM_OBP1,
    0xff, OAM_OBP1
};

const uint8_t CutAnimationOffsets[] = {
    8, 36,   // player is facing down
    8, 4,    // player is facing up
    -8, 20,  // player is facing left
    24, 20   // player is facing right
};

const uint8_t BoulderDustAnimationOffsets[] = {
    8, 52,   // player is facing down
    8, -12,  // player is facing up
    -24, 20, // player is facing left
    40, 20   // player is facing right
};

const uint8_t CutTreeBlockSwaps[] = {
    // Placeholder for tree block swap data
    0xff // end marker
}; 