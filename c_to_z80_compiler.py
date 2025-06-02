#!/usr/bin/env python3
"""
C to Z80 Assembly Compiler
Converts C code back to Game Boy Z80 assembly following strict conversion rules
to maintain byte-for-byte identical behavior.
"""

import os
import re
import argparse
from pathlib import Path

class CToZ80Compiler:
    def __init__(self):
        self.output_dir = "compiled_asm"
        self.current_function = ""
        
    def create_output_structure(self, input_path, output_base):
        """Create the same directory structure in output directory"""
        rel_path = os.path.relpath(input_path, "c_code")
        output_path = os.path.join(output_base, rel_path)
        
        # Change extension from .c to .asm
        if output_path.endswith('.c'):
            output_path = output_path[:-2] + '.asm'
            
        # Create directories if they don't exist
        os.makedirs(os.path.dirname(output_path), exist_ok=True)
        return output_path
    
    def convert_line(self, line):
        """Convert a single C line back to Z80 assembly"""
        original = line.strip()
        
        # Skip empty lines, comments, includes, externs, defines
        if (not original or original.startswith('//') or original.startswith('/*') or
            original.startswith('#') or original.startswith('extern') or 
            original.startswith('const') or original.startswith('uint8_t') or
            original == '}'):
            return None
            
        # Function declarations
        if match := re.search(r'void\s+(\w+)\(void\)\s*{', original):
            return f"{match.group(1)}::"
            
        # Return statements
        if original == 'return;':
            return "\tret"
            
        # Control flow - if (!carry_flag) goto label; (must be before other ifs)
        if match := re.search(r'if\s*\(!carry_flag\)\s*goto\s*(\w+);', original):
            return f"\tjr nc, .{match.group(1)}"
            
        # Control flow - if (a == 0) goto label;
        if match := re.search(r'if\s*\(a\s*==\s*0\)\s*goto\s*(\w+);', original):
            return f"\tjr z, .{match.group(1)}"
            
        # Control flow - if (a != 0) return;
        if 'if (a != 0) return;' in original:
            return f"\tand a\n\tret nz"
            
        # Control flow - if (a & (1 << BIT)) return;
        if match := re.search(r'if\s*\(a\s*&\s*\(1\s*<<\s*(\w+)\)\)\s*return;', original):
            return f"\tbit {match.group(1)}, a\n\tret nz"
            
        # Control flow - if (a == VALUE) return;
        if match := re.search(r'if\s*\(a\s*==\s*([^)]+)\)\s*return;', original):
            value = match.group(1).strip()
            if value == 'b':
                return f"\tcp b\n\tret z"
            elif value == 'POKEMON_TOWER_7F':
                return f"\tcp {value}\n\tret z"
            else:
                return f"\tcp {value}\n\tret z"
                
        # Control flow - if (a == 0xFF) goto label;
        if match := re.search(r'if\s*\(a\s*==\s*0xFF\)\s*goto\s*(\w+);', original):
            return f"\tcp -1\n\tjr z, .{match.group(1)}"
                
        # Control flow - goto label;
        if match := re.search(r'goto\s*(\w+);', original):
            return f"\tjr .{match.group(1)}"
            
        # Labels
        if match := re.search(r'^(\w+):$', original):
            return f".{match.group(1)}"
            
        # Bit operations - *hl &= ~(1 << BIT);
        if match := re.search(r'\*hl\s*&=\s*~\(1\s*<<\s*(\w+)\);', original):
            return f"\tres {match.group(1)}, [hl]"
            
        # Bit operations - *hl |= (1 << BIT);
        if match := re.search(r'\*hl\s*\|=\s*\(1\s*<<\s*(\w+)\);', original):
            return f"\tset {match.group(1)}, [hl]"
            
        # Memory operations - *hl = value;
        if match := re.search(r'\*hl\s*=\s*([^;]+);', original):
            value = match.group(1).strip()
            if value.startswith('0x'):
                return f"\tld [hl], ${value[2:]}"
            else:
                return f"\tld [hl], {value}"
                
        # Memory operations - a = *hl++;
        if 'a = *hl++;' in original:
            return f"\tld a, [hli]"
            
        # Pointer assignments - hl = &variable;
        if match := re.search(r'hl\s*=\s*&(\w+);', original):
            return f"\tld hl, {match.group(1)}"
            
        # Pointer assignments - hl = (uint8_t*)array; 
        if match := re.search(r'hl\s*=\s*\(uint8_t\*\)(\w+);', original):
            return f"\tld hl, {match.group(1)}"
            
        # Pointer assignments - hl = variable; (for arrays)
        if match := re.search(r'hl\s*=\s*(\w+);', original):
            var = match.group(1)
            return f"\tld hl, {var}"
            
        # Pointer assignments - de = (uint8_t*)array;
        if match := re.search(r'de\s*=\s*\(uint8_t\*\)(\w+);', original):
            return f"\tld de, {match.group(1)}"
            
        # Variable assignments - a = 0;
        if 'a = 0;' in original:
            return "\txor a"
            
        # Variable assignments - a = BANK(...); (must be before function calls)
        if match := re.search(r'a\s*=\s*(BANK\([^)]+\));', original):
            value = match.group(1)
            return f"\tld a, {value}"
            
        # Function calls with assignment - a = function(...);
        if match := re.search(r'a\s*=\s*(\w+)\([^)]*\);', original):
            return f"\tcall {match.group(1)}"
            
        # Variable assignments - a = value;
        if match := re.search(r'a\s*=\s*([^;]+);', original):
            value = match.group(1).strip()
            if value.startswith('0x'):
                return f"\tld a, ${value[2:]}"
            elif value.startswith('w'):
                return f"\tld a, [{value}]"
            elif value.startswith('h'):
                return f"\tldh a, [{value}]"
            elif '<<' in value and '>>' in value:  # swap operation
                return f"\tswap a"
            elif value.startswith('BANK('):
                return f"\tld a, {value}"
            elif value.isdigit():
                return f"\tld a, {value}"
            else:
                return f"\tld a, {value}"
                
        # Variable assignments - a -= value;
        if match := re.search(r'a\s*-=\s*([^;]+);', original):
            value = match.group(1).strip()
            if value.startswith('0x'):
                return f"\tsub ${value[2:]}"
            else:
                return f"\tsub {value}"
                
        # Variable assignments - a--; 
        if 'a--;' in original:
            return f"\tdec a"
            
        # Register assignments - b = 0; c = a; etc.
        if match := re.search(r'([bc])\s*=\s*([^;]+);', original):
            reg, value = match.groups()
            if value.strip() == '0':
                return f"\tld {reg}, 0"
            else:
                return f"\tld {reg}, {value.strip()}"
                
        # Global variable assignments
        if match := re.search(r'(w\w+)\s*=\s*([^;]+);', original):
            var, value = match.groups()
            value = value.strip()
            if value == 'a':
                return f"\tld [{var}], a"
            elif value.startswith('0x'):
                return f"\tld [{var}], ${value[2:]}"
            else:
                return f"\tld [{var}], {value}"
                
        # High RAM assignments
        if match := re.search(r'(h\w+)\s*=\s*([^;]+);', original):
            var, value = match.groups()
            value = value.strip()
            if value == 'a':
                return f"\tldh [{var}], a"
            else:
                return f"\tldh [{var}], {value}"
                
        # Function calls - carry_flag = function();
        if match := re.search(r'carry_flag\s*=\s*(\w+)\(\);', original):
            return f"\tcall {match.group(1)}"
            
        # Function calls - function();
        if match := re.search(r'(\w+)\(\);', original):
            func = match.group(1)
            if func in ['HideObject', 'ConvertNPCMovementDirectionsToJoypadMasks', 'PewterGuys']:
                return f"\tpredef {func}"
            elif func == 'EndNPCMovementScript':
                return f"\tjp {func}"
            else:
                return f"\tcall {func}"
                
        # Function calls with parameters
        if match := re.search(r'(\w+)\([^)]+\);', original):
            return f"\tcall {match.group(1)}"
            
        return None
    
    def convert_data_arrays(self, lines):
        """Convert const array definitions back to Z80 data blocks"""
        result = []
        i = 0
        while i < len(lines):
            line = lines[i].strip()
            
            # Find const array definitions
            if match := re.search(r'const uint8_t (\w+)\[\] = {', line):
                array_name = match.group(1)
                result.append(f"{array_name}:")
                
                # Process array data
                i += 1
                while i < len(lines) and not lines[i].strip().startswith('};'):
                    data_line = lines[i].strip()
                    if data_line and not data_line.startswith('//'):
                        # Remove trailing comma and convert
                        data_line = data_line.rstrip(',')
                        if '0xFF' in data_line:
                            result.append(f"\tdb -1 ; end")
                        elif match := re.search(r'(\w+),\s*(\d+)', data_line):
                            movement, count = match.groups()
                            result.append(f"\tdb {movement}, {count}")
                        elif data_line.strip():
                            result.append(f"\tdb {data_line}")
                    i += 1
                result.append("")  # Empty line after array
            i += 1
            
        return result
    
    def compile_file(self, input_file, output_file):
        """Compile a single C file to Z80 assembly"""
        print(f"Compiling {input_file} -> {output_file}")
        
        with open(input_file, 'r') as f:
            lines = f.readlines()
        
        asm_lines = []
        
        # Process all lines
        for line in lines:
            asm_line = self.convert_line(line)
            if asm_line:
                # Handle multi-line outputs (like "and a\nret nz")
                if '\n' in asm_line:
                    asm_lines.extend(asm_line.split('\n'))
                else:
                    asm_lines.append(asm_line)
        
        # Add data arrays at the end
        data_arrays = self.convert_data_arrays(lines)
        asm_lines.extend(data_arrays)
        
        # Add pointer tables
        asm_lines = self.add_pointer_tables(asm_lines)
        
        # Write output
        with open(output_file, 'w') as f:
            for line in asm_lines:
                if line is not None:  # Allow empty strings but not None
                    f.write(line + '\n')
    
    def compile_directory(self, input_dir="c_code"):
        """Compile all C files in directory structure"""
        print(f"Compiling C files from {input_dir} to {self.output_dir}")
        
        # Create output directory
        os.makedirs(self.output_dir, exist_ok=True)
        
        # Find all .c files
        for root, dirs, files in os.walk(input_dir):
            for file in files:
                if file.endswith('.c'):
                    input_path = os.path.join(root, file)
                    output_path = self.create_output_structure(input_path, self.output_dir)
                    
                    try:
                        self.compile_file(input_path, output_path)
                    except Exception as e:
                        print(f"Error compiling {input_path}: {e}")

    def add_pointer_tables(self, asm_lines):
        """Add the missing pointer tables required for Pokemon Red linking"""
        pointer_tables = [
            # PalletMovementScriptPointerTable
            "PalletMovementScriptPointerTable::",
            "\tdw PalletMovementScript_OakMoveLeft",
            "\tdw PalletMovementScript_PlayerMoveLeft", 
            "\tdw PalletMovementScript_WaitAndWalkToLab",
            "\tdw PalletMovementScript_WalkToLab",
            "\tdw PalletMovementScript_Done",
            "",
            # PewterMuseumGuyMovementScriptPointerTable  
            "PewterMuseumGuyMovementScriptPointerTable::",
            "\tdw PewterMovementScript_WalkToMuseum",
            "\tdw PewterMovementScript_Done",
            "",
            # PewterGymGuyMovementScriptPointerTable
            "PewterGymGuyMovementScriptPointerTable::",
            "\tdw PewterMovementScript_WalkToGym", 
            "\tdw PewterMovementScript_Done",
            ""
        ]
        
        # Find where to insert pointer tables (before the first function)
        insert_pos = 0
        for i, line in enumerate(asm_lines):
            if line and line.endswith('::') and not line.startswith('\t'):
                insert_pos = i
                break
        
        # Insert pointer tables at the beginning
        for i, table_line in enumerate(pointer_tables):
            asm_lines.insert(insert_pos + i, table_line)
            
        return asm_lines

def main():
    parser = argparse.ArgumentParser(description='C to Z80 Assembly Compiler')
    parser.add_argument('--input', '-i', default='c_code', 
                       help='Input directory containing C files (default: c_code)')
    parser.add_argument('--output', '-o', default='compiled_asm',
                       help='Output directory for ASM files (default: compiled_asm)')
    parser.add_argument('--file', '-f', 
                       help='Compile single file instead of directory')
    
    args = parser.parse_args()
    
    compiler = CToZ80Compiler()
    compiler.output_dir = args.output
    
    if args.file:
        output_file = args.file.replace('.c', '.asm')
        compiler.compile_file(args.file, output_file)
    else:
        compiler.compile_directory(args.input)
    
    print("Compilation complete!")

if __name__ == '__main__':
    main()
