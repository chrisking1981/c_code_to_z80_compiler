# AI DEVELOPMENT HANDBOOK
## C Code to Z80 Assembly Compiler Project

**Last Updated:** 2025-06-03  
**Project Status:** Grade A (91.8% average functional similarity)  
**Goal:** Byte-exact round-trip compilation: ASM → C → Z80 ASM

---

## 📋 PROJECT OVERVIEW

This project creates an automated toolchain to convert Game Boy Z80 assembly files to C code and back to assembly, maintaining byte-exact behavior for the pokered project.

### Core Components:
1. **`asm_to_c_converter.py`** - LLM-powered ASM to C conversion
2. **`c_to_z80_compiler.py`** - Automated C to Z80 assembly compilation  
3. **`compare_assembly.py`** - Quality analysis and progress tracking

### Success Metrics:
- **Target:** 95%+ functional similarity
- **Current:** 91.8% average (Grade A)
- **Perfect Files:** 2/5 (auto_movement.asm, elevator.asm)

---

## 🔄 COMPLETE DEVELOPMENT FLOW

### Phase 1: ASM → C Conversion
```bash
python asm_to_c_converter.py --emotion-bubbles --model gpt-4o-mini
```

**Input:** `pokered/engine/overworld/emotion_bubbles.asm`  
**Output:** `c_code/engine/overworld/emotion_bubbles.c`  
**Process:** LLM converts using specialized prompts and rules

### Phase 2: C → Z80 Compilation  
```bash
python c_to_z80_compiler.py -f c_code/engine/overworld/emotion_bubbles.c
```

**Input:** Generated C code  
**Output:** `compiled_asm/engine/overworld/emotion_bubbles.asm`  
**Process:** Pattern-based compiler reconstructs assembly

### Phase 3: Quality Analysis
```bash
python compare_assembly.py
```

**Process:** Compares original vs compiled, saves timestamped results  
**Output:** Detailed similarity scores and difference analysis

---

## 🎯 CURRENT PERFORMANCE STATUS

| File | Functional Similarity | Status | Key Issues |
|------|----------------------|--------|------------|
| auto_movement.asm | 100.0% | ✅ Perfect | None |
| elevator.asm | 100.0% | ✅ Perfect | None |
| dust_smoke.asm | 89.0% | 🔧 Good | Function pointer comments |
| cut.asm | 88.7% | 🔧 Good | Text section placement |
| emotion_bubbles.asm | 81.2% | 🔧 Good | Graphics section mapping |

**Overall Grade: A (91.8%)**

---

## 🔧 COMMON ISSUES & SOLUTIONS

### Issue 1: Graphics Sections Not Converting
**Symptoms:** Missing INCBIN statements, incomplete graphics data  
**Root Cause:** LLM skips graphics sections at end of ASM files  
**Solution:** Enhanced prompt with explicit graphics requirements

**Fix Applied:**
```markdown
=== KRITIEKE VEREISTE ===
CONVERTEER ALLE SECTIES IN DE ASM FILE - inclusief graphics data aan het einde!

Voor graphics secties zoals:
EmotionBubbles:
ShockEmote:    INCBIN "gfx/emotes/shock.2bpp"

Genereer C code als:
const uint8_t EmotionBubbles_MARKER = 0;
const uint8_t ShockEmote_INCBIN[] = { /* gfx/emotes/shock.2bpp */ };
```

### Issue 2: Pointer Table Label Mismatches
**Symptoms:** `dw ShockEmote_INCBIN` instead of `dw ShockEmote`  
**Root Cause:** C→Z80 compiler references wrong labels  
**Status:** IDENTIFIED - needs fix in `c_to_z80_compiler.py`

**Planned Fix:**
- Update pointer table to reference actual labels, not `_INCBIN` arrays
- Map `ShockEmote_INCBIN` → `ShockEmote` in output

### Issue 3: Local Labels Double Dots
**Symptoms:** `.loop` becomes `..loop`  
**Root Cause:** Label processing in C→Z80 compiler  
**Status:** IDENTIFIED - minor fix needed

### Issue 4: Text Section Placement
**Symptoms:** Text sections appear in wrong locations  
**Root Cause:** `jp PrintText` handling not perfect  
**Status:** PARTIALLY FIXED - some files still affected

---

## 🛠️ TROUBLESHOOTING GUIDE FOR AI ASSISTANTS

### When C Code Generation Fails:

1. **Check API Key Setup:**
   ```bash
   # Verify API key exists
   cat api_key.txt
   # Or check environment
   echo $OPENAI_API_KEY
   ```

2. **Review Conversion Rules:**
   - Check `asm_naar_c_code_regels.md` exists
   - Check `C_TO_Z80_PATTERNS.md` exists  
   - Ensure graphics conversion rules are present

3. **Model Selection Issues:**
   ```bash
   # List available models
   python asm_to_c_converter.py --list-models
   # Try different model if current fails
   python asm_to_c_converter.py --emotion-bubbles --model gpt-4o
   ```

### When C→Z80 Compilation Fails:

