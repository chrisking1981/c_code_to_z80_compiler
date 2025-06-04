void ReplaceTileBlock(void) {
// replaces a tile block with the one specified in [wNewTileBlockID]
// and redraws the map view if necessary
// b = Y
// c = X
ReplaceTileBlock:
// call GetPredefRegisters
    GetPredefRegisters();
// ld hl, wOverworldMap
    hl = wOverworldMap;
// ld a, [wCurMapWidth]
    a = wCurMapWidth;
// add $6
// ld e, a
// ld d, $0
// add hl, de
// add hl, de
// add hl, de
// ld e, $3
// add hl, de
// ld e, a
// ld a, b
    a = b;
// and a
// jr z, .addX
// add width * Y
// .addWidthYTimesLoop
// add hl, de
// dec b
// jr nz, .addWidthYTimesLoop
// .addX
// add hl, bc ; add X
// ld a, [wNewTileBlockID]
    a = wNewTileBlockID;
// ld [hl], a
    *hl = a;
// ld a, [wCurrentTileBlockMapViewPointer]
    a = wCurrentTileBlockMapViewPointer;
// ld c, a
// ld a, [wCurrentTileBlockMapViewPointer + 1]
    a = wCurrentTileBlockMapViewPointer + 1;
// ld b, a
// call CompareHLWithBC
    CompareHLWithBC();
// ret c ; return if the replaced tile block is below the map view in memory
    return;
// push hl
// ld l, e
// ld h, $0
// ld e, $6
// ld d, h
// add hl, hl
// add hl, hl
// add hl, de
// add hl, bc
// pop bc
// call CompareHLWithBC
    CompareHLWithBC();
// ret c ; return if the replaced tile block is above the map view in memory
    return;
//
RedrawMapView:
// ld a, [wIsInBattle]
    a = wIsInBattle;
// inc a
// ret z
    return;
// ldh a, [hAutoBGTransferEnabled]
    a = hAutoBGTransferEnabled;
// push af
// ldh a, [hTileAnimations]
    a = hTileAnimations;
// push af
// xor a
    a = 0;
// ldh [hAutoBGTransferEnabled], a
    hAutoBGTransferEnabled = a;
// ldh [hTileAnimations], a
    hTileAnimations = a;
// call LoadCurrentMapView
    LoadCurrentMapView();
// call RunDefaultPaletteCommand
    RunDefaultPaletteCommand();
// ld hl, wMapViewVRAMPointer
    hl = wMapViewVRAMPointer;
// ld a, [hli]
    a = hli;
// ld h, [hl]
// ld l, a
// ld de, -2 * BG_MAP_WIDTH
    de = -2 * BG_MAP_WIDTH;
// add hl, de
// ld a, h
    a = h;
// and $3
// or $98
// ld a, l
    a = l;
// ld [wBuffer], a
    wBuffer = a;
// ld a, h
    a = h;
// ld [wBuffer + 1], a ; this copy of the address is not used
    wBuffer + 1 = a;
// ld a, 2
    a = 2;
// ldh [hRedrawMapViewRowOffset], a
    hRedrawMapViewRowOffset = a;
// ld c, SCREEN_HEIGHT / 2 ; number of rows of 2x2 tiles (this covers the whole screen)
// .redrawRowLoop
// push bc
// push hl
// push hl
// ld hl, wTileMap - 2 * SCREEN_WIDTH
    hl = wTileMap - 2 * SCREEN_WIDTH;
// ld de, SCREEN_WIDTH
    de = SCREEN_WIDTH;
// ldh a, [hRedrawMapViewRowOffset]
    a = hRedrawMapViewRowOffset;
// .calcWRAMAddrLoop
// add hl, de
// dec a
// jr nz, .calcWRAMAddrLoop
// call CopyToRedrawRowOrColumnSrcTiles
    CopyToRedrawRowOrColumnSrcTiles();
// pop hl
// ld de, BG_MAP_WIDTH
    de = BG_MAP_WIDTH;
// ldh a, [hRedrawMapViewRowOffset]
    a = hRedrawMapViewRowOffset;
// ld c, a
// .calcVRAMAddrLoop
// add hl, de
// ld a, h
    a = h;
// and $3
// or $98
// dec c
// jr nz, .calcVRAMAddrLoop
// ldh [hRedrawRowOrColumnDest + 1], a
    hRedrawRowOrColumnDest + 1 = a;
// ld a, l
    a = l;
// ldh [hRedrawRowOrColumnDest], a
    hRedrawRowOrColumnDest = a;
// ld a, REDRAW_ROW
    a = REDRAW_ROW;
// ldh [hRedrawRowOrColumnMode], a
    hRedrawRowOrColumnMode = a;
// call DelayFrame
    DelayFrame();
// ld hl, hRedrawMapViewRowOffset
    hl = hRedrawMapViewRowOffset;
// inc [hl]
// inc [hl]
// pop hl
// pop bc
// dec c
// jr nz, .redrawRowLoop
// pop af
// ldh [hTileAnimations], a
    hTileAnimations = a;
// pop af
// ldh [hAutoBGTransferEnabled], a
    hAutoBGTransferEnabled = a;
// ret
    return;
//
CompareHLWithBC:
// ld a, h
    a = h;
// sub b
// ret nz
    return;
// ld a, l
    a = l;
// sub c
// ret
    return;
}
