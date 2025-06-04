void UpdateSpriteFacingOffsetAndDelayMovement(void) {
UpdateSpriteFacingOffsetAndDelayMovement:
// ld h, HIGH(wSpriteStateData2)
// ldh a, [hCurrentSpriteOffset]
    a = hCurrentSpriteOffset;
// add $8
// ld l, a
// ld a, $7f ; maximum movement delay
    a = $7f ; maximum movement delay;
// ld [hl], a ; x#SPRITESTATEDATA2_MOVEMENTDELAY
    *hl = a;
// dec h ; HIGH(wSpriteStateData1)
// ldh a, [hCurrentSpriteOffset]
    a = hCurrentSpriteOffset;
// add $9
// ld l, a
// ld a, [hld] ; x#SPRITESTATEDATA1_FACINGDIRECTION
    a = hld;
// ld b, a
// xor a
    a = 0;
// ld [hld], a ; x#SPRITESTATEDATA1_ANIMFRAMECOUNTER
    hld = a;
// ld [hl], a ; x#SPRITESTATEDATA1_INTRAANIMFRAMECOUNTER
    *hl = a;
// ldh a, [hCurrentSpriteOffset]
    a = hCurrentSpriteOffset;
// add SPRITESTATEDATA1_IMAGEINDEX
// ld l, a
// ld a, [hl] ; x#SPRITESTATEDATA1_IMAGEINDEX
    a = hl;
// or b ; or in the facing direction
// ld [hld], a
    hld = a;
// ld a, $2 ; delayed movement status
    a = $2 ; delayed movement status;
// ld [hl], a ; x#SPRITESTATEDATA1_MOVEMENTSTATUS
    *hl = a;
// ret
    return;
}