1. **Check Data Array Detection:**
   - Look for `const uint8_t`, `const uint16_t`, `const char` patterns
   - Verify `_INCBIN`, `_MARKER` naming conventions
   - Check regex patterns in `handle_data_array()`

2. **Debug Processing Steps:**
   - Add debug prints in `preprocess_lines()`
   - Check `processed` array content
   - Verify ASM comment priority over C statements

3. **File Path Issues:**
   - Ensure output directory structure exists
   - Check relative vs absolute path handling
   - Verify permissions on output directories

### When Quality Analysis Shows Problems:

1. **Run Individual Comparisons:**
   ```bash
   # Check specific file
   python compare_assembly.py | grep emotion_bubbles
   # Review detailed results
   cat comparison_results/emotion_bubbles_YYYYMMDD_HHMMSS.txt
   ```

2. **Pattern Analysis:**
   - Look for systematic issues across files
   - Check if problem is in ASM→C or C→Z80 phase
   - Review specific difference types

---

## 🎯 IMPROVEMENT ITERATION PROCESS

### Step 1: Identify Root Cause
```bash
# Run full analysis
python compare_assembly.py

# Examine specific failures  
cat comparison_results/summary_YYYYMMDD_HHMMSS.txt
```

### Step 2: Determine Fix Location
- **LLM Issues:** Update prompts in `asm_to_c_converter.py`
- **Compiler Issues:** Fix patterns in `c_to_z80_compiler.py`  
- **Mapping Issues:** Update conversion rules in `.md` files

### Step 3: Implement & Test
```bash
# Test single file after changes
python asm_to_c_converter.py --emotion-bubbles --model gpt-4o-mini
python c_to_z80_compiler.py -f c_code/engine/overworld/emotion_bubbles.c
python compare_assembly.py
```

### Step 4: Verify Improvement
- Compare new results with previous timestamps
- Ensure no regression in other files
- Update this handbook with lessons learned

---

## 📝 PROMPT ENGINEERING GUIDELINES

### Effective ASM→C Conversion Prompts:

1. **Be Explicit About Requirements:**
   - "CONVERTEER ALLE SECTIES" not "converteer de code"
   - Specify exact output format with examples
   - Use "BELANGRIJK:" for critical requirements

2. **Graphics Section Handling:**
   - Always include graphics examples in prompt
   - Specify `_MARKER` and `_INCBIN` conventions
   - Emphasize preserving ALL sections

3. **Model Selection:**
   - **gpt-4o-mini:** Fast, good for tested patterns
   - **gpt-4o:** Better for complex files  
   - **o3-mini:** Best reasoning for difficult cases

### Pattern Examples to Include:
```c
// For graphics:
const uint8_t EmotionBubbles_MARKER = 0;
const uint8_t ShockEmote_INCBIN[] = { /* gfx/emotes/shock.2bpp */ };

// For pointers:
const uint16_t EmotionBubblesPointerTable[] = {
    (uint16_t)ShockEmote,  // Not ShockEmote_INCBIN!
    -1 // Use -1, not 0xFF for end markers
};
```

---

## 🚀 SCALING TO FULL POKERED PROJECT

### Phase 1: Perfect Current Files (Target: 95%+)
- Fix emotion_bubbles.asm graphics mapping
- Resolve pointer table label references  
- Fix local label processing

### Phase 2: Batch Processing
```bash
# Convert entire engine/overworld directory
python asm_to_c_converter.py --dir pokered/engine/overworld --model gpt-4o-mini
python c_to_z80_compiler.py --input c_code --output compiled_asm
python compare_assembly.py
```

### Phase 3: Full Codebase
- Extend to all pokered directories
- Handle special cases (battle, menu, etc.)
- Optimize for cost and speed

---

## 📊 SUCCESS CRITERIA & EXIT CONDITIONS

### Quality Targets:
- **95%+ average functional similarity**
- **80%+ perfect matches**
- **No critical functionality differences**

### Automation Targets:
- **One-command full conversion**
- **Automatic quality regression detection**
- **Self-correcting prompt improvement**

### Documentation:
- **Complete troubleshooting coverage**
- **Reproducible development process**
- **Clear escalation paths for edge cases**

---

## 🔄 SELF-CORRECTION PROTOCOL FOR AI ASSISTANTS

### When Working on This Project:

1. **Always Run Quality Analysis First:**
   ```bash
   python compare_assembly.py
   ```

2. **Read Recent Results:**
   ```bash
   ls -la comparison_results/ | tail -5
   cat comparison_results/summary_*.txt | tail -1
   ```

3. **Before Making Changes:**
   - Backup current working state
   - Understand root cause from quality analysis
   - Choose targeted fix approach

4. **After Changes:**
   - Re-run full analysis
   - Compare with previous timestamp
   - Document improvement/regression

5. **If Stuck:**
   - Review this handbook thoroughly
   - Check for new edge cases not covered
   - Update handbook with new learnings

### Critical Files to Monitor:
- `asm_naar_c_code_regels.md` - Core conversion rules
- `C_TO_Z80_PATTERNS.md` - Compilation patterns
- Latest comparison results - Quality metrics

---

**Remember: This is an iterative process. Each change should improve overall quality while maintaining existing perfect matches.** 