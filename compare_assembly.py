#!/usr/bin/env python3
"""
Assembly File Comparison Tool
Compares original assembly files with compiled ones and provides detailed scoring.
"""

import os
import re
from pathlib import Path
from difflib import SequenceMatcher
from datetime import datetime


class AssemblyComparer:
    def __init__(self):
        self.original_dir = "pokered/engine/overworld"
        self.compiled_dir = "compiled_asm/engine/overworld"
        self.results_dir = "comparison_results"
        
        # Create results directory if it doesn't exist
        Path(self.results_dir).mkdir(exist_ok=True)
        
    def normalize_line(self, line):
        """Normalize a line for comparison by removing extra whitespace and standardizing format"""
        line = line.strip()
        # Remove multiple tabs/spaces
        line = re.sub(r'\s+', ' ', line)
        return line
    
    def is_functional_line(self, line):
        """Check if a line contains functional assembly code (not just comments or empty)"""
        line = line.strip()
        if not line:
            return False
        if line.startswith(';'):
            return False
        if line.startswith('//'):
            return False
        # Remove inline comments for comparison
        if ';' in line:
            line = line.split(';')[0].strip()
        if '//' in line:
            line = line.split('//')[0].strip()
        return bool(line)
    
    def extract_functional_lines(self, content):
        """Extract only functional assembly lines"""
        lines = content.split('\n')
        functional = []
        for line in lines:
            if self.is_functional_line(line):
                normalized = self.normalize_line(line)
                # Remove inline comments for functional comparison
                if ';' in normalized:
                    normalized = normalized.split(';')[0].strip()
                if '//' in normalized:
                    normalized = normalized.split('//')[0].strip()
                if normalized:
                    functional.append(normalized)
        return functional
    
    def compare_files(self, original_file, compiled_file):
        """Compare two assembly files and return detailed results"""
        try:
            with open(original_file, 'r', encoding='utf-8', errors='ignore') as f:
                original_content = f.read()
        except FileNotFoundError:
            return None
        
        try:
            with open(compiled_file, 'r', encoding='utf-8', errors='ignore') as f:
                compiled_content = f.read()
        except FileNotFoundError:
            return None
        
        # Get all lines
        original_lines = [line.rstrip() for line in original_content.split('\n')]
        compiled_lines = [line.rstrip() for line in compiled_content.split('\n')]
        
        # Get functional lines only
        original_functional = self.extract_functional_lines(original_content)
        compiled_functional = self.extract_functional_lines(compiled_content)
        
        # Calculate similarity scores
        all_lines_similarity = SequenceMatcher(None, original_lines, compiled_lines).ratio()
        functional_similarity = SequenceMatcher(None, original_functional, compiled_functional).ratio()
        
        # Line count comparison
        original_line_count = len([l for l in original_lines if l.strip()])
        compiled_line_count = len([l for l in compiled_lines if l.strip()])
        
        # Find specific differences
        differences = []
        max_compare = min(len(original_functional), len(compiled_functional))
        
        for i in range(max_compare):
            if original_functional[i] != compiled_functional[i]:
                differences.append({
                    'line': i + 1,
                    'original': original_functional[i],
                    'compiled': compiled_functional[i]
                })
                if len(differences) >= 10:  # Limit to first 10 differences
                    break
        
        # Extra lines
        if len(original_functional) != len(compiled_functional):
            if len(original_functional) > len(compiled_functional):
                missing_lines = original_functional[len(compiled_functional):]
                differences.append({
                    'type': 'missing_in_compiled',
                    'lines': missing_lines[:5]  # Show first 5
                })
            else:
                extra_lines = compiled_functional[len(original_functional):]
                differences.append({
                    'type': 'extra_in_compiled', 
                    'lines': extra_lines[:5]  # Show first 5
                })
        
        return {
            'original_file': original_file,
            'compiled_file': compiled_file,
            'all_lines_similarity': all_lines_similarity,
            'functional_similarity': functional_similarity,
            'original_line_count': original_line_count,
            'compiled_line_count': compiled_line_count,
            'original_functional_count': len(original_functional),
            'compiled_functional_count': len(compiled_functional),
            'differences': differences,
            'perfect_match': functional_similarity == 1.0
        }
    
    def compare_all_files(self):
        """Compare all assembly files and return summary"""
        # Automatically find all .asm files in compiled directory
        compiled_path = Path(self.compiled_dir)
        if not compiled_path.exists():
            print(f"❌ Compiled directory not found: {self.compiled_dir}")
            return []
        
        asm_files = list(compiled_path.glob("*.asm"))
        if not asm_files:
            print(f"❌ No .asm files found in {self.compiled_dir}")
            return []
        
        files_to_compare = [f.name for f in asm_files]
        files_to_compare.sort()  # Sort alphabetically for consistent order
        
        results = []
        total_functional_similarity = 0
        perfect_matches = 0
        summary_lines = []
        
        summary_lines.append("🔍 ASSEMBLY FILE COMPARISON ANALYSIS")
        summary_lines.append("=" * 60)
        summary_lines.append(f"Found {len(files_to_compare)} files to compare")
        summary_lines.append("")
        print("🔍 ASSEMBLY FILE COMPARISON ANALYSIS")
        print("=" * 60)
        print(f"Found {len(files_to_compare)} files to compare")
        print("")
        
        for filename in files_to_compare:
            original_path = os.path.join(self.original_dir, filename)
            compiled_path = os.path.join(self.compiled_dir, filename)
            
            result = self.compare_files(original_path, compiled_path)
            if result:
                results.append(result)
                total_functional_similarity += result['functional_similarity']
                if result['perfect_match']:
                    perfect_matches += 1
                
                file_summary = self.get_file_summary(result)
                summary_lines.extend(file_summary)
                self.print_file_analysis(result)
            else:
                # File not found
                missing_msg = f"\n📄 {filename}\n" + "-" * 40 + f"\n❌ ORIGINAL FILE NOT FOUND\n"
                summary_lines.append(missing_msg)
                print(missing_msg)
        
        # Overall summary
        if results:
            avg_similarity = total_functional_similarity / len(results)
            
            overall_summary = [
                "\n📊 OVERALL SUMMARY",
                "=" * 60,
                f"Files analyzed: {len(results)}",
                f"Perfect matches: {perfect_matches}/{len(results)}",
                f"Average functional similarity: {avg_similarity:.1%}",
                f"Overall grade: {self.get_grade(avg_similarity)}"
            ]
            
            # Quality assessment
            if avg_similarity >= 0.95:
                overall_summary.append("🏆 EXCELLENT - Near perfect compilation!")
            elif avg_similarity >= 0.85:
                overall_summary.append("✅ VERY GOOD - Minor differences only")
            elif avg_similarity >= 0.70:
                overall_summary.append("🔧 GOOD - Some issues to fix")
            elif avg_similarity >= 0.50:
                overall_summary.append("⚠️  FAIR - Major improvements needed")
            else:
                overall_summary.append("❌ POOR - Significant problems")
            
            summary_lines.extend(overall_summary)
            for line in overall_summary:
                print(line)
        
        # Save results to files
        summary_text = '\n'.join(summary_lines)
        self.save_results(results, summary_text)
        
        return results
    
    def print_file_analysis(self, result):
        """Print detailed analysis for a single file"""
        filename = os.path.basename(result['compiled_file'])
        print(f"\n📄 {filename}")
        print("-" * 40)
        print(f"Functional similarity: {result['functional_similarity']:.1%}")
        print(f"Overall similarity: {result['all_lines_similarity']:.1%}")
        print(f"Line count - Original: {result['original_functional_count']}, Compiled: {result['compiled_functional_count']}")
        
        if result['perfect_match']:
            print("✅ PERFECT MATCH!")
        else:
            print(f"❌ {len([d for d in result['differences'] if 'line' in d])} functional differences found")
            
            # Show first few differences
            for diff in result['differences'][:3]:
                if 'line' in diff:
                    print(f"  Line {diff['line']}:")
                    print(f"    Original:  {diff['original']}")
                    print(f"    Compiled:  {diff['compiled']}")
                elif diff['type'] == 'missing_in_compiled':
                    print(f"  Missing in compiled: {len(diff['lines'])} lines")
                    for line in diff['lines'][:2]:
                        print(f"    {line}")
                elif diff['type'] == 'extra_in_compiled':
                    print(f"  Extra in compiled: {len(diff['lines'])} lines")
                    for line in diff['lines'][:2]:
                        print(f"    {line}")
    
    def get_grade(self, similarity):
        """Convert similarity score to letter grade"""
        if similarity >= 0.95:
            return "A+"
        elif similarity >= 0.90:
            return "A"
        elif similarity >= 0.85:
            return "B+"
        elif similarity >= 0.80:
            return "B"
        elif similarity >= 0.75:
            return "B-"
        elif similarity >= 0.70:
            return "C+"
        elif similarity >= 0.65:
            return "C"
        elif similarity >= 0.60:
            return "C-"
        elif similarity >= 0.50:
            return "D"
        else:
            return "F"
    
    def save_results(self, results, summary_text):
        """Save comparison results to timestamped files organized by file"""
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        
        # Save summary in main results directory
        summary_file = Path(self.results_dir) / f"summary_{timestamp}.txt"
        with open(summary_file, 'w', encoding='utf-8') as f:
            f.write(summary_text)
        
        # Save detailed results for each file in separate folders
        for result in results:
            filename = os.path.basename(result['compiled_file']).replace('.asm', '')
            similarity_pct = f"{result['functional_similarity']:.1%}"
            
            # Create file-specific directory
            file_dir = Path(self.results_dir) / filename
            file_dir.mkdir(exist_ok=True)
            
            # Save detailed result with similarity in filename
            detail_file = file_dir / f"{filename}_{similarity_pct}_{timestamp}.txt"
            
            with open(detail_file, 'w', encoding='utf-8') as f:
                f.write(f"=== DETAILED COMPARISON: {filename}.asm ===\n")
                f.write(f"Timestamp: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n\n")
                f.write(f"Functional similarity: {result['functional_similarity']:.1%}\n")
                f.write(f"Overall similarity: {result['all_lines_similarity']:.1%}\n")
                f.write(f"Original lines: {result['original_functional_count']}\n")
                f.write(f"Compiled lines: {result['compiled_functional_count']}\n")
                f.write(f"Perfect match: {result['perfect_match']}\n")
                f.write(f"Grade: {self.get_grade(result['functional_similarity'])}\n\n")
                
                if result['differences']:
                    f.write("=== DIFFERENCES ===\n")
                    for i, diff in enumerate(result['differences'], 1):
                        if 'line' in diff:
                            f.write(f"{i}. Line {diff['line']} mismatch:\n")
                            f.write(f"   Original:  {diff['original']}\n")
                            f.write(f"   Compiled:  {diff['compiled']}\n\n")
                        elif diff['type'] == 'missing_in_compiled':
                            f.write(f"{i}. Missing in compiled ({len(diff['lines'])} lines):\n")
                            for line in diff['lines']:
                                f.write(f"   {line}\n")
                            f.write("\n")
                        elif diff['type'] == 'extra_in_compiled':
                            f.write(f"{i}. Extra in compiled ({len(diff['lines'])} lines):\n")
                            for line in diff['lines']:
                                f.write(f"   {line}\n")
                            f.write("\n")
                else:
                    f.write("=== NO DIFFERENCES - PERFECT MATCH! ===\n")
        
        print(f"\n💾 Results saved to organized folders in {self.results_dir}/ with timestamp {timestamp}")
        print(f"📁 Structure:")
        for result in results:
            filename = os.path.basename(result['compiled_file']).replace('.asm', '')
            similarity_pct = f"{result['functional_similarity']:.1%}"
            print(f"   {self.results_dir}/{filename}/{filename}_{similarity_pct}_{timestamp}.txt")
        
        return timestamp
    
    def get_file_summary(self, result):
        """Get summary text for a single file"""
        filename = os.path.basename(result['compiled_file'])
        summary = [
            f"\n📄 {filename}",
            "-" * 40,
            f"Functional similarity: {result['functional_similarity']:.1%}",
            f"Overall similarity: {result['all_lines_similarity']:.1%}",
            f"Line count - Original: {result['original_functional_count']}, Compiled: {result['compiled_functional_count']}"
        ]
        
        if result['perfect_match']:
            summary.append("✅ PERFECT MATCH!")
        else:
            diff_count = len([d for d in result['differences'] if 'line' in d])
            summary.append(f"❌ {diff_count} functional differences found")
            
            # Show first few differences
            for diff in result['differences'][:3]:
                if 'line' in diff:
                    summary.extend([
                        f"  Line {diff['line']}:",
                        f"    Original:  {diff['original']}",
                        f"    Compiled:  {diff['compiled']}"
                    ])
                elif diff['type'] == 'missing_in_compiled':
                    summary.append(f"  Missing in compiled: {len(diff['lines'])} lines")
                    for line in diff['lines'][:2]:
                        summary.append(f"    {line}")
                elif diff['type'] == 'extra_in_compiled':
                    summary.append(f"  Extra in compiled: {len(diff['lines'])} lines")
                    for line in diff['lines'][:2]:
                        summary.append(f"    {line}")
        
        return summary


def main():
    comparer = AssemblyComparer()
    results = comparer.compare_all_files()
    
    # Additional detailed output for debugging
    if any(not r['perfect_match'] for r in results):
        print("\n🔧 DETAILED DIFFERENCE ANALYSIS")
        print("=" * 60)
        for result in results:
            if not result['perfect_match']:
                filename = os.path.basename(result['compiled_file'])
                print(f"\n{filename} - Issues found:")
                for i, diff in enumerate(result['differences'][:5]):
                    if 'line' in diff:
                        print(f"{i+1}. Line {diff['line']} mismatch")
                        print(f"   Expected: {diff['original']}")
                        print(f"   Got:      {diff['compiled']}")


if __name__ == '__main__':
    main() 