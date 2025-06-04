void LoadSpinnerArrowTiles(void) {
LoadSpinnerArrowTiles:
// ld a, [wSpritePlayerStateData1ImageIndex]
    a = wSpritePlayerStateData1ImageIndex;
// srl a
// srl a
// ld hl, SpinnerPlayerFacingDirections
    hl = SpinnerPlayerFacingDirections;
// ld c, a
// ld b, $0
// add hl, bc
// ld a, [hl]
    a = hl;
// ld [wSpritePlayerStateData1ImageIndex], a
    wSpritePlayerStateData1ImageIndex = a;
// ld a, [wCurMapTileset]
    a = wCurMapTileset;
// cp FACILITY
// ld hl, FacilitySpinnerArrows
    hl = FacilitySpinnerArrows;
// jr z, .gotSpinnerArrows
// ld hl, GymSpinnerArrows
    hl = GymSpinnerArrows;
// .gotSpinnerArrows
// ld a, [wSimulatedJoypadStatesIndex]
    a = wSimulatedJoypadStatesIndex;
// bit 0, a ; even or odd?
// jr nz, .alternateGraphics
// ld de, 6 * 4
    de = 6 * 4;
// add hl, de
// .alternateGraphics
// ld a, $4
    a = $4;
// ld bc, $0
    bc = $0;
// .loop
// push af
// push hl
// push bc
// add hl, bc
// ld a, [hli]
    a = hli;
// ld e, a
// ld a, [hli]
    a = hli;
// ld d, a
// ld a, [hli]
    a = hli;
// ld c, a
// ld a, [hli]
    a = hli;
// ld b, a
// ld a, [hli]
    a = hli;
// ld h, [hl]
// ld l, a
// call CopyVideoData
    CopyVideoData();
// pop bc
// ld a, $6
    a = $6;
// add c
// ld c, a
// pop hl
// pop af
// dec a
// jr nz, .loop
// ret
    return;
//
// INCLUDE "data/tilesets/spinner_tiles.asm"
//
SpinnerPlayerFacingDirections:
// This isn't the order of the facing directions.  Rather, it's a list of
// the facing directions that come next. For example, when the player is
// facing down (00), the next facing direction is left (08).
// db $08 ; down -> left
// db $0C ; up -> right
// db $04 ; left -> up
// db $00 ; right -> down
//
// these tiles are the animation for the tiles that push the player in dungeons like Rocket HQ
SpinnerArrowAnimTiles:
// INCBIN "gfx/overworld/spinners.2bpp"
}
