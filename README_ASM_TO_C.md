# ASM to C Code Converter

Automatische conversie van Game Boy Z80 assembly bestanden naar C code via LLM API calls. Gebruikt alle ontdekte regels en patronen voor byte-exacte round-trip compilatie.

## 🚀 Features

- **Automatische conversie** van ASM naar C via LLM API
- **API key uit bestand** - Eenvoudige setup via `api_key.txt`
- **Nieuwste 2025 models** - o3, GPT-4.1, GPT-4o en meer
- **Ingebouwde regels** uit `asm_naar_c_code_regels.md`
- **Compilatie patronen** uit `C_TO_Z80_PATTERNS.md`
- **Batch processing** voor hele directories
- **Clean output** met automatische formatting
- **Error handling** en validatie

## 📋 Vereisten

```bash
pip install -r requirements.txt
```

## 🔑 API Key Setup

### Methode 1: api_key.txt bestand (Aanbevolen)
```bash
# Maak het template bestand
python asm_to_c_converter.py --create-key-file

# Bewerk api_key.txt en plak je API key
# sk-proj-abcd1234...
```

### Methode 2: Environment Variable
```bash
export OPENAI_API_KEY='sk-proj-your-api-key-here'
```

### Methode 3: Command Line
```bash
python asm_to_c_converter.py --api-key sk-proj-your-key-here --emotion-bubbles
```

🔑 **Get API key**: https://platform.openai.com/api-keys

## 🤖 Model Selection

### Bekijk beschikbare models:
```bash
python asm_to_c_converter.py --list-models
```

### 2025 Models beschikbaar:
- **o3** - Meest geavanceerde reasoning model
- **o3-mini** - Snellere reasoning model  
- **gpt-4.1** - Nieuwste met 1M context window
- **gpt-4.1-mini** - Sneller en goedkoper
- **gpt-4o** - Geoptimaliseerd multimodaal model (default)
- **gpt-4o-mini** - Snelste en goedkoopste
- **gpt-3.5-turbo** - Meest economische optie

## 🎯 Gebruik

### Emotion Bubbles Conversie (Voorbeeld)
```bash
# Basis conversie met default model (gpt-4o)
python asm_to_c_converter.py --emotion-bubbles

# Met specifiek model
python asm_to_c_converter.py --emotion-bubbles --model gpt-4.1

# Met reasoning model voor complexe code
python asm_to_c_converter.py --emotion-bubbles --model o3-mini
```

### Enkel Bestand Conversie
```bash
# Converteer één ASM bestand
python asm_to_c_converter.py --file pokered/engine/overworld/some_file.asm

# Met premium model
python asm_to_c_converter.py --file input.asm --model gpt-4.1 --output my_c_code

# Goedkope optie voor eenvoudige bestanden
python asm_to_c_converter.py --file simple.asm --model gpt-3.5-turbo
```

### Directory Batch Conversie
```bash
# Converteer alle ASM bestanden in directory
python asm_to_c_converter.py --dir pokered/engine

# Met specifiek model voor de hele batch
python asm_to_c_converter.py --dir pokered/ --model o3-mini --output converted_c_code

# Budget-vriendelijk voor grote batches
python asm_to_c_converter.py --dir pokered/engine --model gpt-4o-mini
```

### Advanced Options
```bash
# Custom API endpoint (voor andere LLM providers)
python asm_to_c_converter.py --emotion-bubbles --api-url https://api.anthropic.com/...

# Bekijk alle opties
python asm_to_c_converter.py --help
```

## 📁 Output Structuur

```
c_code/
├── engine/
│   └── overworld/
│       ├── emotion_bubbles.c  ← Nieuw gegenereerd
│       ├── auto_movement.c    ← Bestaand
│       ├── dust_smoke.c       ← Bestaand
│       ├── elevator.c         ← Bestaand
│       └── cut.c              ← Bestaand
```

## 🎛️ Ingebouwde Regels

Het script gebruikt automatisch alle regels uit:

### **asm_naar_c_code_regels.md**
- Spiegel elke ASM-regel exact
- Behoud register-flow en side-effects
- Gebruik pokered symbolische labels
- Simuleer Z80 instructies correct

### **C_TO_Z80_PATTERNS.md**
- Gebruik `-1` in plaats van `0xFF` voor eindmarkeringen
- Correcte function label formatting (`::` vs `:`)
- Function pointer call patronen
- Text section automatische detectie
- Assembly comment prioriteit

