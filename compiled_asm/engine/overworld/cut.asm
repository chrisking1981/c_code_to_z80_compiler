UsedCut:
	xor a
	ld [wActionResultOrTookBattleTurn], a
	ld a, [wCurMapTileset]
	and a
	cp GYM
	ld a, [wTileInFrontOfPlayer]
	cp $50 ; gym cut tree
	jr ...canCut
...overworld
	dec a
	ld a, [wTileInFrontOfPlayer]
	cp $3d ; cut tree
	cp $52 ; grass
...nothingToCut
	ld hl, .NothingToCutText
	jp PrintText
	ret
...canCut
	ld [wCutTile], a
	ld a, 1
	ld [wActionResultOrTookBattleTurn], a ; used cut
	ld a, [wWhichPokemon]
	ld hl, wPartyMonNicks
	call GetPartyMonName
	ld hl, wStatusFlags5
	set BIT_NO_TEXT_DELAY, [hl]
	call GBPalWhiteOutWithDelay3
	call ClearSprites
	call RestoreScreenTilesAndReloadTilePatterns
	ld a, SCREEN_HEIGHT_PX
	ldh [hWY], a
	call Delay3
	call LoadGBPal
	call LoadCurrentMapView
	call SaveScreenTilesToBuffer2
	call Delay3
	xor a
	ldh [hWY], a
	ld hl, UsedCutText
	call PrintText
	call LoadScreenTilesFromBuffer2
	ld hl, wStatusFlags5
	res BIT_NO_TEXT_DELAY, [hl]
	ld a, $ff
	ld [wUpdateSpritesEnabled], a
	call InitCutAnimOAM
	ld de, CutTreeBlockSwaps
	call ReplaceTreeTileBlock
	call RedrawMapView
	farcall AnimCut
	ld a, $1
	ld [wUpdateSpritesEnabled], a
	ld a, SFX_CUT
	call PlaySound
	ld a, $90
	ldh [hWY], a
	call UpdateSprites
	jp RedrawMapView

InitCutAnimOAM:
	xor a
	ld [wWhichAnimationOffsets], a
	ld a, %11100100
	ldh [rOBP1], a
	ld a, [wCutTile]
	cp $52
	ld de, Overworld_GFX tile $2d ; cuttable tree sprite top row
	ld hl, vChars1 tile $7c
	lb bc, BANK(Overworld_GFX), 2
	call CopyVideoData
	ld de, Overworld_GFX tile $3d ; cuttable tree sprite bottom row
	ld hl, vChars1 tile $7e
	call CopyVideoData
	jr ...WriteCutOrBoulderDustAnimationOAMBlock
...grass
	ld hl, vChars1 tile $7c
	call LoadCutGrassAnimationTilePattern
	ld hl, vChars1 tile $7d
	call LoadCutGrassAnimationTilePattern
	ld hl, vChars1 tile $7e
	call LoadCutGrassAnimationTilePattern
	ld hl, vChars1 tile $7f
	call LoadCutGrassAnimationTilePattern
	call WriteCutOrBoulderDustAnimationOAMBlock
	ld hl, wShadowOAMSprite36Attributes
	ld de, 4
	ld a, $30
	ld c, e
	ld [hl], a
	add hl, de
	xor $60
	dec c
	jr nz, .loop
	ret

LoadCutGrassAnimationTilePattern:
	ld de, MoveAnimationTiles1 tile 6 ; tile depicting a leaf
	lb bc, BANK(MoveAnimationTiles1), 1
	call CopyVideoData

WriteCutOrBoulderDustAnimationOAMBlock:
	call GetCutOrBoulderDustAnimationOffsets
	ld a, $9
	ld de, .OAMBlock
	jp WriteOAMBlock

GetCutOrBoulderDustAnimationOffsets:
	ld hl, wSpritePlayerStateData1YPixels
	ld a, [hli] ; player's sprite screen Y position
	inc hl
	ld a, [hli] ; player's sprite screen X position
	inc hl
	ld a, [hl] ; a holds direction of player (00: down, 04: up, 08: left, 0C: right)
	srl a
	ld e, a
	ld d, $0 ; de holds direction (00: down, 02: up, 04: left, 06: right)
	ld a, [wWhichAnimationOffsets]
	and a
	ld hl, BoulderDustAnimationOffsets
	jr ...addOffsets
...next
	ld hl, CutAnimationOffsets
...addOffsets
	add hl, de
	ld e, [hl]
	inc hl
	ld d, [hl]
	ld a, b
	add d
	ld b, a
	ld a, c
	add e
	ld c, a
	ret

ReplaceTreeTileBlock:
	push de
	ld a, [wCurMapWidth]
	add 6
	ld c, a
	ld b, 0
	ld d, 0
	ld hl, wCurrentTileBlockMapViewPointer
	ld a, [hli]
	ld h, [hl]
	ld l, a
	add hl, bc
	ld a, [wSpritePlayerStateData1FacingDirection]
	and a
	cp SPRITE_FACING_UP
	cp SPRITE_FACING_LEFT
	ld a, [wXBlockCoord]
	and a
	jr ...rightOfCenter
...down
	ld a, [wYBlockCoord]
	and a
	jr ...belowCenter
...up
	ld a, [wYBlockCoord]
	and a
	jr ...aboveCenter
...left
	ld a, [wXBlockCoord]
	and a
	jr ...centerTileBlock
...belowCenter
	add hl, bc
...centerTileBlock
	add hl, bc
...aboveCenter
	ld e, $2
	add hl, de
	jr ...next
...leftOfCenter
	ld e, $1
	add hl, bc
	add hl, de
	jr ...next
...rightOfCenter
	ld e, $3
	add hl, bc
	add hl, de
...next
	pop de
	ld a, [hl]
	ld c, a
	call while
	ld a, [de]
	inc de
	inc de
	cp $ff
	cp c
	dec de
	ld a, [de] ; replacement tile block from matching array entry
	ld [hl], a
	ret



CutTreeBlockSwaps:

CutAnimationOffsets:
; Each pair represents the x and y pixels offsets from the player
	db  8, $24 ; player is facing down
	db  4, $14 ; player is facing up

BoulderDustAnimationOffsets:
; Each pair represents the x and y pixels offsets from the player
	db  8, $34 ; player is facing down
	db -12, $14 ; player is facing up
