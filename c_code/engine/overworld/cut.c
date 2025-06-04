#include <stdint.h>

// Extern declarations
extern uint8_t wActionResultOrTookBattleTurn;
extern uint8_t wCurMapTileset;
extern uint8_t wTileInFrontOfPlayer;
extern uint8_t wCutTile;
extern uint8_t wWhichPokemon;
extern uint8_t wPartyMonNicks;
extern uint8_t wStatusFlags5;
extern uint8_t wUpdateSpritesEnabled;
extern uint8_t wWhichAnimationOffsets;
extern uint8_t wSpritePlayerStateData1YPixels;
extern uint8_t wSpritePlayerStateData1FacingDirection;
extern uint8_t wXBlockCoord;
extern uint8_t wYBlockCoord;
extern uint8_t wCurrentTileBlockMapViewPointer;
extern uint8_t rOBP1;
extern uint8_t hWY;
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
extern void CopyVideoData(void);
extern void LoadCutGrassAnimationTilePattern(void);
extern void WriteOAMBlock(void);
extern void GetCutOrBoulderDustAnimationOffsets(void);
extern void ReplaceTreeTileBlock(void);
extern void AnimCut(void);

// Constants
#define GYM 0x01
#define SPRITE_FACING_UP 0x04
#define OAM_OBP1 0x01
#define BIT_NO_TEXT_DELAY 0x00

// Function prototypes
void UsedCut(void);
void InitCutAnimOAM(void);
void LoadCutGrassAnimationTilePattern(void);
void WriteCutOrBoulderDustAnimationOAMBlock(void);
void GetCutOrBoulderDustAnimationOffsets(void);
void ReplaceTreeTileBlock(void);

// Function definitions
void UsedCut(void) {
    // xor a
    uint8_t a = 0; // a = 0
    // ld [wActionResultOrTookBattleTurn], a
    wActionResultOrTookBattleTurn = a; // initialise to failure value
    // ld a, [wCurMapTileset]
    a = wCurMapTileset;
    // and a
    // OVERWORLD
    if (a == 0) goto ..overworld;
    // cp GYM
    if (a != GYM) goto ..nothingToCut;
    // ld a, [wTileInFrontOfPlayer]
    a = wTileInFrontOfPlayer;
    // cp $50 ; gym cut tree
    if (a != 0x50) goto ..nothingToCut;
    goto ..canCut;
..overworld:
    // dec a
    a--; 
    // ld a, [wTileInFrontOfPlayer]
    a = wTileInFrontOfPlayer;
    // cp $3d ; cut tree
    if (a == 0x3d) goto ..canCut;
    // cp $52 ; grass
    if (a == 0x52) goto ..canCut;
..nothingToCut:
    // ld hl, .NothingToCutText
    uint8_t* hl = (uint8_t*)&NothingToCutText;
    // jp PrintText
    PrintText((const char*)hl); /* jp */
    return;

..canCut:
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
    hl = UsedCutText;
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
    // Assuming CutTreeBlockSwaps is defined elsewhere
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
    // Assuming SFX_CUT is defined elsewhere
    // call PlaySound
    PlaySound(SFX_CUT);
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
    // xor a
    uint8_t a = 0; // a = 0
    // ld [wWhichAnimationOffsets], a
    wWhichAnimationOffsets = a;
    // ld a, %11100100
    a = 0b11100100;
    // ldh [rOBP1], a
    rOBP1 = a;
    // ld a, [wCutTile]
    a = wCutTile;
    // cp $52
    if (a == 0x52) goto ..grass;
    // tree
    // ld de, Overworld_GFX tile $2d ; cuttable tree sprite top row
    // Assuming Overworld_GFX is defined elsewhere
    // ld hl, vChars1 tile $7c
    // Assuming vChars1 is defined elsewhere
    // lb bc, BANK(Overworld_GFX), 2
    // Assuming BANK is defined elsewhere
    // call CopyVideoData
    CopyVideoData();
    // ld de, Overworld_GFX tile $3d ; cuttable tree sprite bottom row
    // ld hl, vChars1 tile $7e
    // call CopyVideoData
    CopyVideoData();
    goto ..WriteCutOrBoulderDustAnimationOAMBlock;
..grass:
    // ld hl, vChars1 tile $7c
    hl = vChars1 + 0x7c; // Assuming vChars1 is defined elsewhere
    // call LoadCutGrassAnimationTilePattern
    LoadCutGrassAnimationTilePattern();
    // ld hl, vChars1 tile $7d
    hl = vChars1 + 0x7d;
    // call LoadCutGrassAnimationTilePattern
    LoadCutGrassAnimationTilePattern();
    // ld hl, vChars1 tile $7e
    hl = vChars1 + 0x7e;
    // call LoadCutGrassAnimationTilePattern
    LoadCutGrassAnimationTilePattern();
    // ld hl, vChars1 tile $7f
    hl = vChars1 + 0x7f;
    // call LoadCutGrassAnimationTilePattern
    LoadCutGrassAnimationTilePattern();
    // call WriteCutOrBoulderDustAnimationOAMBlock
    WriteCutOrBoulderDustAnimationOAMBlock();
    // ld hl, wShadowOAMSprite36Attributes
    // Assuming wShadowOAMSprite36Attributes is defined elsewhere
    // ld de, 4
    // ld a, $30
    // ld c, e
    // Assuming e is defined elsewhere
    // .loop
    // ld [hl], a
    // add hl, de
    // xor $60
    // dec c
    // jr nz, .loop
    return;
}

