#!/usr/bin/env python3
"""
Automated Bidirectional Transpiler Test Script

This script automates the complete testing process:
1. Runs the C to Z80 compiler
2. Copies the compiled assembly to pokered directory
3. Runs the Pokemon Red build and verification
4. Reports the results

Usage: python test_transpiler.py
"""

import os
import sys
import subprocess
import shutil
import time
import argparse
from pathlib import Path

class Colors:
    """ANSI color codes for terminal output"""
    GREEN = '\033[92m'
    RED = '\033[91m'
    YELLOW = '\033[93m'
    BLUE = '\033[94m'
    CYAN = '\033[96m'
    BOLD = '\033[1m'
    END = '\033[0m'

def print_header(text):
    """Print a colored header"""
    print(f"\n{Colors.BLUE}{Colors.BOLD}{'='*60}{Colors.END}")
    print(f"{Colors.BLUE}{Colors.BOLD}{text:^60}{Colors.END}")
    print(f"{Colors.BLUE}{Colors.BOLD}{'='*60}{Colors.END}")

def print_success(text):
    """Print success message in green"""
    print(f"{Colors.GREEN}✅ {text}{Colors.END}")

def print_error(text):
    """Print error message in red"""
    print(f"{Colors.RED}❌ {text}{Colors.END}")

def print_warning(text):
    """Print warning message in yellow"""
    print(f"{Colors.YELLOW}⚠️ {text}{Colors.END}")

def print_info(text):
    """Print info message in cyan"""
    print(f"{Colors.CYAN}ℹ️ {text}{Colors.END}")

def run_command(cmd, cwd=None, capture_output=True, verbose=False):
    """Run a command and return the result"""
    print_info(f"Running: {cmd}")
    try:
        if verbose and not capture_output:
            # Show real-time output
            result = subprocess.run(cmd, shell=True, cwd=cwd, check=True)
            return True, "", ""
        elif capture_output:
            result = subprocess.run(
                cmd, 
                shell=True, 
                cwd=cwd, 
                capture_output=True, 
                text=True, 
                check=True
            )
            return True, result.stdout, result.stderr
        else:
            result = subprocess.run(cmd, shell=True, cwd=cwd, check=True)
            return True, "", ""
    except subprocess.CalledProcessError as e:
        if capture_output:
            return False, e.stdout if e.stdout else "", e.stderr if e.stderr else ""
        else:
            return False, "", str(e)

def backup_original_file():
    """Create backup of original assembly file"""
    print_header("CREATING BACKUP")
    
    source = Path("pokered/engine/overworld/auto_movement.asm")
    backup_dir = Path("backup_original")
    backup_file = backup_dir / "auto_movement_original.asm"
    
    if not backup_dir.exists():
        backup_dir.mkdir()
        print_info("Created backup directory")
    
    if source.exists() and not backup_file.exists():
        shutil.copy2(source, backup_file)
        print_success(f"Backed up original file to {backup_file}")
    elif backup_file.exists():
        print_info("Backup already exists")
    else:
        print_error(f"Source file {source} not found!")
        return False
    
    return True

def run_c_to_z80_compiler():
    """Run the C to Z80 compiler"""
    print_header("RUNNING C TO Z80 COMPILER")
    
    success, stdout, stderr = run_command("python c_to_z80_compiler.py")
    
    if success:
        print_success("C to Z80 compilation completed")
        if stdout:
            print(f"Output: {stdout}")
        return True
    else:
        print_error("C to Z80 compilation failed!")
        if stderr:
            print(f"Error: {stderr}")
        return False

def copy_compiled_file():
    """Copy compiled assembly file to pokered directory"""
    print_header("COPYING COMPILED FILE")
    
    source = Path("compiled_asm/engine/overworld/auto_movement.asm")
    destination = Path("pokered/engine/overworld/auto_movement.asm")
    
    if not source.exists():
        print_error(f"Compiled file {source} not found!")
        return False
    
    try:
        shutil.copy2(source, destination)
        print_success(f"Copied {source} -> {destination}")
        
        # Show file sizes for comparison
        src_size = source.stat().st_size
        dst_size = destination.stat().st_size
        print_info(f"Source size: {src_size} bytes, Destination size: {dst_size} bytes")
        
        return True
    except Exception as e:
        print_error(f"Failed to copy file: {e}")
        return False

