#include <stdint.h>

// Forward declarations for external functions and variables
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
extern uint8_t wXCoord;
extern uint8_t wNumStepsToTake;
extern uint8_t wNPCMovementDirections2[];
extern uint8_t wSpriteIndex;
extern uint8_t wStatusFlags7;
extern uint8_t wOverrideSimulatedJoypadStatesMask;
extern uint8_t wSpritePlayerStateData2MovementByte1;
extern uint8_t wSimulatedJoypadStatesEnd;
extern uint8_t wMissableObjectIndex;
extern uint8_t wAudioROMBank;
extern uint8_t wAudioSavedROMBank;
extern uint8_t wNewSoundID;
extern uint8_t wWhichPewterGuy;
extern uint8_t wCurMap;
extern uint8_t wEngagedTrainerClass;

// Constants
#define BIT_EXITING_DOOR 6
#define BIT_STANDING_ON_DOOR 7
#define BIT_SCRIPTED_MOVEMENT_STATE 5
#define BIT_INIT_SCRIPTED_MOVEMENT 4
#define BIT_SCRIPTED_NPC_MOVEMENT 6
#define BIT_NO_MAP_MUSIC 7

#define SELECT 0x20
#define START 0x10
#define D_RIGHT 0x01
#define D_LEFT 0x02
#define D_UP 0x04
#define D_DOWN 0x08
#define NO_INPUT 0x00

#define NPC_MOVEMENT_LEFT 1
#define NPC_MOVEMENT_DOWN 2
#define NPC_MOVEMENT_UP 3
#define NPC_MOVEMENT_RIGHT 4
#define NPC_CHANGE_FACING 5

#define HS_PALLET_TOWN_OAK 0x1A
#define BANK(x) 0x08
#define MUSIC_MUSEUM_GUY 0x22
#define POKEMON_TOWER_7F 0x8E

#define OPP_RIVAL1 0xF4
#define OPP_RIVAL2 0xF5
#define OPP_RIVAL3 0xF6

// External function prototypes
extern uint8_t IsPlayerStandingOnDoorTile(void);
extern void StartSimulatingJoypadStates(void);
extern void FillMemory(uint8_t* dest, uint8_t value, uint16_t count);
extern void MoveSprite(uint8_t spriteIndex, uint8_t* directions);
extern void ConvertNPCMovementDirectionsToJoypadMasks(void);
extern uint8_t DecodeRLEList(uint8_t* dest, const uint8_t* src);
extern void HideObject(void);
extern void EndNPCMovementScript(void);
extern void PlaySound(void);
extern void PewterGuys(void);
extern void SetSpriteMovementBytesToFF(void);

// External register variables
extern uint8_t hSpriteIndex;
extern uint8_t hNPCMovementDirections2Index;

void PlayerStepOutFromDoor(void) {
    uint8_t* hl;
    uint8_t a;
    uint8_t carry_flag;
    
    // ld hl, wStatusFlags5
    hl = &wStatusFlags5;
    // res BIT_EXITING_DOOR, [hl]
    *hl &= ~(1 << BIT_EXITING_DOOR);
    // call IsPlayerStandingOnDoorTile
    carry_flag = IsPlayerStandingOnDoorTile();
    // jr nc, .notStandingOnDoor
    if (!carry_flag) goto notStandingOnDoor;
    
    // ld a, SELECT | START | D_RIGHT | D_LEFT | D_UP | D_DOWN
    a = SELECT | START | D_RIGHT | D_LEFT | D_UP | D_DOWN;
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
    uint8_t* hl;
    uint8_t a;
    
    // ld hl, wStatusFlags5
    hl = &wStatusFlags5;
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
}

