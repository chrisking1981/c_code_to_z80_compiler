#include <stdint.h>

// Extern declarations for variables/functions
extern uint8_t wStatusFlags5;
extern uint8_t wMovementFlags;
extern uint8_t wJoyIgnore;
extern uint8_t wSimulatedJoypadStatesIndex;
extern uint8_t wSimulatedJoypadStatesEnd;
extern uint8_t wSpritePlayerStateData1ImageIndex;
extern uint8_t wUnusedOverrideSimulatedJoypadStatesIndex;
extern uint8_t wStatusFlags4;
extern uint8_t wNPCMovementScriptSpriteOffset;
extern uint8_t wNPCMovementScriptPointerTableNum;
extern uint8_t wNPCMovementScriptFunctionNum;
extern uint8_t wWhichPewterGuy;
extern uint8_t wSpriteIndex;
extern uint8_t wAudioROMBank;
extern uint8_t wAudioSavedROMBank;
extern uint8_t wNewSoundID;
extern uint8_t wSimulatedJoypadStatesIndex;
extern uint8_t wStatusFlags7;
extern uint8_t hSpriteIndex;
extern uint8_t hNPCMovementDirections2Index;
extern uint8_t wSpritePlayerStateData2MovementByte1;
extern uint8_t wCurMap;
extern uint8_t wNumStepsToTake;
extern uint8_t rOBP1;

// Constants
#define BIT_EXITING_DOOR 0
#define BIT_STANDING_ON_DOOR 1
#define BIT_SCRIPTED_MOVEMENT_STATE 2
#define BIT_INIT_SCRIPTED_MOVEMENT 3
#define BIT_SCRIPTED_NPC_MOVEMENT 4
#define SELECT 0x01
#define START 0x02
#define D_RIGHT 0x04
#define D_LEFT 0x08
#define D_UP 0x10
#define D_DOWN 0x20
#define NPC_MOVEMENT_LEFT 0x01
#define NPC_MOVEMENT_DOWN 0x02
#define NPC_MOVEMENT_RIGHT 0x03
#define NPC_MOVEMENT_UP 0x04
#define NPC_CHANGE_FACING 0x05
#define NO_INPUT 0x00
#define HS_PALLET_TOWN_OAK 0x01
#define POKEMON_TOWER_7F 0x02

// Function prototypes
void IsPlayerStandingOnDoorTile(void);
void StartSimulatingJoypadStates(void);
void FillMemory(uint8_t value);
void MoveSprite(void);
void PlaySound(void);
void DecodeRLEList(void);
void HideObject(void);
void SetSpriteMovementBytesToFF(void);
void PewterGuys(void);
void ConvertNPCMovementDirectionsToJoypadMasks(void);

// Function definitions
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
    a = 0x01;
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

PalletMovementScriptPointerTable::
dw PalletMovementScript_OakMoveLeft
dw PalletMovementScript_PlayerMoveLeft
dw PalletMovementScript_WaitAndWalkToLab
dw PalletMovementScript_WalkToLab
dw PalletMovementScript_Done

void PalletMovementScript_OakMoveLeft(void) {
    // ld a, [wXCoord]
    uint8_t a = wXCoord;
    // sub $a
    a -= a; // Assuming $a is a constant value
    // ld [wNumStepsToTake], a
    wNumStepsToTake = a;
    // jr z, .playerOnLeftTile
    if (a == 0) goto playerOnLeftTile;
    // The player is on the right tile of the northern path out of Pallet Town and
    // Prof. Oak is below.
    // Make Prof. Oak step to the left.
    // ld b, 0
    uint8_t b = 0;
    // ld c, a
    uint8_t c = a;
    // ld hl, wNPCMovementDirections2
    uint8_t* hl = &wNPCMovementDirections2;
    // ld a, NPC_MOVEMENT_LEFT
    a = NPC_MOVEMENT_LEFT;
    // call FillMemory
    FillMemory(a);
    // ld [hl], $ff
    *hl = 0xFF;
    // ld a, [wSpriteIndex]
    a = wSpriteIndex;
    // ldh [hSpriteIndex], a
    hSpriteIndex = a;
    // ld de, wNPCMovementDirections2
    uint8_t* de = &wNPCMovementDirections2;
    // call MoveSprite
    MoveSprite();
    // ld a, $1
    a = 0x01;
    // ld [wNPCMovementScriptFunctionNum], a
    wNPCMovementScriptFunctionNum = a;
    // jr .done
    goto done;

playerOnLeftTile:
    // ld a, $3
    a = 0x03;
    // ld [wNPCMovementScriptFunctionNum], a
    wNPCMovementScriptFunctionNum = a;

done:
    // ld hl, wStatusFlags7
    hl = &wStatusFlags7;
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
    a = 0x02;
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
    // ret
    return;
}

void PalletMovementScript_WalkToLab(void) {
    // xor a
    uint8_t a = 0;
    // ld [wOverrideSimulatedJoypadStatesMask], a
    wOverrideSimulatedJoypadStatesMask = a;
    // ld a, [wSpriteIndex]
    a = wSpriteIndex;
    // swap a
    a = (a << 4) | (a >> 4); // Simulating swap
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
    DecodeRLEList();
    // dec a
    a--;
    // ld [wSimulatedJoypadStatesIndex], a
    wSimulatedJoypadStatesIndex = a;
    // ld hl, wNPCMovementDirections2
    hl = &wNPCMovementDirections2;
    // ld de, RLEList_ProfOakWalkToLab
    de = RLEList_ProfOakWalkToLab;
    // call DecodeRLEList
    DecodeRLEList();
    // ld hl, wStatusFlags4
    hl = &wStatusFlags4;
    // res BIT_INIT_SCRIPTED_MOVEMENT, [hl]
    *hl &= ~(1 << BIT_INIT_SCRIPTED_MOVEMENT);
    // ld hl, wStatusFlags5
    hl = &wStatusFlags5;
    // set BIT_SCRIPTED_MOVEMENT_STATE, [hl]
    *hl |= (1 << BIT_SCRIPTED_MOVEMENT_STATE);
    // ld a, $4
    a = 0x04;
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

PewterMuseumGuyMovementScriptPointerTable::
dw PewterMovementScript_WalkToMuseum
dw PewterMovementScript_Done

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
    a = (a << 4) | (a >> 4); // Simulating swap
    // ld [wNPCMovementScriptSpriteOffset], a
    wNPCMovementScriptSpriteOffset = a;
    // call StartSimulatingJoypadStates
    StartSimulatingJoypadStates();
    // ld hl, wSimulatedJoypadStatesEnd
    uint8_t* hl = &wSimulatedJoypadStatesEnd;
    // ld de, RLEList_PewterMuseumPlayer
    uint8_t* de = RLEList_PewterMuseumPlayer;
    // call DecodeRLEList
    DecodeRLEList();
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
    hl = &wNPCMovementDirections2;
    // ld de, RLEList_PewterMuseumGuy
    de = RLEList_PewterMuseumGuy;
    // call DecodeRLEList
    DecodeRLEList();
    // ld hl, wStatusFlags4
    hl = &wStatusFlags4;
    // res BIT_INIT_SCRIPTED_MOVEMENT, [hl]
    *hl &= ~(1 << BIT_INIT_SCRIPTED_MOVEMENT);
    // ld a, $1
    a = 0x01;
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

PewterGymGuyMovementScriptPointerTable::
dw PewterMovementScript_WalkToGym
dw PewterMovementScript_Done

void PewterMovementScript_WalkToGym(void) {