def analyze_assembly_differences():
    """Compare original and compiled assembly files"""
    print_header("ANALYZING ASSEMBLY DIFFERENCES")
    
    original = Path("backup_original/auto_movement_original.asm")
    compiled = Path("compiled_asm/engine/overworld/auto_movement.asm")
    
    if not original.exists() or not compiled.exists():
        print_warning("Cannot compare - files missing")
        return
    
    try:
        with open(original, 'r') as f:
            orig_lines = f.readlines()
        with open(compiled, 'r') as f:
            comp_lines = f.readlines()
        
        print_info(f"Original file: {len(orig_lines)} lines")
        print_info(f"Compiled file: {len(comp_lines)} lines")
        print_info(f"Line difference: {len(comp_lines) - len(orig_lines)}")
        
        # Count functional differences (ignoring comments and empty lines)
        orig_functional = [line.strip() for line in orig_lines 
                          if line.strip() and not line.strip().startswith(';')]
        comp_functional = [line.strip() for line in comp_lines 
                          if line.strip() and not line.strip().startswith(';')]
        
        print_info(f"Functional lines - Original: {len(orig_functional)}, Compiled: {len(comp_functional)}")
        
    except Exception as e:
        print_warning(f"Could not analyze differences: {e}")

def test_pokemon_red_build(verbose=False):
    """Test Pokemon Red build with compiled assembly"""
    print_header("TESTING POKEMON RED BUILD")
    
    pokered_dir = Path("pokered")
    if not pokered_dir.exists():
        print_error("Pokered directory not found!")
        return False, "Directory not found"
    
    # Measure build time
    print_info("Starting Pokemon Red build (this should take 30+ seconds)...")
    start_time = time.time()
    
    # Run the build and verify script with real-time output
    success, stdout, stderr = run_command("python build_and_verify.py", 
                                          cwd=pokered_dir, 
                                          capture_output=not verbose, 
                                          verbose=verbose)
    
    end_time = time.time()
    build_duration = end_time - start_time
    print_info(f"Build completed in {build_duration:.1f} seconds")
    
    # Show detailed output for debugging
    if stdout:
        print_info("Build STDOUT:")
        # Show last 20 lines of stdout to avoid spam
        stdout_lines = stdout.split('\n')
        for line in stdout_lines[-20:]:
            if line.strip():
                print(f"  {line}")
    
    if stderr:
        print_warning("Build STDERR:")
        # Show last 20 lines of stderr
        stderr_lines = stderr.split('\n') 
        for line in stderr_lines[-20:]:
            if line.strip():
                print(f"  {line}")
    
    if success:
        print_success("🎉 POKEMON RED BUILD SUCCESSFUL! 🎉")
        print_success("Checksums match - Bidirectional transpiler works perfectly!")
        return True, "Build successful with matching checksums"
    else:
        print_error("Pokemon Red build failed")
        
        # More detailed error analysis
        full_output = (stdout + "\n" + stderr).lower()
        
        if "syntax error" in full_output:
            print_warning("Syntax errors detected in assembly")
            return False, "Assembly syntax errors"
        elif "unknown symbol" in full_output:
            print_warning("Missing symbols (likely pointer tables)")
            return False, "Missing symbols/pointer tables"
        elif "checksum" in full_output and "mismatch" in full_output:
            print_warning("Build succeeded but checksum mismatch")
            return False, "Checksum mismatch"
        elif build_duration < 5:
            print_warning(f"Build failed too quickly ({build_duration:.1f}s) - likely early error")
            return False, "Build failed quickly - early error"
        else:
            print_warning("Unknown build error")
            return False, "Unknown build error"

