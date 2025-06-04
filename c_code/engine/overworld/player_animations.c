void EnterMapAnim(void) {
EnterMapAnim:
// call InitFacingDirectionList
    InitFacingDirectionList();
// ld a, $ec
    a = $ec;
// ld [wSpritePlayerStateData1YPixels], a
    wSpritePlayerStateData1YPixels = a;
// call Delay3
    Delay3();
// push hl
// call GBFadeInFromWhite
    GBFadeInFromWhite();
// ld hl, wStatusFlags7
    hl = wStatusFlags7;
// bit BIT_USED_FLY, [hl]
// res BIT_USED_FLY, [hl]
// jr nz, .flyAnimation
// ld a, SFX_TELEPORT_ENTER_1
    a = SFX_TELEPORT_ENTER_1;
// call PlaySound
    PlaySound();
// ld hl, wStatusFlags6
    hl = wStatusFlags6;
// bit BIT_DUNGEON_WARP, [hl]
// res BIT_DUNGEON_WARP, [hl]
// pop hl
// jr nz, .dungeonWarpAnimation
// call PlayerSpinWhileMovingDown
    PlayerSpinWhileMovingDown();
// ld a, SFX_TELEPORT_ENTER_2
    a = SFX_TELEPORT_ENTER_2;
// call PlaySound
    PlaySound();
// call IsPlayerStandingOnWarpPadOrHole
    IsPlayerStandingOnWarpPadOrHole();
// ld a, b
    a = b;
// and a
// jr nz, .done
// if the player is not standing on a warp pad or hole
// ld hl, wPlayerSpinInPlaceAnimFrameDelay
    hl = wPlayerSpinInPlaceAnimFrameDelay;
// xor a
    a = 0;
// ld [hli], a ; wPlayerSpinInPlaceAnimFrameDelay
    *hl++ = a;
// inc a
// ld [hli], a ; wPlayerSpinInPlaceAnimFrameDelayDelta
    *hl++ = a;
// ld a, $8
    a = $8;
// ld [hli], a ; wPlayerSpinInPlaceAnimFrameDelayEndValue
    *hl++ = a;
// ld [hl], $ff ; wPlayerSpinInPlaceAnimSoundID
// ld hl, wFacingDirectionList
    hl = wFacingDirectionList;
// call PlayerSpinInPlace
    PlayerSpinInPlace();
// .restoreDefaultMusic
// call PlayDefaultMusic
    PlayDefaultMusic();
// .done
// jp RestoreFacingDirectionAndYScreenPos
    RestoreFacingDirectionAndYScreenPos(); /* jp */
// .dungeonWarpAnimation
// ld c, 50
// call DelayFrames
    DelayFrames();
// call PlayerSpinWhileMovingDown
    PlayerSpinWhileMovingDown();
// jr .done
// .flyAnimation
// pop hl
// ld de, BirdSprite
    de = BirdSprite;
// ld hl, vNPCSprites
    hl = vNPCSprites;
// lb bc, BANK(BirdSprite), $0c
// call CopyVideoData
    CopyVideoData();
// call LoadBirdSpriteGraphics
    LoadBirdSpriteGraphics();
// ld a, SFX_FLY
    a = SFX_FLY;
// call PlaySound
    PlaySound();
// ld hl, wFlyAnimUsingCoordList
    hl = wFlyAnimUsingCoordList;
// xor a ; is using coord list
    a = 0;
// ld [hli], a ; wFlyAnimUsingCoordList
    *hl++ = a;
// ld a, 12
    a = 12;
// ld [hli], a ; wFlyAnimCounter
    *hl++ = a;
// ld [hl], $8 ; wFlyAnimBirdSpriteImageIndex (facing right)
// ld de, FlyAnimationEnterScreenCoords
    de = FlyAnimationEnterScreenCoords;
// call DoFlyAnimation
    DoFlyAnimation();
// call LoadPlayerSpriteGraphics
    LoadPlayerSpriteGraphics();
// jr .restoreDefaultMusic
//
FlyAnimationEnterScreenCoords:
// y, x pairs
// This is the sequence of screen coordinates used by the overworld
// Fly animation when the player is entering a map.
// db $05, $98
// db $0F, $90
// db $18, $88
// db $20, $80
// db $27, $78
// db $2D, $70
// db $32, $68
// db $36, $60
// db $39, $58
// db $3B, $50
// db $3C, $48
// db $3C, $40
//
PlayerSpinWhileMovingDown:
// ld hl, wPlayerSpinWhileMovingUpOrDownAnimDeltaY
    hl = wPlayerSpinWhileMovingUpOrDownAnimDeltaY;
// ld a, $10
    a = $10;
// ld [hli], a ; wPlayerSpinWhileMovingUpOrDownAnimDeltaY
    *hl++ = a;
// ld a, $3c
    a = $3c;
// ld [hli], a ; wPlayerSpinWhileMovingUpOrDownAnimMaxY
    *hl++ = a;
// call GetPlayerTeleportAnimFrameDelay
    GetPlayerTeleportAnimFrameDelay();
// ld [hl], a ; wPlayerSpinWhileMovingUpOrDownAnimFrameDelay
    *hl = a;
// jp PlayerSpinWhileMovingUpOrDown
    PlayerSpinWhileMovingUpOrDown(); /* jp */
//
_LeaveMapAnim:
// call InitFacingDirectionList
    InitFacingDirectionList();
// call IsPlayerStandingOnWarpPadOrHole
    IsPlayerStandingOnWarpPadOrHole();
// ld a, b
    a = b;
// and a
// jr z, .playerNotStandingOnWarpPadOrHole
// dec a
// jp nz, LeaveMapThroughHoleAnim
    nz, LeaveMapThroughHoleAnim(); /* jp */
// .spinWhileMovingUp
// ld a, SFX_TELEPORT_EXIT_1
    a = SFX_TELEPORT_EXIT_1;
// call PlaySound
    PlaySound();
// ld hl, wPlayerSpinWhileMovingUpOrDownAnimDeltaY
    hl = wPlayerSpinWhileMovingUpOrDownAnimDeltaY;
// ld a, -$10
    a = -$10;
// ld [hli], a ; wPlayerSpinWhileMovingUpOrDownAnimDeltaY
    *hl++ = a;
// ld a, $ec
    a = $ec;
// ld [hli], a ; wPlayerSpinWhileMovingUpOrDownAnimMaxY
    *hl++ = a;
// call GetPlayerTeleportAnimFrameDelay
    GetPlayerTeleportAnimFrameDelay();
// ld [hl], a ; wPlayerSpinWhileMovingUpOrDownAnimFrameDelay
    *hl = a;
// call PlayerSpinWhileMovingUpOrDown
    PlayerSpinWhileMovingUpOrDown();
// call IsPlayerStandingOnWarpPadOrHole
    IsPlayerStandingOnWarpPadOrHole();
// ld a, b
    a = b;
// dec a
// jr z, .playerStandingOnWarpPad
// if not standing on a warp pad, there is an extra delay
// ld c, 10
// call DelayFrames
    DelayFrames();
// .playerStandingOnWarpPad
// call GBFadeOutToWhite
    GBFadeOutToWhite();
// jp RestoreFacingDirectionAndYScreenPos
    RestoreFacingDirectionAndYScreenPos(); /* jp */
// .playerNotStandingOnWarpPadOrHole
// ld a, $4
    a = $4;
// call StopMusic
    StopMusic();
// ld a, [wStatusFlags6]
    a = wStatusFlags6;
// bit BIT_ESCAPE_WARP, a
// jr z, .flyAnimation
// if going to the last used pokemon center
// ld hl, wPlayerSpinInPlaceAnimFrameDelay
    hl = wPlayerSpinInPlaceAnimFrameDelay;
// ld a, 16
    a = 16;
// ld [hli], a ; wPlayerSpinInPlaceAnimFrameDelay
    *hl++ = a;
// ld a, -1
    a = -1;
// ld [hli], a ; wPlayerSpinInPlaceAnimFrameDelayDelta
    *hl++ = a;
// xor a
    a = 0;
// ld [hli], a ; wPlayerSpinInPlaceAnimFrameDelayEndValue
    *hl++ = a;
// ld [hl], SFX_TELEPORT_EXIT_2 ; wPlayerSpinInPlaceAnimSoundID
// ld hl, wFacingDirectionList
    hl = wFacingDirectionList;
// call PlayerSpinInPlace
    PlayerSpinInPlace();
// jr .spinWhileMovingUp
// .flyAnimation
// call LoadBirdSpriteGraphics
    LoadBirdSpriteGraphics();
// ld hl, wFlyAnimUsingCoordList
    hl = wFlyAnimUsingCoordList;
// ld a, $ff ; is not using coord list (flap in place)
    a = $ff ; is not using coord list (flap in place);
// ld [hli], a ; wFlyAnimUsingCoordList
    *hl++ = a;
// ld a, 8
    a = 8;
// ld [hli], a ; wFlyAnimCounter
    *hl++ = a;
// ld [hl], $c ; wFlyAnimBirdSpriteImageIndex
// call DoFlyAnimation
    DoFlyAnimation();
// ld a, SFX_FLY
    a = SFX_FLY;
// call PlaySound
    PlaySound();
// ld hl, wFlyAnimUsingCoordList
    hl = wFlyAnimUsingCoordList;
// xor a ; is using coord list
    a = 0;
// ld [hli], a ; wFlyAnimUsingCoordList
    *hl++ = a;
// ld a, $c
    a = $c;
// ld [hli], a ; wFlyAnimCounter
    *hl++ = a;
// ld [hl], $c ; wFlyAnimBirdSpriteImageIndex (facing right)
// ld de, FlyAnimationScreenCoords1
    de = FlyAnimationScreenCoords1;
// call DoFlyAnimation
    DoFlyAnimation();
// ld c, 40
// call DelayFrames
    DelayFrames();
// ld hl, wFlyAnimCounter
    hl = wFlyAnimCounter;
// ld a, 11
    a = 11;
// ld [hli], a ; wFlyAnimCounter
    *hl++ = a;
// ld [hl], $8 ; wFlyAnimBirdSpriteImageIndex (facing left)
// ld de, FlyAnimationScreenCoords2
    de = FlyAnimationScreenCoords2;
// call DoFlyAnimation
    DoFlyAnimation();
// call GBFadeOutToWhite
    GBFadeOutToWhite();
// jp RestoreFacingDirectionAndYScreenPos
    RestoreFacingDirectionAndYScreenPos(); /* jp */
//
FlyAnimationScreenCoords1:
// y, x pairs
// This is the sequence of screen coordinates used by the first part
// of the Fly overworld animation.
// db $3C, $48
// db $3C, $50
// db $3B, $58
// db $3A, $60
// db $39, $68
// db $37, $70
// db $37, $78
// db $33, $80
// db $30, $88
// db $2D, $90
// db $2A, $98
// db $27, $A0
//
FlyAnimationScreenCoords2:
// y, x pairs
// This is the sequence of screen coordinates used by the second part
// of the Fly overworld animation.
// db $1A, $90
// db $19, $80
// db $17, $70
// db $15, $60
// db $12, $50
// db $0F, $40
// db $0C, $30
// db $09, $20
// db $05, $10
// db $00, $00
//
// db $F0, $00
//
LeaveMapThroughHoleAnim:
// ld a, $ff
    a = $ff;
// ld [wUpdateSpritesEnabled], a ; disable UpdateSprites
    wUpdateSpritesEnabled = a;
// shift upper half of player's sprite down 8 pixels and hide lower half
// ld a, [wShadowOAMSprite00TileID]
    a = wShadowOAMSprite00TileID;
// ld [wShadowOAMSprite02TileID], a
    wShadowOAMSprite02TileID = a;
// ld a, [wShadowOAMSprite01TileID]
    a = wShadowOAMSprite01TileID;
// ld [wShadowOAMSprite03TileID], a
    wShadowOAMSprite03TileID = a;
// ld a, $a0
    a = $a0;
// ld [wShadowOAMSprite00YCoord], a
    wShadowOAMSprite00YCoord = a;
// ld [wShadowOAMSprite01YCoord], a
    wShadowOAMSprite01YCoord = a;
// ld c, 2
// call DelayFrames
    DelayFrames();
// hide upper half of player's sprite
// ld a, $a0
    a = $a0;
// ld [wShadowOAMSprite02YCoord], a
    wShadowOAMSprite02YCoord = a;
// ld [wShadowOAMSprite03YCoord], a
    wShadowOAMSprite03YCoord = a;
// call GBFadeOutToWhite
    GBFadeOutToWhite();
// ld a, $1
    a = $1;
// ld [wUpdateSpritesEnabled], a ; enable UpdateSprites
    wUpdateSpritesEnabled = a;
// jp RestoreFacingDirectionAndYScreenPos
    RestoreFacingDirectionAndYScreenPos(); /* jp */
//
DoFlyAnimation:
// ld a, [wFlyAnimBirdSpriteImageIndex]
    a = wFlyAnimBirdSpriteImageIndex;
// xor $1 ; make the bird flap its wings
// ld [wFlyAnimBirdSpriteImageIndex], a
    wFlyAnimBirdSpriteImageIndex = a;
// ld [wSpritePlayerStateData1ImageIndex], a
    wSpritePlayerStateData1ImageIndex = a;
// call Delay3
    Delay3();
// ld a, [wFlyAnimUsingCoordList]
    a = wFlyAnimUsingCoordList;
// cp $ff
// jr z, .skipCopyingCoords ; if the bird is flapping its wings in place
// ld hl, wSpritePlayerStateData1YPixels
    hl = wSpritePlayerStateData1YPixels;
// ld a, [de]
    a = de;
// inc de
// ld [hli], a ; y
    *hl++ = a;
// inc hl
// ld a, [de]
    a = de;
// inc de
// ld [hl], a ; x
    *hl = a;
// .skipCopyingCoords
// ld a, [wFlyAnimCounter]
    a = wFlyAnimCounter;
// dec a
// ld [wFlyAnimCounter], a
    wFlyAnimCounter = a;
// jr nz, DoFlyAnimation
// ret
    return;
//
LoadBirdSpriteGraphics:
// ld de, BirdSprite
    de = BirdSprite;
// ld hl, vNPCSprites
    hl = vNPCSprites;
// lb bc, BANK(BirdSprite), 12
// call CopyVideoData
    CopyVideoData();
// ld de, BirdSprite tile 12 ; moving animation sprite
    de = BirdSprite tile 12 ; moving animation sprite;
// ld hl, vNPCSprites2
    hl = vNPCSprites2;
// lb bc, BANK(BirdSprite), 12
// jp CopyVideoData
    CopyVideoData(); /* jp */
//
InitFacingDirectionList:
// ld a, [wSpritePlayerStateData1ImageIndex] ; (image index is locked to standing images)
    a = wSpritePlayerStateData1ImageIndex;
// ld [wSavedPlayerFacingDirection], a
    wSavedPlayerFacingDirection = a;
// ld a, [wSpritePlayerStateData1YPixels]
    a = wSpritePlayerStateData1YPixels;
// ld [wSavedPlayerScreenY], a
    wSavedPlayerScreenY = a;
// ld hl, PlayerSpinningFacingOrder
    hl = PlayerSpinningFacingOrder;
// ld de, wFacingDirectionList
    de = wFacingDirectionList;
// ld bc, 4
    bc = 4;
// call CopyData
    CopyData();
// ld a, [wSpritePlayerStateData1ImageIndex] ; (image index is locked to standing images)
    a = wSpritePlayerStateData1ImageIndex;
// ld hl, wFacingDirectionList
    hl = wFacingDirectionList;
// find the place in the list that matches the current facing direction
// .loop
// cp [hl]
// inc hl
// jr nz, .loop
// dec hl
// ret
    return;
//
PlayerSpinningFacingOrder:
// The order of the direction the player's sprite is facing when teleporting
// away. Creates a spinning effect.
// db SPRITE_FACING_DOWN, SPRITE_FACING_LEFT, SPRITE_FACING_UP, SPRITE_FACING_RIGHT
//
SpinPlayerSprite:
// copy the current value from the list into the sprite data and rotate the list
// ld a, [hl]
    a = hl;
// ld [wSpritePlayerStateData1ImageIndex], a ; (image index is locked to standing images)
    wSpritePlayerStateData1ImageIndex = a;
// push hl
// ld hl, wFacingDirectionList
    hl = wFacingDirectionList;
// ld de, wFacingDirectionList - 1
    de = wFacingDirectionList - 1;
// ld bc, 4
    bc = 4;
// call CopyData
    CopyData();
// ld a, [wFacingDirectionList - 1]
    a = wFacingDirectionList - 1;
// ld [wFacingDirectionList + 3], a
    wFacingDirectionList + 3 = a;
// pop hl
// ret
    return;
//
PlayerSpinInPlace:
// call SpinPlayerSprite
    SpinPlayerSprite();
// ld a, [wPlayerSpinInPlaceAnimFrameDelay]
    a = wPlayerSpinInPlaceAnimFrameDelay;
// ld c, a
// and $3
// jr nz, .skipPlayingSound
// when the last delay was a multiple of 4, play a sound if there is one
// ld a, [wPlayerSpinInPlaceAnimSoundID]
    a = wPlayerSpinInPlaceAnimSoundID;
// cp $ff
// call nz, PlaySound
    nz, PlaySound();
// .skipPlayingSound
// ld a, [wPlayerSpinInPlaceAnimFrameDelayDelta]
    a = wPlayerSpinInPlaceAnimFrameDelayDelta;
// add c
// ld [wPlayerSpinInPlaceAnimFrameDelay], a
    wPlayerSpinInPlaceAnimFrameDelay = a;
// ld c, a
// ld a, [wPlayerSpinInPlaceAnimFrameDelayEndValue]
    a = wPlayerSpinInPlaceAnimFrameDelayEndValue;
// cp c
// ret z
    return;
// call DelayFrames
    DelayFrames();
// jr PlayerSpinInPlace
//
PlayerSpinWhileMovingUpOrDown:
// call SpinPlayerSprite
    SpinPlayerSprite();
// ld a, [wPlayerSpinWhileMovingUpOrDownAnimDeltaY]
    a = wPlayerSpinWhileMovingUpOrDownAnimDeltaY;
// ld c, a
// ld a, [wSpritePlayerStateData1YPixels]
    a = wSpritePlayerStateData1YPixels;
// add c
// ld [wSpritePlayerStateData1YPixels], a
    wSpritePlayerStateData1YPixels = a;
// ld c, a
// ld a, [wPlayerSpinWhileMovingUpOrDownAnimMaxY]
    a = wPlayerSpinWhileMovingUpOrDownAnimMaxY;
// cp c
// ret z
    return;
// ld a, [wPlayerSpinWhileMovingUpOrDownAnimFrameDelay]
    a = wPlayerSpinWhileMovingUpOrDownAnimFrameDelay;
// ld c, a
// call DelayFrames
    DelayFrames();
// jr PlayerSpinWhileMovingUpOrDown
//
RestoreFacingDirectionAndYScreenPos:
// ld a, [wSavedPlayerScreenY]
    a = wSavedPlayerScreenY;
// ld [wSpritePlayerStateData1YPixels], a
    wSpritePlayerStateData1YPixels = a;
// ld a, [wSavedPlayerFacingDirection]
    a = wSavedPlayerFacingDirection;
// ld [wSpritePlayerStateData1ImageIndex], a ; (image index is locked to standing images)
    wSpritePlayerStateData1ImageIndex = a;
// ret
    return;
//
// if SGB, 2 frames, else 3 frames
GetPlayerTeleportAnimFrameDelay:
// ld a, [wOnSGB]
    a = wOnSGB;
// xor $1
// inc a
// inc a
// ret
    return;
//
IsPlayerStandingOnWarpPadOrHole:
// ld b, 0
// ld hl, WarpPadAndHoleData
    hl = WarpPadAndHoleData;
// ld a, [wCurMapTileset]
    a = wCurMapTileset;
// ld c, a
// .loop
// ld a, [hli]
    a = hli;
// cp $ff
// jr z, .done
// cp c
// jr nz, .nextEntry
// lda_coord 8, 9
// cp [hl]
// jr z, .foundMatch
// .nextEntry
// inc hl
// inc hl
// jr .loop
// .foundMatch
// inc hl
// ld b, [hl]
// .done
// ld a, b
    a = b;
// ld [wStandingOnWarpPadOrHole], a
    wStandingOnWarpPadOrHole = a;
// ret
    return;
//
// INCLUDE "data/tilesets/warp_pad_hole_tile_ids.asm"
//
FishingAnim:
// ld c, 10
// call DelayFrames
    DelayFrames();
// ld hl, wMovementFlags
    hl = wMovementFlags;
// set BIT_LEDGE_OR_FISHING, [hl]
// ld de, RedSprite
    de = RedSprite;
// ld hl, vNPCSprites tile $00
    hl = vNPCSprites tile $00;
// lb bc, BANK(RedSprite), 12
// call CopyVideoData
    CopyVideoData();
// ld a, $4
    a = $4;
// ld hl, RedFishingTiles
    hl = RedFishingTiles;
// call LoadAnimSpriteGfx
    LoadAnimSpriteGfx();
// ld a, [wSpritePlayerStateData1ImageIndex]
    a = wSpritePlayerStateData1ImageIndex;
// ld c, a
// ld b, $0
// ld hl, FishingRodOAM
    hl = FishingRodOAM;
// add hl, bc
// ld de, wShadowOAMSprite39
    de = wShadowOAMSprite39;
// ld bc, $4
    bc = $4;
// call CopyData
    CopyData();
// ld c, 100
// call DelayFrames
    DelayFrames();
// ld a, [wRodResponse]
    a = wRodResponse;
// and a
// ld hl, NoNibbleText
    hl = NoNibbleText;
// jr z, .done
// cp $2
// ld hl, NothingHereText
    hl = NothingHereText;
// jr z, .done
//
// there was a bite
//
// shake the player's sprite vertically
// ld b, 10
// .loop
// ld hl, wSpritePlayerStateData1YPixels
    hl = wSpritePlayerStateData1YPixels;
// call .ShakePlayerSprite
    .ShakePlayerSprite();
// ld hl, wShadowOAMSprite39
    hl = wShadowOAMSprite39;
// call .ShakePlayerSprite
    .ShakePlayerSprite();
// call Delay3
    Delay3();
// dec b
// jr nz, .loop
//
// If the player is facing up, hide the fishing rod so it doesn't overlap with
// the exclamation bubble that will be shown next.
// ld a, [wSpritePlayerStateData1ImageIndex] ; (image index is locked to standing images)
    a = wSpritePlayerStateData1ImageIndex;
// cp SPRITE_FACING_UP
// jr nz, .skipHidingFishingRod
// ld a, $a0
    a = $a0;
// ld [wShadowOAMSprite39YCoord], a
    wShadowOAMSprite39YCoord = a;
//
// .skipHidingFishingRod
// ld hl, wEmotionBubbleSpriteIndex
    hl = wEmotionBubbleSpriteIndex;
// xor a
    a = 0;
// ld [hli], a ; player's sprite
    *hl++ = a;
// ld [hl], a ; EXCLAMATION_BUBBLE
    *hl = a;
// predef EmotionBubble
//
// If the player is facing up, unhide the fishing rod.
// ld a, [wSpritePlayerStateData1ImageIndex] ; (image index is locked to standing images)
    a = wSpritePlayerStateData1ImageIndex;
// cp SPRITE_FACING_UP
// jr nz, .skipUnhidingFishingRod
// ld a, $44
    a = $44;
// ld [wShadowOAMSprite39YCoord], a
    wShadowOAMSprite39YCoord = a;
//
// .skipUnhidingFishingRod
// ld hl, ItsABiteText
    hl = ItsABiteText;
//
// .done
// call PrintText
    PrintText();
// ld hl, wMovementFlags
    hl = wMovementFlags;
// res BIT_LEDGE_OR_FISHING, [hl]
// call LoadFontTilePatterns
    LoadFontTilePatterns();
// ret
    return;
//
// .ShakePlayerSprite
// ld a, [hl]
    a = hl;
// xor $1
// ld [hl], a
    *hl = a;
// ret
    return;
//
NoNibbleText:
// text_far _NoNibbleText
// text_end
//
NothingHereText:
// text_far _NothingHereText
// text_end
//
ItsABiteText:
// text_far _ItsABiteText
// text_end
//
FishingRodOAM:
// specifies how the fishing rod should be drawn on the screen
// dbsprite  9, 11,  4,  3, $fd, 0         ; down
// dbsprite  9,  8,  4,  4, $fd, 0         ; up
// dbsprite  8, 10,  0,  0, $fe, 0         ; left
// dbsprite 11, 10,  0,  0, $fe, OAM_HFLIP ; right
//
// MACRO fishing_gfx
// dw \1
// db \2
// db BANK(\1)
// dw vNPCSprites tile \3
// ENDM
//
RedFishingTiles:
// fishing_gfx RedFishingTilesFront, 2, $02
// fishing_gfx RedFishingTilesBack,  2, $06
// fishing_gfx RedFishingTilesSide,  2, $0a
// fishing_gfx RedFishingRodTiles,   3, $fd
//
_HandleMidJump:
// ld a, [wPlayerJumpingYScreenCoordsIndex]
    a = wPlayerJumpingYScreenCoordsIndex;
// ld c, a
// inc a
// cp $10
// jr nc, .finishedJump
// ld [wPlayerJumpingYScreenCoordsIndex], a
    wPlayerJumpingYScreenCoordsIndex = a;
// ld b, 0
// ld hl, PlayerJumpingYScreenCoords
    hl = PlayerJumpingYScreenCoords;
// add hl, bc
// ld a, [hl]
    a = hl;
// ld [wSpritePlayerStateData1YPixels], a
    wSpritePlayerStateData1YPixels = a;
// ret
    return;
// .finishedJump
// ld a, [wWalkCounter]
    a = wWalkCounter;
// cp 0
// ret nz
    return;
// call UpdateSprites
    UpdateSprites();
// call Delay3
    Delay3();
// xor a
    a = 0;
// ldh [hJoyHeld], a
    hJoyHeld = a;
// ldh [hJoyPressed], a
    hJoyPressed = a;
// ldh [hJoyReleased], a
    hJoyReleased = a;
// ld [wPlayerJumpingYScreenCoordsIndex], a
    wPlayerJumpingYScreenCoordsIndex = a;
// ld hl, wMovementFlags
    hl = wMovementFlags;
// res BIT_LEDGE_OR_FISHING, [hl]
// ld hl, wStatusFlags5
    hl = wStatusFlags5;
// res BIT_SCRIPTED_MOVEMENT_STATE, [hl]
// xor a
    a = 0;
// ld [wJoyIgnore], a
    wJoyIgnore = a;
// ret
    return;
//
PlayerJumpingYScreenCoords:
// Sequence of y screen coordinates for player's sprite when jumping over a ledge.
// db $38, $36, $34, $32, $31, $30, $30, $30, $31, $32, $33, $34, $36, $38, $3C, $3C
}
