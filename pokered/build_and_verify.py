#!/usr/bin/env python3
"""
Pokemon Red/Blue Build and Verification Script

This script builds the Pokemon Red/Blue ROMs and verifies that the checksums
match the expected values. Perfect for verifying that code comments or 
documentation changes don't affect the compiled output.

Usage: python build_and_verify.py
"""

import os
import sys
import subprocess
import hashlib
import shutil
from pathlib import Path

# Expected SHA1 checksums for the official ROMs
EXPECTED_CHECKSUMS = {
    'pokered.gbc': 'ea9bcae617fdf159b045185467ae58b2e4a48b9a',
    'pokeblue.gbc': 'd7037c83e1ae5b39bde3c30787637ba1d4c48ce2',
    'pokeblue_debug.gbc': '5b1456177671b79b263c614ea0e7cc9ac542e9c4'
}

class Colors:
    """ANSI color codes for terminal output"""
    GREEN = '\033[92m'
    RED = '\033[91m'
    YELLOW = '\033[93m'
    BLUE = '\033[94m'
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
    """Print info message"""
    print(f"ℹ️ {text}")

def calculate_sha1(filepath):
    """Calculate SHA1 hash of a file"""
    sha1_hash = hashlib.sha1()
    with open(filepath, "rb") as f:
        for chunk in iter(lambda: f.read(4096), b""):
            sha1_hash.update(chunk)
    return sha1_hash.hexdigest()

def run_command(cmd, cwd=None):
    """Run a command and return the result"""
    try:
        result = subprocess.run(
            cmd, 
            shell=True, 
            cwd=cwd, 
            capture_output=True, 
            text=True, 
            check=True
        )
        return True, result.stdout, result.stderr
    except subprocess.CalledProcessError as e:
        return False, e.stdout, e.stderr

def setup_build_directory():
    """Create and setup the build output directory"""
    build_dir = Path("build_output")
    
    # Clean up any existing build directory
    if build_dir.exists():
        print_info("Cleaning up previous build directory...")
        shutil.rmtree(build_dir)
    
    # Create new build directory
    build_dir.mkdir()
    print_success(f"Created build directory: {build_dir.absolute()}")
    
    return build_dir

def check_dependencies():
    """Check if required tools are available"""
    print_header("CHECKING DEPENDENCIES")
    
    dependencies = []
    
    # Check if RGBDS is available
    rgbds_path = Path("rgbds")
    if rgbds_path.exists():
        rgbasm_path = rgbds_path / "rgbasm.exe"
        if rgbasm_path.exists():
            print_success("RGBDS found in local directory")
            dependencies.append(("RGBDS", str(rgbds_path.absolute())))
        else:
            print_error("RGBDS directory found but rgbasm.exe missing")
            return False, dependencies
    else:
        print_error("RGBDS not found in local directory")
        return False, dependencies
    
    # Check if make is available
    success, stdout, stderr = run_command("make --version")
    if success:
        version_line = stdout.split('\n')[0]
        print_success(f"Make found: {version_line}")
        dependencies.append(("Make", version_line))
    else:
        print_error("Make not found")
        return False, dependencies
    
    return True, dependencies

def clean_previous_build():
    """Manually clean previous build files"""
    print_info("Cleaning previous build files...")
    
    # Files to clean
    files_to_clean = [
        # ROM files
        "pokered.gbc", "pokeblue.gbc", "pokeblue_debug.gbc",
        # Symbol files
        "pokered.sym", "pokeblue.sym", "pokeblue_debug.sym",
        # Map files
        "pokered.map", "pokeblue.map", "pokeblue_debug.map",
        # Object files
        "rgbdscheck.o"
    ]
    
    # Object file patterns
    obj_patterns = [
        "audio_*.o", "home_*.o", "main_*.o", "maps_*.o", "ram_*.o", "text_*.o"
    ]
    
    cleaned_count = 0
    
    # Clean specific files
    for filename in files_to_clean:
        if Path(filename).exists():
            try:
                Path(filename).unlink()
                cleaned_count += 1
            except Exception as e:
                print_warning(f"Could not delete {filename}: {e}")
    
    # Clean object files
    for pattern in obj_patterns:
        for filepath in Path(".").glob(pattern):
            try:
                filepath.unlink()
                cleaned_count += 1
            except Exception as e:
                print_warning(f"Could not delete {filepath}: {e}")
    
    # Clean gfx object files
    gfx_dir = Path("gfx")
    if gfx_dir.exists():
        for obj_file in gfx_dir.glob("*.o"):
            try:
                obj_file.unlink()
                cleaned_count += 1
            except Exception as e:
                print_warning(f"Could not delete {obj_file}: {e}")
    
    if cleaned_count > 0:
        print_info(f"Cleaned {cleaned_count} files")

def build_roms():
    """Build the Pokemon ROMs"""
    print_header("BUILDING ROMS")
    
    # Clean previous build first
    clean_previous_build()
    
    # Build command with local RGBDS - just build, don't clean
    build_cmd = "make RGBDS=rgbds/"
    
    print_info(f"Running: {build_cmd}")
    success, stdout, stderr = run_command(build_cmd)
    
    if success:
        print_success("Build completed successfully!")
        return True
    else:
        print_error("Build failed!")
        print(f"STDOUT:\n{stdout}")
        print(f"STDERR:\n{stderr}")
        return False