def restore_original_file():
    """Restore the original assembly file"""
    print_header("RESTORING ORIGINAL FILE")
    
    backup_file = Path("backup_original/auto_movement_original.asm")
    destination = Path("pokered/engine/overworld/auto_movement.asm")
    
    if backup_file.exists():
        shutil.copy2(backup_file, destination)
        print_success("Restored original assembly file")
        return True
    else:
        print_error("Backup file not found!")
        return False

def generate_test_report(results):
    """Generate a comprehensive test report"""
    print_header("TEST REPORT")
    
    print(f"{Colors.BOLD}Bidirectional Transpiler Test Results{Colors.END}")
    print(f"{'='*50}")
    
    for step, (success, message) in results.items():
        status = f"{Colors.GREEN}PASS{Colors.END}" if success else f"{Colors.RED}FAIL{Colors.END}"
        print(f"{step:30} [{status}] {message}")
    
    # Overall assessment
    total_steps = len(results)
    passed_steps = sum(1 for success, _ in results.values() if success)
    
    print(f"\n{Colors.BOLD}Overall Results:{Colors.END}")
    print(f"Steps passed: {passed_steps}/{total_steps}")
    print(f"Success rate: {(passed_steps/total_steps)*100:.1f}%")
    
    if passed_steps == total_steps:
        print(f"\n{Colors.GREEN}{Colors.BOLD}🎉 PERFECT SUCCESS! 🎉{Colors.END}")
        print(f"{Colors.GREEN}The bidirectional transpiler works flawlessly!{Colors.END}")
    elif passed_steps >= total_steps - 1:
        print(f"\n{Colors.YELLOW}{Colors.BOLD}⚡ NEAR SUCCESS ⚡{Colors.END}")
        print(f"{Colors.YELLOW}The transpiler is very close to working perfectly!{Colors.END}")
    else:
        print(f"\n{Colors.RED}{Colors.BOLD}🔧 NEEDS WORK 🔧{Colors.END}")
        print(f"{Colors.RED}The transpiler needs more development.{Colors.END}")

def main():
    """Main test function"""
    parser = argparse.ArgumentParser(description='Automated Bidirectional Transpiler Test')
    parser.add_argument('--verbose', '-v', action='store_true', 
                       help='Show real-time build output instead of capturing it')
    args = parser.parse_args()
    
    print_header("BIDIRECTIONAL TRANSPILER AUTOMATED TEST")
    print_info("This script will test the complete C ↔ Z80 transpilation pipeline")
    print_info("Testing with Pokemon Red auto_movement.asm file")
    
    if args.verbose:
        print_info("Verbose mode enabled - showing real-time output")
    
    results = {}
    
    try:
        # Step 1: Backup original file
        success = backup_original_file()
        results["Backup Original"] = (success, "File backed up successfully" if success else "Failed to backup")
        
        # Step 2: Run C to Z80 compiler
        success = run_c_to_z80_compiler()
        results["C to Z80 Compilation"] = (success, "Compilation successful" if success else "Compilation failed")
        
        if success:
            # Step 3: Analyze differences
            analyze_assembly_differences()
            
            # Step 4: Copy compiled file
            success = copy_compiled_file()
            results["Copy Compiled File"] = (success, "File copied successfully" if success else "Copy failed")
            
            if success:
                # Step 5: Test Pokemon Red build
                success, message = test_pokemon_red_build(verbose=args.verbose)
                results["Pokemon Red Build"] = (success, message)
        
        # Step 6: Restore original (always try this)
        restore_success = restore_original_file()
        results["Restore Original"] = (restore_success, "Original restored" if restore_success else "Restore failed")
        
    except KeyboardInterrupt:
        print_error("\nTest interrupted by user")
        restore_original_file()
        sys.exit(1)
    except Exception as e:
        print_error(f"Unexpected error: {e}")
        restore_original_file()
        sys.exit(1)
    
    # Generate final report
    generate_test_report(results)
    
    # Exit with appropriate code
    all_critical_passed = all(success for step, (success, _) in results.items() 
                             if step != "Restore Original")
    sys.exit(0 if all_critical_passed else 1)

if __name__ == "__main__":
    main() 