#!/usr/bin/env python3
"""
ASM to C Code Converter
Automatically converts Game Boy Z80 assembly files to C code using LLM API calls.
Supports both OpenAI and Google Gemini models with the same conversion prompt.
Follows strict conversion rules for byte-exact round-trip compilation.
"""

import os
import re
import argparse
from pathlib import Path
import requests
import json
from typing import Optional, Dict, Any

# Available OpenAI models (2025)
AVAILABLE_OPENAI_MODELS = {
    # Latest reasoning models (2025)
    'o3': 'OpenAI o3 - Most advanced reasoning model',
    'o3-mini': 'OpenAI o3-mini - Faster reasoning model', 
    'o1': 'OpenAI o1 - Advanced reasoning model',
    'o1-mini': 'OpenAI o1-mini - Faster reasoning',
    'o1-pro': 'OpenAI o1-pro - Professional reasoning',
    
    # Latest GPT models (2025)
    'gpt-4.1': 'GPT-4.1 - Latest with 1M context window',
    'gpt-4.1-mini': 'GPT-4.1 Mini - Faster and cheaper',
    'gpt-4.1-nano': 'GPT-4.1 Nano - Ultra fast',
    'gpt-4.5': 'GPT-4.5 Preview - Next generation',
    'gpt-4o': 'GPT-4o - Optimized multimodal model',
    'gpt-4o-mini': 'GPT-4o Mini - Faster GPT-4o',
    
    # Reliable older models
    'gpt-4-turbo': 'GPT-4 Turbo - Previous generation',
    'gpt-3.5-turbo': 'GPT-3.5 Turbo - Fast and economical'
}

# Available Google Gemini models (2025)
AVAILABLE_GEMINI_MODELS = {
    # Latest Gemini 2.5 models (Preview)
    'gemini-2.5-flash-preview': 'Gemini 2.5 Flash Preview - Best for fast performance on complex tasks',
    'gemini-2.5-pro-preview': 'Gemini 2.5 Pro Preview - Most advanced reasoning model',
    
    # Gemini 2.0 models (General Availability)
    'gemini-2.0-flash': 'Gemini 2.0 Flash - Next-gen features with superior speed',
    'gemini-2.0-flash-lite': 'Gemini 2.0 Flash-Lite - Cost-efficient performance',
    
    # Gemini 1.5 models (Stable)
    'gemini-1.5-flash': 'Gemini 1.5 Flash - Fast and versatile multimodal model',
    'gemini-1.5-flash-8b': 'Gemini 1.5 Flash-8B - Smaller model for high volume tasks',
    'gemini-1.5-pro': 'Gemini 1.5 Pro - Complex reasoning tasks',
}

