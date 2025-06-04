void ShakeElevator(void) {
ShakeElevator:
// ld de, -$20
    de = -$20;
// call ShakeElevatorRedrawRow
    ShakeElevatorRedrawRow();
// ld de, SCREEN_HEIGHT * $20
    de = SCREEN_HEIGHT * $20;
// call ShakeElevatorRedrawRow
    ShakeElevatorRedrawRow();
// call Delay3
    Delay3();
// ld a, SFX_STOP_ALL_MUSIC
    a = SFX_STOP_ALL_MUSIC;
// call PlaySound
    PlaySound();
// ldh a, [hSCY]
    a = hSCY;
// ld d, a
// ld e, $1
// ld b, 100
// .shakeLoop ; scroll the BG up and down and play a sound effect
// ld a, e
    a = e;
// xor $fe
// ld e, a
// add d
// ldh [hSCY], a
    hSCY = a;
// push bc
// ld c, BANK(SFX_Collision_1)
// ld a, SFX_COLLISION
    a = SFX_COLLISION;
// call PlayMusic
    PlayMusic();
// pop bc
// ld c, 2
// call DelayFrames
    DelayFrames();
// dec b
// jr nz, .shakeLoop
// ld a, d
    a = d;
// ldh [hSCY], a
    hSCY = a;
// ld a, SFX_STOP_ALL_MUSIC
    a = SFX_STOP_ALL_MUSIC;
// call PlaySound
    PlaySound();
// ld c, BANK(SFX_Safari_Zone_PA)
// ld a, SFX_SAFARI_ZONE_PA
    a = SFX_SAFARI_ZONE_PA;
// call PlayMusic
    PlayMusic();
// .musicLoop
// ld a, [wChannelSoundIDs + CHAN5]
    a = wChannelSoundIDs + CHAN5;
// cp SFX_SAFARI_ZONE_PA
// jr z, .musicLoop
// call UpdateSprites
    UpdateSprites();
// jp PlayDefaultMusic
    PlayDefaultMusic(); /* jp */
//
ShakeElevatorRedrawRow:
// This function is used to redraw certain portions of the screen, but it does
// not appear to ever result in any visible effect, so this function seems to
// be pointless.
// ld hl, wMapViewVRAMPointer + 1
    hl = wMapViewVRAMPointer + 1;
// ld a, [hld]
    a = hld;
// push af
// ld a, [hl]
    a = hl;
// push af
// push hl
// push hl
// ld a, [hli]
    a = hli;
// ld h, [hl]
// ld l, a
// add hl, de
// ld a, h
    a = h;
// and $3
// or HIGH(vBGMap0)
// ld d, a
// ld a, l
    a = l;
// pop hl
// ld [hli], a
    *hl++ = a;
// ld [hl], d
// call ScheduleNorthRowRedraw
    ScheduleNorthRowRedraw();
// pop hl
// pop af
// ld [hli], a
    *hl++ = a;
// pop af
// ld [hl], a
    *hl = a;
// jp Delay3
    Delay3(); /* jp */
}
