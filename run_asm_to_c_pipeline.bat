@echo off
setlocal enabledelayedexpansion

echo ======================================================================
echo  🚀 ASM-to-C AUTOMATED PIPELINE 🚀
echo ======================================================================
echo  This script automates the complete ASM-to-C conversion and testing
echo  pipeline for Game Boy pokered assembly files.
echo ======================================================================

:: Set timestamp for this run
set timestamp=%date:~-4%%date:~3,2%%date:~0,2%_%time:~0,2%%time:~3,2%%time:~6,2%
set timestamp=%timestamp: =0%

echo 📅 Pipeline started at: %timestamp%
echo.

echo ======================================================================
echo  🧹 CLEANUP: Clearing old data
echo ======================================================================
echo  Removing old C and compiled ASM files for fresh run...

:: Clear c_code directory
if exist "c_code" (
    echo  🗑️  Clearing c_code directory...
    rmdir /s /q "c_code"
)

:: Clear compiled_asm directory
if exist "compiled_asm" (
    echo  🗑️  Clearing compiled_asm directory...
    rmdir /s /q "compiled_asm"
)

echo ✅ Cleanup completed - starting with fresh directories!
echo.

:: Define the 5 target ASM files
set "files=pokered/engine/overworld/auto_movement.asm pokered/engine/overworld/elevator.asm pokered/engine/overworld/emotion_bubbles.asm pokered/engine/overworld/dust_smoke.asm pokered/engine/overworld/cut.asm"

:: Choose model (can be modified)
set "model=gpt-4o-mini"

echo ======================================================================
echo  📝 STEP 1: ASM → C CONVERSION (Using %model%)
echo ======================================================================
echo  Converting 5 ASM files to C using improved function-first prompt...
echo.

python asm_to_c_converter.py --files %files% --model %model%

if !errorlevel! neq 0 (
    echo ❌ ERROR: ASM to C conversion failed!
    echo    Check your API key and network connection.
    pause
    exit /b 1
)

echo ✅ ASM to C conversion completed successfully!
echo.

echo ======================================================================
echo  🔧 STEP 2: C → ASM COMPILATION (Function-first ordering)
echo ======================================================================
echo  Compiling all C files to ASM using improved compiler...
echo.

:: Compile each C file to ASM individually
python c_to_z80_compiler.py --file c_code/engine/overworld/auto_movement.c
python c_to_z80_compiler.py --file c_code/engine/overworld/elevator.c
python c_to_z80_compiler.py --file c_code/engine/overworld/emotion_bubbles.c
python c_to_z80_compiler.py --file c_code/engine/overworld/dust_smoke.c
python c_to_z80_compiler.py --file c_code/engine/overworld/cut.c

if !errorlevel! neq 0 (
    echo ❌ ERROR: C to ASM compilation failed!
    echo    Check generated C files for syntax errors.
    pause
    exit /b 1
)

echo ✅ C to ASM compilation completed successfully!
echo.

echo ======================================================================
echo  📊 STEP 3: COMPARISON & SCORING
echo ======================================================================
echo  Comparing original ASM with compiled ASM files...
echo.

python compare_assembly.py pokered/engine/overworld/ compiled_asm/engine/overworld/ --detailed

if !errorlevel! neq 0 (
    echo ❌ ERROR: Assembly comparison failed!
    pause
    exit /b 1
)

echo.
echo ======================================================================
echo  🎉 PIPELINE COMPLETED SUCCESSFULLY! 🎉
echo ======================================================================
echo  📁 Results saved in: comparison_results/
echo  📈 Check the summary file for overall score and analysis
echo  ⏱️  Pipeline completed at: %time%
echo.
echo  💡 TIP: Look for the latest summary_[timestamp].txt file
echo      in comparison_results/ for detailed analysis.
echo ======================================================================

pause 