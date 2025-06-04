void ClearVariablesOnEnterMap(void) {
ClearVariablesOnEnterMap:
// ld a, SCREEN_HEIGHT_PX
    a = SCREEN_HEIGHT_PX;
// ldh [hWY], a
    hWY = a;
// ldh [rWY], a
    rWY = a;
// xor a
    a = 0;
// ldh [hAutoBGTransferEnabled], a
    hAutoBGTransferEnabled = a;
// ld [wStepCounter], a
    wStepCounter = a;
// ld [wLoneAttackNo], a
    wLoneAttackNo = a;
// ldh [hJoyPressed], a
    hJoyPressed = a;
// ldh [hJoyReleased], a
    hJoyReleased = a;
// ldh [hJoyHeld], a
    hJoyHeld = a;
// ld [wActionResultOrTookBattleTurn], a
    wActionResultOrTookBattleTurn = a;
// ld [wUnusedMapVariable], a
    wUnusedMapVariable = a;
// ld hl, wCardKeyDoorY
    hl = wCardKeyDoorY;
// ld [hli], a
    *hl++ = a;
// ld [hl], a
    *hl = a;
// ld hl, wWhichTrade
    hl = wWhichTrade;
// ld bc, wStandingOnWarpPadOrHole - wWhichTrade
    bc = wStandingOnWarpPadOrHole - wWhichTrade;
// call FillMemory
    FillMemory();
// ret
    return;
}