class ASMToCConverter:
    def __init__(self, api_key: Optional[str] = None, api_url: Optional[str] = None, provider: str = "openai"):
        self.provider = provider.lower()
        self.api_key = api_key or self.load_api_key()
        
        # Set default API URLs based on provider
        if self.provider == "gemini":
            self.api_url = api_url or "https://generativelanguage.googleapis.com/v1beta/models"
        else:  # openai
            self.api_url = api_url or "https://api.openai.com/v1/chat/completions"
        
        self.output_dir = "c_code"
        
        # Load conversion rules and patterns
        self.load_conversion_rules()
        self.load_c_patterns()
    
    def load_api_key(self) -> Optional[str]:
        """Load API key from multiple sources in order of preference"""
        if self.provider == "gemini":
            # Try Gemini-specific sources first
            # 1. Try from gemini_api_key.txt file
            try:
                with open('gemini_api_key.txt', 'r', encoding='utf-8') as f:
                    key = f.read().strip()
                    if key:
                        print("✅ Gemini API key loaded from gemini_api_key.txt")
                        return key
            except FileNotFoundError:
                pass
            
            # 2. Try from environment variable
            env_key = os.getenv('GEMINI_API_KEY')
            if env_key:
                print("✅ Gemini API key loaded from environment variable")
                return env_key
        
        else:  # OpenAI
            # 1. Try from api_key.txt file
            try:
                with open('api_key.txt', 'r', encoding='utf-8') as f:
                    key = f.read().strip()
                    if key:
                        print("✅ OpenAI API key loaded from api_key.txt")
                        return key
            except FileNotFoundError:
                pass
            
            # 2. Try from environment variable
            env_key = os.getenv('OPENAI_API_KEY')
            if env_key:
                print("✅ OpenAI API key loaded from environment variable")
                return env_key
        
        # 3. No key found
        return None
    
    def load_conversion_rules(self):
        """Load ASM to C conversion rules"""
        try:
            with open('asm_naar_c_code_regels.md', 'r', encoding='utf-8') as f:
                self.conversion_rules = f.read()
        except FileNotFoundError:
            self.conversion_rules = """
Spiegel elke ASM-regel – behoud register-flow, side-effects en volgorde (geen optimalisaties).
Gebruik dezelfde symbolische labels/variabelen als in pokered.
Vervang xor a door uint8_t a = 0; en behoud store-instructies.
Simuleer swap a met bit-shifts ( (x<<4)|(x>>4) ).
Voor bit-instructies: res BIT_X, [hl] → flags &= ~(1 << BIT_X);
Roep subroutines op met exact dezelfde naam.
Voeg beknopte inline comments toe voor elke ASM-regel.
Geen extra logica of optimalisaties.

SPECIALE INSTRUCTIES VOOR LOCAL LABELS:
Local labels beginnen met een punt (.labelname) en zijn GEEN aparte functies!
Converteer deze naar goto labels binnen dezelfde functie:

ASM:
  call .someLabel
  ret z
.someLabel
  add a
  ret

C:
  goto someLabel;
  if (a == 0) return;
someLabel:
  a += b;
  return;

BELANGRIJK: 
- .labelname → gewoon labelname: (zonder punt)
- call .labelname → goto labelname;
- Geen aparte functie definities maken!
- Alle code blijft in dezelfde functie

SPECIALE INSTRUCTIES VOOR GRAPHICS SECTIES:
Voor INCBIN graphics data zoals:
EmotionBubbles:
ShockEmote:    INCBIN "gfx/emotes/shock.2bpp"
QuestionEmote: INCBIN "gfx/emotes/question.2bpp"
HappyEmote:    INCBIN "gfx/emotes/happy.2bpp"

Genereer C code als:
// EmotionBubbles:
const uint8_t EmotionBubbles_MARKER = 0;

// ShockEmote: INCBIN "gfx/emotes/shock.2bpp"
const uint8_t ShockEmote_INCBIN[] = { /* gfx/emotes/shock.2bpp */ };

// QuestionEmote: INCBIN "gfx/emotes/question.2bpp"
const uint8_t QuestionEmote_INCBIN[] = { /* gfx/emotes/question.2bpp */ };

// HappyEmote: INCBIN "gfx/emotes/happy.2bpp"
const uint8_t HappyEmote_INCBIN[] = { /* gfx/emotes/happy.2bpp */ };

BELANGRIJK:
- Gebruik _MARKER voor label-only definities
- Gebruik _INCBIN[] voor graphics data met comment die het pad bevat
- Geen string literals voor graphics data
- Geen extern declarations voor graphics labels"""
    
    def load_c_patterns(self):
        """Load C to Z80 compilation patterns"""
        try:
            with open('C_TO_Z80_PATTERNS.md', 'r', encoding='utf-8') as f:
                self.c_patterns = f.read()
        except FileNotFoundError:
            self.c_patterns = """
BELANGRIJK voor correcte compilatie:
- Gebruik -1 in plaats van 0xFF voor eindmarkeringen
- Function pointer calls moeten specifieke structuur hebben
- Text sections worden automatisch gedetecteerd na jp PrintText
- Assembly comments hebben prioriteit over C statements
"""
    
    def create_conversion_prompt(self, asm_content: str, filename: str) -> str:
        """Create the LLM prompt for ASM to C conversion"""
        
        # ANALYZE ASM CONTENT TO IDENTIFY FIRST FUNCTION
        lines = asm_content.split('\n')
        first_function = None
        for line in lines:
            line = line.strip()
            if line and line.endswith(':') and not line.startswith('.') and not line.startswith(';'):
                # Skip data tables and arrays - look for actual functions
                if not any(keyword in line.lower() for keyword in ['table', 'list', 'data', 'offsets', 'block']):
                    if any(keyword in line.lower() for keyword in ['function', 'script', 'used', 'player', 'move', 'init', 'animate', 'shake', 'display']):
                        first_function = line.replace(':', '')
                        break
                # If no clear function keywords, take first non-data label
                elif not first_function and len([c for c in line if c.isupper()]) > 0:
                    first_function = line.replace(':', '')
                    break
        
        example_block = '''
// Functie met local label
void MyFunction(void) {
    // ... code ...
    goto label_loop;
..label_loop:
    // ... code ...
}

// Pointer table
MyPointerTable::
dw MyFunction

dw AndereFunctie

// Gewone label
MyLabel:
    // ... code ...
'''
        prompt = f"""Je bent een expert Game Boy assembly naar C code converter voor het pokered project.

DOEL: Converteer de volgende Z80 assembly code naar functionele C code die byte-voor-byte identiek gedrag oplevert.

=== 🚨 CRITICAL STRUCTURE & LABEL RULES 🚨 ===
1. **FUNCTIE VOLGORDE**: Begin ALTIJD met de eerste functie, nooit met pointer tables/data arrays!
2. **LABELS**:
   - Local labels: `.label` in ASM → `..label` in ASM output (dubbele puntjes)
   - Pointer table labels: `Label:` in ASM → `Label::` in ASM output (dubbele dubbele-punt)
   - Gewone labels: `Label:` in ASM → `Label:` in ASM output
3. **POINTER TABLES**:
   - Gebruik `dw FunctionName` (zonder &)
   - Pointer table labels ALTIJD met dubbele dubbele-punt (`Label::`)
4. **DATA ARRAYS**: Altijd NA functies
5. **ALLE LABELS MOETEN CORRECT GEFORMAAT ZIJN!**

=== VOORBEELDSTRUCTUUR ===
{example_block}

=== CONVERSION RULES ===
{self.conversion_rules}

=== C CODE PATTERNS ===  
{self.c_patterns}

=== SPECIFIEKE EISEN ===
1. **🎯 FUNCTIE VOLGORDE**: Begin met de eerste functie uit de ASM, NOOIT met data!
2. **🎯 COMPLETENESS**: Converteer ALLE {len(asm_content.split(chr(10)))} lijnen - laat NIETS weg!
3. **POINTER FORMAT**: Gebruik `dw FunctionName` NIET `dw &FunctionName`  
4. **DATA ARRAYS**: Gebruik ALTIJD -1 in plaats van 0xFF voor eindmarkeringen
5. **LABELS**: Local labels: .label → ..label; Pointer tables: Label: → Label::
6. **COMMENTS**: Behoud alle assembly comments als // comments boven elke C regel
7. **ANNOTATIONS**: Voor PrintText calls: gebruik /* jp */ annotatie
8. **FARCALL**: Voor farcall: gebruik /* farcall */ annotatie  
9. **PREDEF**: Voor predef: gebruik /* predef */ annotatie
10. **LOCAL LABELS**: .labelname wordt goto labelname; binnen dezelfde functie

=== CRITICAL STRUCTURE REQUIREMENTS ===
**ALWAYS start with the FIRST FUNCTION you see in the ASM file!**
**NEVER start with pointer tables, data arrays, or constants!**
**CONVERT EVERYTHING - this file has {len(asm_content.split(chr(10)))} lines of ASM code!**

=== ASM BESTAND: {filename} ===
HET BESTAND HEEFT {len(asm_content.split(chr(10)))} LIJNEN - CONVERTEER ALLES!

```asm
{asm_content}
```

=== GEWENSTE OUTPUT ===
Genereer de COMPLETE C code met:
1. **Alle benodigde #include statements**
2. **Extern declarations voor variabelen/functies**  
3. **Constanten/defines**
4. **🎯 FUNCTIONS FIRST (exact ASM order) - BEGIN MET {first_function if first_function else "EERSTE FUNCTIE"}**
5. **🎯 DATA ARRAYS LAST (exact ASM order)**
6. **ALLE {len(asm_content.split(chr(10)))} lijnen van ASM geconverteerd**
7. **Gebruik -1 voor array eindmarkeringen**
8. **Geen & voor function pointers**
9. **ALLE LABELS MOETEN CORRECT GEFORMAAT ZIJN!**

ABSOLUTE VEREISTE: 
- Begin met {f"void {first_function}(void)" if first_function else "de EERSTE FUNCTIE"}
- Converteer ALLE {len(asm_content.split(chr(10)))} lijnen ASM code
- Respecteer de exacte volgorde van functies en data
- LAAT NIETS WEG - het origineel heeft {len(asm_content.split(chr(10)))} lijnen!
- STOP PAS wanneer ALLE ASM content is geconverteerd

⚠️ WAARSCHUWING: Als je stopt voordat alle {len(asm_content.split(chr(10)))} lijnen zijn geconverteerd, is je conversie ONVOLLEDIG en GEFAALD!

Geen extra uitleg, alleen de C code die direct kan worden gecompileerd en ALLE ASM content dekt.
"""
        return prompt
    
    def call_llm_api(self, prompt: str, model: str = "gpt-4o") -> Optional[str]:
        """Call LLM API to convert ASM to C - supports both OpenAI and Gemini"""
        if not self.api_key:
            provider_name = "Gemini" if self.provider == "gemini" else "OpenAI"
            print(f"❌ No {provider_name} API key found. Please set up your API key:")
            if self.provider == "gemini":
                print("  1. Create 'gemini_api_key.txt' file with your Gemini API key")
                print("  2. Or set GEMINI_API_KEY environment variable")
                print("  3. Or pass --api-key argument")
                print()
                print("🔑 Get API key from: https://makersuite.google.com/app/apikey")
            else:
                print("  1. Create 'api_key.txt' file with your OpenAI API key")
                print("  2. Or set OPENAI_API_KEY environment variable")
                print("  3. Or pass --api-key argument")
                print()
                print("🔑 Get API key from: https://platform.openai.com/api-keys")
            return None
        
        if self.provider == "gemini":
            return self._call_gemini_api(prompt, model)
        else:
            return self._call_openai_api(prompt, model)
    
    def _call_openai_api(self, prompt: str, model: str) -> Optional[str]:
        """Call OpenAI API"""
        headers = {
            "Authorization": f"Bearer {self.api_key}",
            "Content-Type": "application/json"
        }
        
        data = {
            "model": model,
            "messages": [
                {
                    "role": "system",
                    "content": "You are an expert Game Boy assembly to C code converter. Generate clean, compilable C code that follows the exact patterns specified."
                },
                {
                    "role": "user", 
                    "content": prompt
                }
            ],
            "max_tokens": 4000,
            "temperature": 0.0  # Low temperature for consistent output
        }
        
        try:
            print(f"🤖 Calling OpenAI {model} API for conversion...")
            response = requests.post(self.api_url, headers=headers, json=data)
            response.raise_for_status()
            
            result = response.json()
            return result['choices'][0]['message']['content']
        
        except requests.exceptions.RequestException as e:
            print(f"❌ OpenAI API call failed: {e}")
            return None
        except KeyError as e:
            print(f"❌ Unexpected OpenAI API response format: {e}")
            return None
    
    def _call_gemini_api(self, prompt: str, model: str) -> Optional[str]:
        """Call Gemini API"""
        # Construct the full URL for the specific model
        url = f"{self.api_url}/{model}:generateContent?key={self.api_key}"
        
        headers = {
            "Content-Type": "application/json"
        }
        
        data = {
            "contents": [
                {
                    "parts": [
                        {
                            "text": prompt
                        }
                    ]
                }
            ],
            "generationConfig": {
                "temperature": 0.1,  # Low temperature for consistent output
                "topK": 40,
                "topP": 0.95,
                "maxOutputTokens": 8192,
            }
        }
        
        try:
            print(f"🤖 Calling Gemini {model} API for conversion...")
            response = requests.post(url, headers=headers, json=data)
            response.raise_for_status()
            
            result = response.json()
            
            # Extract text from Gemini response format
            if 'candidates' in result and len(result['candidates']) > 0:
                candidate = result['candidates'][0]
                if 'content' in candidate and 'parts' in candidate['content']:
                    parts = candidate['content']['parts']
                    if len(parts) > 0 and 'text' in parts[0]:
                        return parts[0]['text']
            
            print("❌ Unexpected Gemini API response format")
            print(f"Response: {json.dumps(result, indent=2)}")
            return None
        
        except requests.exceptions.RequestException as e:
            print(f"❌ Gemini API call failed: {e}")
            if hasattr(e, 'response') and e.response is not None:
                print(f"Response status: {e.response.status_code}")
                print(f"Response body: {e.response.text}")
            return None
        except KeyError as e:
            print(f"❌ Unexpected Gemini API response format: {e}")
            return None
    
    def clean_c_code(self, c_code: str) -> str:
        """Clean and format the generated C code"""
        # Remove markdown code blocks if present
        c_code = re.sub(r'```c\n?', '', c_code)
        c_code = re.sub(r'```\n?', '', c_code)
        
        # Remove any extra explanatory text at the beginning/end
        lines = c_code.split('\n')
        
        # Find first line with #include or actual C code
        start_idx = 0
        for i, line in enumerate(lines):
            if (line.strip().startswith('#include') or 
                line.strip().startswith('extern') or
                line.strip().startswith('//') or
                line.strip().startswith('#define')):
                start_idx = i
                break
        
        # Clean up the code
        cleaned_lines = lines[start_idx:]
        
        # Remove empty lines at the end
        while cleaned_lines and not cleaned_lines[-1].strip():
            cleaned_lines.pop()
        
        return '\n'.join(cleaned_lines)
    
    def convert_asm_file(self, asm_file: str, output_file: str, model: str = "gpt-4o") -> bool:
        """Convert a single ASM file to C"""
        try:
            # Read ASM file
            with open(asm_file, 'r', encoding='utf-8') as f:
                asm_content = f.read()
            
            filename = os.path.basename(asm_file)
            print(f"📄 Converting {filename}...")
            
            # Create conversion prompt
            prompt = self.create_conversion_prompt(asm_content, filename)
            
            # Call LLM API
            c_code = self.call_llm_api(prompt, model)
            if not c_code:
                return False
            
            # Clean and format C code
            c_code = self.clean_c_code(c_code)
            
            # Ensure output directory exists
            os.makedirs(os.path.dirname(output_file), exist_ok=True)
            
            # Write C file
            with open(output_file, 'w', encoding='utf-8') as f:
                f.write(c_code)
            
            print(f"✅ Successfully converted to {output_file}")
            return True
            
        except Exception as e:
            print(f"❌ Error converting {asm_file}: {e}")
            return False
    
    def convert_emotion_bubbles(self, model: str = "gpt-4o"):
        """Convert emotion_bubbles.asm specifically"""
        asm_file = "pokered/engine/overworld/emotion_bubbles.asm"
        output_file = "c_code/engine/overworld/emotion_bubbles.c"
        
        return self.convert_asm_file(asm_file, output_file, model)
    
    def convert_directory(self, input_dir: str, pattern: str = "*.asm", model: str = "gpt-4o"):
        """Convert all ASM files in a directory"""
        input_path = Path(input_dir)
        
        if not input_path.exists():
            print(f"❌ Input directory not found: {input_dir}")
            return
        
        asm_files = list(input_path.rglob(pattern))
        
        if not asm_files:
            print(f"❌ No ASM files found in {input_dir}")
            return
        
        print(f"🔄 Found {len(asm_files)} ASM files to convert...")
        print(f"🤖 Using model: {model}")
        
        successful = 0
        for asm_file in asm_files:
            # Calculate relative path and output file
            relative_path = asm_file.relative_to(input_path)
            output_file = Path(self.output_dir) / relative_path.with_suffix('.c')
            
            if self.convert_asm_file(str(asm_file), str(output_file), model):
                successful += 1
        
        print(f"✅ Converted {successful}/{len(asm_files)} files successfully")
    
    def validate_api_setup(self) -> bool:
        """Validate that API is properly configured"""
        if not self.api_key:
            provider_name = "Gemini" if self.provider == "gemini" else "OpenAI"
            print(f"❌ No {provider_name} API key configured!")
            print()
            print("💡 Setup instructions:")
            if self.provider == "gemini":
                print("  1. Create 'gemini_api_key.txt' file with your Gemini API key")
                print("  2. Or set environment variable: export GEMINI_API_KEY='your-key'")
                print("  3. Or use --api-key argument")
                print()
                print("🔑 Get API key from: https://makersuite.google.com/app/apikey")
            else:
                print("  1. Create 'api_key.txt' file with your OpenAI API key")
                print("  2. Or set environment variable: export OPENAI_API_KEY='your-key'")
                print("  3. Or use --api-key argument")
                print()
                print("🔑 Get API key from: https://platform.openai.com/api-keys")
            return False
        
        provider_name = "Gemini" if self.provider == "gemini" else "OpenAI"
        print(f"✅ {provider_name} API key configured")
        return True
    
    def list_models(self):
        """List available models for both providers"""
        print("🤖 Available LLM Models for ASM-to-C Conversion:")
        print()
        
        # OpenAI Models
        print("🔵 OpenAI Models (use --provider openai):")
        openai_categories = {
            'Reasoning Models (o-series)': ['o3', 'o3-mini', 'o1', 'o1-mini', 'o1-pro'],
            'Latest GPT Models': ['gpt-4.1', 'gpt-4.1-mini', 'gpt-4.1-nano', 'gpt-4.5', 'gpt-4o', 'gpt-4o-mini'],
            'Reliable Models': ['gpt-4-turbo', 'gpt-3.5-turbo']
        }
        
        for category, models in openai_categories.items():
            print(f"  📂 {category}:")
            for model in models:
                if model in AVAILABLE_OPENAI_MODELS:
                    print(f"     • {model:<15} - {AVAILABLE_OPENAI_MODELS[model]}")
            print()
        
        # Gemini Models
        print("🟢 Google Gemini Models (use --provider gemini):")
        gemini_categories = {
            'Latest Gemini 2.5 (Preview)': ['gemini-2.5-flash-preview', 'gemini-2.5-pro-preview'],
            'Gemini 2.0 (General Availability)': ['gemini-2.0-flash', 'gemini-2.0-flash-lite'],
            'Gemini 1.5 (Stable)': ['gemini-1.5-flash', 'gemini-1.5-flash-8b', 'gemini-1.5-pro']
        }
        
        for category, models in gemini_categories.items():
            print(f"  📂 {category}:")
            for model in models:
                if model in AVAILABLE_GEMINI_MODELS:
                    print(f"     • {model:<25} - {AVAILABLE_GEMINI_MODELS[model]}")
            print()
        
        print("💰 Cost & Performance Recommendations:")
        print("  🔵 OpenAI:")
        print("     • o3, gpt-4.1: Most expensive but best quality")
        print("     • o3-mini, gpt-4.1-mini: Good balance of cost/quality")
        print("     • gpt-4o-mini: Fastest and cheapest")
        print("     • gpt-3.5-turbo: Most economical option")
        print()
        print("  🟢 Gemini:")
        print("     • gemini-2.5-flash-preview: Best performance, fast thinking")
        print("     • gemini-2.0-flash: Good balance of speed and quality")
        print("     • gemini-1.5-flash: Most economical, proven stable")
        print()
        print("💡 Usage examples:")
        print("   python asm_to_c_converter.py --provider openai --model gpt-4o --file input.asm")
        print("   python asm_to_c_converter.py --provider gemini --model gemini-2.0-flash --file input.asm")

