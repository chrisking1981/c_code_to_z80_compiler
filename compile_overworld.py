from c_to_z80_compiler import CToZ80Compiler
from pathlib import Path


def main():
    compiler = CToZ80Compiler()
    compiler.output_dir = "compiled_asm"
    input_dir = Path("c_code/engine/overworld")
    compiler.compile_directory(str(input_dir))
    print("Overworld compilation complete!")


if __name__ == "__main__":
    main()
