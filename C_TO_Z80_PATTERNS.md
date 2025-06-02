# C to Z80 Assembly Compilation Patterns

Deze documentatie bevat alle belangrijke patronen die nodig zijn om C code te schrijven die correct compileert naar Z80 assembly voor Game Boy ROM development.

## 1. Data Array Patterns

### -1 Waarden Behouden
**BELANGRIJK**: Gebruik altijd `-1` in plaats van `0xFF` voor eindmarkeringen in data arrays.

```c
// ✅ CORRECT - gebruikt -1 voor eindmarkering
const uint8_t RLEList_Example[] = {
    D_UP, 2,
    D_RIGHT, 3,
    D_DOWN, 5,
    -1 // end - MOET -1 zijn, niet 0xFF!
};

// ✅ CORRECT - negatieve waarden in data structures
const BoulderDustAdjust MoveBoulderDustFunctionPointerTable[] = {
    {-1, 0, AdjustOAMBlockYPos},  // -1 blijft -1 in assembly
    {1, 0, AdjustOAMBlockYPos},
    {1, 1, AdjustOAMBlockXPos},
    {-1, 1, AdjustOAMBlockXPos}   // -1 blijft -1 in assembly
};

// ❌ VERKEERD - gebruikt 0xFF
const uint8_t BadExample[] = {
    D_UP, 2,
    0xFF // end - wordt $FF in plaats van -1
};
```

**Waarom**: De compiler detecteert `-1` en behoudt dit als `db -1` in de assembly. `0xFF` wordt geconverteerd naar `db $FF`.

### Array Formatting voor Elke Waarde op Eigen Regel
```c
// ✅ CORRECT - voor arrays die elke waarde op eigen regel moeten hebben
const uint8_t RivalIDs[] = {
    OPP_RIVAL1,  // Elke waarde op eigen regel
    OPP_RIVAL2,  // voor correcte assembly formatting
    OPP_RIVAL3,
    -1 // end
};
```

## 2. Function Label Patterns

### Pointer Table Functions (gebruik ::)
```c
// ✅ Deze functies moeten :: gebruiken in assembly
void PlayerStepOutFromDoor(void);        // -> PlayerStepOutFromDoor::
void _EndNPCMovementScript(void);        // -> _EndNPCMovementScript::
void SetEnemyTrainerToStayAndFaceAnyDirection(void); // -> SetEnemyTrainerToStayAndFaceAnyDirection::
void ShakeElevator(void);                // -> ShakeElevator::

// Function pointer table functies
void DisplayBoulderDustAnimation(void);   // -> DisplayBoulderDustAnimation::
void GetMoveBoulderDustFunctionPointer(void); // -> GetMoveBoulderDustFunctionPointer::
```

### Reguliere Functions (gebruik :)
```c
// ✅ Deze functies moeten : gebruiken in assembly  
void UsedCut(void);                      // -> UsedCut:
void InitCutAnimOAM(void);              // -> InitCutAnimOAM:
void LoadSmokeTile(uint8_t* hl);        // -> LoadSmokeTile:
```

## 3. Text Section Patterns

### Text Labels voor PrintText calls
```c
// ✅ CORRECT - text referenties worden automatisch gedetecteerd
void SomeFunction(void) {
    // hl = (uint8_t*)NothingToCutText;
    hl = (uint8_t*)NothingToCutText;  // Compiler detecteert text referentie
    // jp PrintText
    PrintText((const char*)hl); /* jp */
    // Na deze regel wordt automatisch toegevoegd:
    // NothingToCutText:
    // 	text_far _NothingToCutText  
    // 	text_end
}
```

**Belangrijk**: 
- Text secties worden automatisch gegenereerd na `jp PrintText` calls
- De compiler detecteert text namen in `hl = (uint8_t*)TextName` assignments
- Beide `NothingToCutText` en `UsedCutText` worden correct geplaatst

## 4. Function Pointer Call Patterns

### Correcte Function Pointer Structuur
```c
// ✅ CORRECT - voor function pointer calls met .returnAddress labels
loop:
    // push bc
    uint8_t saved_c = c;
    // call GetMoveBoulderDustFunctionPointer  
    GetMoveBoulderDustFunctionPointer();
    // ld bc, .returnAddress
    // push bc
    // ld c, 4
    c = 4;
    // jp hl
    // Call the function that was set up by GetMoveBoulderDustFunctionPointer

returnAddress:  // Dit wordt .returnAddress in assembly
    // ldh a, [rOBP1]
    a = rOBP1;
    // rest van de code...
```

