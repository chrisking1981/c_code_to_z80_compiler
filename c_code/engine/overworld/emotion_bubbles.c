#include <stdint.h>

// Extern declarations for variables and functions
extern uint8_t wWhichEmotionBubble;
extern uint8_t wUpdateSpritesEnabled;
extern uint8_t wMovementFlags;
extern uint8_t wShadowOAMSprite31Attributes;
extern uint8_t wShadowOAMSprite35Attributes;
extern uint8_t wShadowOAMSprite39Attributes;
extern uint8_t wSpritePlayerStateData1YPixels;
extern uint8_t wEmotionBubbleSpriteIndex;
extern void CopyVideoData(void);
extern void WriteOAMBlock(void);
extern void DelayFrames(uint8_t frames);
extern void DelayFrame(void);
extern void UpdateSprites(void);

// Constants
#define BIT_LEDGE_OR_FISHING 0
#define EmotionBubblesOAMBlock 0x00 // Placeholder for actual OAM block address

// Function implementation
void EmotionBubble(void) {
    // ld a, [wWhichEmotionBubble]
    uint8_t a = wWhichEmotionBubble;
    // ld c, a
    uint8_t c = a;
    // ld b, 0
    uint8_t b = 0;
    // ld hl, EmotionBubblesPointerTable
    uint16_t hl = (uint16_t)EmotionBubblesPointerTable;
    // add hl, bc
    hl += (b << 8) | c; /* add hl, bc */
    // add hl, bc
    hl += (b << 8) | c; /* add hl, bc */
    // ld e, [hl]
    uint8_t e = *((uint8_t*)hl);
    // inc hl
    hl++;
    // ld d, [hl]
    uint8_t d = *((uint8_t*)hl);
    // ld hl, vChars1 tile $78
    hl = (uint16_t)&vChars1 + 0x78; /* tile $78 */
    // lb bc, BANK(EmotionBubbles), 4
    // Placeholder for bank loading logic
    // call CopyVideoData
    CopyVideoData();
    // ld a, [wUpdateSpritesEnabled]
    a = wUpdateSpritesEnabled;
    // push af
    uint8_t saved_af = a;
    // ld a, $ff
    a = 0xFF;
    // ld [wUpdateSpritesEnabled], a
    wUpdateSpritesEnabled = a;
    // ld a, [wMovementFlags]
    a = wMovementFlags;
    // bit BIT_LEDGE_OR_FISHING, a
    // are the last 4 OAM entries reserved for a shadow or fishing rod?
    // ld hl, wShadowOAMSprite35Attributes
    hl = (uint16_t)&wShadowOAMSprite35Attributes;
    // ld de, wShadowOAMSprite39Attributes
    uint16_t de = (uint16_t)&wShadowOAMSprite39Attributes;
    // jr z, .next
    if (!(a & (1 << BIT_LEDGE_OR_FISHING))) goto next;
    // ld hl, wShadowOAMSprite31Attributes
    hl = (uint16_t)&wShadowOAMSprite31Attributes;
    // ld de, wShadowOAMSprite35Attributes
    de = (uint16_t)&wShadowOAMSprite35Attributes;

next:
    // ld bc, $90
    uint16_t bc = 0x90;
.loop:
    // ld a, [hl]
    a = *(uint8_t*)hl;
    // ld [de], a
    *(uint8_t*)de = a;
    // dec hl
    hl--;
    // dec de
    de--;
    // dec bc
    bc--;
    // ld a, c
    a = c;
    // or b
    a |= b;
    // jr nz, .loop
    if (a != 0) goto loop;

    // get the screen coordinates of the sprite the bubble is to be displayed above
    // ld hl, wSpritePlayerStateData1YPixels
    hl = (uint16_t)&wSpritePlayerStateData1YPixels;
    // ld a, [wEmotionBubbleSpriteIndex]
    a = wEmotionBubbleSpriteIndex;
    // swap a
    a = (a << 4) | (a >> 4); // Simulate swap
    // ld c, a
    c = a;
    // ld b, 0
    b = 0;
    // add hl, bc
    hl += (b << 8) | c; /* add hl, bc */
    // ld a, [hli]
    a = *(uint8_t*)hl;
    // ld b, a
    b = a;
    // inc hl
    hl++;
    // ld a, [hl]
    a = *(uint8_t*)hl;
    // add $8
    a += 0x08;
    // ld c, a
    c = a;

    // ld de, EmotionBubblesOAMBlock
    de = EmotionBubblesOAMBlock;
    // xor a
    a = 0;
    // call WriteOAMBlock
    WriteOAMBlock();
    // ld c, 60
    c = 60;
    // call DelayFrames
    DelayFrames(c);
    // pop af
    a = saved_af;
    // ld [wUpdateSpritesEnabled], a
    wUpdateSpritesEnabled = a;
    // call DelayFrame
    DelayFrame();
    // jp UpdateSprites
    UpdateSprites();
}

// Data arrays
const uint16_t EmotionBubblesPointerTable[] = {
    (uint16_t)&ShockEmote,
    (uint16_t)&QuestionEmote,
    (uint16_t)&HappyEmote,
    -1 // end
};

const uint8_t EmotionBubblesOAMBlock[] = {
    0xf8, 0,
    0xf9, 0,
    0xfa, 0,
    0xfb, 0
};

// Graphics sections
// ShockEmote:    INCBIN "gfx/emotes/shock.2bpp"
const uint8_t ShockEmote_INCBIN[] = { /* gfx/emotes/shock.2bpp */ };
// QuestionEmote: INCBIN "gfx/emotes/question.2bpp"
const uint8_t QuestionEmote_INCBIN[] = { /* gfx/emotes/question.2bpp */ };
// HappyEmote:    INCBIN "gfx/emotes/happy.2bpp"
const uint8_t HappyEmote_INCBIN[] = { /* gfx/emotes/happy.2bpp */ };