void PalletMovementScript_OakMoveLeft(void) {
    uint8_t* hl;
    uint8_t a;
    uint8_t b, c;
    uint8_t* de;
    
    // ld a, [wXCoord]
    a = wXCoord;
    // sub $a
    a -= 0xa;
    // ld [wNumStepsToTake], a
    wNumStepsToTake = a;
    // jr z, .playerOnLeftTile
    if (a == 0) goto playerOnLeftTile;
    
    // ld b, 0
    b = 0;
    // ld c, a
    c = a;
    // ld hl, wNPCMovementDirections2
    hl = wNPCMovementDirections2;
    // ld a, NPC_MOVEMENT_LEFT
    a = NPC_MOVEMENT_LEFT;
    // call FillMemory
    FillMemory(hl, a, c);
    // ld [hl], $ff
    *hl = 0xff;
    // ld a, [wSpriteIndex]
    a = wSpriteIndex;
    // ldh [hSpriteIndex], a
    hSpriteIndex = a;
    // ld de, wNPCMovementDirections2
    de = wNPCMovementDirections2;
    // call MoveSprite
    MoveSprite(hSpriteIndex, de);
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
    uint8_t a;
    
    // ld a, [wStatusFlags5]
    a = wStatusFlags5;
    // bit BIT_SCRIPTED_NPC_MOVEMENT, a
    // ret nz
    if (a & (1 << BIT_SCRIPTED_NPC_MOVEMENT)) return;
    
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
    uint8_t a;
    
    // ld a, [wSimulatedJoypadStatesIndex]
    a = wSimulatedJoypadStatesIndex;
    // and a
    // ret nz
    if (a != 0) return;
    
    // Continue to PalletMovementScript_WalkToLab
    // (fall through)
}

void PalletMovementScript_WalkToLab(void) {
    uint8_t* hl;
    uint8_t* de;
    uint8_t a;
    
    // xor a
    a = 0;
    // ld [wOverrideSimulatedJoypadStatesMask], a
    wOverrideSimulatedJoypadStatesMask = a;
    // ld a, [wSpriteIndex]
    a = wSpriteIndex;
    // swap a
    a = (a << 4) | (a >> 4);
    // ld [wNPCMovementScriptSpriteOffset], a
    wNPCMovementScriptSpriteOffset = a;
    // xor a
    a = 0;
    // ld [wSpritePlayerStateData2MovementByte1], a
    wSpritePlayerStateData2MovementByte1 = a;
    // ld hl, wSimulatedJoypadStatesEnd
    hl = &wSimulatedJoypadStatesEnd;
    // ld de, RLEList_PlayerWalkToLab
    de = (uint8_t*)RLEList_PlayerWalkToLab;
    // call DecodeRLEList
    a = DecodeRLEList(hl, de);
    // dec a
    a--;
    // ld [wSimulatedJoypadStatesIndex], a
    wSimulatedJoypadStatesIndex = a;
    // ld hl, wNPCMovementDirections2
    hl = wNPCMovementDirections2;
    // ld de, RLEList_ProfOakWalkToLab
    de = (uint8_t*)RLEList_ProfOakWalkToLab;
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

// RLE Lists as const arrays
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
    uint8_t* hl;
    uint8_t a;
    
    // ld a, [wSimulatedJoypadStatesIndex]
    a = wSimulatedJoypadStatesIndex;
    // and a
    // ret nz
    if (a != 0) return;
    
    // ld a, HS_PALLET_TOWN_OAK
    a = HS_PALLET_TOWN_OAK;
    // ld [wMissableObjectIndex], a
    wMissableObjectIndex = a;
    // predef HideObject
    HideObject();
    // ld hl, wStatusFlags5
    hl = &wStatusFlags5;
    // res BIT_SCRIPTED_MOVEMENT_STATE, [hl]
    *hl &= ~(1 << BIT_SCRIPTED_MOVEMENT_STATE);
    // ld hl, wStatusFlags4
    hl = &wStatusFlags4;
    // res BIT_INIT_SCRIPTED_MOVEMENT, [hl]
    *hl &= ~(1 << BIT_INIT_SCRIPTED_MOVEMENT);
    // jp EndNPCMovementScript
    EndNPCMovementScript(); /* jp */
}

