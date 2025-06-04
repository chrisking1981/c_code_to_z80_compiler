void PlayerStepOutFromDoor(void) {
PlayerStepOutFromDoor:
// ld hl, wStatusFlags5 ; should this be wMovementFlags?
    hl = wStatusFlags5 ; should this be wMovementFlags?;
// res BIT_EXITING_DOOR, [hl]
// call IsPlayerStandingOnDoorTile
    IsPlayerStandingOnDoorTile();
// jr nc, .notStandingOnDoor
// ld a, SELECT | START | D_RIGHT | D_LEFT | D_UP | D_DOWN
    a = SELECT | START | D_RIGHT | D_LEFT | D_UP | D_DOWN;
// ld [wJoyIgnore], a
    wJoyIgnore = a;
// ld hl, wMovementFlags
    hl = wMovementFlags;
// set BIT_EXITING_DOOR, [hl]
// ld a, $1
    a = $1;
// ld [wSimulatedJoypadStatesIndex], a
    wSimulatedJoypadStatesIndex = a;
// ld a, D_DOWN
    a = D_DOWN;
// ld [wSimulatedJoypadStatesEnd], a
    wSimulatedJoypadStatesEnd = a;
// xor a
    a = 0;
// ld [wSpritePlayerStateData1ImageIndex], a
    wSpritePlayerStateData1ImageIndex = a;
// call StartSimulatingJoypadStates
    StartSimulatingJoypadStates();
// ret
    return;
// .notStandingOnDoor
// xor a
    a = 0;
// ld [wUnusedOverrideSimulatedJoypadStatesIndex], a
    wUnusedOverrideSimulatedJoypadStatesIndex = a;
// ld [wSimulatedJoypadStatesIndex], a
    wSimulatedJoypadStatesIndex = a;
// ld [wSimulatedJoypadStatesEnd], a
    wSimulatedJoypadStatesEnd = a;
// ld hl, wMovementFlags
    hl = wMovementFlags;
// res BIT_STANDING_ON_DOOR, [hl]
// res BIT_EXITING_DOOR, [hl]
// ld hl, wStatusFlags5
    hl = wStatusFlags5;
// res BIT_SCRIPTED_MOVEMENT_STATE, [hl]
// ret
    return;
//
_EndNPCMovementScript:
// ld hl, wStatusFlags5
    hl = wStatusFlags5;
// res BIT_SCRIPTED_MOVEMENT_STATE, [hl]
// ld hl, wStatusFlags4
    hl = wStatusFlags4;
// res BIT_INIT_SCRIPTED_MOVEMENT, [hl]
// ld hl, wMovementFlags
    hl = wMovementFlags;
// res BIT_STANDING_ON_DOOR, [hl]
// res BIT_EXITING_DOOR, [hl]
// xor a
    a = 0;
// ld [wNPCMovementScriptSpriteOffset], a
    wNPCMovementScriptSpriteOffset = a;
// ld [wNPCMovementScriptPointerTableNum], a
    wNPCMovementScriptPointerTableNum = a;
// ld [wNPCMovementScriptFunctionNum], a
    wNPCMovementScriptFunctionNum = a;
// ld [wUnusedOverrideSimulatedJoypadStatesIndex], a
    wUnusedOverrideSimulatedJoypadStatesIndex = a;
// ld [wSimulatedJoypadStatesIndex], a
    wSimulatedJoypadStatesIndex = a;
// ld [wSimulatedJoypadStatesEnd], a
    wSimulatedJoypadStatesEnd = a;
// ret
    return;
//
PalletMovementScriptPointerTable:
// dw PalletMovementScript_OakMoveLeft
// dw PalletMovementScript_PlayerMoveLeft
// dw PalletMovementScript_WaitAndWalkToLab
// dw PalletMovementScript_WalkToLab
// dw PalletMovementScript_Done
//
PalletMovementScript_OakMoveLeft:
// ld a, [wXCoord]
    a = wXCoord;
// sub $a
// ld [wNumStepsToTake], a
    wNumStepsToTake = a;
// jr z, .playerOnLeftTile
// The player is on the right tile of the northern path out of Pallet Town and
// Prof. Oak is below.
// Make Prof. Oak step to the left.
// ld b, 0
// ld c, a
// ld hl, wNPCMovementDirections2
    hl = wNPCMovementDirections2;
// ld a, NPC_MOVEMENT_LEFT
    a = NPC_MOVEMENT_LEFT;
// call FillMemory
    FillMemory();
// ld [hl], $ff
// ld a, [wSpriteIndex]
    a = wSpriteIndex;
// ldh [hSpriteIndex], a
    hSpriteIndex = a;
// ld de, wNPCMovementDirections2
    de = wNPCMovementDirections2;
// call MoveSprite
    MoveSprite();
// ld a, $1
    a = $1;
// ld [wNPCMovementScriptFunctionNum], a
    wNPCMovementScriptFunctionNum = a;
// jr .done
// The player is on the left tile of the northern path out of Pallet Town and
// Prof. Oak is below.
// Prof. Oak is already where he needs to be.
// .playerOnLeftTile
// ld a, $3
    a = $3;
// ld [wNPCMovementScriptFunctionNum], a
    wNPCMovementScriptFunctionNum = a;
// .done
// ld hl, wStatusFlags7
    hl = wStatusFlags7;
// set BIT_NO_MAP_MUSIC, [hl]
// ld a, SELECT | START | D_RIGHT | D_LEFT | D_UP | D_DOWN
    a = SELECT | START | D_RIGHT | D_LEFT | D_UP | D_DOWN;
// ld [wJoyIgnore], a
    wJoyIgnore = a;
// ret
    return;
//
PalletMovementScript_PlayerMoveLeft:
// ld a, [wStatusFlags5]
    a = wStatusFlags5;
// bit BIT_SCRIPTED_NPC_MOVEMENT, a
// ret nz ; return if Oak is still moving
    return;
// ld a, [wNumStepsToTake]
    a = wNumStepsToTake;
// ld [wSimulatedJoypadStatesIndex], a
    wSimulatedJoypadStatesIndex = a;
// ldh [hNPCMovementDirections2Index], a
    hNPCMovementDirections2Index = a;
// predef ConvertNPCMovementDirectionsToJoypadMasks
// call StartSimulatingJoypadStates
    StartSimulatingJoypadStates();
// ld a, $2
    a = $2;
// ld [wNPCMovementScriptFunctionNum], a
    wNPCMovementScriptFunctionNum = a;
// ret
    return;
//
PalletMovementScript_WaitAndWalkToLab:
// ld a, [wSimulatedJoypadStatesIndex]
    a = wSimulatedJoypadStatesIndex;
// and a ; is the player done moving left yet?
// ret nz
    return;
//
PalletMovementScript_WalkToLab:
// xor a
    a = 0;
// ld [wOverrideSimulatedJoypadStatesMask], a
    wOverrideSimulatedJoypadStatesMask = a;
// ld a, [wSpriteIndex]
    a = wSpriteIndex;
// swap a
// ld [wNPCMovementScriptSpriteOffset], a
    wNPCMovementScriptSpriteOffset = a;
// xor a
    a = 0;
// ld [wSpritePlayerStateData2MovementByte1], a
    wSpritePlayerStateData2MovementByte1 = a;
// ld hl, wSimulatedJoypadStatesEnd
    hl = wSimulatedJoypadStatesEnd;
// ld de, RLEList_PlayerWalkToLab
    de = RLEList_PlayerWalkToLab;
// call DecodeRLEList
    DecodeRLEList();
// dec a
// ld [wSimulatedJoypadStatesIndex], a
    wSimulatedJoypadStatesIndex = a;
// ld hl, wNPCMovementDirections2
    hl = wNPCMovementDirections2;
// ld de, RLEList_ProfOakWalkToLab
    de = RLEList_ProfOakWalkToLab;
// call DecodeRLEList
    DecodeRLEList();
// ld hl, wStatusFlags4
    hl = wStatusFlags4;
// res BIT_INIT_SCRIPTED_MOVEMENT, [hl]
// ld hl, wStatusFlags5
    hl = wStatusFlags5;
// set BIT_SCRIPTED_MOVEMENT_STATE, [hl]
// ld a, $4
    a = $4;
// ld [wNPCMovementScriptFunctionNum], a
    wNPCMovementScriptFunctionNum = a;
// ret
    return;
//
RLEList_ProfOakWalkToLab:
// db NPC_MOVEMENT_DOWN, 5
// db NPC_MOVEMENT_LEFT, 1
// db NPC_MOVEMENT_DOWN, 5
// db NPC_MOVEMENT_RIGHT, 3
// db NPC_MOVEMENT_UP, 1
// db NPC_CHANGE_FACING, 1
// db -1 ; end
//
RLEList_PlayerWalkToLab:
// db D_UP, 2
// db D_RIGHT, 3
// db D_DOWN, 5
// db D_LEFT, 1
// db D_DOWN, 6
// db -1 ; end
//
PalletMovementScript_Done:
// ld a, [wSimulatedJoypadStatesIndex]
    a = wSimulatedJoypadStatesIndex;
// and a
// ret nz
    return;
// ld a, HS_PALLET_TOWN_OAK
    a = HS_PALLET_TOWN_OAK;
// ld [wMissableObjectIndex], a
    wMissableObjectIndex = a;
// predef HideObject
// ld hl, wStatusFlags5
    hl = wStatusFlags5;
// res BIT_SCRIPTED_MOVEMENT_STATE, [hl]
// ld hl, wStatusFlags4
    hl = wStatusFlags4;
// res BIT_INIT_SCRIPTED_MOVEMENT, [hl]
// jp EndNPCMovementScript
    EndNPCMovementScript(); /* jp */
//
PewterMuseumGuyMovementScriptPointerTable:
// dw PewterMovementScript_WalkToMuseum
// dw PewterMovementScript_Done
//
PewterMovementScript_WalkToMuseum:
// ld a, BANK(Music_MuseumGuy)
    a = BANK(Music_MuseumGuy);
// ld [wAudioROMBank], a
    wAudioROMBank = a;
// ld [wAudioSavedROMBank], a
    wAudioSavedROMBank = a;
// ld a, MUSIC_MUSEUM_GUY
    a = MUSIC_MUSEUM_GUY;
// ld [wNewSoundID], a
    wNewSoundID = a;
// call PlaySound
    PlaySound();
// ld a, [wSpriteIndex]
    a = wSpriteIndex;
// swap a
// ld [wNPCMovementScriptSpriteOffset], a
    wNPCMovementScriptSpriteOffset = a;
// call StartSimulatingJoypadStates
    StartSimulatingJoypadStates();
// ld hl, wSimulatedJoypadStatesEnd
    hl = wSimulatedJoypadStatesEnd;
// ld de, RLEList_PewterMuseumPlayer
    de = RLEList_PewterMuseumPlayer;
// call DecodeRLEList
    DecodeRLEList();
// dec a
// ld [wSimulatedJoypadStatesIndex], a
    wSimulatedJoypadStatesIndex = a;
// xor a
    a = 0;
// ld [wWhichPewterGuy], a
    wWhichPewterGuy = a;
// predef PewterGuys
// ld hl, wNPCMovementDirections2
    hl = wNPCMovementDirections2;
// ld de, RLEList_PewterMuseumGuy
    de = RLEList_PewterMuseumGuy;
// call DecodeRLEList
    DecodeRLEList();
// ld hl, wStatusFlags4
    hl = wStatusFlags4;
// res BIT_INIT_SCRIPTED_MOVEMENT, [hl]
// ld a, $1
    a = $1;
// ld [wNPCMovementScriptFunctionNum], a
    wNPCMovementScriptFunctionNum = a;
// ret
    return;
//
RLEList_PewterMuseumPlayer:
// db NO_INPUT, 1
// db D_UP, 3
// db D_LEFT, 13
// db D_UP, 6
// db -1 ; end
//
RLEList_PewterMuseumGuy:
// db NPC_MOVEMENT_UP, 6
// db NPC_MOVEMENT_LEFT, 13
// db NPC_MOVEMENT_UP, 3
// db NPC_MOVEMENT_LEFT, 1
// db -1 ; end
//
PewterMovementScript_Done:
// ld a, [wSimulatedJoypadStatesIndex]
    a = wSimulatedJoypadStatesIndex;
// and a
// ret nz
    return;
// ld hl, wStatusFlags5
    hl = wStatusFlags5;
// res BIT_SCRIPTED_MOVEMENT_STATE, [hl]
// ld hl, wStatusFlags4
    hl = wStatusFlags4;
// res BIT_INIT_SCRIPTED_MOVEMENT, [hl]
// jp EndNPCMovementScript
    EndNPCMovementScript(); /* jp */
//
PewterGymGuyMovementScriptPointerTable:
// dw PewterMovementScript_WalkToGym
// dw PewterMovementScript_Done
//
PewterMovementScript_WalkToGym:
// ld a, BANK(Music_MuseumGuy)
    a = BANK(Music_MuseumGuy);
// ld [wAudioROMBank], a
    wAudioROMBank = a;
// ld [wAudioSavedROMBank], a
    wAudioSavedROMBank = a;
// ld a, MUSIC_MUSEUM_GUY
    a = MUSIC_MUSEUM_GUY;
// ld [wNewSoundID], a
    wNewSoundID = a;
// call PlaySound
    PlaySound();
// ld a, [wSpriteIndex]
    a = wSpriteIndex;
// swap a
// ld [wNPCMovementScriptSpriteOffset], a
    wNPCMovementScriptSpriteOffset = a;
// xor a
    a = 0;
// ld [wSpritePlayerStateData2MovementByte1], a
    wSpritePlayerStateData2MovementByte1 = a;
// ld hl, wSimulatedJoypadStatesEnd
    hl = wSimulatedJoypadStatesEnd;
// ld de, RLEList_PewterGymPlayer
    de = RLEList_PewterGymPlayer;
// call DecodeRLEList
    DecodeRLEList();
// dec a
// ld [wSimulatedJoypadStatesIndex], a
    wSimulatedJoypadStatesIndex = a;
// ld a, 1
    a = 1;
// ld [wWhichPewterGuy], a
    wWhichPewterGuy = a;
// predef PewterGuys
// ld hl, wNPCMovementDirections2
    hl = wNPCMovementDirections2;
// ld de, RLEList_PewterGymGuy
    de = RLEList_PewterGymGuy;
// call DecodeRLEList
    DecodeRLEList();
// ld hl, wStatusFlags4
    hl = wStatusFlags4;
// res BIT_INIT_SCRIPTED_MOVEMENT, [hl]
// ld hl, wStatusFlags5
    hl = wStatusFlags5;
// set BIT_SCRIPTED_MOVEMENT_STATE, [hl]
// ld a, $1
    a = $1;
// ld [wNPCMovementScriptFunctionNum], a
    wNPCMovementScriptFunctionNum = a;
// ret
    return;
//
RLEList_PewterGymPlayer:
// db NO_INPUT, 1
// db D_RIGHT, 2
// db D_DOWN, 5
// db D_LEFT, 11
// db D_UP, 5
// db D_LEFT, 15
// db -1 ; end
//
RLEList_PewterGymGuy:
// db NPC_MOVEMENT_DOWN, 2
// db NPC_MOVEMENT_LEFT, 15
// db NPC_MOVEMENT_UP, 5
// db NPC_MOVEMENT_LEFT, 11
// db NPC_MOVEMENT_DOWN, 5
// db NPC_MOVEMENT_RIGHT, 3
// db -1 ; end
//
SetEnemyTrainerToStayAndFaceAnyDirection:
// ld a, [wCurMap]
    a = wCurMap;
// cp POKEMON_TOWER_7F
// ret z ; the Rockets on Pokemon Tower 7F leave after battling, so don't set them
    return;
// ld hl, RivalIDs
    hl = RivalIDs;
// ld a, [wEngagedTrainerClass]
    a = wEngagedTrainerClass;
// ld b, a
// .loop
// ld a, [hli]
    a = hli;
// cp -1
// jr z, .notRival
// cp b
// ret z ; the rival leaves after battling, so don't set him
    return;
// jr .loop
// .notRival
// ld a, [wSpriteIndex]
    a = wSpriteIndex;
// ldh [hSpriteIndex], a
    hSpriteIndex = a;
// jp SetSpriteMovementBytesToFF
    SetSpriteMovementBytesToFF(); /* jp */
//
RivalIDs:
// db OPP_RIVAL1
// db OPP_RIVAL2
// db OPP_RIVAL3
// db -1 ; end
}
