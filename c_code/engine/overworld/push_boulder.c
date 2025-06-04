void TryPushingBoulder(void) {
TryPushingBoulder:
// ld a, [wStatusFlags1]
    a = wStatusFlags1;
// bit BIT_STRENGTH_ACTIVE, a
// ret z
    return;
// ld a, [wMiscFlags]
    a = wMiscFlags;
// bit BIT_BOULDER_DUST, a
// ret nz
    return;
// xor a
    a = 0;
// ldh [hSpriteIndex], a
    hSpriteIndex = a;
// call IsSpriteInFrontOfPlayer
    IsSpriteInFrontOfPlayer();
// ldh a, [hSpriteIndex]
    a = hSpriteIndex;
// ld [wBoulderSpriteIndex], a
    wBoulderSpriteIndex = a;
// and a
// jp z, ResetBoulderPushFlags
    z, ResetBoulderPushFlags(); /* jp */
// ld hl, wSpritePlayerStateData1MovementStatus
    hl = wSpritePlayerStateData1MovementStatus;
// ld d, $0
// ldh a, [hSpriteIndex]
    a = hSpriteIndex;
// swap a
// ld e, a
// add hl, de
// res BIT_FACE_PLAYER, [hl]
// call GetSpriteMovementByte2Pointer
    GetSpriteMovementByte2Pointer();
// ld a, [hl]
    a = hl;
// cp BOULDER_MOVEMENT_BYTE_2
// jp nz, ResetBoulderPushFlags
    nz, ResetBoulderPushFlags(); /* jp */
// ld hl, wMiscFlags
    hl = wMiscFlags;
// bit BIT_TRIED_PUSH_BOULDER, [hl]
// set BIT_TRIED_PUSH_BOULDER, [hl]
// ret z ; the player must try pushing twice before the boulder will move
    return;
// ldh a, [hJoyHeld]
    a = hJoyHeld;
// and D_RIGHT | D_LEFT | D_UP | D_DOWN
// ret z
    return;
// predef CheckForCollisionWhenPushingBoulder
// ld a, [wTileInFrontOfBoulderAndBoulderCollisionResult]
    a = wTileInFrontOfBoulderAndBoulderCollisionResult;
// and a ; was there a collision?
// jp nz, ResetBoulderPushFlags
    nz, ResetBoulderPushFlags(); /* jp */
// ldh a, [hJoyHeld]
    a = hJoyHeld;
// ld b, a
// ld a, [wSpritePlayerStateData1FacingDirection]
    a = wSpritePlayerStateData1FacingDirection;
// cp SPRITE_FACING_UP
// jr z, .pushBoulderUp
// cp SPRITE_FACING_LEFT
// jr z, .pushBoulderLeft
// cp SPRITE_FACING_RIGHT
// jr z, .pushBoulderRight
// .pushBoulderDown
// bit BIT_D_DOWN, b
// ret z
    return;
// ld de, PushBoulderDownMovementData
    de = PushBoulderDownMovementData;
// jr .done
// .pushBoulderUp
// bit BIT_D_UP, b
// ret z
    return;
// ld de, PushBoulderUpMovementData
    de = PushBoulderUpMovementData;
// jr .done
// .pushBoulderLeft
// bit BIT_D_LEFT, b
// ret z
    return;
// ld de, PushBoulderLeftMovementData
    de = PushBoulderLeftMovementData;
// jr .done
// .pushBoulderRight
// bit BIT_D_RIGHT, b
// ret z
    return;
// ld de, PushBoulderRightMovementData
    de = PushBoulderRightMovementData;
// .done
// call MoveSprite
    MoveSprite();
// ld a, SFX_PUSH_BOULDER
    a = SFX_PUSH_BOULDER;
// call PlaySound
    PlaySound();
// ld hl, wMiscFlags
    hl = wMiscFlags;
// set BIT_BOULDER_DUST, [hl]
// ret
    return;
//
PushBoulderUpMovementData:
// db NPC_MOVEMENT_UP
// db -1 ; end
//
PushBoulderDownMovementData:
// db NPC_MOVEMENT_DOWN
// db -1 ; end
//
PushBoulderLeftMovementData:
// db NPC_MOVEMENT_LEFT
// db -1 ; end
//
PushBoulderRightMovementData:
// db NPC_MOVEMENT_RIGHT
// db -1 ; end
//
DoBoulderDustAnimation:
// ld a, [wStatusFlags5]
    a = wStatusFlags5;
// bit BIT_SCRIPTED_NPC_MOVEMENT, a
// ret nz
    return;
// callfar AnimateBoulderDust
// call DiscardButtonPresses
    DiscardButtonPresses();
// ld [wJoyIgnore], a
    wJoyIgnore = a;
// call ResetBoulderPushFlags
    ResetBoulderPushFlags();
// set BIT_PUSHED_BOULDER, [hl]
// ld a, [wBoulderSpriteIndex]
    a = wBoulderSpriteIndex;
// ldh [hSpriteIndex], a
    hSpriteIndex = a;
// call GetSpriteMovementByte2Pointer
    GetSpriteMovementByte2Pointer();
// ld [hl], $10
// ld a, SFX_CUT
    a = SFX_CUT;
// jp PlaySound
    PlaySound(); /* jp */
//
ResetBoulderPushFlags:
// ld hl, wMiscFlags
    hl = wMiscFlags;
// res BIT_BOULDER_DUST, [hl]
// res BIT_TRIED_PUSH_BOULDER, [hl]
// ret
    return;
}
