#!/usr/bin/env python3
"""
ASM to C Code Converter
Automatically converts Game Boy Z80 assembly files to C code using LLM API calls.
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
AVAILABLE_MODELS = {
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

class ASMToCConverter:
    def __init__(self, api_key: Optional[str] = None, api_url: Optional[str] = None):
        self.api_key = api_key or self.load_api_key()
        self.api_url = api_url or "https://api.openai.com/v1/chat/completions"
        self.output_dir = "c_code"
        
        # Load conversion rules and patterns
        self.load_conversion_rules()
        self.load_c_patterns()
    
    def load_api_key(self) -> Optional[str]:
        """Load API key from multiple sources in order of preference"""
        # 1. Try from api_key.txt file
        try:
            with open('api_key.txt', 'r', encoding='utf-8') as f:
                key = f.read().strip()
                if key:
                    print("✅ API key loaded from api_key.txt")
                    return key
        except FileNotFoundError:
            pass
        
        # 2. Try from environment variable
        env_key = os.getenv('OPENAI_API_KEY')
        if env_key:
            print("✅ API key loaded from environment variable")
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
        prompt = f"""Je bent een expert Game Boy assembly naar C code converter voor het pokered project.

DOEL: Converteer de volgende Z80 assembly code naar functionele C code die byte-voor-byte identiek gedrag oplevert.

=== CONVERSION RULES ===
{self.conversion_rules}

=== C CODE PATTERNS ===  
{self.c_patterns}

=== SPECIFIEKE EISEN ===
1. Gebruik ALTIJD -1 in plaats van 0xFF voor eindmarkeringen in arrays
2. Voor function pointer tables: gebruik :: in plaats van : voor labels
3. Behoud alle assembly comments als // comments boven elke C regel
4. Voor PrintText calls: gebruik /* jp */ annotatie
5. Voor farcall: gebruik /* farcall */ annotatie  
6. Voor predef: gebruik /* predef */ annotatie
7. Local labels worden .labelname in assembly

=== KRITIEKE VEREISTE ===
CONVERTEER ALLE SECTIES IN DE ASM FILE - inclusief graphics data aan het einde!

Voor graphics secties zoals:
EmotionBubbles:
ShockEmote:    INCBIN "gfx/emotes/shock.2bpp"
QuestionEmote: INCBIN "gfx/emotes/question.2bpp"  
HappyEmote:    INCBIN "gfx/emotes/happy.2bpp"

Genereer C code als:
// EmotionBubbles:
const uint8_t EmotionBubbles_MARKER = 0;

// ShockEmote: INCBIN "gfx/emotes/shock.2bpp"  
const uint8_t ShockEmote_INCBIN[] = {{ /* gfx/emotes/shock.2bpp */ }};

// QuestionEmote: INCBIN "gfx/emotes/question.2bpp"
const uint8_t QuestionEmote_INCBIN[] = {{ /* gfx/emotes/question.2bpp */ }};

// HappyEmote: INCBIN "gfx/emotes/happy.2bpp"
const uint8_t HappyEmote_INCBIN[] = {{ /* gfx/emotes/happy.2bpp */ }};

=== ASM BESTAND: {filename} ===
```asm
{asm_content}
```

=== GEWENSTE OUTPUT ===
Genereer de COMPLETE C code met:
1. Alle benodigde #include statements
2. Extern declarations voor variabelen/functies  
3. Constanten/defines
4. De functie implementaties
5. Data arrays (gebruik -1 voor eindmarkeringen!)
6. **ALLE graphics secties aan het einde** in _MARKER en _INCBIN formaat

BELANGRIJK: Converteer ELKE regel van de ASM file. Laat niets weg!