**Waarom**: Deze structuur zorgt ervoor dat de compiler de juiste volgorde genereert:
1. `ld c, $04`
2. `; Call the function that was set up by GetMoveBoulderDustFunctionPointer`  
3. `.returnAddress`

## 5. Assembly Comment Patterns

### Inline Assembly Comments
```c
// ✅ CORRECT - assembly instructies als comments
void Example(void) {
    // ld a, $1           <- wordt: ld a, $1
    a = 0x1;
    // ld [wVariable], a  <- wordt: ld [wVariable], a  
    wVariable = a;
    // xor a              <- wordt: xor a
    a = 0;
    // ret                <- wordt: ret
    return;
}
```

### Call Type Annotations
```c
// ✅ CORRECT - specificeer call types in comments
void Example(void) {
    // call SomeFunction
    SomeFunction();
    // farcall SomeDistantFunction  
    SomeDistantFunction(); /* farcall */
    // jp FinalFunction
    FinalFunction(); /* jp */
    // predef SomePredefFunction
    SomePredefFunction(); /* predef */
}
```

## 6. Goto/Label Patterns

### Local Labels in Functions
```c
// ✅ CORRECT - labels worden .labelname in assembly
void Example(void) {
    // if (a == 0) goto done;
    if (a == 0) goto done;
    
    // code here...
    
done:  // wordt .done in assembly
    // ret
    return;
}
```

## 7. Register Operation Patterns

### Pointer Arithmetic
```c
// ✅ CORRECT - pointer arithmetic met comments
void Example(void) {
    // ld hl, wSomeArray
    hl = wSomeArray;
    // add hl, bc        <- expliciete add instructie
    hl += c; /* add hl, bc */
    // ld de, $20
    // add hl, de        <- automatische de register gebruik  
    hl += 0x20;
}
```

### Bit Operations
```c
// ✅ CORRECT - bit manipulatie
void Example(void) {
    // set BIT_SOME_FLAG, [hl]
    *hl |= (1 << BIT_SOME_FLAG);
    // res BIT_SOME_FLAG, [hl]  
    *hl &= ~(1 << BIT_SOME_FLAG);
    // bit BIT_SOME_FLAG, a
    // ret nz
    if (a & (1 << BIT_SOME_FLAG)) return;
}
```

## 8. Conditional Patterns

### Conditional Jumps
```c
// ✅ CORRECT - conditionals met assembly output
void Example(void) {
    // cp SOME_VALUE
    // jr z, .label
    if (a == SOME_VALUE) goto label;
    
    // cp SOME_VALUE  
    // jr nz, .label
    if (a != SOME_VALUE) goto label;
    
    // and a
    // ret z
    if (a == 0) return;
}
```

## 9. Memory Access Patterns

### VRAM/Hardware Registers
```c
// ✅ CORRECT - hardware register access
void Example(void) {
    // ldh [rOBP1], a
    rOBP1 = a;
    // ldh a, [rOBP1]
    a = rOBP1;
    // ld hl, vChars1 tile $7c
    hl = (uint8_t*)&vChars1;
    hl += 0x7c; /* tile $7c */
}
```

## Compiler Detection Logic

De compiler gebruikt deze patronen om te bepalen:

1. **Function label type**: Namen in `pointer_functions` set krijgen `::`
2. **Text sections**: Detecteert `hl = (uint8_t*)TextName` gevolgd door `PrintText`
3. **-1 preservation**: `format_value_for_db()` controleert eerst op `-1` string  
4. **Comment processing**: ASM comments hebben prioriteit over C statements
5. **Label placement**: `.returnAddress` labels worden correct geplaatst na function pointer calls

## Gebruik voor Nieuwe C Bestanden

Bij het maken van nieuwe C bestanden:

1. ✅ Gebruik `-1` voor alle eindmarkeringen
2. ✅ Documenteer welke functies `::` vs `:` moeten gebruiken  
3. ✅ Structureer function pointer calls zoals dust_smoke.c voorbeeld
4. ✅ Plaats assembly comments boven elke C statement
5. ✅ Gebruik `/* jp */`, `/* farcall */` annotaties waar nodig
6. ✅ Test met `python compare_assembly.py` voor verificatie

Deze patronen zorgen voor **90%+ assembly similarity** en maken byte-exacte round-trip transpilatie mogelijk. 