void IsPlayerJustOutsideMap(void) {
// returns whether the player is one tile outside the map in Z
IsPlayerJustOutsideMap:
// ld a, [wYCoord]
    a = wYCoord;
// ld b, a
// ld a, [wCurMapHeight]
    a = wCurMapHeight;
// call .compareCoordWithMapDimension
    .compareCoordWithMapDimension();
// ret z
    return;
// ld a, [wXCoord]
    a = wXCoord;
// ld b, a
// ld a, [wCurMapWidth]
    a = wCurMapWidth;
// .compareCoordWithMapDimension
// add a
// cp b
// ret z
    return;
// inc b
// ret
    return;
}
