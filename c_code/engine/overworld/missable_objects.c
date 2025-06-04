void MarkTownVisitedAndLoadMissableObjects(void) {
MarkTownVisitedAndLoadMissableObjects:
// ld a, [wCurMap]
    a = wCurMap;
// cp FIRST_ROUTE_MAP
// jr nc, .notInTown
// ld c, a
// ld b, FLAG_SET
// ld hl, wTownVisitedFlag   ; mark town as visited (for flying)
    hl = wTownVisitedFlag   ; mark town as visited (for flying);
// predef FlagActionPredef
// .notInTown
// ld hl, MapHSPointers
    hl = MapHSPointers;
// ld a, [wCurMap]
    a = wCurMap;
// ld b, $0
// ld c, a
// add hl, bc
// add hl, bc
// ld a, [hli]                ; load missable objects pointer in hl
    a = hli;
// ld h, [hl]
// fall through
//
LoadMissableObjects:
// ld l, a
// push hl
// ld de, MissableObjects     ; calculate difference between out pointer and the base pointer
    de = MissableObjects     ; calculate difference between out pointer and the base pointer;
// ld a, l
    a = l;
// sub e
// jr nc, .noCarry
// dec h
// .noCarry
// ld l, a
// ld a, h
    a = h;
// sub d
// ld h, a
// ld a, h
    a = h;
// ldh [hDividend], a
    hDividend = a;
// ld a, l
    a = l;
// ldh [hDividend+1], a
    hDividend+1 = a;
// xor a
    a = 0;
// ldh [hDividend+2], a
    hDividend+2 = a;
// ldh [hDividend+3], a
    hDividend+3 = a;
// ld a, $3
    a = $3;
// ldh [hDivisor], a
    hDivisor = a;
// ld b, $2
// call Divide                ; divide difference by 3, resulting in the global offset (number of missable items before ours)
    Divide                ; divide difference by 3, resulting in the global offset (number of missable items before ours)();
// ld a, [wCurMap]
    a = wCurMap;
// ld b, a
// ldh a, [hDividend+3]
    a = hDividend+3;
// ld c, a                    ; store global offset in c
// ld de, wMissableObjectList
    de = wMissableObjectList;
// pop hl
// .writeMissableObjectsListLoop
// ld a, [hli]
    a = hli;
// cp -1
// jr z, .done     ; end of list
// cp b
// jr nz, .done    ; not for current map anymore
// ld a, [hli]
    a = hli;
// inc hl
// ld [de], a                 ; write (map-local) sprite ID
    de = a;
// inc de
// ld a, c
    a = c;
// inc c
// ld [de], a                 ; write (global) missable object index
    de = a;
// inc de
// jr .writeMissableObjectsListLoop
// .done
// ld a, -1
    a = -1;
// ld [de], a                 ; write sentinel
    de = a;
// ret
    return;
//
InitializeMissableObjectsFlags:
// ld hl, wMissableObjectFlags
    hl = wMissableObjectFlags;
// ld bc, wMissableObjectFlagsEnd - wMissableObjectFlags
    bc = wMissableObjectFlagsEnd - wMissableObjectFlags;
// xor a
    a = 0;
// call FillMemory ; clear missable objects flags
    FillMemory ; clear missable objects flags();
// ld hl, MissableObjects
    hl = MissableObjects;
// xor a
    a = 0;
// ld [wMissableObjectCounter], a
    wMissableObjectCounter = a;
// .missableObjectsLoop
// ld a, [hli]
    a = hli;
// cp -1           ; end of list
// ret z
    return;
// push hl
// inc hl
// ld a, [hl]
    a = hl;
// cp HIDE
// jr nz, .skip
// ld hl, wMissableObjectFlags
    hl = wMissableObjectFlags;
// ld a, [wMissableObjectCounter]
    a = wMissableObjectCounter;
// ld c, a
// ld b, FLAG_SET
// call MissableObjectFlagAction ; set flag if Item is hidden
    MissableObjectFlagAction ; set flag if Item is hidden();
// .skip
// ld hl, wMissableObjectCounter
    hl = wMissableObjectCounter;
// inc [hl]
// pop hl
// inc hl
// inc hl
// jr .missableObjectsLoop
//
// tests if current sprite is a missable object that is hidden/has been removed
IsObjectHidden:
// ldh a, [hCurrentSpriteOffset]
    a = hCurrentSpriteOffset;
// swap a
// ld b, a
// ld hl, wMissableObjectList
    hl = wMissableObjectList;
// .loop
// ld a, [hli]
    a = hli;
// cp -1
// jr z, .notHidden ; not missable -> not hidden
// cp b
// ld a, [hli]
    a = hli;
// jr nz, .loop
// ld c, a
// ld b, FLAG_TEST
// ld hl, wMissableObjectFlags
    hl = wMissableObjectFlags;
// call MissableObjectFlagAction
    MissableObjectFlagAction();
// ld a, c
    a = c;
// and a
// jr nz, .hidden
// .notHidden
// xor a
    a = 0;
// .hidden
// ldh [hIsHiddenMissableObject], a
    hIsHiddenMissableObject = a;
// ret
    return;
//
// adds missable object (items, leg. pokemon, etc.) to the map
// [wMissableObjectIndex]: index of the missable object to be added (global index)
ShowObject:
ShowObject2:
// ld hl, wMissableObjectFlags
    hl = wMissableObjectFlags;
// ld a, [wMissableObjectIndex]
    a = wMissableObjectIndex;
// ld c, a
// ld b, FLAG_RESET
// call MissableObjectFlagAction   ; reset "removed" flag
    MissableObjectFlagAction   ; reset "removed" flag();
// jp UpdateSprites
    UpdateSprites(); /* jp */
//
// removes missable object (items, leg. pokemon, etc.) from the map
// [wMissableObjectIndex]: index of the missable object to be removed (global index)
HideObject:
// ld hl, wMissableObjectFlags
    hl = wMissableObjectFlags;
// ld a, [wMissableObjectIndex]
    a = wMissableObjectIndex;
// ld c, a
// ld b, FLAG_SET
// call MissableObjectFlagAction   ; set "removed" flag
    MissableObjectFlagAction   ; set "removed" flag();
// jp UpdateSprites
    UpdateSprites(); /* jp */
//
MissableObjectFlagAction:
// identical to FlagAction
//
// push hl
// push de
// push bc
//
// bit
// ld a, c
    a = c;
// ld d, a
// and 7
// ld e, a
//
// byte
// ld a, d
    a = d;
// srl a
// srl a
// srl a
// add l
// ld l, a
// jr nc, .ok
// inc h
// .ok
//
// d = 1 << e (bitmask)
// inc e
// ld d, 1
// .shift
// dec e
// jr z, .shifted
// sla d
// jr .shift
// .shifted
//
// ld a, b
    a = b;
// and a
// jr z, .reset
// cp 2
// jr z, .read
//
// .set
// ld a, [hl]
    a = hl;
// ld b, a
// ld a, d
    a = d;
// or b
// ld [hl], a
    *hl = a;
// jr .done
//
// .reset
// ld a, [hl]
    a = hl;
// ld b, a
// ld a, d
    a = d;
// xor $ff
// and b
// ld [hl], a
    *hl = a;
// jr .done
//
// .read
// ld a, [hl]
    a = hl;
// ld b, a
// ld a, d
    a = d;
// and b
//
// .done
// pop bc
// pop de
// pop hl
// ld c, a
// ret
    return;
}
