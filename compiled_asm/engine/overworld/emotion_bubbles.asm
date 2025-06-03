EmotionBubble:
	ld a, [wWhichEmotionBubble]
	ld c, a
	ld b, 0
	ld hl, EmotionBubblesPointerTable
	add hl, bc
	add hl, bc
	ld e, [hl]
	inc hl
	ld d, [hl]
	ld hl, vChars1 tile $78
	lb bc, BANK(EmotionBubbles), 4
	call CopyVideoData
	ld a, [wUpdateSpritesEnabled]
	push af
	ld a, $ff
	ld [wUpdateSpritesEnabled], a
	ld a, [wMovementFlags]
	bit BIT_LEDGE_OR_FISHING, a
	ld hl, wShadowOAMSprite35Attributes
	ld de, wShadowOAMSprite39Attributes
	jr z, .next
	ld hl, wShadowOAMSprite31Attributes
	ld de, wShadowOAMSprite35Attributes
.next
	ld bc, $90
..loop
	ld a, [hl]
	ld [de], a
	dec hl
	dec de
	dec bc
	ld a, c
	or b
	jr nz, .loop
	ld hl, wSpritePlayerStateData1YPixels
	ld a, [wEmotionBubbleSpriteIndex]
	swap a
	ld c, a
	ld b, 0
	add hl, bc
	ld a, [hli]
	ld b, a
	inc hl
	ld a, [hl]
	add $8
	ld c, a
	ld de, EmotionBubblesOAMBlock
	xor a
	call WriteOAMBlock
	ld c, 60
	call DelayFrames
	pop af
	ld [wUpdateSpritesEnabled], a
	call DelayFrame
	jp UpdateSprites

EmotionBubblesPointerTable::
	dw &ShockEmote
	dw &QuestionEmote
	dw &HappyEmote
	dw -1
EmotionBubblesOAMBlock:
; tile ID, attributes
	db $F8, 0
	db $F9, 0
	db $FA, 0
	db $FB, 0
ShockEmote:
	INCBIN "gfx/emotes/shock.2bpp"
QuestionEmote:
	INCBIN "gfx/emotes/question.2bpp"
HappyEmote:
	INCBIN "gfx/emotes/happy.2bpp"