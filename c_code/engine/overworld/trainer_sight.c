void _GetSpritePosition1(void) {
_GetSpritePosition1:
// ld hl, wSpriteStateData1
    hl = wSpriteStateData1;
// ld de, SPRITESTATEDATA1_YPIXELS
    de = SPRITESTATEDATA1_YPIXELS;
// ld a, [wSpriteIndex]
    a = wSpriteIndex;
// ldh [hSpriteIndex], a
    hSpriteIndex = a;
// call GetSpriteDataPointer
    GetSpriteDataPointer();
// ld a, [hli] ; x#SPRITESTATEDATA1_YPIXELS
    a = hli;
// ldh [hSpriteScreenYCoord], a
    hSpriteScreenYCoord = a;
// inc hl
// ld a, [hl] ; x#SPRITESTATEDATA1_XPIXELS
    a = hl;
// ldh [hSpriteScreenXCoord], a
    hSpriteScreenXCoord = a;
// ld de, wSpritePlayerStateData2MapY - wSpritePlayerStateData1XPixels
    de = wSpritePlayerStateData2MapY - wSpritePlayerStateData1XPixels;
// add hl, de
// ld a, [hli] ; x#SPRITESTATEDATA2_MAPY
    a = hli;
// ldh [hSpriteMapYCoord], a
    hSpriteMapYCoord = a;
// ld a, [hl] ; x#SPRITESTATEDATA2_MAPX
    a = hl;
// ldh [hSpriteMapXCoord], a
    hSpriteMapXCoord = a;
// ret
    return;
//
_GetSpritePosition2:
// ld hl, wSpriteStateData1
    hl = wSpriteStateData1;
// ld de, SPRITESTATEDATA1_YPIXELS
    de = SPRITESTATEDATA1_YPIXELS;
// ld a, [wSpriteIndex]
    a = wSpriteIndex;
// ldh [hSpriteIndex], a
    hSpriteIndex = a;
// call GetSpriteDataPointer
    GetSpriteDataPointer();
// ld a, [hli] ; x#SPRITESTATEDATA1_YPIXELS
    a = hli;
// ld [wSavedSpriteScreenY], a
    wSavedSpriteScreenY = a;
// inc hl
// ld a, [hl] ; x#SPRITESTATEDATA1_XPIXELS
    a = hl;
// ld [wSavedSpriteScreenX], a
    wSavedSpriteScreenX = a;
// ld de, wSpritePlayerStateData2MapY - wSpritePlayerStateData1XPixels
    de = wSpritePlayerStateData2MapY - wSpritePlayerStateData1XPixels;
// add hl, de
// ld a, [hli] ; x#SPRITESTATEDATA2_MAPY
    a = hli;
// ld [wSavedSpriteMapY], a
    wSavedSpriteMapY = a;
// ld a, [hl] ; x#SPRITESTATEDATA2_MAPX
    a = hl;
// ld [wSavedSpriteMapX], a
    wSavedSpriteMapX = a;
// ret
    return;
//
_SetSpritePosition1:
// ld hl, wSpriteStateData1
    hl = wSpriteStateData1;
// ld de, SPRITESTATEDATA1_YPIXELS
    de = SPRITESTATEDATA1_YPIXELS;
// ld a, [wSpriteIndex]
    a = wSpriteIndex;
// ldh [hSpriteIndex], a
    hSpriteIndex = a;
// call GetSpriteDataPointer
    GetSpriteDataPointer();
// ldh a, [hSpriteScreenYCoord] ; x#SPRITESTATEDATA1_YPIXELS
    a = hSpriteScreenYCoord;
// ld [hli], a
    *hl++ = a;
// inc hl
// ldh a, [hSpriteScreenXCoord] ; x#SPRITESTATEDATA1_XPIXELS
    a = hSpriteScreenXCoord;
// ld [hl], a
    *hl = a;
// ld de, wSpritePlayerStateData2MapY - wSpritePlayerStateData1XPixels
    de = wSpritePlayerStateData2MapY - wSpritePlayerStateData1XPixels;
// add hl, de
// ldh a, [hSpriteMapYCoord] ; x#SPRITESTATEDATA2_MAPY
    a = hSpriteMapYCoord;
// ld [hli], a
    *hl++ = a;
// ldh a, [hSpriteMapXCoord] ; x#SPRITESTATEDATA2_MAPX
    a = hSpriteMapXCoord;
// ld [hl], a
    *hl = a;
// ret
    return;
//
_SetSpritePosition2:
// ld hl, wSpriteStateData1
    hl = wSpriteStateData1;
// ld de, SPRITESTATEDATA1_YPIXELS
    de = SPRITESTATEDATA1_YPIXELS;
// ld a, [wSpriteIndex]
    a = wSpriteIndex;
// ldh [hSpriteIndex], a
    hSpriteIndex = a;
// call GetSpriteDataPointer
    GetSpriteDataPointer();
// ld a, [wSavedSpriteScreenY]
    a = wSavedSpriteScreenY;
// ld [hli], a ; x#SPRITESTATEDATA1_YPIXELS
    *hl++ = a;
// inc hl
// ld a, [wSavedSpriteScreenX]
    a = wSavedSpriteScreenX;
// ld [hl], a ; x#SPRITESTATEDATA1_XPIXELS
    *hl = a;
// ld de, wSpritePlayerStateData2MapY - wSpritePlayerStateData1XPixels
    de = wSpritePlayerStateData2MapY - wSpritePlayerStateData1XPixels;
// add hl, de
// ld a, [wSavedSpriteMapY]
    a = wSavedSpriteMapY;
// ld [hli], a ; x#SPRITESTATEDATA2_MAPY
    *hl++ = a;
// ld a, [wSavedSpriteMapX]
    a = wSavedSpriteMapX;
// ld [hl], a ; x#SPRITESTATEDATA2_MAPX
    *hl = a;
// ret
    return;
//
TrainerWalkUpToPlayer:
// ld a, [wSpriteIndex]
    a = wSpriteIndex;
// swap a
// ld [wTrainerSpriteOffset], a
    wTrainerSpriteOffset = a;
// call ReadTrainerScreenPosition
    ReadTrainerScreenPosition();
// ld a, [wTrainerFacingDirection]
    a = wTrainerFacingDirection;
// and a ; SPRITE_FACING_DOWN
// jr z, .facingDown
// cp SPRITE_FACING_UP
// jr z, .facingUp
// cp SPRITE_FACING_LEFT
// jr z, .facingLeft
// jr .facingRight
// .facingDown
// ld a, [wTrainerScreenY]
    a = wTrainerScreenY;
// ld b, a
// ld a, $3c           ; (fixed) player screen Y pos
    a = $3c           ; (fixed) player screen Y pos;
// call CalcDifference
    CalcDifference();
// cp $10              ; trainer is right above player
// ret z
    return;
// swap a
// dec a
// ld c, a             ; bc = steps yet to go to reach player
// xor a ; NPC_MOVEMENT_DOWN
    a = 0;
// ld b, a
// jr .writeWalkScript
// .facingUp
// ld a, [wTrainerScreenY]
    a = wTrainerScreenY;
// ld b, a
// ld a, $3c           ; (fixed) player screen Y pos
    a = $3c           ; (fixed) player screen Y pos;
// call CalcDifference
    CalcDifference();
// cp $10              ; trainer is right below player
// ret z
    return;
// swap a
// dec a
// ld c, a             ; bc = steps yet to go to reach player
// ld b, $0
// ld a, NPC_MOVEMENT_UP
    a = NPC_MOVEMENT_UP;
// jr .writeWalkScript
// .facingRight
// ld a, [wTrainerScreenX]
    a = wTrainerScreenX;
// ld b, a
// ld a, $40           ; (fixed) player screen X pos
    a = $40           ; (fixed) player screen X pos;
// call CalcDifference
    CalcDifference();
// cp $10              ; trainer is directly left of player
// ret z
    return;
// swap a
// dec a
// ld c, a             ; bc = steps yet to go to reach player
// ld b, $0
// ld a, NPC_MOVEMENT_RIGHT
    a = NPC_MOVEMENT_RIGHT;
// jr .writeWalkScript
// .facingLeft
// ld a, [wTrainerScreenX]
    a = wTrainerScreenX;
// ld b, a
// ld a, $40           ; (fixed) player screen X pos
    a = $40           ; (fixed) player screen X pos;
// call CalcDifference
    CalcDifference();
// cp $10              ; trainer is directly right of player
// ret z
    return;
// swap a
// dec a
// ld c, a             ; bc = steps yet to go to reach player
// ld b, $0
// ld a, NPC_MOVEMENT_LEFT
    a = NPC_MOVEMENT_LEFT;
// .writeWalkScript
// ld hl, wNPCMovementDirections2
    hl = wNPCMovementDirections2;
// ld de, wNPCMovementDirections2
    de = wNPCMovementDirections2;
// call FillMemory     ; write the necessary steps to reach player
    FillMemory     ; write the necessary steps to reach player();
// ld [hl], $ff        ; write end of list sentinel
// ld a, [wSpriteIndex]
    a = wSpriteIndex;
// ldh [hSpriteIndex], a
    hSpriteIndex = a;
// jp MoveSprite_
    MoveSprite_(); /* jp */
//
// input: de = offset within sprite entry
// output: hl = pointer to sprite data
GetSpriteDataPointer:
// push de
// add hl, de
// ldh a, [hSpriteIndex]
    a = hSpriteIndex;
// swap a
// ld d, $0
// ld e, a
// add hl, de
// pop de
// ret
    return;
//
// tests if this trainer is in the right position to engage the player and do so if she is.
TrainerEngage:
// push hl
// push de
// ld a, [wTrainerSpriteOffset]
    a = wTrainerSpriteOffset;
// add SPRITESTATEDATA1_IMAGEINDEX
// ld d, $0
// ld e, a
// ld hl, wSpriteStateData1
    hl = wSpriteStateData1;
// add hl, de
// ld a, [hl]             ; x#SPRITESTATEDATA1_IMAGEINDEX
    a = hl;
// sub $ff
// jr nz, .spriteOnScreen ; test if sprite is on screen
// jp .noEngage
    .noEngage(); /* jp */
// .spriteOnScreen
// ld a, [wTrainerSpriteOffset]
    a = wTrainerSpriteOffset;
// add SPRITESTATEDATA1_FACINGDIRECTION
// ld d, $0
// ld e, a
// ld hl, wSpriteStateData1
    hl = wSpriteStateData1;
// add hl, de
// ld a, [hl]             ; x#SPRITESTATEDATA1_FACINGDIRECTION
    a = hl;
// ld [wTrainerFacingDirection], a
    wTrainerFacingDirection = a;
// call ReadTrainerScreenPosition
    ReadTrainerScreenPosition();
// ld a, [wTrainerScreenY]          ; sprite screen Y pos
    a = wTrainerScreenY;
// ld b, a
// ld a, $3c
    a = $3c;
// cp b
// jr z, .linedUpY
// ld a, [wTrainerScreenX]          ; sprite screen X pos
    a = wTrainerScreenX;
// ld b, a
// ld a, $40
    a = $40;
// cp b
// jr z, .linedUpX
// xor a
    a = 0;
// jp .noEngage
    .noEngage(); /* jp */
// .linedUpY
// ld a, [wTrainerScreenX]        ; sprite screen X pos
    a = wTrainerScreenX;
// ld b, a
// ld a, $40            ; (fixed) player X position
    a = $40            ; (fixed) player X position;
// call CalcDifference  ; calc distance
    CalcDifference  ; calc distance();
// jr z, .noEngage      ; exact same position as player
// call CheckSpriteCanSeePlayer
    CheckSpriteCanSeePlayer();
// jr c, .engage
// xor a
    a = 0;
// jr .noEngage
// .linedUpX
// ld a, [wTrainerScreenY]        ; sprite screen Y pos
    a = wTrainerScreenY;
// ld b, a
// ld a, $3c            ; (fixed) player Y position
    a = $3c            ; (fixed) player Y position;
// call CalcDifference  ; calc distance
    CalcDifference  ; calc distance();
// jr z, .noEngage      ; exact same position as player
// call CheckSpriteCanSeePlayer
    CheckSpriteCanSeePlayer();
// jr c, .engage
// xor a
    a = 0;
// jp .noEngage
    .noEngage(); /* jp */
// .engage
// call CheckPlayerIsInFrontOfSprite
    CheckPlayerIsInFrontOfSprite();
// ld a, [wTrainerSpriteOffset]
    a = wTrainerSpriteOffset;
// and a
// jr z, .noEngage
// ld hl, wMiscFlags
    hl = wMiscFlags;
// set BIT_SEEN_BY_TRAINER, [hl]
// call EngageMapTrainer
    EngageMapTrainer();
// ld a, $ff
    a = $ff;
// .noEngage
// ld [wTrainerSpriteOffset], a
    wTrainerSpriteOffset = a;
// pop de
// pop hl
// ret
    return;
//
// reads trainer's Y position to wTrainerScreenY and X position to wTrainerScreenX
ReadTrainerScreenPosition:
// ld a, [wTrainerSpriteOffset]
    a = wTrainerSpriteOffset;
// add SPRITESTATEDATA1_YPIXELS
// ld d, $0
// ld e, a
// ld hl, wSpriteStateData1
    hl = wSpriteStateData1;
// add hl, de
// ld a, [hl] ; x#SPRITESTATEDATA1_YPIXELS
    a = hl;
// ld [wTrainerScreenY], a
    wTrainerScreenY = a;
// ld a, [wTrainerSpriteOffset]
    a = wTrainerSpriteOffset;
// add SPRITESTATEDATA1_XPIXELS
// ld d, $0
// ld e, a
// ld hl, wSpriteStateData1
    hl = wSpriteStateData1;
// add hl, de
// ld a, [hl] ; x#SPRITESTATEDATA1_XPIXELS
    a = hl;
// ld [wTrainerScreenX], a
    wTrainerScreenX = a;
// ret
    return;
//
// checks if the sprite is properly lined up with the player with respect to the direction it's looking. Also checks the distance between player and sprite
// note that this does not necessarily mean the sprite is seeing the player, he could be behind it's back
// a: distance player to sprite
CheckSpriteCanSeePlayer:
// ld b, a
// ld a, [wTrainerEngageDistance] ; how far the trainer can see
    a = wTrainerEngageDistance;
// cp b
// jr nc, .checkIfLinedUp
// jr .notInLine         ; player too far away
// .checkIfLinedUp
// ld a, [wTrainerFacingDirection]         ; sprite facing direction
    a = wTrainerFacingDirection;
// cp SPRITE_FACING_DOWN
// jr z, .checkXCoord
// cp SPRITE_FACING_UP
// jr z, .checkXCoord
// cp SPRITE_FACING_LEFT
// jr z, .checkYCoord
// cp SPRITE_FACING_RIGHT
// jr z, .checkYCoord
// jr .notInLine
// .checkXCoord
// ld a, [wTrainerScreenX]         ; sprite screen X position
    a = wTrainerScreenX;
// ld b, a
// cp $40
// jr z, .inLine
// jr .notInLine
// .checkYCoord
// ld a, [wTrainerScreenY]         ; sprite screen Y position
    a = wTrainerScreenY;
// ld b, a
// cp $3c
// jr nz, .notInLine
// .inLine
// scf
// ret
    return;
// .notInLine
// and a
// ret
    return;
//
// tests if the player is in front of the sprite (rather than behind it)
CheckPlayerIsInFrontOfSprite:
// ld a, [wCurMap]
    a = wCurMap;
// cp POWER_PLANT
// jp z, .engage       ; bypass this for power plant to get voltorb fake items to work
    z, .engage       ; bypass this for power plant to get voltorb fake items to work(); /* jp */
// ld a, [wTrainerSpriteOffset]
    a = wTrainerSpriteOffset;
// add SPRITESTATEDATA1_YPIXELS
// ld d, $0
// ld e, a
// ld hl, wSpriteStateData1
    hl = wSpriteStateData1;
// add hl, de
// ld a, [hl]          ; x#SPRITESTATEDATA1_YPIXELS
    a = hl;
// cp $fc
// jr nz, .notOnTopmostTile ; special case if sprite is on topmost tile (Y = $fc (-4)), make it come down a block
// ld a, $c
    a = $c;
// .notOnTopmostTile
// ld [wTrainerScreenY], a
    wTrainerScreenY = a;
// ld a, [wTrainerSpriteOffset]
    a = wTrainerSpriteOffset;
// add SPRITESTATEDATA1_XPIXELS
// ld d, $0
// ld e, a
// ld hl, wSpriteStateData1
    hl = wSpriteStateData1;
// add hl, de
// ld a, [hl]          ; x#SPRITESTATEDATA1_XPIXELS
    a = hl;
// ld [wTrainerScreenX], a
    wTrainerScreenX = a;
// ld a, [wTrainerFacingDirection]       ; facing direction
    a = wTrainerFacingDirection;
// cp SPRITE_FACING_DOWN
// jr nz, .notFacingDown
// ld a, [wTrainerScreenY]       ; sprite screen Y pos
    a = wTrainerScreenY;
// cp $3c
// jr c, .engage       ; sprite above player
// jr .noEngage        ; sprite below player
// .notFacingDown
// cp SPRITE_FACING_UP
// jr nz, .notFacingUp
// ld a, [wTrainerScreenY]       ; sprite screen Y pos
    a = wTrainerScreenY;
// cp $3c
// jr nc, .engage      ; sprite below player
// jr .noEngage        ; sprite above player
// .notFacingUp
// cp SPRITE_FACING_LEFT
// jr nz, .notFacingLeft
// ld a, [wTrainerScreenX]       ; sprite screen X pos
    a = wTrainerScreenX;
// cp $40
// jr nc, .engage      ; sprite right of player
// jr .noEngage        ; sprite left of player
// .notFacingLeft
// ld a, [wTrainerScreenX]       ; sprite screen X pos
    a = wTrainerScreenX;
// cp $40
// jr nc, .noEngage    ; sprite right of player
// .engage
// ld a, $ff
    a = $ff;
// jr .done
// .noEngage
// xor a
    a = 0;
// .done
// ld [wTrainerSpriteOffset], a
    wTrainerSpriteOffset = a;
// ret
    return;
}
