#include <stdint.h>

// Extern declarations for variables and functions
extern uint8_t wStatusFlags5;
extern uint8_t wMovementFlags;
extern uint8_t wJoyIgnore;
extern uint8_t wSimulatedJoypadStatesIndex;
extern uint8_t wSimulatedJoypadStatesEnd;
extern uint8_t wSpritePlayerStateData1ImageIndex;
extern uint8_t wUnusedOverrideSimulatedJoypadStatesIndex;
extern uint8_t wSpriteIndex;
extern uint8_t wNPCMovementScriptSpriteOffset;
extern uint8_t wNPCMovementScriptFunctionNum;
extern uint8_t wStatusFlags4;
extern uint8_t wStatusFlags7;
extern uint8_t wNumStepsToTake;
extern uint8_t wWhichPewterGuy;
extern uint8_t hSpriteIndex;
extern uint8_t wAudioROMBank;
extern uint8_t wAudioSavedROMBank;
extern uint8_t wNewSoundID;
extern uint8_t wNPCMovementDirections2[];
extern uint8_t wSpritePlayerStateData2MovementByte1;
extern uint8_t RLEList_PlayerWalkToLab[];
extern uint8_t RLEList_ProfOakWalkToLab[];
extern uint8_t RLEList_PewterMuseumPlayer[];
extern uint8_t RLEList_PewterMuseumGuy[];
extern uint8_t RLEList_PewterGymPlayer[];
extern uint8_t RLEList_PewterGymGuy[];
extern void IsPlayerStandingOnDoorTile(void);
extern void StartSimulatingJoypadStates(void);
extern void DecodeRLEList(uint8_t* dst, const uint8_t* src);
extern void PlaySound(void);
extern void FillMemory(void);
extern void MoveSprite(void);
extern void PewterGuys(void);
extern void HideObject(void);
extern void EndNPCMovementScript(void);

// Constants
#define BIT_EXITING_DOOR 0
#define BIT_STANDING_ON_DOOR 1
#define BIT_SCRIPTED_MOVEMENT_STATE 2
#define BIT_INIT_SCRIPTED_MOVEMENT 3
#define BIT_NO_MAP_MUSIC 4
#define BIT_SCRIPTED_NPC_MOVEMENT 5

// Function implementations
void PlayerStepOutFromDoor(void) {
    // ld hl, wStatusFlags5
    uint8_t* hl = &wStatusFlags5;
    // res BIT_EXITING_DOOR, [hl]
    *hl &= ~(1 << BIT_EXITING_DOOR);
    // call IsPlayerStandingOnDoorTile
    IsPlayerStandingOnDoorTile();
    // jr nc, .notStandingOnDoor
    if (/* condition for carry flag */) goto notStandingOnDoor;
    // ld a, SELECT | START | D_RIGHT | D_LEFT | D_UP | D_DOWN
    uint8_t a = SELECT | START | D_RIGHT | D_LEFT | D_UP | D_DOWN;
    // ld [wJoyIgnore], a
    wJoyIgnore = a;
    // ld hl, wMovementFlags
    hl = &wMovementFlags;
    // set BIT_EXITING_DOOR, [hl]
    *hl |= (1 << BIT_EXITING_DOOR);
    // ld a, $1
    a = 0x1;
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

notStandingOnDoor:
    // xor a
    a = 0;
    // ld [wUnusedOverrideSimulatedJoypadStatesIndex], a
    wUnusedOverrideSimulatedJoypadStatesIndex = a;
    // ld [wSimulatedJoypadStatesIndex], a
    wSimulatedJoypadStatesIndex = a;
    // ld [wSimulatedJoypadStatesEnd], a
    wSimulatedJoypadStatesEnd = a;
    // ld hl, wMovementFlags
    hl = &wMovementFlags;
    // res BIT_STANDING_ON_DOOR, [hl]
    *hl &= ~(1 << BIT_STANDING_ON_DOOR);
    // res BIT_EXITING_DOOR, [hl]
    *hl &= ~(1 << BIT_EXITING_DOOR);
    // ld hl, wStatusFlags5
    hl = &wStatusFlags5;
    // res BIT_SCRIPTED_MOVEMENT_STATE, [hl]
    *hl &= ~(1 << BIT_SCRIPTED_MOVEMENT_STATE);
    // ret
    return;
}