Geen extra uitleg, alleen de C code die direct kan worden gecompileerd.
"""
        return prompt
    
    def call_llm_api(self, prompt: str, model: str = "gpt-4o") -> Optional[str]:
        """Call LLM API to convert ASM to C"""
        if not self.api_key:
            print("❌ No API key found. Please set up your API key:")
            print("  1. Create 'api_key.txt' file with your OpenAI API key")
            print("  2. Or set OPENAI_API_KEY environment variable")
            print("  3. Or pass --api-key argument")
            return None
        
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
            "temperature": 0.1  # Low temperature for consistent output
        }
        
        try:
            print(f"🤖 Calling {model} API for conversion...")
            response = requests.post(self.api_url, headers=headers, json=data)
            response.raise_for_status()
            
            result = response.json()
            return result['choices'][0]['message']['content']
        
        except requests.exceptions.RequestException as e:
            print(f"❌ API call failed: {e}")
            return None
        except KeyError as e:
            print(f"❌ Unexpected API response format: {e}")
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
            print("❌ No API key configured!")
            print()
            print("💡 Setup instructions:")
            print("  1. Create 'api_key.txt' file and paste your OpenAI API key")
            print("  2. Or set environment variable: export OPENAI_API_KEY='your-key'")
            print("  3. Or use --api-key argument")
            print()
            print("🔑 Get API key from: https://platform.openai.com/api-keys")
            return False
        
        print("✅ API key configured")
        return True
    
    def list_models(self):
        """List available models"""
        print("🤖 Available OpenAI Models (2025):")
        print()
        
        categories = {
            'Reasoning Models (o-series)': ['o3', 'o3-mini', 'o1', 'o1-mini', 'o1-pro'],
            'Latest GPT Models': ['gpt-4.1', 'gpt-4.1-mini', 'gpt-4.1-nano', 'gpt-4.5', 'gpt-4o', 'gpt-4o-mini'],
            'Reliable Models': ['gpt-4-turbo', 'gpt-3.5-turbo']
        }
        
        for category, models in categories.items():
            print(f"📂 {category}:")
            for model in models:
                if model in AVAILABLE_MODELS:
                    print(f"   • {model:<15} - {AVAILABLE_MODELS[model]}")
            print()
        
        print("💰 Cost considerations:")
        print("   • o3, gpt-4.1: Most expensive but best quality")
        print("   • o3-mini, gpt-4.1-mini: Good balance of cost/quality")
        print("   • gpt-4o-mini: Fastest and cheapest")
        print("   • gpt-3.5-turbo: Most economical option")

def create_api_key_file():
    """Create api_key.txt template if it doesn't exist"""
    if not os.path.exists('api_key.txt'):
        with open('api_key.txt', 'w', encoding='utf-8') as f:
            f.write("# Paste your OpenAI API key here (remove this line)\n")
            f.write("# Get your key from: https://platform.openai.com/api-keys\n")
            f.write("# Example: sk-proj-abcd1234...\n")
        print("📝 Created api_key.txt template - please add your API key")
        return False
    return True

def main():
    parser = argparse.ArgumentParser(description='Convert Game Boy ASM files to C code using LLM')
    parser.add_argument('--api-key', help='OpenAI API key (or use api_key.txt file)')
    parser.add_argument('--api-url', help='Custom API URL (default: OpenAI)')
    parser.add_argument('--model', default='gpt-4o', 
                       help='LLM model to use (default: gpt-4o). Use --list-models to see options')
    parser.add_argument('--file', help='Convert single ASM file')
    parser.add_argument('--dir', help='Convert all ASM files in directory')
    parser.add_argument('--emotion-bubbles', action='store_true', 
                       help='Convert emotion_bubbles.asm specifically')
    parser.add_argument('--output', default='c_code', help='Output directory')
    parser.add_argument('--list-models', action='store_true', 
                       help='List available models and exit')
    parser.add_argument('--create-key-file', action='store_true',
                       help='Create api_key.txt template file')
    
    args = parser.parse_args()
    
    # Handle special commands first
    if args.list_models:
        converter = ASMToCConverter()
        converter.list_models()
        return 0
    
    if args.create_key_file:
        create_api_key_file()
        return 0
    
    # Validate model
    if args.model not in AVAILABLE_MODELS:
        print(f"❌ Unknown model: {args.model}")
        print(f"Available models: {', '.join(AVAILABLE_MODELS.keys())}")
        print("Use --list-models for detailed descriptions")
        return 1
    
    # Create converter
    converter = ASMToCConverter(api_key=args.api_key, api_url=args.api_url)
    converter.output_dir = args.output
    
    # Validate API setup
    if not converter.validate_api_setup():
        return 1
    
    print(f"🤖 Using model: {args.model} - {AVAILABLE_MODELS[args.model]}")
    print()
    
    # Execute conversion based on arguments
    if args.emotion_bubbles:
        success = converter.convert_emotion_bubbles(args.model)
        return 0 if success else 1
    
    elif args.file:
        output_file = Path(args.output) / Path(args.file).with_suffix('.c').name
        success = converter.convert_asm_file(args.file, str(output_file), args.model)
        return 0 if success else 1
    
    elif args.dir:
        converter.convert_directory(args.dir, model=args.model)
        return 0
    
    else:
        print("Please specify one of:")
        print("  --emotion-bubbles    Convert emotion_bubbles.asm")
        print("  --file <path>        Convert single ASM file")
        print("  --dir <path>         Convert directory of ASM files")
        print("  --list-models        Show available models")
        print("  --create-key-file    Create API key template file")
        return 1

if __name__ == '__main__':
    exit(main()) 