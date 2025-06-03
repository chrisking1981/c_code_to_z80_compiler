# QUICK REFERENCE CARD
## C Code to Z80 Assembly Compiler Project

### 🚀 ESSENTIAL COMMANDS

```bash
# Full workflow (3 commands)
python asm_to_c_converter.py --emotion-bubbles --model gpt-4o-mini
python c_to_z80_compiler.py -f c_code/engine/overworld/emotion_bubbles.c  
python compare_assembly.py

# Check current quality status
cat comparison_results/summary_*.txt | tail -1

# List available models
python asm_to_c_converter.py --list-models
```

### 📊 CURRENT PROJECT STATUS (2025-06-03)
- **Overall Grade:** A (91.8%)
- **Perfect Files:** 2/5 (auto_movement, elevator)
- **Main Issues:** Graphics mapping, pointer table labels

### 🔧 IMMEDIATE FIXES NEEDED

| Issue | File | Fix Location | Priority |
|-------|------|--------------|----------|
| `dw ShockEmote_INCBIN` → `dw ShockEmote` | emotion_bubbles | c_to_z80_compiler.py | HIGH |
| `..loop` → `.loop` | emotion_bubbles | c_to_z80_compiler.py | MED |
| Missing EmotionBubbles label | emotion_bubbles | c_to_z80_compiler.py | HIGH |

### 🆘 EMERGENCY TROUBLESHOOTING

**API Issues:**
```bash
cat api_key.txt  # Check key exists
echo $OPENAI_API_KEY  # Check environment
```

**Conversion Fails:**
```bash
# Try different model
python asm_to_c_converter.py --emotion-bubbles --model gpt-4o
```

**Compilation Fails:**
```bash
# Check file exists
ls -la c_code/engine/overworld/emotion_bubbles.c
# Check output directory
mkdir -p compiled_asm/engine/overworld
```

### 📁 KEY FILES TO MONITOR
- `AI_DEVELOPMENT_HANDBOOK.md` - Complete guide
- `comparison_results/summary_*.txt` - Latest quality metrics
- `asm_naar_c_code_regels.md` - LLM conversion rules
- `C_TO_Z80_PATTERNS.md` - Compiler patterns

**Last Quality Check:** 81.2% (emotion_bubbles), 91.8% (overall) 