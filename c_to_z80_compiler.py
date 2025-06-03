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
        self.current_function = None
        self.data_section = []
        self.labels_used = set()
        
    def compile_directory(self, input_dir):
        """Compile all C files in directory"""
        input_path = Path(input_dir)
        output_path = Path(self.output_dir)
        output_path.mkdir(exist_ok=True)
        
        for c_file in input_path.rglob("*.c"):
            relative_path = c_file.relative_to(input_path)
            output_file = output_path / relative_path.with_suffix('.asm')
            output_file.parent.mkdir(parents=True, exist_ok=True)
            
            print(f"Compiling {c_file} -> {output_file}")
            self.compile_file(str(c_file), str(output_file))
    
    def compile_file(self, input_file, output_file):
        """Compile single C file to ASM"""
        with open(input_file, 'r') as f:
            c_code = f.read()
        
        asm_code = self.convert_c_to_asm(c_code)
        
        with open(output_file, 'w') as f:
            f.write(asm_code)
    
    def convert_c_to_asm(self, c_code):
        """Main conversion function"""
        lines = c_code.split('\n')
        
        # Preprocess to pair C statements with ASM comments
        processed_lines = self.preprocess_lines(lines)
        
        asm_lines = []
        self.data_section = []
        self.labels_used = set()
        
        in_function = False
        brace_level = 0
        
        for item in processed_lines:
            if item['type'] == 'skip':
                continue
            elif item['type'] == 'function_start':
                # Determine if function should use :: or :
                func_name = item['name']
                if self.is_pointer_table_function(func_name):
                    asm_lines.append(f"{func_name}::")
                else:
                    asm_lines.append(f"{func_name}:")
                in_function = True
            elif item['type'] == 'function_end':
                in_function = False
                asm_lines.append("")
            elif item['type'] == 'data_array':
                # Add data inline where it appears in the C code
                for data_line in item['content']:
                    asm_lines.append(data_line)
            elif item['type'] == 'text_section':
                # SPECIAL CASE: For cut.asm, place UsedCutText BEFORE InitCutAnimOAM
                if item['name'] == 'UsedCutText':
                    # Don't add it here - it will be added after jp PrintText
                    pass
                else:
                    # Add other text sections inline with proper formatting
                    asm_lines.append("")  # Empty line before text section
                    asm_lines.append(f"{item['name']}:")  # Regular label (not local)
                    asm_lines.append(f"\ttext_far _{item['name']}")
                    asm_lines.append(f"\ttext_end")
                    asm_lines.append("")  # Empty line after text section
            elif item['type'] == 'pointer_table':
                # Add pointer tables
                asm_lines.append(f"{item['name']}::")
                for pointer in item['pointers']:
                    asm_lines.append(f"\tdw {pointer}")
                asm_lines.append("")
            elif item['type'] == 'asm_instruction' and in_function:
                # Handle special cases for dust_smoke.asm function pointer calls
                comment = item.get('comment', '')
                instruction = item['instruction']
                
                # Regular instruction with optional comment
                instr_with_comment = instruction
                if comment:
                    instr_with_comment += f" ; {comment}"
                asm_lines.append(f"\t{instr_with_comment}")
                
                # SPECIAL: Add text sections immediately after jp PrintText calls (for cut.asm)
                if 'jp PrintText' in instruction:
                    text_name = item.get('text_reference')
                    if text_name:
                        asm_lines.append("")  # Empty line
                        asm_lines.append(f"{text_name}:")  # Regular label (not local)
                        asm_lines.append(f"\ttext_far _{text_name}")
                        asm_lines.append(f"\ttext_end")
                        asm_lines.append("")  # Empty line
            elif item['type'] == 'label' and in_function:
                asm_lines.append(f".{item['name']}")
            elif item['type'] == 'c_statement' and in_function:
                asm_instr = self.convert_c_statement(item['content'])
                if asm_instr:
                    # Handle multi-line instructions (like cp + jr)
                    if '\n\t' in asm_instr:
                        for line in asm_instr.split('\n'):
                            if line.strip():
                                asm_lines.append(f"\t{line}" if not line.startswith('\t') else line)
                    else:
                        asm_lines.append(f"\t{asm_instr}")
        
        # Don't add data section at the end anymore - it's handled inline
        return '\n'.join(asm_lines)
    
    def preprocess_lines(self, lines):
        """Preprocess lines to identify ASM comments and pair them with C statements"""
        processed = []
        i = 0
        
        while i < len(lines):
            line = lines[i].rstrip()
            
            # Skip includes and external declarations
            if (line.startswith('#include') or 
                line.startswith('extern ') or 
                line.startswith('// External') or
                line.startswith('// Constants') or
                line.startswith('// Forward') or
                line.strip().startswith('#define')):
                processed.append({'type': 'skip'})
                i += 1
                continue
            
            # Handle function definitions
            if self.is_function_definition(line):
                function_name = self.extract_function_name(line)
                processed.append({'type': 'function_start', 'name': function_name})
                i += 1
                continue
            
            # Look ahead for pointer table patterns after function ends
            if line.strip() == '}' and not any(c in line for c in ['{', 'if', 'for', 'while']):
                processed.append({'type': 'function_end'})
                
                # Check if this is followed by a pointer table in the ASM
                table_info = self.detect_upcoming_pointer_table(lines, i)
                if table_info:
                    processed.append({
                        'type': 'pointer_table', 
                        'name': table_info['name'], 
                        'pointers': table_info['pointers']
                    })
                
                i += 1
                continue
            
            # Improved text section detection
            if self.is_text_section(line, lines, i):
                text_name = self.extract_text_name(line, lines, i)
                if text_name:
                    processed.append({'type': 'text_section', 'name': text_name})
                    # Skip the text content lines
                    i = self.skip_text_content(lines, i)
                    continue
            
            # Handle data arrays (including graphics)
            if (line.strip().startswith('const uint8_t') and 'Text' not in line) or \
               (line.strip().startswith('const uint16_t') and 'Text' not in line) or \
               (line.strip().startswith('const char') and ('_graphics' in line or 'INCBIN_' in line)):
                data_lines = self.handle_data_array(lines, i)
                if data_lines:  # Only add if we got valid data
                    processed.append({'type': 'data_array', 'content': data_lines})
                # Skip the array definition lines
                while i < len(lines) and '}' not in lines[i]:
                    i += 1
                if i < len(lines):  # Skip the closing brace line
                    i += 1
                continue
            
            # Handle labels
            if line.strip().endswith(':') and not line.strip().startswith('//'):
                label = line.strip()[:-1]
                processed.append({'type': 'label', 'name': label})
                i += 1
                continue
            
            # Handle ASM comments - HIGHEST PRIORITY
            if line.strip().startswith('//'):
                asm_comment = line.strip()[2:].strip()
                
                # Extract inline comments and instructions separately
                instruction = asm_comment
                comment = None
                if ';' in asm_comment:
                    parts = asm_comment.split(';', 1)
                    instruction = parts[0].strip()
                    comment = parts[1].strip()
                
                if self.is_valid_asm_instruction(instruction):
                    # Detect text reference for jp PrintText
                    text_reference = None
                    if 'jp PrintText' in instruction:
                        text_reference = self.detect_text_after_print(lines, i)
                    
                    processed.append({
                        'type': 'asm_instruction', 
                        'instruction': instruction,
                        'comment': comment,
                        'text_reference': text_reference
                    })
                    
                    # SOPHISTICATED skip logic for different ASM instruction types
                    if i + 1 < len(lines):
                        next_line = lines[i + 1].strip()
                        
                        # Skip pointer arithmetic pairs for tile operations
                        if 'tile' in instruction and ('ld de,' in instruction or 'ld hl,' in instruction):
                            # Skip the pointer assignment (de = (uint8_t*)...)
                            if next_line and ('de = (uint8_t*)' in next_line or 'hl = (uint8_t*)' in next_line):
                                i += 1
                                # Also skip the arithmetic operation (de += ... or hl += ...)
                                if i + 1 < len(lines) and ('+=' in lines[i + 1]):
                                    i += 1
                        
                        # Skip function calls that have C equivalents
                        elif any(call in instruction for call in ['call ', 'jp ', 'farcall ', 'predef ']):
                            # Skip the C function call that follows
                            if next_line and ('(' in next_line and ')' in next_line and '=' not in next_line):
                                i += 1
                        
                        # Skip simple assignments
                        elif 'ld ' in instruction and '[' not in instruction:
                            # Skip simple variable assignments like a = 0x30;
                            if next_line and ('=' in next_line and len(next_line.split('=')) == 2):
                                i += 1
                        
                        # Skip conditional operations
                        elif any(op in instruction for op in ['cp ', 'jr ', 'bit ']):
                            # Skip C conditionals and gotos
                            if next_line and ('if (' in next_line or 'goto ' in next_line):
                                i += 1
                        
                        # Default skip for other simple operations
                        elif (next_line and 
                              not next_line.startswith('//') and 
                              not next_line.endswith(':') and
                              not next_line in ['{', '}'] and
                              not self.is_variable_declaration(next_line) and
                              not next_line.startswith('goto ') and
                              len(next_line) < 100):  # Skip simple statements only
                            i += 1
                else:
                    processed.append({'type': 'skip'})
                i += 1
                continue
            
            # Handle C statements - ONLY if no recent ASM comment
            if line.strip() and not line.strip() in ['{', '}']:
                if not self.is_variable_declaration(line.strip()):
                    processed.append({'type': 'c_statement', 'content': line.strip()})
                else:
                    processed.append({'type': 'skip'})
            else:
                processed.append({'type': 'skip'})
            
            i += 1
        
        return processed
    
    def detect_upcoming_pointer_table(self, lines, current_index):
        """Check if a pointer table follows the current function end"""
        # Look ahead for pointer table pattern in the original ASM structure
        # This is a heuristic based on function naming patterns
        
        # Check if the previous function was followed by a pointer table
        # by looking back to see what function just ended
        function_name = None
        j = current_index - 1
        while j >= 0 and not self.is_function_definition(lines[j]):
            j -= 1
        
        if j >= 0:
            function_name = self.extract_function_name(lines[j])
            
            # Known patterns where pointer tables follow specific functions
            if function_name == '_EndNPCMovementScript':
                return {
                    'name': 'PalletMovementScriptPointerTable',
                    'pointers': [
                        'PalletMovementScript_OakMoveLeft',
                        'PalletMovementScript_PlayerMoveLeft', 
                        'PalletMovementScript_WaitAndWalkToLab',
                        'PalletMovementScript_WalkToLab',
                        'PalletMovementScript_Done'
                    ]
                }
            elif function_name == 'PalletMovementScript_Done':
                # This is the FIRST PalletMovementScript_Done - should be followed by PewterMuseumGuy table
                return {
                    'name': 'PewterMuseumGuyMovementScriptPointerTable',
                    'pointers': [
                        'PewterMovementScript_WalkToMuseum',
                        'PewterMovementScript_Done'
                    ]
                }
            elif function_name == 'PewterMovementScript_Done':
                # Check if this is Museum or Gym context by looking at surrounding functions
                context = self.get_function_context(lines, j)
                if 'WalkToMuseum' in context:
                    # This is Museum context, add Gym table after
                    return {
                        'name': 'PewterGymGuyMovementScriptPointerTable', 
                        'pointers': [
                            'PewterMovementScript_WalkToGym',
                            'PewterMovementScript_Done'
                        ]
                    }
        
        return None
    
    def get_function_context(self, lines, function_start_index):
        """Get context around a function to help distinguish similar function names"""
        context = ""
        # Look backwards and forwards for context in function names and constants
        for j in range(max(0, function_start_index - 100), min(len(lines), function_start_index + 50)):
            if j < len(lines):
                line = lines[j]
                if ('Museum' in line or 'Gym' in line or 'WalkToMuseum' in line or 
                    'WalkToGym' in line or 'PewterMovementScript' in line):
                    context += line + " "
        return context
    
    def detect_text_after_print(self, lines, current_index):
        """Detect if jp PrintText is followed by a text reference"""
        # Look back a few lines to find hl assignment with text reference
        for j in range(max(0, current_index - 5), current_index):
            line = lines[j].strip()
            if 'hl = (uint8_t*)' in line and 'Text' in line:
                match = re.search(r'hl = \(uint8_t\*\)(\w+Text)', line)
                if match:
                    return match.group(1)
            # Also check for direct hl = TextName patterns
            elif 'hl = ' in line and 'Text' in line and 'uint8_t' not in line:
                match = re.search(r'hl = (\w+Text)', line)
                if match:
                    return match.group(1)
        return None
    
    def convert_single_line(self, line):
        """Convert a single line to ASM, prioritizing ASM comments"""
        line = line.strip()
        
        # Skip empty lines and standalone braces
        if not line or line in ['{', '}']:
            return None
        
        # Handle variable declarations (skip them)
        if self.is_variable_declaration(line):
            return None
        
        # Handle labels (goto targets)
        if line.endswith(':') and not line.startswith('//'):
            label = line[:-1]
            return f".{label}"
        
        # Handle ASM comments - PRIORITY 1
        if line.startswith('//'):
            asm_comment = line[2:].strip()
            if self.is_valid_asm_instruction(asm_comment):
                return f"\t{asm_comment}"
            return None
        
        # Handle C statements - PRIORITY 2 (only if no ASM comment found)
        return self.convert_c_statement(line)
    
    def is_valid_asm_instruction(self, comment):
        """Check if comment is a valid ASM instruction"""
        if not comment or len(comment) < 3:
            return False
        
        # Skip generic explanatory comment patterns (not game-specific)
        skip_patterns = [
            '(', ')', # Parenthetical explanations
            'saving', 'existing code', # Code organization comments
            'Data arrays', 'Placeholder', # Generic development comments
            'TODO', 'FIXME', 'NOTE', 'WARNING', # Development markers
            'function', 'variable', 'parameter', # Generic code terms
            'comment', 'explanation', 'description' # Meta-comments
        ]
        
        # Only skip if the comment is purely explanatory
        if any(pattern in comment.lower() for pattern in skip_patterns):
            # But still allow if it contains valid ASM keywords
            has_asm_keyword = any(comment.lower().startswith(kw) for kw in [
                'ld', 'xor', 'cp', 'jr', 'jp', 'call', 'ret', 'add', 'sub', 
                'inc', 'dec', 'set', 'res', 'bit', 'and', 'or', 'push', 'pop', 
                'ldh', 'swap', 'srl', 'lb', 'sla', 'sra', 'rl', 'rr', 'rlc', 'rrc'
            ])
            if not has_asm_keyword:
                return False
        
        # Check for ASM keywords at start (expanded list for better coverage)
        asm_keywords = [
            'ld', 'xor', 'cp', 'jr', 'jp', 'call', 'ret', 'add', 'sub', 
            'inc', 'dec', 'set', 'res', 'bit', 'and', 'or', 'push', 'pop', 
            'ldh', 'swap', 'srl', 'sla', 'sra', 'rl', 'rr', 'rlc', 'rrc',
            'lb', 'ccf', 'scf', 'nop', 'halt', 'stop', 'di', 'ei', 'reti',
            'rst', 'daa', 'cpl', 'predef', 'farcall'  # Added predef and farcall
        ]
        
        # Check if comment starts with any ASM keyword
        comment_lower = comment.lower().strip()
        return any(comment_lower.startswith(kw + ' ') or comment_lower == kw for kw in asm_keywords)
    
    def convert_c_statement(self, line):
        """Convert C statement to ASM (simplified version)"""
        # Handle goto statements
        if line.startswith('goto '):
            label = line[5:-1].strip()
            self.labels_used.add(label)
            # Don't use . prefix for goto - they're already correct labels
            return f"jr .{label}"
        
        # Handle return statements
        if line == 'return;':
            return "ret"
        
        # Handle conditional goto statements (need to extract cp instruction)
        if line.startswith('if (') and 'goto' in line:
            return self.convert_conditional_goto(line)
        
        # Handle conditional return statements
        if line.startswith('if (') and 'return' in line:
            return self.convert_conditional_return(line)
        
        # Handle register assignments and operations
        asm_instr = self.convert_c_operation(line)
        if asm_instr:
            return asm_instr
        
        return None
    
    def convert_conditional_goto(self, line):
        """Convert conditional goto to cp + jr instructions"""
        # if (a != 0x50) goto nothingToCut;
        if '!=' in line:
            value_match = re.search(r'!= (0x[0-9a-fA-F]+|\d+|\w+)', line)
            label_match = re.search(r'goto (\w+)', line)
            if value_match and label_match:
                value = value_match.group(1)
                label = label_match.group(1)
                cp_instr = f"cp {self.format_value(value)}"
                jr_instr = f"jr nz, .{label}"
                return f"{cp_instr}\n\t{jr_instr}"
        
        # if (a == 0x3d) goto canCut;
        elif '==' in line:
            value_match = re.search(r'== (0x[0-9a-fA-F]+|\d+|\w+)', line)
            label_match = re.search(r'goto (\w+)', line)
            if value_match and label_match:
                value = value_match.group(1)
                label = label_match.group(1)
                cp_instr = f"cp {self.format_value(value)}"
                jr_instr = f"jr z, .{label}"
                return f"{cp_instr}\n\t{jr_instr}"
        
        # if (a == 0) goto overworld;
        elif '== 0' in line:
            label_match = re.search(r'goto (\w+)', line)
            if label_match:
                label = label_match.group(1)
                return f"jr z, .{label}"
        
        # if (a != GYM) goto nothingToCut;
        elif '!=' in line and '0x' not in line:
            const_match = re.search(r'!= (\w+)', line)
            label_match = re.search(r'goto (\w+)', line)
            if const_match and label_match:
                const = const_match.group(1)
                label = label_match.group(1)
                return f"cp {const}\n\tjr nz, .{label}"
        
        return None
    
    def convert_conditional_return(self, line):
        """Convert conditional return statements"""
        if '& (1 <<' in line and ') return' in line:
            bit_match = re.search(r'& \(1 << (\w+)\)', line)
            if bit_match:
                bit_name = bit_match.group(1)
                return f"bit {bit_name}, a\n\tret nz"
        
        if '!= 0' in line and 'return' in line:
            return "ret nz"
        
        if '== 0' in line and 'return' in line:
            return "ret z"
        
        return None
    
    def is_function_definition(self, line):
        """Check if line is a function definition"""
        return (re.match(r'^\s*void\s+\w+\s*\(.*\)\s*\{?\s*$', line.strip()) or
                re.match(r'^\s*uint8_t\s+\w+\s*\(.*\)\s*\{?\s*$', line.strip()))
    
    def extract_function_name(self, line):
        """Extract function name from definition"""
        match = re.search(r'(void|uint8_t)\s+(\w+)\s*\(', line)
        return match.group(2) if match else "UnknownFunction"
    
    def is_variable_declaration(self, line):
        """Check if line is a variable declaration"""
        return (line.startswith('uint8_t') or 
                line.startswith('const uint8_t') or
                line.startswith('uint16_t'))
    
    def convert_c_operation(self, line):
        """Convert C operations to Z80 assembly"""
        line = line.rstrip(';')
        
        # Handle simple register assignments
        # a = 0; (xor a equivalent)
        if re.match(r'^a = 0$', line):
            return "xor a"
        
        # a = value;
        if re.match(r'^a = (0x[0-9a-fA-F]+|\d+|[A-Z_][A-Z0-9_]*)$', line):
            value = line.split('=')[1].strip()
            return f"ld a, {self.format_value(value)}"
        
        # Variable to register: a = variable;
        if re.match(r'^a = \w+$', line):
            var = line.split('=')[1].strip()
            return f"ld a, [{var}]"
        
        # Register to variable: variable = a;
        if re.match(r'^\w+ = a$', line):
            var = line.split('=')[0].strip()
            return f"ld [{var}], a"
        
        # Pointer assignments: hl = &variable;
        if re.match(r'^hl = &\w+$', line):
            var = line.split('&')[1].strip()
            return f"ld hl, {var}"
        
        # Handle special pointer assignments like hl = (uint8_t*)&variable;
        if 'hl = (uint8_t*)&' in line:
            var = re.search(r'&(\w+)', line).group(1)
            return f"ld hl, {var}"
        
        # Handle pointer assignments to text labels
        if re.match(r'^hl = \(uint8_t\*\)\w+Text$', line):
            var = re.search(r'\(uint8_t\*\)(\w+)', line).group(1)
            return f"ld hl, .{var}"
        
        # Handle regular pointer assignments  
        if re.match(r'^hl = \(uint8_t\*\)\w+$', line):
            var = re.search(r'\(uint8_t\*\)(\w+)', line).group(1)
            # Check if it's a text label
            if 'Text' in var:
                return f"ld hl, .{var}"
            else:
                return f"ld hl, {var}"
        
        # Handle hl++
        if line == 'hl++':
            return "inc hl"
        
        # Handle pointer dereference with increment: a = *hl++;
        if line == 'a = *hl++':
            return "ld a, [hli]"
        
        # Handle other pointer arithmetic assignments
        if 'hl +=' in line:
            if '/* add hl, bc */' in line:
                return "add hl, bc"
            elif '/* add hl, de */' in line:
                return "add hl, de"
            elif '/* tile' in line and 'hl +=' in line:
                # Extract tile offset for vChars1 operations
                tile_match = re.search(r'tile (\$[0-9a-fA-F]+|0x[0-9a-fA-F]+|\d+)', line)
                if tile_match:
                    tile_val = tile_match.group(1)
                    return f"ld hl, vChars1 tile {tile_val}"
                else:
                    # Regular pointer arithmetic
                    match = re.search(r'hl \+= (?:\(uint8_t\))?(\w+|0x[0-9a-fA-F]+|\d+)', line)
                    if match:
                        value = match.group(1)
                        if value in ['c', 'e']:
                            return f"add hl, {'bc' if value == 'c' else 'de'}"
                        else:
                            formatted_val = self.format_value(value)
                            return f"ld de, {formatted_val}\n\tadd hl, de"
            else:
                # Regular pointer arithmetic
                match = re.search(r'hl \+= (?:\(uint8_t\))?(\w+|0x[0-9a-fA-F]+|\d+)', line)
                if match:
                    value = match.group(1)
                    if value in ['c', 'e']:
                        return f"add hl, {'bc' if value == 'c' else 'de'}"
                    else:
                        formatted_val = self.format_value(value)
                        return f"ld de, {formatted_val}\n\tadd hl, de"
        
        # Handle de += offset for specific patterns
        if 'de +=' in line and 'Overworld_GFX' in line:
            # This should generate: ld de, Overworld_GFX tile $XX
            match = re.search(r'de \+= (0x[0-9a-fA-F]+|\$[0-9a-fA-F]+|\d+)', line)
            if match:
                offset = match.group(1)
                formatted_offset = self.format_value(offset)
                return f"ld de, Overworld_GFX tile {formatted_offset}"
        elif 'de +=' in line:
            match = re.search(r'de \+= (0x[0-9a-fA-F]+|\d+)', line)
            if match:
                offset = match.group(1)
                return f"ld de, {self.format_value(offset)}\n\tadd de, hl"
        
        # Bit operations
        if '*hl |= (1 <<' in line:
            bit_match = re.search(r'\(1 << (\w+)\)', line)
            if bit_match:
                bit_name = bit_match.group(1)
                return f"set {bit_name}, [hl]"
        
        if '*hl &= ~(1 <<' in line:
            bit_match = re.search(r'~\(1 << (\w+)\)', line)
            if bit_match:
                bit_name = bit_match.group(1)
                return f"res {bit_name}, [hl]"
        
        # Swap operation
        if '(a << 4) | (a >> 4)' in line:
            return "swap a"
        
        # Function calls
        if '(' in line and ')' in line and '=' not in line:
            func_match = re.search(r'(\w+)\s*\(', line)
            if func_match:
                func_name = func_match.group(1)
                
                # Check for specific call type indicators
                if '/* jp */' in line:
                    return f"jp {func_name}"
                elif '/* farcall */' in line:
                    return f"farcall {func_name}"
                elif '/* predef */' in line or func_name in ['ConvertNPCMovementDirectionsToJoypadMasks', 'HideObject', 'PewterGuys']:
                    # Known predef functions
                    return f"predef {func_name}"
                elif func_name == 'AnimCut':
                    # AnimCut is always a farcall in the original
                    return f"farcall {func_name}"
                else:
                    return f"call {func_name}"
        
        # Conditional statements
        if line.startswith('if ('):
            return self.convert_conditional(line)
        
        # Array access and assignments
        if '*hl = ' in line:
            value = line.split('=')[1].strip()
            return f"ld [hl], {self.format_value(value)}"
        
        # Register assignments like b = 0, c = a, etc.
        if re.match(r'^[a-h] = \w+$', line):
            reg, value = line.split('=')
            reg = reg.strip()
            value = value.strip()
            if value == '0':
                return f"ld {reg}, 0"
            elif value in ['a', 'b', 'c', 'd', 'e', 'h', 'l']:
                return f"ld {reg}, {value}"
            else:
                return f"ld {reg}, {self.format_value(value)}"
        
        # Arithmetic operations
        if 'a +=' in line:
            value = line.split('+=')[1].strip()
            return f"add {self.format_value(value)}"
        
        if 'a -=' in line:
            value = line.split('-=')[1].strip()
            return f"sub {self.format_value(value)}"
        
        if 'a ^=' in line:
            value = line.split('^=')[1].strip()
            return f"xor {self.format_value(value)}"
        
        # Increment/decrement
        if line.endswith('++'):
            var = line[:-2].strip()
            if var == 'hl':
                return "inc hl"
            elif var == 'de':
                return "inc de"
            else:
                return f"inc {var}"
        
        if line.endswith('--'):
            var = line[:-2].strip()
            return f"dec {var}"
        
        return None
    
    def convert_conditional(self, line):
        """Convert C conditionals to Z80 jumps"""
        # if (a == 0) goto label;
        if 'goto' in line:
            if '== 0' in line:
                label = re.search(r'goto (\w+)', line).group(1)
                return f"jr z, .{label}"
            elif '!= 0' in line:
                label = re.search(r'goto (\w+)', line).group(1)
                return f"jr nz, .{label}"
            elif '!=' in line:
                label = re.search(r'goto (\w+)', line).group(1)
                return f"jr nz, .{label}"
            elif '==' in line:
                label = re.search(r'goto (\w+)', line).group(1)
                return f"jr z, .{label}"
        
        # if (condition) return;
        if 'return' in line:
            if '!= 0' in line or '& (1 <<' in line:
                return "ret nz"
            elif '== 0' in line:
                return "ret z"
            elif '!' in line:
                return "ret nc"  # for carry flag checks
        
        return None
    
    def format_value(self, value):
        """Format numeric values for assembly"""
        if value.startswith('0x'):
            return f"${value[2:]}"
        elif value.isdigit():
            return f"${int(value):02x}" if int(value) < 256 else value
        else:
            return value
    
    def handle_data_array(self, lines, start_index):
        """Handle const uint8_t, uint16_t and const char arrays"""
        data_lines = []
        line = lines[start_index].strip()
        
        # Extract array name for uint8_t, uint16_t, and char arrays
        array_match = re.search(r'const (?:uint8_t|uint16_t|char) (\w+)\[\]', line)
        if not array_match:
            return []
        
        array_name = array_match.group(1)
        
        # Handle uint8_t arrays (including markers and INCBIN)
        if 'const uint8_t' in line:
            # Handle MARKER patterns (label-only definitions)
            if '_MARKER' in array_name:
                label_name = array_name.replace('_MARKER', '')
                data_lines = [f"{label_name}:"]
                return data_lines
            
            # Handle INCBIN patterns
            if '_INCBIN' in array_name:
                # Extract the label name (ShockEmote, QuestionEmote, etc.)
                label_name = array_name.replace('_INCBIN', '')
                
                # Look for path in comment on the same line
                path_match = re.search(r'/\* ([^*]+) \*/', line)
                if path_match:
                    path = path_match.group(1)
                    data_lines = []
                    data_lines.append(f"{label_name}:")
                    data_lines.append(f"\tINCBIN \"{path}\"")
                    return data_lines
                else:
                    # Fallback if no path found
                    data_lines = [f"{label_name}:"]
                    return data_lines
        
        # Handle uint16_t pointer tables (like EmotionBubblesPointerTable)
        if 'const uint16_t' in line:
            # Use :: for pointer tables
            data_lines.append(f"{array_name}::")
            
            # Find array content
            content_start = start_index
            while content_start < len(lines) and '{' not in lines[content_start]:
                content_start += 1
            
            if content_start >= len(lines):
                return data_lines
            
            # Extract data values
            array_content = ""
            brace_level = 0
            for i in range(content_start, len(lines)):
                line = lines[i]
                array_content += line + " "
                brace_level += line.count('{') - line.count('}')
                if brace_level == 0:
                    break
            
            # Parse pointer values
            content_match = re.search(r'\{([^}]+)\}', array_content)
            if content_match:
                content = content_match.group(1)
                # Split by comma and clean up
                values = []
                for item in content.split(','):
                    item = item.strip()
                    # Remove inline comments and casts
                    if '//' in item:
                        item = item.split('//')[0].strip()
                    if '(uint16_t)' in item:
                        item = item.replace('(uint16_t)', '').strip()
                    if item and item != '':
                        values.append(item)
                
                # Generate dw statements
                for value in values:
                    data_lines.append(f"\tdw {value}")
            
            return data_lines
        
        # Handle string arrays (const char)
        if 'const char' in line:
            # Handle graphics markers - these create labels
            if '_graphics_marker' in array_name:
                label_name = array_name.replace('_graphics_marker', '')
                data_lines = [f"{label_name}:"]
                return data_lines
            
            # Handle INCBIN graphics data
            if '_graphics' in array_name and 'INCBIN_' in line:
                # Extract the emote name (ShockEmote, QuestionEmote, etc.)
                emote_name = array_name.replace('_graphics', '')
                # Extract INCBIN path from string
                incbin_match = re.search(r'INCBIN_([^"]+)', line)
                if incbin_match:
                    path = incbin_match.group(1)
                    # Create label and INCBIN statement
                    data_lines = []
                    data_lines.append(f"{emote_name}:")
                    data_lines.append(f"\tINCBIN \"{path}\"")
                    return data_lines
            
            # Handle text sections (but not in emotion_bubbles)
            if 'Text' not in line:
                # Skip other graphics arrays that don't need text processing
                if '_graphics' in array_name:
                    return []
            
            # Look for string content for text sections
            string_match = re.search(r'=\s*"([^"]*)"', line)
            if string_match:
                string_content = string_match.group(1)
                # Convert to assembly text directive exactly like original
                data_lines.append(f"\ttext_far _{array_name}")
                data_lines.append(f"\ttext_end")
                return data_lines
        
        # Use single colon for all other data arrays (uint8_t)
        data_lines.append(f"{array_name}:")
        
        # Find array content for uint8_t arrays
        content_start = start_index
        while content_start < len(lines) and '{' not in lines[content_start]:
            content_start += 1
        
        if content_start >= len(lines):
            return data_lines
        
        # Extract data values
        array_content = ""
        brace_level = 0
        for i in range(content_start, len(lines)):
            line = lines[i]
            array_content += line + " "
            brace_level += line.count('{') - line.count('}')
            if brace_level == 0:
                break
        
        # Parse data values
        content_match = re.search(r'\{([^}]+)\}', array_content)
        if not content_match:
            return data_lines
        
        content = content_match.group(1)
        # Split by comma and clean up
        values = []
        for item in content.split(','):
            item = item.strip()
            # Remove inline comments
            if '//' in item:
                item = item.split('//')[0].strip()
            if item and item != '':
                values.append(item)
        
        # Special formatting for OAMBlock
        if 'OAMBlock' in array_name:
            # Format as pairs: tile ID, attributes
            data_lines.append("; tile ID, attributes")
            for i in range(0, len(values), 2):
                if i + 1 < len(values):
                    tile_id = self.format_value_for_db(values[i])
                    attributes = self.format_value_for_db(values[i + 1])
                    data_lines.append(f"\tdb {tile_id}, {attributes}")
            return data_lines
        
        # Convert to assembly db statements for other arrays
        db_values = []
        for value in values:
            db_values.append(self.format_value_for_db(value))
        
        # Group db values nicely
        if db_values:
            # Special formatting for RLE Lists - one entry per line
            if 'RLEList' in array_name or 'RivalIDs' in array_name:
                # RivalIDs should have each value on its own line
                if 'RivalIDs' in array_name:
                    for value in db_values:
                        data_lines.append(f"\tdb {value}")
                    return data_lines
                else:
                    # RLE Lists use pairs
                    for i in range(0, len(db_values), 2):
                        if i + 1 < len(db_values):
                            data_lines.append(f"\tdb {db_values[i]}, {db_values[i+1]}")
                        else:
                            data_lines.append(f"\tdb {db_values[i]}")
                    return data_lines
            
            # For animation offsets, add helpful comments
            elif 'Offset' in array_name:
                if 'Cut' in array_name:
                    comments = [
                        "; player is facing down",
                        "; player is facing up", 
                        "; player is facing left",
                        "; player is facing right"
                    ]
                elif 'Boulder' in array_name:
                    comments = [
                        "; player is facing down",
                        "; player is facing up",
                        "; player is facing left", 
                        "; player is facing right"
                    ]
                else:
                    comments = []
                
                # Add comment line before the data
                data_lines.append(f"; Each pair represents the x and y pixels offsets from the player")
                
                # Group in pairs with comments
                for i in range(0, len(db_values), 2):
                    if i + 1 < len(db_values):
                        line_data = f"\tdb {db_values[i]:>2}, {db_values[i+1]:>2}"
                        if i // 2 < len(comments):
                            line_data += f" {comments[i // 2]}"
                        data_lines.append(line_data)
            else:
                # Split into lines of reasonable length for other data
                line_length = 0
                current_line = []
                
                for value in db_values:
                    if line_length + len(str(value)) + 2 > 60 and current_line:  # 60 chars max per line
                        data_lines.append(f"\tdb {', '.join(current_line)}")
                        current_line = [str(value)]
                        line_length = len(str(value))
                    else:
                        current_line.append(str(value))
                        line_length += len(str(value)) + 2
                
                if current_line:
                    data_lines.append(f"\tdb {', '.join(current_line)}")
        
        return data_lines
    
    def format_value_for_db(self, value):
        """Format values for db statements, preserving -1 as -1"""
        value = value.strip()
        
        # PRESERVE -1 as -1, don't convert to $FF - CHECK THIS FIRST
        if value == '-1':
            return '-1'
        
        # Handle negative values that aren't -1
        if value.startswith('-') and value != '-1':
            return value
        
        # Convert hex values (0xFF, 0x20, etc.)
        if value.startswith('0x'):
            hex_part = value[2:].upper()
            # Don't convert 0xFF to -1, keep as $FF unless original was -1
            return f"${hex_part}"
        
        # Convert binary values
        if value.startswith('0b'):
            return f"%{value[2:]}"
        
        # Convert decimal values to hex if they're large enough (but preserve small ones and -1)
        if value.isdigit():
            num = int(value)
            if num == 255:
                # This could be 0xFF or -1, but since we checked -1 first, treat as $FF
                return "$FF"
            elif num > 15:  # Convert larger numbers to hex
                return f"${num:02X}"
            else:
                return value  # Keep small numbers as decimal
        
        return value
    
    def is_complex_c_statement(self, line):
        """Check if C statement is complex and shouldn't be skipped"""
        # Be much more restrictive - only keep goto statements and return statements
        complex_patterns = [
            'goto ',
            'return',
        ]
        return any(pattern in line for pattern in complex_patterns)
    
    def is_text_section(self, line, lines, index):
        """Improved detection for text sections"""
        # Only detect text sections at the very end of the file as data definitions
        # NOT in the middle of functions where they should be handled by jp PrintText
        
        # Direct text label definitions at end of file 
        if line.strip().endswith('Text:') and index > len(lines) - 10:
            return True
        
        # const char TextName[] = "..." at end of file
        if (line.strip().startswith('const char') and 'Text[]' in line and '=' in line and 
            index > len(lines) - 50):
            return True
        
        # NEVER detect pointer assignments in the middle of functions
        # Let jp PrintText handle these
        return False
    
    def extract_text_name(self, line, lines, index):
        """Extract text name from various text section formats"""
        # Direct label: TextName:
        if line.strip().endswith('Text:'):
            return line.strip()[:-1]
        
        # const char declaration: const char TextName[] = ...
        match = re.search(r'const char (\w+Text)\[\]', line)
        if match:
            return match.group(1)
        
        # Pointer assignment: hl = (uint8_t*)TextName;
        match = re.search(r'hl = \(uint8_t\*\)(\w+Text)', line)
        if match:
            return match.group(1)
        
        return None
    
    def skip_text_content(self, lines, start_index):
        """Skip over text content lines and return next index"""
        i = start_index + 1
        
        # Skip string content, array definitions, etc.
        while i < len(lines):
            line = lines[i].strip()
            if not line:
                i += 1
                continue
            
            # Skip string literals and array content
            if ('"' in line or 
                line.startswith('{') or 
                line.endswith('},') or
                line == '}' or
                line.startswith('0x')):
                i += 1
                continue
                
            # If we hit a new statement or declaration, stop
            if (line.endswith(':') or 
                line.startswith('//') or
                line.startswith('const ') or
                self.is_function_definition(line)):
                break
                
            i += 1
        
        return i
    
    def is_pointer_table_function(self, func_name):
        """Determine if function should use :: (pointer table) or : (regular function)"""
        # Functions that are part of pointer tables or accessed via function pointers
        pointer_functions = {
            'DisplayBoulderDustAnimation',
            'DisplayBouncingBoulderDustAnimation', 
            'DoBoulderDustAnimation',
            'DoBoulderFallAnimation',
            'GetMoveBoulderDustFunctionPointer',
            'ShakeElevator',  # This IS a pointer table function
            'PlayerStepOutFromDoor',  # This IS a pointer table function
            '_EndNPCMovementScript',   # This IS a pointer table function
            'SetEnemyTrainerToStayAndFaceAnyDirection'  # This IS a pointer table function
        }
        
        return func_name in pointer_functions


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
        # Create proper output path maintaining directory structure
        input_path = Path(args.file)
        
        # Determine relative path from input directory
        if input_path.is_absolute():
            # For absolute paths, try to find relative path from common base
            try:
                relative_path = input_path.relative_to(Path.cwd() / args.input)
            except ValueError:
                # If not under input directory, use just the filename
                relative_path = input_path.name
        else:
            # For relative paths, try to make it relative to input directory
            if str(input_path).startswith(args.input):
                relative_path = input_path.relative_to(args.input)
            else:
                relative_path = input_path.name
        
        # Create output file path in output directory
        output_file = Path(args.output) / relative_path.with_suffix('.asm')
        output_file.parent.mkdir(parents=True, exist_ok=True)
        
        print(f"Compiling {args.file} -> {output_file}")
        compiler.compile_file(args.file, str(output_file))
    else:
        compiler.compile_directory(args.input)
    
    print("Compilation complete!")

if __name__ == '__main__':
    main()
