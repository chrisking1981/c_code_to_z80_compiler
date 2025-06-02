#!/usr/bin/env python3
"""
Assembly File Comparison Tool
Compares original assembly files with compiled ones and provides detailed scoring.
"""

import os
import re
from pathlib import Path
from difflib import SequenceMatcher


class AssemblyComparer:
    def __init__(self):
        self.original_dir = "pokered/engine/overworld"
        self.compiled_dir = "compiled_asm/engine/overworld"
        
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
        files_to_compare = [
            'cut.asm',
            'auto_movement.asm',
            'dust_smoke.asm',
            'elevator.asm'
        ]
        
        results = []
        total_functional_similarity = 0
        perfect_matches = 0
        
        print("🔍 ASSEMBLY FILE COMPARISON ANALYSIS")
        print("=" * 60)
        
        for filename in files_to_compare:
            original_path = os.path.join(self.original_dir, filename)
            compiled_path = os.path.join(self.compiled_dir, filename)
            
            result = self.compare_files(original_path, compiled_path)
            if result:
                results.append(result)
                total_functional_similarity += result['functional_similarity']
                if result['perfect_match']:
                    perfect_matches += 1
                
                self.print_file_analysis(result)
        
        # Overall summary
        if results:
            avg_similarity = total_functional_similarity / len(results)
            print("\n📊 OVERALL SUMMARY")
            print("=" * 60)
            print(f"Files analyzed: {len(results)}")
            print(f"Perfect matches: {perfect_matches}/{len(results)}")
            print(f"Average functional similarity: {avg_similarity:.1%}")
            print(f"Overall grade: {self.get_grade(avg_similarity)}")
            
            # Quality assessment
            if avg_similarity >= 0.95:
                print("🏆 EXCELLENT - Near perfect compilation!")
            elif avg_similarity >= 0.85:
                print("✅ VERY GOOD - Minor differences only")
            elif avg_similarity >= 0.70:
                print("🔧 GOOD - Some issues to fix")
            elif avg_similarity >= 0.50:
                print("⚠️  FAIR - Major improvements needed")
            else:
                print("❌ POOR - Significant problems")
        
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