## 🔧 Voorbeeld Conversie

**Input ASM:**
```asm
EmotionBubble:
	ld a, [wWhichEmotionBubble]
	ld c, a
	ld b, 0
	ld hl, EmotionBubblesPointerTable
	add hl, bc
	add hl, bc
	ret
```

**Generated C:**
```c
void EmotionBubble(void) {
    // ld a, [wWhichEmotionBubble]
    a = wWhichEmotionBubble;
    // ld c, a
    c = a;
    // ld b, 0
    b = 0;
    // ld hl, EmotionBubblesPointerTable
    hl = (uint8_t*)EmotionBubblesPointerTable;
    // add hl, bc
    hl += c; /* add hl, bc */
    // add hl, bc  
    hl += c; /* add hl, bc */
    // ret
    return;
}
```

## 🧪 Testing

Na conversie kun je de resultaten testen:

```bash
# Test de gegenereerde C code
python c_to_z80_compiler.py --input c_code

# Vergelijk met originele assembly
python compare_assembly.py
```

## 💰 Cost Optimization

### Model Keuze per Use Case:

**🏆 Premium Quality (Dure, beste resultaten)**
- `o3` - Voor zeer complexe assembly code
- `gpt-4.1` - Voor belangrijke productie bestanden

**⚖️ Balanced (Goede kwaliteit, redelijke prijs)**
- `o3-mini` - Voor complexe logica met reasoning
- `gpt-4o` - Default keuze, goede balans (aanbevolen)

**💸 Budget-Friendly (Goedkoop, snelle resultaten)**
- `gpt-4o-mini` - Voor eenvoudige assembly bestanden
- `gpt-3.5-turbo` - Voor prototype/test conversies

### Batch Processing Tips:
```bash
# Test eerst met één bestand
python asm_to_c_converter.py --file test.asm --model gpt-4o-mini

# Als resultaat goed is, gebruik voor hele directory
python asm_to_c_converter.py --dir pokered/engine --model gpt-4o-mini
```

## 🛠️ Extensibiliteit

### Eigen LLM Provider
```python
converter = ASMToCConverter(
    api_url="https://api.your-provider.com/v1/chat/completions"
)
```

### Custom Regels Toevoegen
1. Bewerk `asm_naar_c_code_regels.md`
2. Update `C_TO_Z80_PATTERNS.md`
3. Script laadt automatisch nieuwe regels

### Batch Processing Script
```python
from asm_to_c_converter import ASMToCConverter

converter = ASMToCConverter()
converter.convert_directory("pokered/engine", model="gpt-4o")
```

## ⚠️ Belangrijke Notes

1. **API Costs**: Test eerst met goedkope models (gpt-4o-mini, gpt-3.5-turbo)
2. **Rate Limits**: OpenAI heeft rate limits - respecteer deze bij batch processing
3. **Model Selection**: Kies model gebaseerd op complexiteit van de assembly code
4. **Validation**: Controleer altijd gegenereerde C code voor correctheid
5. **API Key Security**: Gebruik `api_key.txt` en deel dit bestand nooit

## 🚀 Quick Start

```bash
# 1. Setup
python asm_to_c_converter.py --create-key-file
# (Edit api_key.txt with your OpenAI API key)

# 2. List models
python asm_to_c_converter.py --list-models

# 3. Test conversion
python asm_to_c_converter.py --emotion-bubbles --model gpt-4o-mini

# 4. Verify results
python c_to_z80_compiler.py --input c_code
python compare_assembly.py
```

## 🎯 Doelen

- **94%+ Assembly Similarity** na round-trip compilatie
- **Byte-exact checksum** matching voor Game Boy ROMs
- **Cost-effective** conversie met model keuze flexibiliteit
- **Herbruikbaar** voor alle pokered assembly bestanden
- **Schaalbaar** voor andere Game Boy projecten

## 🤝 Contributing

Voor nieuwe patronen en regels:
1. Test conversie met `asm_to_c_converter.py --model gpt-4o-mini`
2. Test round-trip met `c_to_z80_compiler.py`
3. Update documentatie in `C_TO_Z80_PATTERNS.md`
4. Valideer met `compare_assembly.py`

Perfect voor team-gebruik en automatisering van Game Boy ROM development! 🎮 