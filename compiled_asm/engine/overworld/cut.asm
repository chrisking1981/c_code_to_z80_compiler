UsedCut:
	xor a
	ld [wActionResultOrTookBattleTurn], a
	ld a, [wCurMapTileset]
	and a
	jr z, .overworld
	cp GYM
	ld a, [wTileInFrontOfPlayer]
	cp $50 ; gym cut tree
	jr nz, .nothingToCut
.overworld
	dec a
	ld a, [wTileInFrontOfPlayer]
	cp $3d ; cut tree
	jr z, .canCut
.nothingToCut
	ld hl, .NothingToCutText
	jp PrintText

NothingToCutText:
	text_far _NothingToCutText
	text_end

	ret
.canCut
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