void PewterMovementScript_WalkToMuseum(void) {
    uint8_t* hl;
    uint8_t* de;
    uint8_t a;
    
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
    a = (a << 4) | (a >> 4);
    // ld [wNPCMovementScriptSpriteOffset], a
    wNPCMovementScriptSpriteOffset = a;
    // call StartSimulatingJoypadStates
    StartSimulatingJoypadStates();
    // ld hl, wSimulatedJoypadStatesEnd
    hl = &wSimulatedJoypadStatesEnd;
    // ld de, RLEList_PewterMuseumPlayer
    de = (uint8_t*)RLEList_PewterMuseumPlayer;
    // call DecodeRLEList
    a = DecodeRLEList(hl, de);
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
    de = (uint8_t*)RLEList_PewterMuseumGuy;
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
    uint8_t* hl;
    uint8_t a;
    
    // ld a, [wSimulatedJoypadStatesIndex]
    a = wSimulatedJoypadStatesIndex;
    // and a
    // ret nz
    if (a != 0) return;
    
    // ld hl, wStatusFlags5
    hl = &wStatusFlags5;
    // res BIT_SCRIPTED_MOVEMENT_STATE, [hl]
    *hl &= ~(1 << BIT_SCRIPTED_MOVEMENT_STATE);
    // ld hl, wStatusFlags4
    hl = &wStatusFlags4;
    // res BIT_INIT_SCRIPTED_MOVEMENT, [hl]
    *hl &= ~(1 << BIT_INIT_SCRIPTED_MOVEMENT);
    // jp EndNPCMovementScript
    EndNPCMovementScript(); /* jp */
}

void PewterMovementScript_WalkToGym(void) {
    uint8_t* hl;
    uint8_t* de;
    uint8_t a;
    
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
    a = (a << 4) | (a >> 4);
    // ld [wNPCMovementScriptSpriteOffset], a
    wNPCMovementScriptSpriteOffset = a;
    // xor a
    a = 0;
    // ld [wSpritePlayerStateData2MovementByte1], a
    wSpritePlayerStateData2MovementByte1 = a;
    // ld hl, wSimulatedJoypadStatesEnd
    hl = &wSimulatedJoypadStatesEnd;
    // ld de, RLEList_PewterGymPlayer
    de = (uint8_t*)RLEList_PewterGymPlayer;
    // call DecodeRLEList
    a = DecodeRLEList(hl, de);
    // dec a
    a--;
    // ld [wSimulatedJoypadStatesIndex], a
    wSimulatedJoypadStatesIndex = a;
    // ld a, 1
    a = 1;
    // ld [wWhichPewterGuy], a
    wWhichPewterGuy = a;
    // predef PewterGuys
    PewterGuys();
    // ld hl, wNPCMovementDirections2
    hl = wNPCMovementDirections2;
    // ld de, RLEList_PewterGymGuy
    de = (uint8_t*)RLEList_PewterGymGuy;
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
    // ld a, $1
    a = 0x1;
    // ld [wNPCMovementScriptFunctionNum], a
    wNPCMovementScriptFunctionNum = a;
    // ret
    return;
}

const uint8_t RLEList_PewterGymPlayer[] = {
    NO_INPUT, 1,
    D_RIGHT, 2,
    D_DOWN, 5,
    D_LEFT, 11,
    D_UP, 5,
    D_LEFT, 15,
    -1 // end
};

const uint8_t RLEList_PewterGymGuy[] = {
    NPC_MOVEMENT_DOWN, 2,
    NPC_MOVEMENT_LEFT, 15,
    NPC_MOVEMENT_UP, 5,
    NPC_MOVEMENT_LEFT, 11,
    NPC_MOVEMENT_DOWN, 5,
    NPC_MOVEMENT_RIGHT, 3,
    -1 // end
};

void SetEnemyTrainerToStayAndFaceAnyDirection(void) {
    uint8_t* hl;
    uint8_t a, b;
    
    // ld a, [wCurMap]
    a = wCurMap;
    // cp POKEMON_TOWER_7F
    // ret z
    if (a == POKEMON_TOWER_7F) return;
    
    // ld hl, RivalIDs
    hl = (uint8_t*)RivalIDs;
    // ld a, [wEngagedTrainerClass]
    a = wEngagedTrainerClass;
    // ld b, a
    b = a;

loop:
    // ld a, [hli]
    a = *hl++;
    // cp -1
    // jr z, .notRival
    if (a == -1) goto notRival;
    // cp b
    // ret z
    if (a == b) return;
    // jr .loop
    goto loop;

notRival:
    // ld a, [wSpriteIndex]
    a = wSpriteIndex;
    // ldh [hSpriteIndex], a
    hSpriteIndex = a;
    // jp SetSpriteMovementBytesToFF
    SetSpriteMovementBytesToFF(); /* jp */
}

const uint8_t RivalIDs[] = {
    OPP_RIVAL1,
    OPP_RIVAL2,
    OPP_RIVAL3,
    -1 // end
}; 