void FindPathToPlayer(void) {
FindPathToPlayer:
// xor a
    a = 0;
// ld hl, hFindPathNumSteps
    hl = hFindPathNumSteps;
// ld [hli], a ; hFindPathNumSteps
    *hl++ = a;
// ld [hli], a ; hFindPathFlags
    *hl++ = a;
// ld [hli], a ; hFindPathYProgress
    *hl++ = a;
// ld [hl], a  ; hFindPathXProgress
    *hl = a;
// ld hl, wNPCMovementDirections2
    hl = wNPCMovementDirections2;
// ld de, $0
    de = $0;
// .loop
// ldh a, [hFindPathYProgress]
    a = hFindPathYProgress;
// ld b, a
// ldh a, [hNPCPlayerYDistance] ; Y distance in steps
    a = hNPCPlayerYDistance;
// call CalcDifference
    CalcDifference();
// ld d, a
// and a
// jr nz, .stillHasYProgress
// ldh a, [hFindPathFlags]
    a = hFindPathFlags;
// set BIT_PATH_FOUND_Y, a
// ldh [hFindPathFlags], a
    hFindPathFlags = a;
// .stillHasYProgress
// ldh a, [hFindPathXProgress]
    a = hFindPathXProgress;
// ld b, a
// ldh a, [hNPCPlayerXDistance] ; X distance in steps
    a = hNPCPlayerXDistance;
// call CalcDifference
    CalcDifference();
// ld e, a
// and a
// jr nz, .stillHasXProgress
// ldh a, [hFindPathFlags]
    a = hFindPathFlags;
// set BIT_PATH_FOUND_X, a
// ldh [hFindPathFlags], a
    hFindPathFlags = a;
// .stillHasXProgress
// ldh a, [hFindPathFlags]
    a = hFindPathFlags;
// cp (1 << BIT_PATH_FOUND_X) | (1 << BIT_PATH_FOUND_Y)
// jr z, .done
// Compare whether the X distance between the player and the current of the path
// is greater or if the Y distance is. Then, try to reduce whichever is greater.
// ld a, e
    a = e;
// cp d
// jr c, .yDistanceGreater
// x distance is greater
// ldh a, [hNPCPlayerRelativePosFlags]
    a = hNPCPlayerRelativePosFlags;
// bit BIT_PLAYER_LOWER_X, a
// jr nz, .playerIsLeftOfNPC
// ld d, NPC_MOVEMENT_RIGHT
// jr .next1
// .playerIsLeftOfNPC
// ld d, NPC_MOVEMENT_LEFT
// .next1
// ldh a, [hFindPathXProgress]
    a = hFindPathXProgress;
// add 1
// ldh [hFindPathXProgress], a
    hFindPathXProgress = a;
// jr .storeDirection
// .yDistanceGreater
// ldh a, [hNPCPlayerRelativePosFlags]
    a = hNPCPlayerRelativePosFlags;
// bit BIT_PLAYER_LOWER_Y, a
// jr nz, .playerIsAboveNPC
// ld d, NPC_MOVEMENT_DOWN
// jr .next2
// .playerIsAboveNPC
// ld d, NPC_MOVEMENT_UP
// .next2
// ldh a, [hFindPathYProgress]
    a = hFindPathYProgress;
// add 1
// ldh [hFindPathYProgress], a
    hFindPathYProgress = a;
// .storeDirection
// ld a, d
    a = d;
// ld [hli], a
    *hl++ = a;
// ldh a, [hFindPathNumSteps]
    a = hFindPathNumSteps;
// inc a
// ldh [hFindPathNumSteps], a
    hFindPathNumSteps = a;
// jp .loop
    .loop(); /* jp */
// .done
// ld [hl], $ff
// ret
    return;
//
CalcPositionOfPlayerRelativeToNPC:
// xor a
    a = 0;
// ldh [hNPCPlayerRelativePosFlags], a
    hNPCPlayerRelativePosFlags = a;
// ld a, [wSpritePlayerStateData1YPixels]
    a = wSpritePlayerStateData1YPixels;
// ld d, a
// ld a, [wSpritePlayerStateData1XPixels]
    a = wSpritePlayerStateData1XPixels;
// ld e, a
// ld hl, wSpriteStateData1
    hl = wSpriteStateData1;
// ldh a, [hNPCSpriteOffset]
    a = hNPCSpriteOffset;
// add l
// add SPRITESTATEDATA1_YPIXELS
// ld l, a
// jr nc, .noCarry
// inc h
// .noCarry
// ld a, d
    a = d;
// ld b, a
// ld a, [hli] ; NPC sprite screen Y position in pixels
    a = hli;
// call CalcDifference
    CalcDifference();
// jr nc, .NPCSouthOfOrAlignedWithPlayer
// .NPCNorthOfPlayer
// push hl
// ld hl, hNPCPlayerRelativePosFlags
    hl = hNPCPlayerRelativePosFlags;
// bit BIT_PLAYER_LOWER_Y, [hl]
// set BIT_PLAYER_LOWER_Y, [hl]
// pop hl
// jr .divideYDistance
// .NPCSouthOfOrAlignedWithPlayer
// push hl
// ld hl, hNPCPlayerRelativePosFlags
    hl = hNPCPlayerRelativePosFlags;
// bit BIT_PLAYER_LOWER_Y, [hl]
// res BIT_PLAYER_LOWER_Y, [hl]
// pop hl
// .divideYDistance
// push hl
// ld hl, hDividend2
    hl = hDividend2;
// ld [hli], a
    *hl++ = a;
// ld a, 16
    a = 16;
// ld [hli], a
    *hl++ = a;
// call DivideBytes ; divide Y absolute distance by 16
    DivideBytes ; divide Y absolute distance by 16();
// ld a, [hl] ; quotient
    a = hl;
// ldh [hNPCPlayerYDistance], a
    hNPCPlayerYDistance = a;
// pop hl
// inc hl
// ld b, e
// ld a, [hl] ; NPC sprite screen X position in pixels
    a = hl;
// call CalcDifference
    CalcDifference();
// jr nc, .NPCEastOfOrAlignedWithPlayer
// .NPCWestOfPlayer
// push hl
// ld hl, hNPCPlayerRelativePosFlags
    hl = hNPCPlayerRelativePosFlags;
// bit BIT_PLAYER_LOWER_X, [hl]
// set BIT_PLAYER_LOWER_X, [hl]
// pop hl
// jr .divideXDistance
// .NPCEastOfOrAlignedWithPlayer
// push hl
// ld hl, hNPCPlayerRelativePosFlags
    hl = hNPCPlayerRelativePosFlags;
// bit BIT_PLAYER_LOWER_X, [hl]
// res BIT_PLAYER_LOWER_X, [hl]
// pop hl
// .divideXDistance
// ldh [hDividend2], a
    hDividend2 = a;
// ld a, 16
    a = 16;
// ldh [hDivisor2], a
    hDivisor2 = a;
// call DivideBytes ; divide X absolute distance by 16
    DivideBytes ; divide X absolute distance by 16();
// ldh a, [hQuotient2]
    a = hQuotient2;
// ldh [hNPCPlayerXDistance], a
    hNPCPlayerXDistance = a;
// ldh a, [hNPCPlayerRelativePosPerspective]
    a = hNPCPlayerRelativePosPerspective;
// and a
// ret z
    return;
// ldh a, [hNPCPlayerRelativePosFlags]
    a = hNPCPlayerRelativePosFlags;
// cpl
// and $3
// ldh [hNPCPlayerRelativePosFlags], a
    hNPCPlayerRelativePosFlags = a;
// ret
    return;
//
ConvertNPCMovementDirectionsToJoypadMasks:
// ldh a, [hNPCMovementDirections2Index]
    a = hNPCMovementDirections2Index;
// ld [wNPCMovementDirections2Index], a
    wNPCMovementDirections2Index = a;
// dec a
// ld de, wSimulatedJoypadStatesEnd
    de = wSimulatedJoypadStatesEnd;
// ld hl, wNPCMovementDirections2
    hl = wNPCMovementDirections2;
// add l
// ld l, a
// jr nc, .loop
// inc h
// .loop
// ld a, [hld]
    a = hld;
// call ConvertNPCMovementDirectionToJoypadMask
    ConvertNPCMovementDirectionToJoypadMask();
// ld [de], a
    de = a;
// inc de
// ldh a, [hNPCMovementDirections2Index]
    a = hNPCMovementDirections2Index;
// dec a
// ldh [hNPCMovementDirections2Index], a
    hNPCMovementDirections2Index = a;
// jr nz, .loop
// ret
    return;
//
ConvertNPCMovementDirectionToJoypadMask:
// push hl
// ld b, a
// ld hl, NPCMovementDirectionsToJoypadMasksTable
    hl = NPCMovementDirectionsToJoypadMasksTable;
// .loop
// ld a, [hli]
    a = hli;
// cp $ff
// jr z, .done
// cp b
// jr z, .loadJoypadMask
// inc hl
// jr .loop
// .loadJoypadMask
// ld a, [hl]
    a = hl;
// .done
// pop hl
// ret
    return;
//
NPCMovementDirectionsToJoypadMasksTable:
// db NPC_MOVEMENT_UP, D_UP
// db NPC_MOVEMENT_DOWN, D_DOWN
// db NPC_MOVEMENT_LEFT, D_LEFT
// db NPC_MOVEMENT_RIGHT, D_RIGHT
// db $ff
//
// unreferenced
// ret
    return;
}
