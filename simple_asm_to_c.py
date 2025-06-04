import re, sys, os

def convert_file(path):
    with open(path) as f:
        lines = [l.strip() for l in f]
    func_name = None
    out_lines = []
    for line in lines:
        if not line:
            out_lines.append('//')
            continue
        if line.startswith(';'):
            out_lines.append('// ' + line[1:].strip())
            continue
        if line.endswith('::') or line.endswith(':'):
            label = line.rstrip(':')
            if func_name is None and not label.startswith('.'):
                func_name = label
            out_lines.append(label + ':')
            continue
        m = re.match(r"xor a", line)
        if m:
            out_lines.append('// ' + line)
            out_lines.append('    a = 0;')
            continue
        m = re.match(r"ld a, \[(.+)\]", line)
        if m:
            out_lines.append('// ' + line)
            out_lines.append(f'    a = {m.group(1)};')
            continue
        m = re.match(r"ld a, (.+)", line)
        if m:
            out_lines.append('// ' + line)
            out_lines.append(f'    a = {m.group(1)};')
            continue
        m = re.match(r"ld \[hli\], a", line)
        if m:
            out_lines.append('// ' + line)
            out_lines.append('    *hl++ = a;')
            continue
        m = re.match(r"ld \[hl\], a", line)
        if m:
            out_lines.append('// ' + line)
            out_lines.append('    *hl = a;')
            continue
        m = re.match(r"ld \[(.+)\], a", line)
        if m:
            out_lines.append('// ' + line)
            out_lines.append(f'    {m.group(1)} = a;')
            continue
        m = re.match(r"ld a, \[(.+)\]", line)
        if m:
            out_lines.append('// ' + line)
            out_lines.append(f'    a = {m.group(1)};')
            continue
        m = re.match(r"ld hl, (.+)", line)
        if m:
            out_lines.append('// ' + line)
            out_lines.append(f'    hl = {m.group(1)};')
            continue
        m = re.match(r"ld bc, (.+)", line)
        if m:
            out_lines.append('// ' + line)
            out_lines.append(f'    bc = {m.group(1)};')
            continue
        m = re.match(r"ld de, (.+)", line)
        if m:
            out_lines.append('// ' + line)
            out_lines.append(f'    de = {m.group(1)};')
            continue
        m = re.match(r"ldh \[(.+)\], a", line)
        if m:
            out_lines.append('// ' + line)
            out_lines.append(f'    {m.group(1)} = a;')
            continue
        m = re.match(r"ldh a, \[(.+)\]", line)
        if m:
            out_lines.append('// ' + line)
            out_lines.append(f'    a = {m.group(1)};')
            continue
        m = re.match(r"call (.+)", line)
        if m:
            out_lines.append('// ' + line)
            out_lines.append(f'    {m.group(1)}();')
            continue
        m = re.match(r"jp (.+)", line)
        if m:
            out_lines.append('// ' + line)
            out_lines.append(f'    {m.group(1)}(); /* jp */')
            continue
        m = re.match(r"ret", line)
        if m:
            out_lines.append('// ' + line)
            out_lines.append('    return;')
            continue
        out_lines.append('// ' + line)
    if not func_name:
        func_name = 'function'
    out_code = [f'void {func_name}(void) {{']
    out_code += out_lines
    out_code.append('}')
    return '\n'.join(out_code)

if __name__ == '__main__':
    for asm in sys.argv[1:]:
        code = convert_file(asm)
        out = asm.replace('pokered/', 'c_code/').replace('.asm', '.c')
        os.makedirs(os.path.dirname(out), exist_ok=True)
        with open(out, 'w') as f:
            f.write(code + '\n')
        print('Converted', asm, '->', out)
