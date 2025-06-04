void PrintStrengthText(void) {
PrintStrengthText:
// ld hl, wStatusFlags1
    hl = wStatusFlags1;
// set BIT_STRENGTH_ACTIVE, [hl]
// ld hl, UsedStrengthText
    hl = UsedStrengthText;
// call PrintText
    PrintText();
// ld hl, CanMoveBouldersText
    hl = CanMoveBouldersText;
// jp PrintText
    PrintText(); /* jp */
//
UsedStrengthText:
// text_far _UsedStrengthText
// text_asm
// ld a, [wCurPartySpecies]
    a = wCurPartySpecies;
// call PlayCry
    PlayCry();
// call Delay3
    Delay3();
// jp TextScriptEnd
    TextScriptEnd(); /* jp */
//
CanMoveBouldersText:
// text_far _CanMoveBouldersText
// text_end
//
IsSurfingAllowed:
// Returns whether surfing is allowed in BIT_SURF_ALLOWED of wStatusFlags1.
// Surfing isn't allowed on the Cycling Road or in the lowest level of the
// Seafoam Islands before the current has been slowed with boulders.
// ld hl, wStatusFlags1
    hl = wStatusFlags1;
// set BIT_SURF_ALLOWED, [hl]
// ld a, [wStatusFlags6]
    a = wStatusFlags6;
// bit BIT_ALWAYS_ON_BIKE, a
// jr nz, .forcedToRideBike
// ld a, [wCurMap]
    a = wCurMap;
// cp SEAFOAM_ISLANDS_B4F
// ret nz
    return;
// CheckBothEventsSet EVENT_SEAFOAM4_BOULDER1_DOWN_HOLE, EVENT_SEAFOAM4_BOULDER2_DOWN_HOLE
// ret z
    return;
// ld hl, SeafoamIslandsB4FStairsCoords
    hl = SeafoamIslandsB4FStairsCoords;
// call ArePlayerCoordsInArray
    ArePlayerCoordsInArray();
// ret nc
    return;
// ld hl, wStatusFlags1
    hl = wStatusFlags1;
// res BIT_SURF_ALLOWED, [hl]
// ld hl, CurrentTooFastText
    hl = CurrentTooFastText;
// jp PrintText
    PrintText(); /* jp */
// .forcedToRideBike
// ld hl, wStatusFlags1
    hl = wStatusFlags1;
// res BIT_SURF_ALLOWED, [hl]
// ld hl, CyclingIsFunText
    hl = CyclingIsFunText;
// jp PrintText
    PrintText(); /* jp */
//
SeafoamIslandsB4FStairsCoords:
// dbmapcoord  7, 11
// db -1 ; end
//
CurrentTooFastText:
// text_far _CurrentTooFastText
// text_end
//
CyclingIsFunText:
// text_far _CyclingIsFunText
// text_end
}