void _EndNPCMovementScript(void) {
    // ld hl, wStatusFlags5
    uint8_t* hl = &wStatusFlags5;
    // res BIT_SCRIPTED_MOVEMENT_STATE, [hl]
    *hl &= ~(1 << BIT_SCRIPTED_MOVEMENT_STATE);
    // ld hl, wStatusFlags4
    hl = &wStatusFlags4;
    // res BIT_INIT_SCRIPTED_MOVEMENT, [hl]
    *hl &= ~(1 << BIT_INIT_SCRIPTED_MOVEMENT);
    // ld hl, wMovementFlags
    hl = &wMovementFlags;
    // res BIT_STANDING_ON_DOOR, [hl]
    *hl &= ~(1 << BIT_STANDING_ON_DOOR);
    // res BIT_EXITING_DOOR, [hl]
    *hl &= ~(1 << BIT_EXITING_DOOR);
    // xor a
    uint8_t a = 0;
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
}

const uint16_t PalletMovementScriptPointerTable[] = {
    (uint16_t)PalletMovementScript_OakMoveLeft,
    (uint16_t)PalletMovementScript_PlayerMoveLeft,
    (uint16_t)PalletMovementScript_WaitAndWalkToLab,
    (uint16_t)PalletMovementScript_WalkToLab,
    (uint16_t)PalletMovementScript_Done,
};

void PalletMovementScript_OakMoveLeft(void) {
    // ld a, [wXCoord]
    uint8_t a = wXCoord;
    // sub $a
    a -= a; // Assuming $a is the same as a
    // ld [wNumStepsToTake], a
    wNumStepsToTake = a;
    // jr z, .playerOnLeftTile
    if (a == 0) goto playerOnLeftTile;
    // ld b, 0
    uint8_t b = 0;
    // ld c, a
    uint8_t c = a;
    // ld hl, wNPCMovementDirections2
    uint8_t* hl = wNPCMovementDirections2;
    // ld a, NPC_MOVEMENT_LEFT
    a = NPC_MOVEMENT_LEFT;
    // call FillMemory
    FillMemory();
    // ld [hl], $ff
    *hl = 0xFF;
    // ld a, [wSpriteIndex]
    a = wSpriteIndex;
    // ldh [hSpriteIndex], a
    hSpriteIndex = a;
    // ld de, wNPCMovementDirections2
    uint8_t* de = wNPCMovementDirections2;
    // call MoveSprite
    MoveSprite();
    // ld a, $1
    a = 0x1;
    // ld [wNPCMovementScriptFunctionNum], a
    wNPCMovementScriptFunctionNum = a;
    // jr .done
    goto done;

playerOnLeftTile:
    // ld a, $3
    a = 0x3;
    // ld [wNPCMovementScriptFunctionNum], a
    wNPCMovementScriptFunctionNum = a;

done:
    // ld hl, wStatusFlags7
    uint8_t* hl = &wStatusFlags7;
    // set BIT_NO_MAP_MUSIC, [hl]
    *hl |= (1 << BIT_NO_MAP_MUSIC);
    // ld a, SELECT | START | D_RIGHT | D_LEFT | D_UP | D_DOWN
    a = SELECT | START | D_RIGHT | D_LEFT | D_UP | D_DOWN;
    // ld [wJoyIgnore], a
    wJoyIgnore = a;
    // ret
    return;
}

void PalletMovementScript_PlayerMoveLeft(void) {
    // ld a, [wStatusFlags5]
    uint8_t a = wStatusFlags5;
    // bit BIT_SCRIPTED_NPC_MOVEMENT, a
    if (a & (1 << BIT_SCRIPTED_NPC_MOVEMENT)) return; // ret nz
    // ld a, [wNumStepsToTake]
    a = wNumStepsToTake;
    // ld [wSimulatedJoypadStatesIndex], a
    wSimulatedJoypadStatesIndex = a;
    // ldh [hNPCMovementDirections2Index], a
    hNPCMovementDirections2Index = a;
    // predef ConvertNPCMovementDirectionsToJoypadMasks
    ConvertNPCMovementDirectionsToJoypadMasks();
    // call StartSimulatingJoypadStates
    StartSimulatingJoypadStates();
    // ld a, $2
    a = 0x2;
    // ld [wNPCMovementScriptFunctionNum], a
    wNPCMovementScriptFunctionNum = a;
    // ret
    return;
}

