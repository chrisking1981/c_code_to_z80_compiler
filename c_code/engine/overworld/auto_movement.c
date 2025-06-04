#include <stdint.h>

// External variables
extern uint8_t wStatusFlags5;
extern uint8_t wMovementFlags;
extern uint8_t wStatusFlags4;
extern uint8_t wStatusFlags7;
extern uint8_t wJoyIgnore;
extern uint8_t wSimulatedJoypadStatesIndex;
extern uint8_t wSimulatedJoypadStatesEnd;
extern uint8_t wSpritePlayerStateData1ImageIndex;
extern uint8_t wSpritePlayerStateData2MovementByte1;
extern uint8_t wUnusedOverrideSimulatedJoypadStatesIndex;
extern uint8_t wOverrideSimulatedJoypadStatesMask;
extern uint8_t wNPCMovementScriptSpriteOffset;
extern uint8_t wNPCMovementScriptPointerTableNum;
extern uint8_t wNPCMovementScriptFunctionNum;
extern uint8_t wNPCMovementDirections2[];
extern uint8_t wNPCMovementDirections2Index;
extern uint8_t wWhichPewterGuy;
extern uint8_t wXCoord;
extern uint8_t wNumStepsToTake;
extern uint8_t wSpriteIndex;
extern uint8_t wCurMap;
extern uint8_t wMissableObjectIndex;
extern uint8_t wEngagedTrainerClass;
extern uint8_t hSpriteIndex;
extern uint8_t wAudioROMBank;
extern uint8_t wAudioSavedROMBank;
extern uint8_t wNewSoundID;

// External functions
void IsPlayerStandingOnDoorTile(void);
void StartSimulatingJoypadStates(void);
void FillMemory(void);
void MoveSprite(void);
void DecodeRLEList(uint8_t* hl, const uint8_t* de);
void ConvertNPCMovementDirectionsToJoypadMasks(void) /* predef */;
void PewterGuys(void) /* predef */;
void HideObject(void) /* predef */;
void PlaySound(void);
void EndNPCMovementScript(void) /* jp */;
void SetSpriteMovementBytesToFF(void) /* jp */;

// Constants
#define BIT_EXITING_DOOR 0
#define BIT_STANDING_ON_DOOR 1
#define BIT_SCRIPTED_MOVEMENT_STATE 2
#define BIT_INIT_SCRIPTED_MOVEMENT 3
#define BIT_SCRIPTED_NPC_MOVEMENT 4
#define BIT_NO_MAP_MUSIC 5

#define SELECT 0x80
#define START 0x08
#define D_RIGHT 0x01
#define D_LEFT 0x02
#define D_UP 0x04
#define D_DOWN 0x08
#define NPC_MOVEMENT_LEFT 0x10
#define NPC_MOVEMENT_RIGHT 0x20
#define NPC_MOVEMENT_UP 0x40
#define NPC_MOVEMENT_DOWN 0x80
#define NPC_CHANGE_FACING 0x01
#define NO_INPUT 0x00
#define HS_PALLET_TOWN_OAK 0x01
#define MUSIC_MUSEUM_GUY 0x02
#define POKEMON_TOWER_7F 0x01
#define OPP_RIVAL1 0x01
#define OPP_RIVAL2 0x02
#define OPP_RIVAL3 0x03
#define BANK(x) 0

// Function declarations
void PlayerStepOutFromDoor(void);
void _EndNPCMovementScript(void);
void PalletMovementScript_OakMoveLeft(void);
void PalletMovementScript_PlayerMoveLeft(void);
void PalletMovementScript_WaitAndWalkToLab(void);
void PalletMovementScript_WalkToLab(void);
void PalletMovementScript_Done(void);
void PewterMovementScript_WalkToMuseum(void);
void PewterMovementScript_Done(void);
void PewterMovementScript_WalkToGym(void);
void SetEnemyTrainerToStayAndFaceAnyDirection(void);

// Pointer tables
void (*PalletMovementScriptPointerTable[])(void) = {
    PalletMovementScript_OakMoveLeft,
    PalletMovementScript_PlayerMoveLeft,
    PalletMovementScript_WaitAndWalkToLab,
    PalletMovementScript_WalkToLab,
    PalletMovementScript_Done
};

void (*PewterMuseumGuyMovementScriptPointerTable[])(void) = {
    PewterMovementScript_WalkToMuseum,
    PewterMovementScript_Done
};

