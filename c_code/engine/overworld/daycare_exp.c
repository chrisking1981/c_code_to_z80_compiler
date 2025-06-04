void IncrementDayCareMonExp(void) {
IncrementDayCareMonExp:
// ld a, [wDayCareInUse]
    a = wDayCareInUse;
// and a
// ret z
    return;
// ld hl, wDayCareMonExp + 2
    hl = wDayCareMonExp + 2;
// inc [hl]
// ret nz
    return;
// dec hl
// inc [hl]
// ret nz
    return;
// dec hl
// inc [hl]
// ld a, [hl]
    a = hl;
// cp $50
// ret c
    return;
// ld a, $50
    a = $50;
// ld [hl], a
    *hl = a;
// ret
    return;
}
