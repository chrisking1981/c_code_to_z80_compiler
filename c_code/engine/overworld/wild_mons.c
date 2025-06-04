void LoadWildData(void) {
LoadWildData:
// ld hl, WildDataPointers
    hl = WildDataPointers;
// ld a, [wCurMap]
    a = wCurMap;
//
// get wild data for current map
// ld c, a
// ld b, 0
// add hl, bc
// add hl, bc
// ld a, [hli]
    a = hli;
// ld h, [hl]
// ld l, a       ; hl now points to wild data for current map
// ld a, [hli]
    a = hli;
// ld [wGrassRate], a
    wGrassRate = a;
// and a
// jr z, .NoGrassData ; if no grass data, skip to surfing data
// push hl
// ld de, wGrassMons ; otherwise, load grass data
    de = wGrassMons ; otherwise, load grass data;
// ld bc, $14
    bc = $14;
// call CopyData
    CopyData();
// pop hl
// ld bc, $14
    bc = $14;
// add hl, bc
// .NoGrassData
// ld a, [hli]
    a = hli;
// ld [wWaterRate], a
    wWaterRate = a;
// and a
// ret z        ; if no water data, we're done
    return;
// ld de, wWaterMons  ; otherwise, load surfing data
    de = wWaterMons  ; otherwise, load surfing data;
// ld bc, $14
    bc = $14;
// jp CopyData
    CopyData(); /* jp */
//
// INCLUDE "data/wild/grass_water.asm"
}
