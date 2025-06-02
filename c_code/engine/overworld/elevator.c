#include <stdint.h>

// External variables
extern uint8_t hSCY;
extern uint8_t wChannelSoundIDs;
extern uint8_t wMapViewVRAMPointer;
extern uint8_t vBGMap0;

// Constants
#define SCREEN_HEIGHT 18
#define SFX_STOP_ALL_MUSIC 0x00
#define SFX_COLLISION 0x22
#define SFX_SAFARI_ZONE_PA 0x3F
#define BANK(x) 0x08
#define CHAN5 4
#define HIGH(x) ((x) >> 8)

// External functions
extern void ShakeElevatorRedrawRow(int16_t de);
extern void Delay3(void);
extern void PlaySound(uint8_t sound);
extern void PlayMusic(uint8_t music);
extern void DelayFrames(uint8_t frames);
extern void UpdateSprites(void);
extern void PlayDefaultMusic(void);
extern void ScheduleNorthRowRedraw(void);

void ShakeElevator(void) {
    int16_t de;
    uint8_t a, d, e, b, c;
    
    // ld de, -$20
    de = -0x20;
    // call ShakeElevatorRedrawRow
    ShakeElevatorRedrawRow(de);
    // ld de, SCREEN_HEIGHT * $20
    de = SCREEN_HEIGHT * 0x20;
    // call ShakeElevatorRedrawRow
    ShakeElevatorRedrawRow(de);
    // call Delay3
    Delay3();
    // ld a, SFX_STOP_ALL_MUSIC
    a = SFX_STOP_ALL_MUSIC;
    // call PlaySound
    PlaySound(a);
    // ldh a, [hSCY]
    a = hSCY;
    // ld d, a
    d = a;
    // ld e, $1
    e = 0x1;
    // ld b, 100
    b = 100;

shakeLoop:
    // .shakeLoop ; scroll the BG up and down and play a sound effect
    // ld a, e
    a = e;
    // xor $fe
    a ^= 0xfe;
    // ld e, a
    e = a;
    // add d
    a += d;
    // ldh [hSCY], a
    hSCY = a;
    // push bc
    uint8_t saved_b = b;
    // ld c, BANK(SFX_Collision_1)
    c = BANK(SFX_Collision_1);
    // ld a, SFX_COLLISION
    a = SFX_COLLISION;
    // call PlayMusic
    PlayMusic(a);
    // pop bc
    b = saved_b;
    // ld c, 2
    c = 2;
    // call DelayFrames
    DelayFrames(c);
    // dec b
    b--;
    // jr nz, .shakeLoop
    if (b != 0) goto shakeLoop;
    
    // ld a, d
    a = d;
    // ldh [hSCY], a
    hSCY = a;
    // ld a, SFX_STOP_ALL_MUSIC
    a = SFX_STOP_ALL_MUSIC;
    // call PlaySound
    PlaySound(a);
    // ld c, BANK(SFX_Safari_Zone_PA)
    c = BANK(SFX_Safari_Zone_PA);
    // ld a, SFX_SAFARI_ZONE_PA
    a = SFX_SAFARI_ZONE_PA;
    // call PlayMusic
    PlayMusic(a);

musicLoop:
    // .musicLoop
    // ld a, [wChannelSoundIDs + CHAN5]
    a = wChannelSoundIDs + CHAN5;
    // cp SFX_SAFARI_ZONE_PA
    // jr z, .musicLoop
    if (a == SFX_SAFARI_ZONE_PA) goto musicLoop;
    
    // call UpdateSprites
    UpdateSprites();
    // jp PlayDefaultMusic
    PlayDefaultMusic(); /* jp */
}

void ShakeElevatorRedrawRow(int16_t de_param) {
    // This function is used to redraw certain portions of the screen, but it does
    // not appear to ever result in any visible effect, so this function seems to
    // be pointless.
    uint8_t* hl;
    uint8_t a, d, h, l;
    int16_t de = de_param;
    
    // ld hl, wMapViewVRAMPointer + 1
    hl = &wMapViewVRAMPointer + 1;
    // ld a, [hld]
    a = *hl--;
    // push af
    uint8_t saved_a1 = a;
    // ld a, [hl]
    a = *hl;
    // push af
    uint8_t saved_a2 = a;
    // push hl
    uint8_t* saved_hl1 = hl;
    // push hl
    uint8_t* saved_hl2 = hl;
    // ld a, [hli]
    a = *hl++;
    // ld h, [hl]
    h = *hl;
    // ld l, a
    l = a;
    // add hl, de
    hl = (uint8_t*)((uint16_t)((h << 8) | l) + de);
    // ld a, h
    a = (uint8_t)((uintptr_t)hl >> 8);
    // and $3
    a &= 0x3;
    // or HIGH(vBGMap0)
    a |= HIGH(vBGMap0);
    // ld d, a
    d = a;
    // ld a, l
    a = (uint8_t)(uintptr_t)hl;
    // pop hl
    hl = saved_hl2;
    // ld [hli], a
    *hl++ = a;
    // ld [hl], d
    *hl = d;
    // call ScheduleNorthRowRedraw
    ScheduleNorthRowRedraw();
    // pop hl
    hl = saved_hl1;
    // pop af
    a = saved_a2;
    // ld [hli], a
    *hl++ = a;
    // pop af
    a = saved_a1;
    // ld [hl], a
    *hl = a;
    // jp Delay3
    Delay3(); /* jp */
}

// External constants that need to be defined elsewhere
extern const uint8_t SFX_Collision_1;
extern const uint8_t SFX_Safari_Zone_PA; 