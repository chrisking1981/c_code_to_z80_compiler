void LoadTilesetHeader(void) {
LoadTilesetHeader:
// call GetPredefRegisters
    GetPredefRegisters();
// push hl
// ld d, 0
// ld a, [wCurMapTileset]
    a = wCurMapTileset;
// add a
// add a
// ld b, a
// add a
// add b ; a = tileset * 12
// jr nc, .noCarry
// inc d
// .noCarry
// ld e, a
// ld hl, Tilesets
    hl = Tilesets;
// add hl, de
// ld de, wTilesetBank
    de = wTilesetBank;
// ld c, $b
// .copyTilesetHeaderLoop
// ld a, [hli]
    a = hli;
// ld [de], a
    de = a;
// inc de
// dec c
// jr nz, .copyTilesetHeaderLoop
// ld a, [hl]
    a = hl;
// ldh [hTileAnimations], a
    hTileAnimations = a;
// xor a
    a = 0;
// ldh [hMovingBGTilesCounter1], a
    hMovingBGTilesCounter1 = a;
// pop hl
// ld a, [wCurMapTileset]
    a = wCurMapTileset;
// push hl
// push de
// ld hl, DungeonTilesets
    hl = DungeonTilesets;
// ld de, $1
    de = $1;
// call IsInArray
    IsInArray();
// pop de
// pop hl
// jr c, .dungeon
// ld a, [wCurMapTileset]
    a = wCurMapTileset;
// ld b, a
// ldh a, [hPreviousTileset]
    a = hPreviousTileset;
// cp b
// jr z, .done
// .dungeon
// ld a, [wDestinationWarpID]
    a = wDestinationWarpID;
// cp $ff
// jr z, .done
// call LoadDestinationWarpPosition
    LoadDestinationWarpPosition();
// ld a, [wYCoord]
    a = wYCoord;
// and $1
// ld [wYBlockCoord], a
    wYBlockCoord = a;
// ld a, [wXCoord]
    a = wXCoord;
// and $1
// ld [wXBlockCoord], a
    wXBlockCoord = a;
// .done
// ret
    return;
//
// INCLUDE "data/tilesets/dungeon_tilesets.asm"
//
// INCLUDE "data/tilesets/tileset_headers.asm"
}
