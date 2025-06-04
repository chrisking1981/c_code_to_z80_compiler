void IsPlayerStandingOnDoorTile(void) {
// returns whether the player is standing on a door tile in carry
IsPlayerStandingOnDoorTile:
// push de
// ld hl, DoorTileIDPointers
    hl = DoorTileIDPointers;
// ld a, [wCurMapTileset]
    a = wCurMapTileset;
// ld de, $3
    de = $3;
// call IsInArray
    IsInArray();
// pop de
// jr nc, .notStandingOnDoor
// inc hl
// ld a, [hli]
    a = hli;
// ld h, [hl]
// ld l, a
// lda_coord 8, 9 ; a = lower left background tile under player's sprite
// ld b, a
// .loop
// ld a, [hli]
    a = hli;
// and a
// jr z, .notStandingOnDoor
// cp b
// jr nz, .loop
// scf
// ret
    return;
// .notStandingOnDoor
// and a
// ret
    return;
//
// INCLUDE "data/tilesets/door_tile_ids.asm"
}
