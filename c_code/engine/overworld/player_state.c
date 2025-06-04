void IsPlayerStandingOnWarp(void) {
// only used for setting BIT_STANDING_ON_WARP of wMovementFlags upon entering a new map
IsPlayerStandingOnWarp:
// ld a, [wNumberOfWarps]
    a = wNumberOfWarps;
// and a
// ret z
    return;
// ld c, a
// ld hl, wWarpEntries
    hl = wWarpEntries;
// .loop
// ld a, [wYCoord]
    a = wYCoord;
// cp [hl]
// jr nz, .nextWarp1
// inc hl
// ld a, [wXCoord]
    a = wXCoord;
// cp [hl]
// jr nz, .nextWarp2
// inc hl
// ld a, [hli] ; target warp
    a = hli;
// ld [wDestinationWarpID], a
    wDestinationWarpID = a;
// ld a, [hl] ; target map
    a = hl;
// ldh [hWarpDestinationMap], a
    hWarpDestinationMap = a;
// ld hl, wMovementFlags
    hl = wMovementFlags;
// set BIT_STANDING_ON_WARP, [hl]
// ret
    return;
// .nextWarp1
// inc hl
// .nextWarp2
// inc hl
// inc hl
// inc hl
// dec c
// jr nz, .loop
// ret
    return;
//
CheckForceBikeOrSurf:
// ld hl, wStatusFlags6
    hl = wStatusFlags6;
// bit BIT_ALWAYS_ON_BIKE, [hl]
// ret nz
    return;
// ld hl, ForcedBikeOrSurfMaps
    hl = ForcedBikeOrSurfMaps;
// ld a, [wYCoord]
    a = wYCoord;
// ld b, a
// ld a, [wXCoord]
    a = wXCoord;
// ld c, a
// ld a, [wCurMap]
    a = wCurMap;
// ld d, a
// .loop
// ld a, [hli]
    a = hli;
// cp $ff
// ret z ; if we reach FF then it's not part of the list
    return;
// cp d ; compare to current map
// jr nz, .incorrectMap
// ld a, [hli]
    a = hli;
// cp b ; compare y-coord
// jr nz, .incorrectY
// ld a, [hli]
    a = hli;
// cp c ; compare x-coord
// jr nz, .loop ; incorrect x-coord, check next item
// ld a, [wCurMap]
    a = wCurMap;
// cp SEAFOAM_ISLANDS_B3F
// ld a, SCRIPT_SEAFOAMISLANDSB3F_MOVE_OBJECT
    a = SCRIPT_SEAFOAMISLANDSB3F_MOVE_OBJECT;
// ld [wSeafoamIslandsB3FCurScript], a
    wSeafoamIslandsB3FCurScript = a;
// jr z, .forceSurfing
// ld a, [wCurMap]
    a = wCurMap;
// cp SEAFOAM_ISLANDS_B4F
// ld a, SCRIPT_SEAFOAMISLANDSB4F_MOVE_OBJECT
    a = SCRIPT_SEAFOAMISLANDSB4F_MOVE_OBJECT;
// ld [wSeafoamIslandsB4FCurScript], a
    wSeafoamIslandsB4FCurScript = a;
// jr z, .forceSurfing
// ld hl, wStatusFlags6
    hl = wStatusFlags6;
// set BIT_ALWAYS_ON_BIKE, [hl]
// ld a, $1
    a = $1;
// ld [wWalkBikeSurfState], a
    wWalkBikeSurfState = a;
// ld [wWalkBikeSurfStateCopy], a
    wWalkBikeSurfStateCopy = a;
// jp ForceBikeOrSurf
    ForceBikeOrSurf(); /* jp */
// .incorrectMap
// inc hl
// .incorrectY
// inc hl
// jr .loop
// .forceSurfing
// ld a, $2
    a = $2;
// ld [wWalkBikeSurfState], a
    wWalkBikeSurfState = a;
// ld [wWalkBikeSurfStateCopy], a
    wWalkBikeSurfStateCopy = a;
// jp ForceBikeOrSurf
    ForceBikeOrSurf(); /* jp */
//
// INCLUDE "data/maps/force_bike_surf.asm"
//
IsPlayerFacingEdgeOfMap:
// push hl
// push de
// push bc
// ld a, [wSpritePlayerStateData1FacingDirection]
    a = wSpritePlayerStateData1FacingDirection;
// srl a
// ld c, a
// ld b, $0
// ld hl, .functionPointerTable
    hl = .functionPointerTable;
// add hl, bc
// ld a, [hli]
    a = hli;
// ld h, [hl]
// ld l, a
// ld a, [wYCoord]
    a = wYCoord;
// ld b, a
// ld a, [wXCoord]
    a = wXCoord;
// ld c, a
// ld de, .return
    de = .return;
// push de
// jp hl
    hl(); /* jp */
// .return
// pop bc
// pop de
// pop hl
// ret
    return;
//
// .functionPointerTable
// dw .facingDown
// dw .facingUp
// dw .facingLeft
// dw .facingRight
//
// .facingDown
// ld a, [wCurMapHeight]
    a = wCurMapHeight;
// add a
// dec a
// cp b
// jr z, .setCarry
// jr .resetCarry
//
// .facingUp
// ld a, b
    a = b;
// and a
// jr z, .setCarry
// jr .resetCarry
//
// .facingLeft
// ld a, c
    a = c;
// and a
// jr z, .setCarry
// jr .resetCarry
//
// .facingRight
// ld a, [wCurMapWidth]
    a = wCurMapWidth;
// add a
// dec a
// cp c
// jr z, .setCarry
// jr .resetCarry
// .resetCarry
// and a
// ret
    return;
// .setCarry
// scf
// ret
    return;
//
IsWarpTileInFrontOfPlayer:
// push hl
// push de
// push bc
// call _GetTileAndCoordsInFrontOfPlayer
    _GetTileAndCoordsInFrontOfPlayer();
// ld a, [wCurMap]
    a = wCurMap;
// cp SS_ANNE_BOW
// jr z, IsSSAnneBowWarpTileInFrontOfPlayer
// ld a, [wSpritePlayerStateData1FacingDirection]
    a = wSpritePlayerStateData1FacingDirection;
// srl a
// ld c, a
// ld b, 0
// ld hl, WarpTileListPointers
    hl = WarpTileListPointers;
// add hl, bc
// ld a, [hli]
    a = hli;
// ld h, [hl]
// ld l, a
// ld a, [wTileInFrontOfPlayer]
    a = wTileInFrontOfPlayer;
// ld de, $1
    de = $1;
// call IsInArray
    IsInArray();
// .done
// pop bc
// pop de
// pop hl
// ret
    return;
//
// INCLUDE "data/tilesets/warp_carpet_tile_ids.asm"
//
IsSSAnneBowWarpTileInFrontOfPlayer:
// ld a, [wTileInFrontOfPlayer]
    a = wTileInFrontOfPlayer;
// cp $15
// jr nz, .notSSAnne5Warp
// scf
// jr IsWarpTileInFrontOfPlayer.done
// .notSSAnne5Warp
// and a
// jr IsWarpTileInFrontOfPlayer.done
//
IsPlayerStandingOnDoorTileOrWarpTile:
// push hl
// push de
// push bc
// farcall IsPlayerStandingOnDoorTile
// jr c, .done
// ld a, [wCurMapTileset]
    a = wCurMapTileset;
// add a
// ld c, a
// ld b, $0
// ld hl, WarpTileIDPointers
    hl = WarpTileIDPointers;
// add hl, bc
// ld a, [hli]
    a = hli;
// ld h, [hl]
// ld l, a
// ld de, $1
    de = $1;
// lda_coord 8, 9
// call IsInArray
    IsInArray();
// jr nc, .done
// ld hl, wMovementFlags
    hl = wMovementFlags;
// res BIT_STANDING_ON_WARP, [hl]
// .done
// pop bc
// pop de
// pop hl
// ret
    return;
//
// INCLUDE "data/tilesets/warp_tile_ids.asm"
//
PrintSafariZoneSteps:
// ld a, [wCurMap]
    a = wCurMap;
// cp SAFARI_ZONE_EAST
// ret c
    return;
// cp CERULEAN_CAVE_2F
// ret nc
    return;
// hlcoord 0, 0
// ld b, 3
// ld c, 7
// call TextBoxBorder
    TextBoxBorder();
// hlcoord 1, 1
// ld de, wSafariSteps
    de = wSafariSteps;
// lb bc, 2, 3
// call PrintNumber
    PrintNumber();
// hlcoord 4, 1
// ld de, SafariSteps
    de = SafariSteps;
// call PlaceString
    PlaceString();
// hlcoord 1, 3
// ld de, SafariBallText
    de = SafariBallText;
// call PlaceString
    PlaceString();
// ld a, [wNumSafariBalls]
    a = wNumSafariBalls;
// cp 10
// jr nc, .tenOrMore
// hlcoord 5, 3
// ld a, " "
    a = " ";
// ld [hl], a
    *hl = a;
// .tenOrMore
// hlcoord 6, 3
// ld de, wNumSafariBalls
    de = wNumSafariBalls;
// lb bc, 1, 2
// jp PrintNumber
    PrintNumber(); /* jp */
//
SafariSteps:
// db "/500@"
//
SafariBallText:
// db "BALL×× @"
//
GetTileAndCoordsInFrontOfPlayer:
// call GetPredefRegisters
    GetPredefRegisters();
//
_GetTileAndCoordsInFrontOfPlayer:
// ld a, [wYCoord]
    a = wYCoord;
// ld d, a
// ld a, [wXCoord]
    a = wXCoord;
// ld e, a
// ld a, [wSpritePlayerStateData1FacingDirection]
    a = wSpritePlayerStateData1FacingDirection;
// and a ; cp SPRITE_FACING_DOWN
// jr nz, .notFacingDown
// facing down
// lda_coord 8, 11
// inc d
// jr .storeTile
// .notFacingDown
// cp SPRITE_FACING_UP
// jr nz, .notFacingUp
// facing up
// lda_coord 8, 7
// dec d
// jr .storeTile
// .notFacingUp
// cp SPRITE_FACING_LEFT
// jr nz, .notFacingLeft
// facing left
// lda_coord 6, 9
// dec e
// jr .storeTile
// .notFacingLeft
// cp SPRITE_FACING_RIGHT
// jr nz, .storeTile
// facing right
// lda_coord 10, 9
// inc e
// .storeTile
// ld c, a
// ld [wTileInFrontOfPlayer], a
    wTileInFrontOfPlayer = a;
// ret
    return;
//
// hPlayerFacing
// const_def
// const BIT_FACING_DOWN  ; 0
// const BIT_FACING_UP    ; 1
// const BIT_FACING_LEFT  ; 2
// const BIT_FACING_RIGHT ; 3
//
GetTileTwoStepsInFrontOfPlayer:
// xor a
    a = 0;
// ldh [hPlayerFacing], a
    hPlayerFacing = a;
// ld hl, wYCoord
    hl = wYCoord;
// ld a, [hli]
    a = hli;
// ld d, a
// ld e, [hl]
// ld a, [wSpritePlayerStateData1FacingDirection]
    a = wSpritePlayerStateData1FacingDirection;
// and a ; cp SPRITE_FACING_DOWN
// jr nz, .notFacingDown
// facing down
// ld hl, hPlayerFacing
    hl = hPlayerFacing;
// set BIT_FACING_DOWN, [hl]
// lda_coord 8, 13
// inc d
// jr .storeTile
// .notFacingDown
// cp SPRITE_FACING_UP
// jr nz, .notFacingUp
// facing up
// ld hl, hPlayerFacing
    hl = hPlayerFacing;
// set BIT_FACING_UP, [hl]
// lda_coord 8, 5
// dec d
// jr .storeTile
// .notFacingUp
// cp SPRITE_FACING_LEFT
// jr nz, .notFacingLeft
// facing left
// ld hl, hPlayerFacing
    hl = hPlayerFacing;
// set BIT_FACING_LEFT, [hl]
// lda_coord 4, 9
// dec e
// jr .storeTile
// .notFacingLeft
// cp SPRITE_FACING_RIGHT
// jr nz, .storeTile
// facing right
// ld hl, hPlayerFacing
    hl = hPlayerFacing;
// set BIT_FACING_RIGHT, [hl]
// lda_coord 12, 9
// inc e
// .storeTile
// ld c, a
// ld [wTileInFrontOfBoulderAndBoulderCollisionResult], a
    wTileInFrontOfBoulderAndBoulderCollisionResult = a;
// ld [wTileInFrontOfPlayer], a
    wTileInFrontOfPlayer = a;
// ret
    return;
//
CheckForCollisionWhenPushingBoulder:
// call GetTileTwoStepsInFrontOfPlayer
    GetTileTwoStepsInFrontOfPlayer();
// ld hl, wTilesetCollisionPtr
    hl = wTilesetCollisionPtr;
// ld a, [hli]
    a = hli;
// ld h, [hl]
// ld l, a
// .loop
// ld a, [hli]
    a = hli;
// cp $ff
// jr z, .done ; if the tile two steps ahead is not passable
// cp c
// jr nz, .loop
// ld hl, TilePairCollisionsLand
    hl = TilePairCollisionsLand;
// call CheckForTilePairCollisions2
    CheckForTilePairCollisions2();
// ld a, $ff
    a = $ff;
// jr c, .done ; if there is an elevation difference between the current tile and the one two steps ahead
// ld a, [wTileInFrontOfBoulderAndBoulderCollisionResult]
    a = wTileInFrontOfBoulderAndBoulderCollisionResult;
// cp $15 ; stairs tile
// ld a, $ff
    a = $ff;
// jr z, .done ; if the tile two steps ahead is stairs
// call CheckForBoulderCollisionWithSprites
    CheckForBoulderCollisionWithSprites();
// .done
// ld [wTileInFrontOfBoulderAndBoulderCollisionResult], a
    wTileInFrontOfBoulderAndBoulderCollisionResult = a;
// ret
    return;
//
// sets a to $ff if there is a collision and $00 if there is no collision
CheckForBoulderCollisionWithSprites:
// ld a, [wBoulderSpriteIndex]
    a = wBoulderSpriteIndex;
// dec a
// swap a
// ld d, 0
// ld e, a
// ld hl, wSprite01StateData2MapY
    hl = wSprite01StateData2MapY;
// add hl, de
// ld a, [hli] ; map Y position
    a = hli;
// ldh [hPlayerYCoord], a
    hPlayerYCoord = a;
// ld a, [hl] ; map X position
    a = hl;
// ldh [hPlayerXCoord], a
    hPlayerXCoord = a;
// ld a, [wNumSprites]
    a = wNumSprites;
// ld c, a
// ld de, $f
    de = $f;
// ld hl, wSprite01StateData2MapY
    hl = wSprite01StateData2MapY;
// ldh a, [hPlayerFacing]
    a = hPlayerFacing;
// and (1 << BIT_FACING_UP) | (1 << BIT_FACING_DOWN)
// jr z, .pushingHorizontallyLoop
// .pushingVerticallyLoop
// inc hl
// ldh a, [hPlayerXCoord]
    a = hPlayerXCoord;
// cp [hl]
// jr nz, .nextSprite1 ; if X coordinates don't match
// dec hl
// ld a, [hli]
    a = hli;
// ld b, a
// ldh a, [hPlayerFacing]
    a = hPlayerFacing;
// ASSERT BIT_FACING_DOWN == 0
// rrca
// jr c, .pushingDown
// pushing up
// ldh a, [hPlayerYCoord]
    a = hPlayerYCoord;
// dec a
// jr .compareYCoords
// .pushingDown
// ldh a, [hPlayerYCoord]
    a = hPlayerYCoord;
// inc a
// .compareYCoords
// cp b
// jr z, .failure
// .nextSprite1
// dec c
// jr z, .success
// add hl, de
// jr .pushingVerticallyLoop
// .pushingHorizontallyLoop
// ld a, [hli]
    a = hli;
// ld b, a
// ldh a, [hPlayerYCoord]
    a = hPlayerYCoord;
// cp b
// jr nz, .nextSprite2
// ld b, [hl]
// ldh a, [hPlayerFacing]
    a = hPlayerFacing;
// bit BIT_FACING_LEFT, a
// jr nz, .pushingLeft
// pushing right
// ldh a, [hPlayerXCoord]
    a = hPlayerXCoord;
// inc a
// jr .compareXCoords
// .pushingLeft
// ldh a, [hPlayerXCoord]
    a = hPlayerXCoord;
// dec a
// .compareXCoords
// cp b
// jr z, .failure
// .nextSprite2
// dec c
// jr z, .success
// add hl, de
// jr .pushingHorizontallyLoop
// .failure
// ld a, $ff
    a = $ff;
// ret
    return;
// .success
// xor a
    a = 0;
// ret
    return;
}
