void EmotionBubble(void) {
EmotionBubble:
// ld a, [wWhichEmotionBubble]
    a = wWhichEmotionBubble;
// ld c, a
// ld b, 0
// ld hl, EmotionBubblesPointerTable
    hl = EmotionBubblesPointerTable;
// add hl, bc
// add hl, bc
// ld e, [hl]
// inc hl
// ld d, [hl]
// ld hl, vChars1 tile $78
    hl = vChars1 tile $78;
// lb bc, BANK(EmotionBubbles), 4
// call CopyVideoData
    CopyVideoData();
// ld a, [wUpdateSpritesEnabled]
    a = wUpdateSpritesEnabled;
// push af
// ld a, $ff
    a = $ff;
// ld [wUpdateSpritesEnabled], a
    wUpdateSpritesEnabled = a;
// ld a, [wMovementFlags]
    a = wMovementFlags;
// bit BIT_LEDGE_OR_FISHING, a ; are the last 4 OAM entries reserved for a shadow or fishing rod?
// ld hl, wShadowOAMSprite35Attributes
    hl = wShadowOAMSprite35Attributes;
// ld de, wShadowOAMSprite39Attributes
    de = wShadowOAMSprite39Attributes;
// jr z, .next
// ld hl, wShadowOAMSprite31Attributes
    hl = wShadowOAMSprite31Attributes;
// ld de, wShadowOAMSprite35Attributes
    de = wShadowOAMSprite35Attributes;
//
// Copy OAM data 16 bytes forward to make room for emotion bubble OAM data at the
// start of the OAM buffer.
// .next
// ld bc, $90
    bc = $90;
// .loop
// ld a, [hl]
    a = hl;
// ld [de], a
    de = a;
// dec hl
// dec de
// dec bc
// ld a, c
    a = c;
// or b
// jr nz, .loop
//
// get the screen coordinates of the sprite the bubble is to be displayed above
// ld hl, wSpritePlayerStateData1YPixels
    hl = wSpritePlayerStateData1YPixels;
// ld a, [wEmotionBubbleSpriteIndex]
    a = wEmotionBubbleSpriteIndex;
// swap a
// ld c, a
// ld b, 0
// add hl, bc
// ld a, [hli]
    a = hli;
// ld b, a
// inc hl
// ld a, [hl]
    a = hl;
// add $8
// ld c, a
//
// ld de, EmotionBubblesOAMBlock
    de = EmotionBubblesOAMBlock;
// xor a
    a = 0;
// call WriteOAMBlock
    WriteOAMBlock();
// ld c, 60
// call DelayFrames
    DelayFrames();
// pop af
// ld [wUpdateSpritesEnabled], a
    wUpdateSpritesEnabled = a;
// call DelayFrame
    DelayFrame();
// jp UpdateSprites
    UpdateSprites(); /* jp */
//
EmotionBubblesPointerTable:
// entries correspond to *_BUBBLE constants
// dw ShockEmote
// dw QuestionEmote
// dw HappyEmote
//
EmotionBubblesOAMBlock:
// tile ID, attributes
// db $f8, 0
// db $f9, 0
// db $fa, 0
// db $fb, 0
//
EmotionBubbles:
// ShockEmote:    INCBIN "gfx/emotes/shock.2bpp"
// QuestionEmote: INCBIN "gfx/emotes/question.2bpp"
// HappyEmote:    INCBIN "gfx/emotes/happy.2bpp"
}
