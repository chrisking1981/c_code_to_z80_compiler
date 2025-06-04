void PrepareForSpecialWarp(void) {
PrepareForSpecialWarp:
// call LoadSpecialWarpData
    LoadSpecialWarpData();
// predef LoadTilesetHeader
// ld hl, wStatusFlags6
    hl = wStatusFlags6;
// bit BIT_FLY_OR_DUNGEON_WARP, [hl]
// res BIT_FLY_OR_DUNGEON_WARP, [hl]
// jr z, .debugNewGameWarp
// ld a, [wDestinationMap]
    a = wDestinationMap;
// jr .next
// .debugNewGameWarp
// bit BIT_DEBUG_MODE, [hl]
// jr z, .setNewGameMatWarp ; apply to StartNewGameDebug only
// call PrepareNewGameDebug
    PrepareNewGameDebug();
// .setNewGameMatWarp
// This is called by OakSpeech during StartNewGame and
// loads the first warp event for the specified map index.
// ld a, PALLET_TOWN
    a = PALLET_TOWN;
// .next
// ld b, a
// ld a, [wStatusFlags3]
    a = wStatusFlags3;
// and a ; ???
// jr nz, .next2
// ld a, b
    a = b;
// .next2
// ld hl, wStatusFlags6
    hl = wStatusFlags6;
// bit BIT_DUNGEON_WARP, [hl]
// ret nz
    return;
// ld [wLastMap], a
    wLastMap = a;
// ret
    return;
//
LoadSpecialWarpData:
// ld a, [wCableClubDestinationMap]
    a = wCableClubDestinationMap;
// cp TRADE_CENTER
// jr nz, .notTradeCenter
// ld hl, TradeCenterPlayerWarp
    hl = TradeCenterPlayerWarp;
// ldh a, [hSerialConnectionStatus]
    a = hSerialConnectionStatus;
// cp USING_INTERNAL_CLOCK
// jr z, .copyWarpData
// ld hl, TradeCenterFriendWarp
    hl = TradeCenterFriendWarp;
// jr .copyWarpData
// .notTradeCenter
// cp COLOSSEUM
// jr nz, .notColosseum
// ld hl, ColosseumPlayerWarp
    hl = ColosseumPlayerWarp;
// ldh a, [hSerialConnectionStatus]
    a = hSerialConnectionStatus;
// cp USING_INTERNAL_CLOCK
// jr z, .copyWarpData
// ld hl, ColosseumFriendWarp
    hl = ColosseumFriendWarp;
// jr .copyWarpData
// .notColosseum
// ld a, [wStatusFlags6]
    a = wStatusFlags6;
// bit BIT_DEBUG_MODE, a
// warp to wLastMap (PALLET_TOWN) for StartNewGameDebug
// jr nz, .notNewGameWarp
// bit BIT_FLY_OR_DUNGEON_WARP, a
// jr nz, .notNewGameWarp
// ld hl, NewGameWarp
    hl = NewGameWarp;
// .copyWarpData
// ld de, wCurMap
    de = wCurMap;
// ld c, $7
// .copyWarpDataLoop
// ld a, [hli]
    a = hli;
// ld [de], a
    de = a;
// inc de
// dec c
// jr nz, .copyWarpDataLoop
// ld a, [hli]
    a = hli;
// ld [wCurMapTileset], a
    wCurMapTileset = a;
// xor a
    a = 0;
// jr .done
// .notNewGameWarp
// ld a, [wLastMap] ; this value is overwritten before it's ever read
    a = wLastMap;
// ld hl, wStatusFlags6
    hl = wStatusFlags6;
// bit BIT_DUNGEON_WARP, [hl]
// jr nz, .usedDungeonWarp
// bit BIT_ESCAPE_WARP, [hl]
// res BIT_ESCAPE_WARP, [hl]
// jr z, .otherDestination
// ld a, [wLastBlackoutMap]
    a = wLastBlackoutMap;
// jr .usedFlyWarp
// .usedDungeonWarp
// ld hl, wStatusFlags3
    hl = wStatusFlags3;
// res BIT_ON_DUNGEON_WARP, [hl]
// ld a, [wDungeonWarpDestinationMap]
    a = wDungeonWarpDestinationMap;
// ld b, a
// ld [wCurMap], a
    wCurMap = a;
// ld a, [wWhichDungeonWarp]
    a = wWhichDungeonWarp;
// ld c, a
// ld hl, DungeonWarpList
    hl = DungeonWarpList;
// ld de, 0
    de = 0;
// ld a, 6
    a = 6;
// ld [wDungeonWarpDataEntrySize], a
    wDungeonWarpDataEntrySize = a;
// .dungeonWarpListLoop
// ld a, [hli]
    a = hli;
// cp b
// jr z, .matchedDungeonWarpDestinationMap
// inc hl
// jr .nextDungeonWarp
// .matchedDungeonWarpDestinationMap
// ld a, [hli]
    a = hli;
// cp c
// jr z, .matchedDungeonWarpID
// .nextDungeonWarp
// ld a, [wDungeonWarpDataEntrySize]
    a = wDungeonWarpDataEntrySize;
// add e
// ld e, a
// jr .dungeonWarpListLoop
// .matchedDungeonWarpID
// ld hl, DungeonWarpData
    hl = DungeonWarpData;
// add hl, de
// jr .copyWarpData2
// .otherDestination
// ld a, [wDestinationMap]
    a = wDestinationMap;
// .usedFlyWarp
// ld b, a
// ld [wCurMap], a
    wCurMap = a;
// ld hl, FlyWarpDataPtr
    hl = FlyWarpDataPtr;
// .flyWarpDataPtrLoop
// ld a, [hli]
    a = hli;
// inc hl
// cp b
// jr z, .foundFlyWarpMatch
// inc hl
// inc hl
// jr .flyWarpDataPtrLoop
// .foundFlyWarpMatch
// ld a, [hli]
    a = hli;
// ld h, [hl]
// ld l, a
// .copyWarpData2
// ld de, wCurrentTileBlockMapViewPointer
    de = wCurrentTileBlockMapViewPointer;
// ld c, $6
// .copyWarpDataLoop2
// ld a, [hli]
    a = hli;
// ld [de], a
    de = a;
// inc de
// dec c
// jr nz, .copyWarpDataLoop2
// xor a ; OVERWORLD
    a = 0;
// ld [wCurMapTileset], a
    wCurMapTileset = a;
// .done
// ld [wYOffsetSinceLastSpecialWarp], a
    wYOffsetSinceLastSpecialWarp = a;
// ld [wXOffsetSinceLastSpecialWarp], a
    wXOffsetSinceLastSpecialWarp = a;
// ld a, -1 ; exclude normal warps
    a = -1 ; exclude normal warps;
// ld [wDestinationWarpID], a
    wDestinationWarpID = a;
// ret
    return;
//
// INCLUDE "data/maps/special_warps.asm"
}