void LoadCutGrassAnimationTilePattern(void) {
    // ld de, MoveAnimationTiles1 tile 6 ; tile depicting a leaf
    // Assuming MoveAnimationTiles1 is defined elsewhere
    // lb bc, BANK(MoveAnimationTiles1), 1
    // call CopyVideoData
    CopyVideoData();
}

void WriteCutOrBoulderDustAnimationOAMBlock(void) {
    // call GetCutOrBoulderDustAnimationOffsets
    GetCutOrBoulderDustAnimationOffsets();
    // ld a, $9
    uint8_t a = 0x9;
    // ld de, .OAMBlock
    // Assuming .OAMBlock is defined elsewhere
    // jp WriteOAMBlock
    WriteOAMBlock();
}

void GetCutOrBoulderDustAnimationOffsets(void) {
    // ld hl, wSpritePlayerStateData1YPixels
    // Assuming wSpritePlayerStateData1YPixels is defined elsewhere
    // ld a, [hli] ; player's sprite screen Y position
    uint8_t b = *hl; // player's sprite screen Y position
    // inc hl
    hl++;
    // ld a, [hli] ; player's sprite screen X position
    uint8_t c = *hl; // player's sprite screen X position
    // inc hl
    hl += 2; // skip two bytes
    // ld a, [hl] ; a holds direction of player (00: down, 04: up, 08: left, 0C: right)
    uint8_t direction = *hl;
    // srl a
    direction >>= 1; // divide by 2
    // ld e, a
    uint8_t e = direction;
    // ld d, $0 ; de holds direction (00: down, 02: up, 04: left, 06: right)
    uint8_t d = 0;
    // ld a, [wWhichAnimationOffsets]
    a = wWhichAnimationOffsets;
    // and a
    if (a == 0) goto ..next;
    // ld hl, BoulderDustAnimationOffsets
    hl = BoulderDustAnimationOffsets; // Assuming BoulderDustAnimationOffsets is defined elsewhere
    goto ..addOffsets;
..next:
    // ld hl, CutAnimationOffsets
    hl = CutAnimationOffsets; // Assuming CutAnimationOffsets is defined elsewhere
..addOffsets:
    // add hl, de
    hl += e;
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
    return;
}

