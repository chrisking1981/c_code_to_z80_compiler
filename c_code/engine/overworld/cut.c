#include <stdint.h>
#include <stdbool.h>

// Extern declarations for variables/functions
extern uint8_t wActionResultOrTookBattleTurn;
extern uint8_t wCurMapTileset;
extern uint8_t wTileInFrontOfPlayer;
extern uint8_t wCutTile;
extern uint8_t wWhichPokemon;
extern uint8_t wPartyMonNicks[];
extern uint8_t wStatusFlags5[];
extern uint8_t wUpdateSpritesEnabled;
extern uint8_t wSpritePlayerStateData1YPixels[];
extern uint8_t wSpritePlayerStateData1FacingDirection;
extern uint8_t wXBlockCoord;
extern uint8_t wYBlockCoord;
extern uint8_t wCurrentTileBlockMapViewPointer;
extern uint8_t hWY;
extern uint8_t rOBP1;
extern uint8_t SCREEN_HEIGHT_PX;
extern void PrintText(const char* text);
extern void GetPartyMonName(void);
extern void GBPalWhiteOutWithDelay3(void);
extern void ClearSprites(void);
extern void RestoreScreenTilesAndReloadTilePatterns(void);
extern void Delay3(void);
extern void LoadGBPal(void);
extern void LoadCurrentMapView(void);
extern void SaveScreenTilesToBuffer2(void);
extern void LoadScreenTilesFromBuffer2(void);
extern void InitCutAnimOAM(void);
extern void PlaySound(uint8_t sound);
extern void UpdateSprites(void);
extern void RedrawMapView(void);
extern void ReplaceTreeTileBlock(void);
extern void AnimCut(void);
extern void CopyVideoData(void);
extern void LoadCutGrassAnimationTilePattern(void);
extern void WriteOAMBlock(void);
extern void GetCutOrBoulderDustAnimationOffsets(void);

// Constants
#define GYM 0x01
#define SPRITE_FACING_UP 0x04
#define OAM_OBP1 0x01
#define SFX_CUT 0x90

// Data arrays
const uint8_t CutAnimationOffsets[] = {
    8, 36, // player is facing down
    8, 4,  // player is facing up
    -8, 20, // player is facing left
    24, 20, // player is facing right
};

const uint8_t BoulderDustAnimationOffsets[] = {
    8, 52, // player is facing down
    8, -12, // player is facing up
    -24, 20, // player is facing left
    40, 20, // player is facing right
};

// Function implementations
void UsedCut(void) {
    // xor a
    uint8_t a = 0; // a = 0
    // ld [wActionResultOrTookBattleTurn], a
    wActionResultOrTookBattleTurn = a; // initialise to failure value
    // ld a, [wCurMapTileset]
    a = wCurMapTileset;
    // and a
    // jr z, .overworld
    if (a == 0) goto overworld;
    // cp GYM
    if (a != GYM) goto nothingToCut;
    // ld a, [wTileInFrontOfPlayer]
    a = wTileInFrontOfPlayer;
    // cp $50 ; gym cut tree
    if (a != 0x50) goto nothingToCut;
    // jr .canCut
    goto canCut;

overworld:
    // dec a
    a--; // decrement a
    // ld a, [wTileInFrontOfPlayer]
    a = wTileInFrontOfPlayer;
    // cp $3d ; cut tree
    if (a == 0x3d) goto canCut;
    // cp $52 ; grass
    if (a == 0x52) goto canCut;

nothingToCut:
    // ld hl, .NothingToCutText
    uint8_t* hl = (uint8_t*)&NothingToCutText;
    // jp PrintText
    PrintText((const char*)hl); /* jp */
    return;

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
    hl = wPartyMonNicks;
    // call GetPartyMonName
    GetPartyMonName();
    // ld hl, wStatusFlags5
    hl = wStatusFlags5;
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
    a = 0; // a = 0
    // ldh [hWY], a
    hWY = a;
    // ld hl, UsedCutText
    hl = (uint8_t*)&UsedCutText;
    // call PrintText
    PrintText((const char*)hl);
    // call LoadScreenTilesFromBuffer2
    LoadScreenTilesFromBuffer2();
    // ld hl, wStatusFlags5
    hl = wStatusFlags5;
    // res BIT_NO_TEXT_DELAY, [hl]
    *hl &= ~(1 << BIT_NO_TEXT_DELAY);
    // ld a, $ff
    a = 0xFF;
    // ld [wUpdateSpritesEnabled], a
    wUpdateSpritesEnabled = a;
    // call InitCutAnimOAM
    InitCutAnimOAM();
    // ld de, CutTreeBlockSwaps
    // call ReplaceTreeTileBlock
    ReplaceTreeTileBlock();
    // call RedrawMapView
    RedrawMapView();
    // farcall AnimCut
    AnimCut(); /* farcall */
    // ld a, $1
    a = 0x01;
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
    RedrawMapView();
}

// Text sections
const uint8_t NothingToCutText[] = {
    // text content here
    -1 // end
};

const uint8_t UsedCutText[] = {
    // text content here
    -1 // end
};

// Function for InitCutAnimOAM
void InitCutAnimOAM(void) {
    // xor a
    uint8_t a = 0; // a = 0
    // ld [wWhichAnimationOffsets], a
    // ld a, %11100100
    a = 0b11100100;
    // ldh [rOBP1], a
    rOBP1 = a;
    // ld a, [wCutTile]
    a = wCutTile;
    // cp $52
    if (a == 0x52) goto grass;
    // ld de, Overworld_GFX tile $2d ; cuttable tree sprite top row
    // ld hl, vChars1 tile $7c
    // lb bc, BANK(Overworld_GFX), 2
    // call CopyVideoData
    CopyVideoData();
    // ld de, Overworld_GFX tile $3d ; cuttable tree sprite bottom row
    // ld hl, vChars1 tile $7e
    // lb bc, BANK(Overworld_GFX), 2
    // call CopyVideoData
    CopyVideoData();
    // jr WriteCutOrBoulderDustAnimationOAMBlock
    goto WriteCutOrBoulderDustAnimationOAMBlock;

grass:
    // ld hl, vChars1 tile $7c
    // call LoadCutGrassAnimationTilePattern
    LoadCutGrassAnimationTilePattern();
    // ld hl, vChars1 tile $7d
    LoadCutGrassAnimationTilePattern();
    // ld hl, vChars1 tile $7e
    LoadCutGrassAnimationTilePattern();
    // ld hl, vChars1 tile $7f
    LoadCutGrassAnimationTilePattern();
    // call WriteCutOrBoulderDustAnimationOAMBlock
    WriteCutOrBoulderDustAnimationOAMBlock();
    // ld hl, wShadowOAMSprite36Attributes
    // ld de, 4
    // ld a, $30
    // ld c, e
    uint8_t* hl = wShadowOAMSprite36Attributes;
    uint8_t de = 4;
    a = 0x30;
    uint8_t c = de;
    // .loop
    while (c != 0) {
        // ld [hl], a
        *hl = a;
        // add hl, de
        hl += de;
        // xor $60
        a ^= 0x60;
        // dec c
        c--;
    }
    // ret
}

// Other functions omitted for brevity