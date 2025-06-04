void HandleLedges(void) {
HandleLedges:
// ld a, [wMovementFlags]
    a = wMovementFlags;
// bit BIT_LEDGE_OR_FISHING, a
// ret nz
    return;
// ld a, [wCurMapTileset]
    a = wCurMapTileset;
// and a ; OVERWORLD
// ret nz
    return;
// predef GetTileAndCoordsInFrontOfPlayer
// ld a, [wSpritePlayerStateData1FacingDirection]
    a = wSpritePlayerStateData1FacingDirection;
// ld b, a
// lda_coord 8, 9
// ld c, a
// ld a, [wTileInFrontOfPlayer]
    a = wTileInFrontOfPlayer;
// ld d, a
// ld hl, LedgeTiles
    hl = LedgeTiles;
// .loop
// ld a, [hli]
    a = hli;
// cp $ff
// ret z
    return;
// cp b
// jr nz, .nextLedgeTile1
// ld a, [hli]
    a = hli;
// cp c
// jr nz, .nextLedgeTile2
// ld a, [hli]
    a = hli;
// cp d
// jr nz, .nextLedgeTile3
// ld a, [hl]
    a = hl;
// ld e, a
// jr .foundMatch
// .nextLedgeTile1
// inc hl
// .nextLedgeTile2
// inc hl
// .nextLedgeTile3
// inc hl
// jr .loop
// .foundMatch
// ldh a, [hJoyHeld]
    a = hJoyHeld;
// and e
// ret z
    return;
// ld a, A_BUTTON | B_BUTTON | SELECT | START | D_RIGHT | D_LEFT | D_UP | D_DOWN
    a = A_BUTTON | B_BUTTON | SELECT | START | D_RIGHT | D_LEFT | D_UP | D_DOWN;
// ld [wJoyIgnore], a
    wJoyIgnore = a;
// ld hl, wMovementFlags
    hl = wMovementFlags;
// set BIT_LEDGE_OR_FISHING, [hl]
// call StartSimulatingJoypadStates
    StartSimulatingJoypadStates();
// ld a, e
    a = e;
// ld [wSimulatedJoypadStatesEnd], a
    wSimulatedJoypadStatesEnd = a;
// ld [wSimulatedJoypadStatesEnd + 1], a
    wSimulatedJoypadStatesEnd + 1 = a;
// ld a, $2
    a = $2;
// ld [wSimulatedJoypadStatesIndex], a
    wSimulatedJoypadStatesIndex = a;
// call LoadHoppingShadowOAM
    LoadHoppingShadowOAM();
// ld a, SFX_LEDGE
    a = SFX_LEDGE;
// call PlaySound
    PlaySound();
// ret
    return;
//
// INCLUDE "data/tilesets/ledge_tiles.asm"
//
LoadHoppingShadowOAM:
// ld hl, vChars1 tile $7f
    hl = vChars1 tile $7f;
// ld de, LedgeHoppingShadow
    de = LedgeHoppingShadow;
// lb bc, BANK(LedgeHoppingShadow), (LedgeHoppingShadowEnd - LedgeHoppingShadow) / $8
// call CopyVideoDataDouble
    CopyVideoDataDouble();
// ld a, $9
    a = $9;
// lb bc, $54, $48 ; b, c = y, x coordinates of shadow
// ld de, LedgeHoppingShadowOAMBlock
    de = LedgeHoppingShadowOAMBlock;
// call WriteOAMBlock
    WriteOAMBlock();
// ret
    return;
//
LedgeHoppingShadow:
// INCBIN "gfx/overworld/shadow.1bpp"
LedgeHoppingShadowEnd:
//
LedgeHoppingShadowOAMBlock:
// tile ID, attributes
// db $ff, OAM_OBP1
// db $ff, OAM_HFLIP
// db $ff, OAM_VFLIP
// db $ff, OAM_HFLIP | OAM_VFLIP
}
