void IsPlayerOnDungeonWarp(void) {
IsPlayerOnDungeonWarp:
// xor a
    a = 0;
// ld [wWhichDungeonWarp], a
    wWhichDungeonWarp = a;
// ld a, [wStatusFlags3]
    a = wStatusFlags3;
// bit BIT_ON_DUNGEON_WARP, a
// ret nz
    return;
// call ArePlayerCoordsInArray
    ArePlayerCoordsInArray();
// ret nc
    return;
// ld a, [wCoordIndex]
    a = wCoordIndex;
// ld [wWhichDungeonWarp], a
    wWhichDungeonWarp = a;
// ld hl, wStatusFlags3
    hl = wStatusFlags3;
// set BIT_ON_DUNGEON_WARP, [hl]
// ld hl, wStatusFlags6
    hl = wStatusFlags6;
// set BIT_DUNGEON_WARP, [hl]
// ret
    return;
//
// if a hidden object was found, stores $00 in [hDidntFindAnyHiddenObject], else stores $ff
CheckForHiddenObject:
// ld hl, hItemAlreadyFound
    hl = hItemAlreadyFound;
// xor a
    a = 0;
// ld [hli], a ; [hItemAlreadyFound]
    *hl++ = a;
// ld [hli], a ; [hSavedMapTextPtr]
    *hl++ = a;
// ld [hli], a ; [hSavedMapTextPtr + 1]
    *hl++ = a;
// ld [hl], a  ; [hDidntFindAnyHiddenObject]
    *hl = a;
// ld de, $0
    de = $0;
// ld hl, HiddenObjectMaps
    hl = HiddenObjectMaps;
// .hiddenMapLoop
// ld a, [hli]
    a = hli;
// ld b, a
// cp $ff
// jr z, .noMatch
// ld a, [wCurMap]
    a = wCurMap;
// cp b
// jr z, .foundMatchingMap
// inc de
// inc de
// jr .hiddenMapLoop
// .foundMatchingMap
// ld hl, HiddenObjectPointers
    hl = HiddenObjectPointers;
// add hl, de
// ld a, [hli]
    a = hli;
// ld h, [hl]
// ld l, a
// push hl
// ld hl, wHiddenObjectFunctionArgument
    hl = wHiddenObjectFunctionArgument;
// xor a
    a = 0;
// ld [hli], a
    *hl++ = a;
// ld [hli], a
    *hl++ = a;
// ld [hl], a
    *hl = a;
// pop hl
// .hiddenObjectLoop
// ld a, [hli]
    a = hli;
// cp $ff
// jr z, .noMatch
// ld [wHiddenObjectY], a
    wHiddenObjectY = a;
// ld b, a
// ld a, [hli]
    a = hli;
// ld [wHiddenObjectX], a
    wHiddenObjectX = a;
// ld c, a
// call CheckIfCoordsInFrontOfPlayerMatch
    CheckIfCoordsInFrontOfPlayerMatch();
// ldh a, [hCoordsInFrontOfPlayerMatch]
    a = hCoordsInFrontOfPlayerMatch;
// and a
// jr z, .foundMatchingObject
// inc hl
// inc hl
// inc hl
// inc hl
// push hl
// ld hl, wHiddenObjectIndex
    hl = wHiddenObjectIndex;
// inc [hl]
// pop hl
// jr .hiddenObjectLoop
// .foundMatchingObject
// ld a, [hli]
    a = hli;
// ld [wHiddenObjectFunctionArgument], a
    wHiddenObjectFunctionArgument = a;
// ld a, [hli]
    a = hli;
// ld [wHiddenObjectFunctionRomBank], a
    wHiddenObjectFunctionRomBank = a;
// ld a, [hli]
    a = hli;
// ld h, [hl]
// ld l, a
// ret
    return;
// .noMatch
// ld a, $ff
    a = $ff;
// ldh [hDidntFindAnyHiddenObject], a
    hDidntFindAnyHiddenObject = a;
// ret
    return;
//
// checks if the coordinates in front of the player's sprite match Y in b and X in c
// [hCoordsInFrontOfPlayerMatch] = $00 if they match, $ff if they don't match
CheckIfCoordsInFrontOfPlayerMatch:
// ld a, [wSpritePlayerStateData1FacingDirection]
    a = wSpritePlayerStateData1FacingDirection;
// cp SPRITE_FACING_UP
// jr z, .facingUp
// cp SPRITE_FACING_LEFT
// jr z, .facingLeft
// cp SPRITE_FACING_RIGHT
// jr z, .facingRight
// facing down
// ld a, [wYCoord]
    a = wYCoord;
// inc a
// jr .upDownCommon
// .facingUp
// ld a, [wYCoord]
    a = wYCoord;
// dec a
// .upDownCommon
// cp b
// jr nz, .didNotMatch
// ld a, [wXCoord]
    a = wXCoord;
// cp c
// jr nz, .didNotMatch
// jr .matched
// .facingLeft
// ld a, [wXCoord]
    a = wXCoord;
// dec a
// jr .leftRightCommon
// .facingRight
// ld a, [wXCoord]
    a = wXCoord;
// inc a
// .leftRightCommon
// cp c
// jr nz, .didNotMatch
// ld a, [wYCoord]
    a = wYCoord;
// cp b
// jr nz, .didNotMatch
// .matched
// xor a
    a = 0;
// jr .done
// .didNotMatch
// ld a, $ff
    a = $ff;
// .done
// ldh [hCoordsInFrontOfPlayerMatch], a
    hCoordsInFrontOfPlayerMatch = a;
// ret
    return;
//
// INCLUDE "data/events/hidden_objects.asm"
}