void PalletMovementScript_WaitAndWalkToLab(void) {
    // ld a, [wSimulatedJoypadStatesIndex]
    uint8_t a = wSimulatedJoypadStatesIndex;
    // and a ; is the player done moving left yet?
    if (a != 0) return; // ret nz
}

void PalletMovementScript_WalkToLab(void) {
    // xor a
    uint8_t a = 0;
    // ld [wOverrideSimulatedJoypadStatesMask], a
    wOverrideSimulatedJoypadStatesMask = a;
    // ld a, [wSpriteIndex]
    a = wSpriteIndex;
    // swap a
    a = (a << 4) | (a >> 4); // Simulate swap
    // ld [wNPCMovementScriptSpriteOffset], a
    wNPCMovementScriptSpriteOffset = a;
    // xor a
    a = 0;
    // ld [wSpritePlayerStateData2MovementByte1], a
    wSpritePlayerStateData2MovementByte1 = a;
    // ld hl, wSimulatedJoypadStatesEnd
    uint8_t* hl = &wSimulatedJoypadStatesEnd;
    // ld de, RLEList_PlayerWalkToLab
    uint8_t* de = RLEList_PlayerWalkToLab;
    // call DecodeRLEList
    DecodeRLEList(hl, de);
    // dec a
    a--;
    // ld [wSimulatedJoypadStatesIndex], a
    wSimulatedJoypadStatesIndex = a;
    // ld hl, wNPCMovementDirections2
    hl = wNPCMovementDirections2;
    // ld de, RLEList_ProfOakWalkToLab
    de = RLEList_ProfOakWalkToLab;
    // call DecodeRLEList
    DecodeRLEList(hl, de);
    // ld hl, wStatusFlags4
    hl = &wStatusFlags4;
    // res BIT_INIT_SCRIPTED_MOVEMENT, [hl]
    *hl &= ~(1 << BIT_INIT_SCRIPTED_MOVEMENT);
    // ld hl, wStatusFlags5
    hl = &wStatusFlags5;
    // set BIT_SCRIPTED_MOVEMENT_STATE, [hl]
    *hl |= (1 << BIT_SCRIPTED_MOVEMENT_STATE);
    // ld a, $4
    a = 0x4;
    // ld [wNPCMovementScriptFunctionNum], a
    wNPCMovementScriptFunctionNum = a;
    // ret
    return;
}

const uint8_t RLEList_ProfOakWalkToLab[] = {
    NPC_MOVEMENT_DOWN, 5,
    NPC_MOVEMENT_LEFT, 1,
    NPC_MOVEMENT_DOWN, 5,
    NPC_MOVEMENT_RIGHT, 3,
    NPC_MOVEMENT_UP, 1,
    NPC_CHANGE_FACING, 1,
    -1 // end
};

const uint8_t RLEList_PlayerWalkToLab[] = {
    D_UP, 2,
    D_RIGHT, 3,
    D_DOWN, 5,
    D_LEFT, 1,
    D_DOWN, 6,
    -1 // end
};

void PalletMovementScript_Done(void) {
    // ld a, [wSimulatedJoypadStatesIndex]
    uint8_t a = wSimulatedJoypadStatesIndex;
    // and a
    if (a != 0) return; // ret nz
    // ld a, HS_PALLET_TOWN_OAK
    a = HS_PALLET_TOWN_OAK;
    // ld [wMissableObjectIndex], a
    wMissableObjectIndex = a;
    // predef HideObject
    HideObject();
    // ld hl, wStatusFlags5
    uint8_t* hl = &wStatusFlags5;
    // res BIT_SCRIPTED_MOVEMENT_STATE, [hl]
    *hl &= ~(1 << BIT_SCRIPTED_MOVEMENT_STATE);
    // ld hl, wStatusFlags4
    hl = &wStatusFlags4;
    // res BIT_INIT_SCRIPTED_MOVEMENT, [hl]
    *hl &= ~(1 << BIT_INIT_SCRIPTED_MOVEMENT);
    // jp EndNPCMovementScript
    EndNPCMovementScript();
}

