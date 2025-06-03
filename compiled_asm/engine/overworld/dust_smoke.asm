SSAnneSmokePuffTile:
	db /* gfx/overworld/smoke.2bpp */
AnimateBoulderDust:
	ld a, $1
	ld [wWhichAnimationOffsets], a
	ld a, [wUpdateSpritesEnabled]
	push af
	ld a, $ff
	ld [wUpdateSpritesEnabled], a
	ld a, %11100100
	ldh [rOBP1], a
	call LoadSmokeTileFourTimes
	farcall WriteCutOrBoulderDustAnimationOAMBlock
	ld c, 8 ; number of steps in animation
..loop
	push bc
	call GetMoveBoulderDustFunctionPointer
	ld bc, .returnAddress
	push bc
	ld c, 4
	jp hl
	jr .*hl; // Assuming hl points to a valid functio
..returnAddress
	ldh a, [rOBP1]
	xor %01100100
	ldh [rOBP1], a
	call Delay3
	pop bc
	dec c
	jr nz, .loop
	pop af
	ld [wUpdateSpritesEnabled], a
	jp LoadPlayerSpriteGraphics