void (*PewterGymGuyMovementScriptPointerTable[])(void) = {
    PewterMovementScript_WalkToGym,
    PewterMovementScript_Done
};

// Data arrays
const uint8_t RLEList_ProfOakWalkToLab[] = {
    NPC_MOVEMENT_DOWN, 5,
    NPC_MOVEMENT_LEFT, 1,
    NPC_MOVEMENT_DOWN, 5,
    NPC_MOVEMENT_RIGHT, 3,
    NPC_MOVEMENT_UP, 1,
    NPC_CHANGE_FACING, 1,
    -1
};

const uint8_t RLEList_PlayerWalkToLab[] = {
    D_UP, 2,
    D_RIGHT, 3,
    D_DOWN, 5,
    D_LEFT, 1,
    D_DOWN, 6,
    -1
};

const uint8_t RLEList_PewterMuseumPlayer[] = {
    NO_INPUT, 1,
    D_UP, 3,
    D_LEFT, 13,
    D_UP, 6,
    -1
};

const uint8_t RLEList_PewterMuseumGuy[] = {
    NPC_MOVEMENT_UP, 6,
    NPC_MOVEMENT_LEFT, 13,
    NPC_MOVEMENT_UP, 3,
    NPC_MOVEMENT_LEFT, 1,
    -1
};

const uint8_t RLEList_PewterGymPlayer[] = {
    NO_INPUT, 1,
    D_RIGHT, 2,
    D_DOWN, 5,
    D_LEFT, 11,
    D_UP, 5,
    D_LEFT, 15,
    -1
};

const uint8_t RLEList_PewterGymGuy[] = {
    NPC_MOVEMENT_DOWN, 2,
    NPC_MOVEMENT_LEFT, 15,
    NPC_MOVEMENT_UP, 5,
    NPC_MOVEMENT_LEFT, 11,
    NPC_MOVEMENT_DOWN, 5,
    NPC_MOVEMENT_RIGHT, 3,
    -1
};

const uint8_t RivalIDs[] = {
    OPP_RIVAL1,
    OPP_RIVAL2,
    OPP_RIVAL3,
    -1
};