const uint16_t PewterMuseumGuyMovementScriptPointerTable[] = {
    (uint16_t)PewterMovementScript_WalkToMuseum,
    (uint16_t)PewterMovementScript_Done,
};

void PewterMovementScript_WalkToMuseum(void) {
    // ld a, BANK(Music_MuseumGuy)
    uint8_t a = BANK(Music_MuseumGuy);
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
    a = (a << 4) | (a >> 4); // Simulate swap
    // ld [wNPCMovementScriptSpriteOffset], a
    wNPCMovementScriptSpriteOffset = a;
    // call StartSimulatingJoypadStates
    StartSimulatingJoypadStates();
    // ld hl, wSimulatedJoypadStatesEnd
    uint8_t* hl = &wSimulatedJoypadStatesEnd;
    // ld de, RLEList_PewterMuseumPlayer
    uint8_t* de = RLEList_PewterMuseumPlayer;
    // call DecodeRLEList
    DecodeRLEList(hl, de);
    // dec a
    a--;
    // ld [wSimulatedJoypadStatesIndex], a
    wSimulatedJoypadStatesIndex = a;
    // xor a
    a = 0;
    // ld [wWhichPewterGuy], a
    wWhichPewterGuy = a;
    // predef PewterGuys
    PewterGuys();
    // ld hl, wNPCMovementDirections2
    hl = wNPCMovementDirections2;
    // ld de, RLEList_PewterMuseumGuy
    de = RLEList_PewterMuseumGuy;
    // call DecodeRLEList
    DecodeRLEList(hl, de);
    // ld hl, wStatusFlags4
    hl = &wStatusFlags4;
    // res BIT_INIT_SCRIPTED_MOVEMENT, [hl]
    *hl &= ~(1 << BIT_INIT_SCRIPTED_MOVEMENT);
    // ld a, $1
    a = 0x1;
    // ld [wNPCMovementScriptFunctionNum], a
    wNPCMovementScriptFunctionNum = a;
    // ret
    return;
}

const uint8_t RLEList_PewterMuseumPlayer[] = {
    NO_INPUT, 1,
    D_UP, 3,
    D_LEFT, 13,
    D_UP, 6,
    -1 // end
};

const uint8_t RLEList_PewterMuseumGuy[] = {
    NPC_MOVEMENT_UP, 6,
    NPC_MOVEMENT_LEFT, 13,
    NPC_MOVEMENT_UP, 3,
    NPC_MOVEMENT_LEFT, 1,
    -1 // end
};

void PewterMovementScript_Done(void) {
    // ld a, [wSimulatedJoypadStatesIndex]
    uint8_t a = wSimulatedJoypadStatesIndex;
    // and a
    if (a != 0) return; // ret nz
    // ld hl, wStatusFlags5
    uint8_t* hl = &wStatusFlags5;
    // res BIT_SCRIPTED_MOVEMENT_STATE, [hl]
    *hl &= ~(1 << BIT_SCRIPTED_MOVEMENT_STATE);
    // ld hl, wStatusFlags4
    hl = &wStatusFlags4;
    // res BIT_INIT_SCRIPTED_MOVEMENT, [hl]
    *hl &= ~(1 << BIT_INIT_SCRIPTED_MOVEMENT);
    // jp EndNPCMovementScript
    EndNPCMovementScript();
}

const uint16_t PewterGymGuyMovementScriptPointerTable[] = {
    (uint16_t)PewterMovementScript_WalkToGym,
    (uint16_t)PewterMovementScript_Done,
};

void PewterMovementScript_WalkToGym(void) {
    // ld a, BANK(Music_MuseumGuy)
    uint8_t a = BANK(Music_MuseumGuy);
    // ld [wAudioROMBank], a
    wAudioROMBank = a;
    // ld [