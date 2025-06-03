# ASM-to-C Pipeline Workflow

## 🔄 Complete Process Flow

### 📁 Project Structure
```
c_code_to_z80_compiler/
├── pokered/engine/overworld/     # Original ASM files (source)
├── c_code/engine/overworld/      # Generated C files (intermediate) 
├── compiled_asm/engine/overworld/ # Compiled ASM files (output)
├── comparison_results/           # Test results with timestamps
├── asm_to_c_converter.py        # ASM → C converter (LLM API)
├── c_to_z80_compiler.py         # C → ASM compiler  
├── compare_assembly.py          # ASM comparison & scoring
└── asm_naar_c_code_regels.md    # Conversion rules
```

## 🚀 Pipeline Steps

### Step 1: ASM → C Conversion (LLM API)
```bash
# Single file
python asm_to_c_converter.py --file pokered/engine/overworld/elevator.asm --model gpt-4o-mini

# Multiple files  
python asm_to_c_converter.py --files file1.asm file2.asm file3.asm --model gpt-4o-mini

# All files in directory
python asm_to_c_converter.py --dir pokered/engine/overworld/ --model gpt-4o-mini
```
**Output**: Creates C files in `c_code/engine/overworld/`

### Step 2: C → ASM Compilation
```bash  
# Single file
python c_to_z80_compiler.py --file c_code/engine/overworld/elevator.c

# Batch directory
python c_to_z80_compiler.py --input c_code --output compiled_asm
```
**Output**: Creates ASM files in `compiled_asm/engine/overworld/`

### Step 3: Comparison & Scoring
```bash
# Single file comparison
python compare_assembly.py pokered/engine/overworld/elevator.asm compiled_asm/engine/overworld/elevator.asm --detailed

# Batch directory comparison  
python compare_assembly.py pokered/engine/overworld/ compiled_asm/engine/overworld/ --detailed
```
**Output**: Similarity scores + detailed diff analysis in `comparison_results/`

## 📊 Complete Workflow Examples

### 🎯 Single File Test
```bash
# 1. Convert ASM to C
python asm_to_c_converter.py --file pokered/engine/overworld/elevator.asm --model gpt-4o-mini

# 2. Compile C to ASM  
python c_to_z80_compiler.py --file c_code/engine/overworld/elevator.c

# 3. Compare & score
python compare_assembly.py pokered/engine/overworld/elevator.asm compiled_asm/engine/overworld/elevator.asm --detailed
```

### 🎯 Batch Test (5 Files)
```bash
# 1. Convert all ASM files to C
python asm_to_c_converter.py --files pokered/engine/overworld/auto_movement.asm pokered/engine/overworld/elevator.asm pokered/engine/overworld/emotion_bubbles.asm pokered/engine/overworld/dust_smoke.asm pokered/engine/overworld/cut.asm --model gpt-4o-mini

# 2. Compile all C files to ASM
python c_to_z80_compiler.py --input c_code --output compiled_asm  

# 3. Compare all files and get overall score
python compare_assembly.py pokered/engine/overworld/ compiled_asm/engine/overworld/ --detailed
```

## 📈 Results Interpretation

### Similarity Scores
- **95-100%**: Grade A+ (Perfect/Near-perfect)
- **90-94%**: Grade A (Excellent)  
- **85-89%**: Grade B+ (Very good)
- **80-84%**: Grade B (Good)
- **75-79%**: Grade B- (Acceptable) 
- **70-74%**: Grade C+ (Fair)
- **65-69%**: Grade C (Poor)
- **<65%**: Grade D/F (Failed)

### Output Files
- **Results**: `comparison_results/filename/filename_score%_timestamp.txt`
- **Structure**: Organized by filename with score and timestamp
- **Analysis**: Detailed line-by-line differences for debugging

## 🔧 Model Selection

### OpenAI Models (--provider openai)
```bash
--model gpt-4o-mini    # Fast, cheap, good quality
--model gpt-4o         # Balanced performance  
--model gpt-4.1        # Best quality, expensive
--model o3-mini        # Reasoning model, faster
```

### Gemini Models (--provider gemini)  
```bash
--model gemini-2.0-flash      # Good balance
--model gemini-1.5-flash      # Most economical
--model gemini-2.5-flash-preview  # Best performance
```

## ⚡ Key Success Factors

1. **API Key Setup**: `api_key.txt` (OpenAI) or `gemini_api_key.txt` (Gemini)
2. **Model Selection**: gpt-4o-mini offers best speed/quality balance
3. **Conversion Rules**: Uses `asm_naar_c_code_regels.md` for consistent conversion
4. **Error Handling**: Pipeline stops if C compilation fails
5. **Quality Threshold**: Target 85%+ for production use

## 🎯 Expected Performance
- **Simple files**: 90-100% similarity
- **Complex files**: 80-95% similarity  
- **Average target**: 85%+ (Grade B+)
- **Perfect matches**: Achievable for simple ASM files 