def verify_checksums(build_dir):
    """Verify the checksums of built ROMs and copy them to build directory"""
    print_header("VERIFYING CHECKSUMS")
    
    all_passed = True
    results = {}
    
    for rom_name, expected_checksum in EXPECTED_CHECKSUMS.items():
        rom_path = Path(rom_name)
        
        if not rom_path.exists():
            print_error(f"{rom_name} not found!")
            all_passed = False
            results[rom_name] = {"status": "missing", "expected": expected_checksum}
            continue
        
        # Calculate actual checksum
        actual_checksum = calculate_sha1(rom_path)
        
        # Copy ROM to build directory
        dest_path = build_dir / rom_name
        shutil.copy2(rom_path, dest_path)
        
        # Compare checksums
        if actual_checksum.lower() == expected_checksum.lower():
            print_success(f"{rom_name}: CHECKSUM MATCH")
            print_info(f"  Expected: {expected_checksum}")
            print_info(f"  Actual:   {actual_checksum}")
            results[rom_name] = {"status": "match", "expected": expected_checksum, "actual": actual_checksum}
        else:
            print_error(f"{rom_name}: CHECKSUM MISMATCH!")
            print_info(f"  Expected: {expected_checksum}")
            print_info(f"  Actual:   {actual_checksum}")
            all_passed = False
            results[rom_name] = {"status": "mismatch", "expected": expected_checksum, "actual": actual_checksum}
    
    return all_passed, results

def cleanup_temp_files():
    """Clean up temporary build files"""
    print_header("CLEANING UP TEMPORARY FILES")
    
    # Save ROMs to temp location
    temp_roms = []
    for rom_name in EXPECTED_CHECKSUMS.keys():
        if Path(rom_name).exists():
            temp_path = Path(f"{rom_name}.temp")
            shutil.copy2(rom_name, temp_path)
            temp_roms.append((rom_name, temp_path))
    
    # Clean up
    clean_previous_build()
    
    # Clean up tools
    success, stdout, stderr = run_command("make clean -C tools/")
    if success:
        print_success("Tool build files cleaned up")
    else:
        print_warning("Could not clean up tool files")
    
    # Remove temp files
    for rom_name, temp_path in temp_roms:
        if temp_path.exists():
            temp_path.unlink()

def generate_report(dependencies, results, build_dir):
    """Generate a build report"""
    report_path = build_dir / "build_report.txt"
    
    with open(report_path, 'w') as f:
        f.write("Pokemon Red/Blue Build Verification Report\n")
        f.write("=" * 50 + "\n\n")
        
        f.write("Dependencies:\n")
        for dep_name, dep_info in dependencies:
            f.write(f"  {dep_name}: {dep_info}\n")
        f.write("\n")
        
        f.write("ROM Verification Results:\n")
        for rom_name, result in results.items():
            f.write(f"  {rom_name}:\n")
            f.write(f"    Status: {result['status']}\n")
            f.write(f"    Expected: {result.get('expected', 'N/A')}\n")
            f.write(f"    Actual: {result.get('actual', 'N/A')}\n")
        f.write("\n")
        
        # Overall result
        all_match = all(r['status'] == 'match' for r in results.values())
        f.write(f"Overall Result: {'PASS' if all_match else 'FAIL'}\n")
    
    print_success(f"Build report generated: {report_path.absolute()}")

def main():
    """Main function"""
    print_header("POKEMON RED/BLUE BUILD VERIFICATION")
    print_info("This script will build Pokemon Red/Blue and verify checksums")
    print_info("Perfect for verifying that code changes don't affect the ROM output\n")
    
    try:
        # Check dependencies
        deps_ok, dependencies = check_dependencies()
        if not deps_ok:
            print_error("Dependency check failed!")
            sys.exit(1)
        
        # Setup build directory
        build_dir = setup_build_directory()
        
        # Build ROMs
        if not build_roms():
            print_error("Build failed!")
            sys.exit(1)
        
        # Verify checksums
        checksums_ok, results = verify_checksums(build_dir)
        
        # Clean up temporary files
        cleanup_temp_files()
        
        # Generate report
        generate_report(dependencies, results, build_dir)
        
        # Final result
        print_header("FINAL RESULT")
        if checksums_ok:
            print_success("🎉 ALL CHECKSUMS MATCH! 🎉")
            print_success("Your code changes do not affect the ROM output.")
            print_info(f"ROMs available in: {build_dir.absolute()}")
            sys.exit(0)
        else:
            print_error("❌ CHECKSUM VERIFICATION FAILED! ❌")
            print_error("Your code changes have affected the ROM output.")
            print_info(f"Check the report in: {build_dir.absolute()}")
            sys.exit(1)
            
    except KeyboardInterrupt:
        print_error("\nBuild interrupted by user")
        sys.exit(1)
    except Exception as e:
        print_error(f"Unexpected error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main() 