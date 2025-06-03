#include <stdint.h>
#include <stdbool.h>

// Extern declarations
extern void ShakeElevatorRedrawRow(void);
extern void Delay3(void);
extern void PlaySound(uint8_t sound);
extern void PlayMusic(uint8_t sound);
extern void UpdateSprites(void);
extern void ScheduleNorthRowRedraw(void);
extern uint8_t hSCY;
extern uint8_t wChannelSoundIDs[];
extern uint8_t SFX_STOP_ALL_MUSIC;
extern uint8_t SFX_COLLISION;
extern uint8_t SFX_SAFARI_ZONE_PA;
extern uint8_t BANK(uint8_t sound);

// Constants
#define SCREEN_HEIGHT 18
#define CHAN5 5

void ShakeElevator(void) {
    // ld de, -$20
    uint16_t de = -0x20; 
    // call ShakeElevatorRedrawRow
    ShakeElevatorRedrawRow();
    // ld de, SCREEN_HEIGHT * $20
    de = SCREEN_HEIGHT * 0x20; 
    // call ShakeElevatorRedrawRow
    ShakeElevatorRedrawRow();
    // call Delay3
    Delay3();
    // ld a, SFX_STOP_ALL_MUSIC
    uint8_t a = SFX_STOP_ALL_MUSIC; 
    // call PlaySound
    PlaySound(a);
    // ldh a, [hSCY]
    a = hSCY; 
    // ld d, a
    uint8_t d = a; 
    // ld e, $1
    uint8_t e = 0x1; 
    // ld b, 100
    uint8_t b = 100; 
.shakeLoop: // .shakeLoop
    // ld a, e
    a = e; 
    // xor $fe
    a ^= 0xFE; 
    // ld e, a
    e = a; 
    // add d
    d += e; 
    // ldh [hSCY], a
    hSCY = a; 
    // push bc
    uint8_t saved_b = b; 
    uint8_t saved_c = 0; // bc is not used, but we need to save it
    // ld c, BANK(SFX_Collision_1)
    saved_c = BANK(SFX_COLLISION); 
    // ld a, SFX_COLLISION
    a = SFX_COLLISION; 
    // call PlayMusic
    PlayMusic(a);
    // pop bc
    b = saved_b; 
    // ld c, 2
    c = 2; 
    // call DelayFrames
    DelayFrames();
    // dec b
    b--; 
    // jr nz, .shakeLoop
    if (b != 0) goto .shakeLoop; 
    // ld a, d
    a = d; 
    // ldh [hSCY], a
    hSCY = a; 
    // ld a, SFX_STOP_ALL_MUSIC
    a = SFX_STOP_ALL_MUSIC; 
    // call PlaySound
    PlaySound(a); 
    // ld c, BANK(SFX_Safari_Zone_PA)
    c = BANK(SFX_SAFARI_ZONE_PA); 
    // ld a, SFX_SAFARI_ZONE_PA
    a = SFX_SAFARI_ZONE_PA; 
    // call PlayMusic
    PlayMusic(a); 
.musicLoop: // .musicLoop
    // ld a, [wChannelSoundIDs + CHAN5]
    a = wChannelSoundIDs[CHAN5]; 
    // cp SFX_SAFARI_ZONE_PA
    if (a == SFX_SAFARI_ZONE_PA) goto .musicLoop; 
    // call UpdateSprites
    UpdateSprites(); 
    // jp PlayDefaultMusic
    PlayDefaultMusic(); /* jp */
}

void ShakeElevatorRedrawRow(void) {
    // ld hl, wMapViewVRAMPointer + 1
    uint16_t hl = wMapViewVRAMPointer + 1; 
    // ld a, [hld]
    uint8_t a = *(uint8_t*)hl; 
    // push af
    uint8_t saved_a = a; 
    // ld a, [hl]
    a = *(uint8_t*)hl; 
    // push af
    saved_a = a; 
    // push hl
    uint16_t saved_hl = hl; 
    // push hl
    saved_hl = hl; 
    // ld a, [hli]
    a = *(uint8_t*)(hl++); 
    // ld h, [hl]
    uint8_t h = *(uint8_t*)hl; 
    // ld l, a
    uint8_t l = a; 
    // add hl, de
    hl += de; 
    // ld a, h
    a = h; 
    // and $3
    a &= 0x3; 
    // or HIGH(vBGMap0)
    a |= HIGH(vBGMap0); 
    // ld d, a
    d = a; 
    // ld a, l
    a = l; 
    // pop hl
    hl = saved_hl; 
    // ld [hli], a
    *(uint8_t*)(hl++) = a; 
    // ld [hl], d
    *(uint8_t*)hl = d; 
    // call ScheduleNorthRowRedraw
    ScheduleNorthRowRedraw(); 
    // pop hl
    hl = saved_hl; 
    // pop af
    a = saved_a; 
    // ld [hli], a
    *(uint8_t*)(hl++) = a; 
    // pop af
    a = saved_a; 
    // ld [hl], a
    *(uint8_t*)hl = a; 
    // jp Delay3
    Delay3(); /* jp */
}