void ReplaceTreeTileBlock(void) {
    // Determine the address of the tile block that contains the tile in front of the
    // player (i.e. where the tree is) and replace it with the corresponding tile
    // block that doesn't have the tree.
    // push de
    // Assuming de is defined elsewhere
    // ld a, [wCurMapWidth]
    uint8_t a = wCurMapWidth; // Assuming wCurMapWidth is defined elsewhere
    // add 6
    a += 6;
    // ld c, a
    uint8_t c = a;
    // ld b, 0
    uint8_t b = 0;
    // ld d, 0
    uint8_t d = 0;
    // ld hl, wCurrentTileBlockMapViewPointer
    hl = wCurrentTileBlockMapViewPointer; // Assuming wCurrentTileBlockMapViewPointer is defined elsewhere
    // ld a, [hli]
    uint8_t tileBlock = *hl; // tile block
    // ld h, [hl]
    // ld l, a
    // add hl, bc
    hl += (b + c);
    // ld a, [wSpritePlayerStateData1FacingDirection]
    a = wSpritePlayerStateData1FacingDirection; // Assuming wSpritePlayerStateData1FacingDirection is defined elsewhere
    // and a
    if (a == 0) goto ..down;
    // cp SPRITE_FACING_UP
    if (a == SPRITE_FACING_UP) goto ..up;
    // cp SPRITE_FACING_LEFT
    if (a == SPRITE_FACING_LEFT) goto ..left;
    // right
    // ld a, [wXBlockCoord]
    a = wXBlockCoord; // Assuming wXBlockCoord is defined elsewhere
    // and a
    if (a == 0) goto ..centerTileBlock;
    goto ..rightOfCenter;
..down:
    // ld a, [wYBlockCoord]
    a = wYBlockCoord; // Assuming wYBlockCoord is defined elsewhere
    // and a
    if (a == 0) goto ..centerTileBlock;
    goto ..belowCenter;
..up:
    // ld a, [wYBlockCoord]
    a = wYBlockCoord; // Assuming wYBlockCoord is defined elsewhere
    // and a
    if (a == 0) goto ..centerTileBlock;
    goto ..aboveCenter;
..left:
    // ld a, [wXBlockCoord]
    a = wXBlockCoord; // Assuming wXBlockCoord is defined elsewhere
    // and a
    if (a == 0) goto ..leftOfCenter;
    goto ..centerTileBlock;
..belowCenter:
    // add hl, bc
    hl += (b + c);
..centerTileBlock:
    // add hl, bc
    hl += (b + c);
..aboveCenter:
    // ld e, $2
    uint8_t e = 0x2;
    // add hl, de
    hl += e;
    goto ..next;
..leftOfCenter:
    // ld e, $1
    e = 0x1;
    // add hl, bc
    hl += (b + c);
    hl += e;
    goto ..next;
..rightOfCenter:
    // ld e, $3
    e = 0x3;
    // add hl, bc
    hl += (b + c);
    hl += e;
..next:
    // pop de
    // ld a, [hl]
    a = *hl; // current tile block
    // ld c, a
    c = a;
    // .loop ; find the matching tile block in the array
    while (1) {
        // ld a, [de]
        a = *de; // Assuming de is defined elsewhere
        // inc de
        de++;
        de++;
        // cp $ff
        if (a == 0xFF) return; // end of array
        // cp c
        if (a != c) continue; // not a match
        // dec de
        de--;
        // ld a, [de] ; replacement tile block from matching array entry
        a = *de; // replacement tile block
        // ld [hl], a
        *hl = a; // replace tile block
        return;
    }
}

// Data arrays
const uint8_t NothingToCutText[] = {
    // Text data goes here
    -1 // end
};

const uint8_t UsedCutText[] = {
    // Text data goes here
    -1 // end
};

const uint8_t CutTreeBlockSwaps[] = {
    // Tile block data goes here
    -1 // end
};

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