def create_api_key_file():
    """Create api_key.txt template if it doesn't exist"""
    if not os.path.exists('api_key.txt'):
        with open('api_key.txt', 'w', encoding='utf-8') as f:
            f.write("# Paste your OpenAI API key here (remove this line)\n")
            f.write("# Get your key from: https://platform.openai.com/api-keys\n")
            f.write("# Example: sk-proj-abcd1234...\n")
        print("📝 Created api_key.txt template - please add your OpenAI API key")
        return False
    return True

def create_gemini_api_key_file():
    """Create gemini_api_key.txt template if it doesn't exist"""
    if not os.path.exists('gemini_api_key.txt'):
        with open('gemini_api_key.txt', 'w', encoding='utf-8') as f:
            f.write("# Paste your Google Gemini API key here (remove this line)\n")
            f.write("# Get your key from: https://makersuite.google.com/app/apikey\n")
            f.write("# Example: AIzaSyAbc123...\n")
        print("📝 Created gemini_api_key.txt template - please add your Gemini API key")
        return False
    return True

def main():
    parser = argparse.ArgumentParser(description='Convert Game Boy ASM files to C code using OpenAI or Gemini LLM APIs')
    parser.add_argument('--provider', choices=['openai', 'gemini'], default='openai',
                       help='LLM provider to use (default: openai)')
    parser.add_argument('--api-key', help='API key (or use api_key.txt/gemini_api_key.txt file)')
    parser.add_argument('--api-url', help='Custom API URL')
    parser.add_argument('--model', default='gpt-4o', 
                       help='Model to use. Use --list-models to see options. Default: gpt-4o (OpenAI) or gemini-2.0-flash (Gemini)')
    parser.add_argument('--file', help='Convert single ASM file')
    parser.add_argument('--files', nargs='+', help='Convert multiple ASM files (space separated)')
    parser.add_argument('--dir', help='Convert all ASM files in directory')
    parser.add_argument('--emotion-bubbles', action='store_true', 
                       help='Convert emotion_bubbles.asm specifically')
    parser.add_argument('--output', default='c_code', help='Output directory')
    parser.add_argument('--list-models', action='store_true', 
                       help='List available models for both providers and exit')
    parser.add_argument('--create-key-file', action='store_true',
                       help='Create API key template files')
    
    args = parser.parse_args()
    
    # Handle special commands first
    if args.list_models:
        converter = ASMToCConverter(provider=args.provider)
        converter.list_models()
        return 0
    
    if args.create_key_file:
        create_api_key_file()
        create_gemini_api_key_file()
        return 0
    
    # Set default model based on provider if not specified
    if args.model == 'gpt-4o' and args.provider == 'gemini':
        args.model = 'gemini-2.0-flash'
        print(f"💡 Using default Gemini model: {args.model}")
    
    # Validate model for the selected provider
    available_models = AVAILABLE_GEMINI_MODELS if args.provider == 'gemini' else AVAILABLE_OPENAI_MODELS
    if args.model not in available_models:
        print(f"❌ Unknown model '{args.model}' for provider '{args.provider}'")
        print(f"Available {args.provider} models: {', '.join(available_models.keys())}")
        print("Use --list-models for detailed descriptions")
        return 1
    
    # Create converter
    converter = ASMToCConverter(api_key=args.api_key, api_url=args.api_url, provider=args.provider)
    converter.output_dir = args.output
    
    # Validate API setup
    if not converter.validate_api_setup():
        return 1
    
    provider_name = "Gemini" if args.provider == "gemini" else "OpenAI"
    model_description = available_models[args.model]
    print(f"🤖 Using {provider_name} model: {args.model}")
    print(f"📝 {model_description}")
    print()
    
    # Execute conversion based on arguments
    if args.emotion_bubbles:
        success = converter.convert_emotion_bubbles(args.model)
        return 0 if success else 1
    
    elif args.files:
        # Convert multiple files
        print(f"🔄 Converting {len(args.files)} files...")
        successful = 0
        
        for file_path in args.files:
            # Preserve directory structure from input file path
            input_path = Path(file_path)
            
            # Try to extract relative path from pokered directory structure
            # Handle both Windows backslashes and Unix forward slashes
            path_str = str(input_path).replace('\\', '/')
            if "pokered/" in path_str:
                # Extract path after pokered/ 
                pokered_relative = path_str.split("pokered/", 1)[1]
                output_file = Path(args.output) / Path(pokered_relative).with_suffix('.c')
                print(f"🔍 Input: {file_path}")
                print(f"🔍 Pokered relative: {pokered_relative}")
                print(f"🔍 Output will be: {output_file}")
            else:
                # Fallback to just filename if not in pokered structure
                output_file = Path(args.output) / input_path.with_suffix('.c').name
                print(f"🔍 Using fallback path: {output_file}")
            
            # Ensure output directory exists
            output_file.parent.mkdir(parents=True, exist_ok=True)
            
            if converter.convert_asm_file(file_path, str(output_file), args.model):
                successful += 1
        
        print(f"✅ Converted {successful}/{len(args.files)} files successfully")
        return 0 if successful == len(args.files) else 1
    
    elif args.file:
        # Preserve directory structure from input file path
        input_path = Path(args.file)
        
        # Try to extract relative path from pokered directory structure
        # Handle both Windows backslashes and Unix forward slashes
        path_str = str(input_path).replace('\\', '/')
        if "pokered/" in path_str:
            # Extract path after pokered/ 
            pokered_relative = path_str.split("pokered/", 1)[1]
            output_file = Path(args.output) / Path(pokered_relative).with_suffix('.c')
            print(f"🔍 Input: {args.file}")
            print(f"🔍 Pokered relative: {pokered_relative}")
            print(f"🔍 Output will be: {output_file}")
        else:
            # Fallback to just filename if not in pokered structure
            output_file = Path(args.output) / input_path.with_suffix('.c').name
            print(f"🔍 Using fallback path: {output_file}")
        
        # Ensure output directory exists
        output_file.parent.mkdir(parents=True, exist_ok=True)
        
        success = converter.convert_asm_file(args.file, str(output_file), args.model)
        return 0 if success else 1
    
    elif args.dir:
        converter.convert_directory(args.dir, model=args.model)
        return 0
    
    else:
        print("Please specify one of:")
        print("  --emotion-bubbles    Convert emotion_bubbles.asm")
        print("  --file <path>        Convert single ASM file")
        print("  --files <path1> <path2> ...  Convert multiple ASM files")
        print("  --dir <path>         Convert directory of ASM files")
        print("  --list-models        Show available models")
        print("  --create-key-file    Create API key template files")
        return 1

if __name__ == '__main__':
    exit(main()) 