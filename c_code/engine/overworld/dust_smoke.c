void AnimateBoulderDust(void) {
AnimateBoulderDust:
// ld a, $1
    a = $1;
// ld [wWhichAnimationOffsets], a ; select the boulder dust offsets
    wWhichAnimationOffsets = a;
// ld a, [wUpdateSpritesEnabled]
    a = wUpdateSpritesEnabled;
// push af
// ld a, $ff
    a = $ff;
// ld [wUpdateSpritesEnabled], a
    wUpdateSpritesEnabled = a;
// ld a, %11100100
    a = %11100100;
// ldh [rOBP1], a
    rOBP1 = a;
// call LoadSmokeTileFourTimes
    LoadSmokeTileFourTimes();
// farcall WriteCutOrBoulderDustAnimationOAMBlock
// ld c, 8 ; number of steps in animation
// .loop
// push bc
// call GetMoveBoulderDustFunctionPointer
    GetMoveBoulderDustFunctionPointer();
// ld bc, .returnAddress
    bc = .returnAddress;
// push bc
// ld c, 4
// jp hl
    hl(); /* jp */
// .returnAddress
// ldh a, [rOBP1]
    a = rOBP1;
// xor %01100100
// ldh [rOBP1], a
    rOBP1 = a;
// call Delay3
    Delay3();
// pop bc
// dec c
// jr nz, .loop
// pop af
// ld [wUpdateSpritesEnabled], a
    wUpdateSpritesEnabled = a;
// jp LoadPlayerSpriteGraphics
    LoadPlayerSpriteGraphics(); /* jp */
//
GetMoveBoulderDustFunctionPointer:
// ld a, [wSpritePlayerStateData1FacingDirection]
    a = wSpritePlayerStateData1FacingDirection;
// ld hl, MoveBoulderDustFunctionPointerTable
    hl = MoveBoulderDustFunctionPointerTable;
// ld c, a
// ld b, $0
// add hl, bc
// ld a, [hli]
    a = hli;
// ld [wCoordAdjustmentAmount], a
    wCoordAdjustmentAmount = a;
// ld a, [hli]
    a = hli;
// ld e, a
// ld a, [hli]
    a = hli;
// ld h, [hl]
// ld l, a
// push hl
// ld hl, wShadowOAMSprite36
    hl = wShadowOAMSprite36;
// ld d, $0
// add hl, de
// ld e, l
// ld d, h
// pop hl
// ret
    return;
//
// MACRO boulder_dust_adjust
// db \1, \2 ; coords
// dw \3 ; function
// ENDM
//
MoveBoulderDustFunctionPointerTable:
// boulder_dust_adjust -1, 0, AdjustOAMBlockYPos ; down
// boulder_dust_adjust  1, 0, AdjustOAMBlockYPos ; up
// boulder_dust_adjust  1, 1, AdjustOAMBlockXPos ; left
// boulder_dust_adjust -1, 1, AdjustOAMBlockXPos ; right
//
LoadSmokeTileFourTimes:
// ld hl, vChars1 tile $7c
    hl = vChars1 tile $7c;
// ld c, 4
// .loop
// push bc
// push hl
// call LoadSmokeTile
    LoadSmokeTile();
// pop hl
// ld bc, 1 tiles
    bc = 1 tiles;
// add hl, bc
// pop bc
// dec c
// jr nz, .loop
// ret
    return;
//
LoadSmokeTile:
// ld de, SSAnneSmokePuffTile
    de = SSAnneSmokePuffTile;
// lb bc, BANK(SSAnneSmokePuffTile), (SSAnneSmokePuffTileEnd - SSAnneSmokePuffTile) / $10
// jp CopyVideoData
    CopyVideoData(); /* jp */
//
SSAnneSmokePuffTile:
// INCBIN "gfx/overworld/smoke.2bpp"
SSAnneSmokePuffTileEnd:
}