// Functions
void PlayerStepOutFromDoor(void) {
    // ld hl, wStatusFlags5
    uint8_t* hl = &wStatusFlags5;
    // res BIT_EXITING_DOOR, [hl]
    *hl &= ~(1 << BIT_EXITING_DOOR);
    // call IsPlayerStandingOnDoorTile
    IsPlayerStandingOnDoorTile();
    // jr nc, .notStandingOnDoor
    if (!(/* carry */0)) goto notStandingOnDoor;
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

void PalletMovementScript_OakMoveLeft(void) {
    // ld a, [wXCoord]
    uint8_t a = wXCoord;
    // sub $a
    a -= 0x0a;
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
    *hl = 0xff;
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
    if (a & (1 << BIT_SCRIPTED_NPC_MOVEMENT)) {
        // ret nz
        return;
    }
    // ld a, [wNumStepsToTake]
    a = wNumStepsToTake;
    // ld [wSimulatedJoypadStatesIndex], a
    wSimulatedJoypadStatesIndex = a;
    // ldh [hNPCMovementDirections2Index], a
    wNPCMovementDirections2Index = a;
    // predef ConvertNPCMovementDirectionsToJoypadMasks
    ConvertNPCMovementDirectionsToJoypadMasks(); /* predef */
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
    // and a
    if (a != 0) return;

    // PalletMovementScript_WalkToLab:
    a = 0;
    wOverrideSimulatedJoypadStatesMask = a;
    a = wSpriteIndex;
    a = (a << 4) | (a >> 4); // swap a
    wNPCMovementScriptSpriteOffset = a;
    a = 0;
    wSpritePlayerStateData2MovementByte1 = a;
    uint8_t* hl = &wSimulatedJoypadStatesEnd;
    const uint8_t* de = RLEList_PlayerWalkToLab;
    DecodeRLEList(hl, de);
    a--;
    wSimulatedJoypadStatesIndex = a;
    hl = wNPCMovementDirections2;
    de = RLEList_ProfOakWalkToLab;
    DecodeRLEList(hl, de);
    hl = &wStatusFlags4;
    *hl &= ~(1 << BIT_INIT_SCRIPTED_MOVEMENT);
    hl = &wStatusFlags5;
    *hl |= (1 << BIT_SCRIPTED_MOVEMENT_STATE);
    a = 0x4;
    wNPCMovementScriptFunctionNum = a;
    return;
}

void PalletMovementScript_Done(void) {
    uint8_t a = wSimulatedJoypadStatesIndex;
    if (a != 0) return;
    a = HS_PALLET_TOWN_OAK;
    wMissableObjectIndex = a;
    HideObject(); /* predef */
    uint8_t* hl = &wStatusFlags5;
    *hl &= ~(1 << BIT_SCRIPTED_MOVEMENT_STATE);
    hl = &wStatusFlags4;
    *hl &= ~(1 << BIT_INIT_SCRIPTED_MOVEMENT);
    EndNPCMovementScript(); /* jp */
}

void PewterMovementScript_WalkToMuseum(void) {
    uint8_t a = BANK(Music_MuseumGuy);
    wAudioROMBank = a;
    wAudioSavedROMBank = a;
    a = MUSIC_MUSEUM_GUY;
    wNewSoundID = a;
    PlaySound();
    a = wSpriteIndex;
    a = (a << 4) | (a >> 4); // swap a
    wNPCMovementScriptSpriteOffset = a;
    StartSimulatingJoypadStates();
    uint8_t* hl = &wSimulatedJoypadStatesEnd;
    const uint8_t* de = RLEList_PewterMuseumPlayer;
    DecodeRLEList(hl, de);
    a--;
    wSimulatedJoypadStatesIndex = a;
    a = 0;
    wWhichPewterGuy = a;
    PewterGuys(); /* predef */
    hl = wNPCMovementDirections2;
    de = RLEList_PewterMuseumGuy;
    DecodeRLEList(hl, de);
    hl = &wStatusFlags4;
    *hl &= ~(1 << BIT_INIT_SCRIPTED_MOVEMENT);
    a = 0x1;
    wNPCMovementScriptFunctionNum = a;
    return;
}

void PewterMovementScript_Done(void) {
    uint8_t a = wSimulatedJoypadStatesIndex;
    if (a != 0) return;
    uint8_t* hl = &wStatusFlags5;
    *hl &= ~(1 << BIT_SCRIPTED_MOVEMENT_STATE);
    hl = &wStatusFlags4;
    *hl &= ~(1 << BIT_INIT_SCRIPTED_MOVEMENT);
    EndNPCMovementScript(); /* jp */
}

void PewterMovementScript_WalkToGym(void) {
    uint8_t a = BANK(Music_MuseumGuy);
    wAudioROMBank = a;
    wAudioSavedROMBank = a;
    a = MUSIC_MUSEUM_GUY;
    wNewSoundID = a;
    PlaySound();
    a = wSpriteIndex;
    a = (a << 4) | (a >> 4); // swap a
    wNPCMovementScriptSpriteOffset = a;
    a = 0;
    wSpritePlayerStateData2MovementByte1 = a;
    uint8_t* hl = &wSimulatedJoypadStatesEnd;
    const uint8_t* de = RLEList_PewterGymPlayer;
    DecodeRLEList(hl, de);
    a--;
    wSimulatedJoypadStatesIndex = a;
    a = 1;
    wWhichPewterGuy = a;
    PewterGuys(); /* predef */
    hl = wNPCMovementDirections2;
    de = RLEList_PewterGymGuy;
    DecodeRLEList(hl, de);
    hl = &wStatusFlags4;
    *hl &= ~(1 << BIT_INIT_SCRIPTED_MOVEMENT);
    hl = &wStatusFlags5;
    *hl |= (1 << BIT_SCRIPTED_MOVEMENT_STATE);
    a = 0x1;
    wNPCMovementScriptFunctionNum = a;
    return;
}

void SetEnemyTrainerToStayAndFaceAnyDirection(void) {
    uint8_t a = wCurMap;
    if (a == POKEMON_TOWER_7F) return;
    uint8_t* hl = (uint8_t*)RivalIDs;
    a = wEngagedTrainerClass;
    uint8_t b = a;
loop:
    a = *hl++;
    if (a == (uint8_t)-1) goto notRival;
    if (a == b) return;
    goto loop;
notRival:
    a = wSpriteIndex;
    hSpriteIndex = a;
    SetSpriteMovementBytesToFF(); /* jp */
}

