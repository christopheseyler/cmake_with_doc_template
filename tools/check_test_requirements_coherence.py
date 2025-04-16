#!/usr/bin/env python3
# filepath: c:\tmp\github\cmake_with_doc_template\tools\check_test_requirement_coherence.py

import os
import re
import argparse
from typing import Dict, List, Set, Tuple, Optional
import glob
import sys

class Requirement:
    def __init__(self, req_id: str, title: str, description: str):
        self.req_id = req_id
        self.title = title
        self.description = description

class TestCase:
    def __init__(self, test_id: str, description: str, checks: List[str], file_path: str):
        self.test_id = test_id
        self.description = description
        self.checks = checks
        self.file_path = file_path

def extract_requirements_from_rst(rst_file: str) -> List[Requirement]:
    """Extract requirements from an RST file using sphinx-needs format."""
    requirements = []
    
    with open(rst_file, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Look for list2need blocks that define requirements
    list2need_blocks = re.findall(r'\.\.\ list2need::(.*?)(?=\.\.\ (?!\ )|\Z)', content, re.DOTALL)
    
    for block in list2need_blocks:
        if 'types: sss' in block or 'types: srs' in block:
            # Extract individual requirements from the block
            req_items = re.findall(r'\*\s+\(([\w-]+)\)\s+(.*?)\.\s+(.*?)(?=\*|\Z)', block, re.DOTALL)
            
            for req_id, title, description in req_items:
                req_id = req_id.strip()
                title = title.strip()
                description = description.strip()
                requirements.append(Requirement(req_id, title, description))
    
    return requirements

def extract_tests_from_cpp(cpp_file: str) -> List[TestCase]:
    """Extract test cases from C++ files with @utdef annotations."""
    tests = []
    
    with open(cpp_file, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()
    
    # Extract test definition blocks
    test_blocks = re.findall(r'/\*\*\s+@utdef\{(.*?)\}(.*?)@endut\s*\*/', content, re.DOTALL)
    
    for test_header, test_body in test_blocks:
        # Extract test ID and description from header
        test_match = re.match(r'([\w-]+)\s*\|\s*(.*)', test_header.strip())
        if not test_match:
            continue
        
        test_id, test_description = test_match.groups()
        test_id = test_id.strip()
        test_description = test_description.strip()
        
        # Extract checks field
        checks_match = re.search(r':checks:\s*(.*?)(?=:[a-z]+:|$)', test_body, re.DOTALL)
        checks = []
        if checks_match:
            checks_str = checks_match.group(1).strip()
            checks = [check.strip() for check in checks_str.split(',')]
        
        tests.append(TestCase(test_id, test_description, checks, cpp_file))
    
    return tests

def analyze_coherence(tests: List[TestCase], requirements: Dict[str, Requirement], verbose: bool = False) -> List[str]:
    """Analyze coherence between tests and requirements."""
    issues = []
    
    # Check test coverage of requirements
    covered_reqs = set()
    for test in tests:
        for req_id in test.checks:
            if req_id not in requirements:
                issues.append(f"ERROR: Test {test.test_id} references non-existent requirement {req_id} in {test.file_path}")
            else:
                covered_reqs.add(req_id)
                
                # Check if the test description aligns with the requirement
                req = requirements[req_id]
                
                # Extract GIVEN/WHEN/THEN structure from test description
                gwt_match = re.search(r'- GIVEN (.*?)(?:- WHEN (.*?))?(?:- THEN (.*?))?(?=$|\n\n)', test.description, re.DOTALL)
                
                if not gwt_match:
                    issues.append(f"WARNING: Test {test.test_id} checking {req_id} does not follow GIVEN/WHEN/THEN format")
                else:
                    given = gwt_match.group(1).strip() if gwt_match.group(1) else ""
                    when = gwt_match.group(2).strip() if gwt_match.group(2) else ""
                    then = gwt_match.group(3).strip() if gwt_match.group(3) else ""
                    
                    # Heuristic check: Look for keywords from requirement in the test description
                    req_key_terms = set(re.findall(r'\b\w+\b', req.description.lower()))
                    test_key_terms = set(re.findall(r'\b\w+\b', (given + " " + when + " " + then).lower()))
                    
                    # Check if at least some terms match (simple heuristic)
                    matching_terms = req_key_terms.intersection(test_key_terms)
                    if len(matching_terms) < 3:  # Arbitrary threshold
                        issues.append(f"WARNING: Test {test.test_id} and requirement {req_id} may not be aligned - few matching terms")
                        if verbose:
                            issues.append(f"  Requirement: {req.description}")
                            issues.append(f"  Test: GIVEN {given} WHEN {when} THEN {then}")
    
    # Check for requirements without tests
    for req_id, req in requirements.items():
        if req_id not in covered_reqs:
            issues.append(f"WARNING: Requirement {req_id} ({req.title}) is not covered by any test")
    
    return issues

def main():
    parser = argparse.ArgumentParser(description='Check coherence between test cases and requirements')
    parser.add_argument('--src-dir', type=str, help='Source directory to scan', required=True)
    parser.add_argument('--req-dir', type=str, help='Requirements directory to scan', required=True)
    parser.add_argument('--verbose', action='store_true', help='Show verbose information')
    args = parser.parse_args()
    
    # Find all C++ files that might contain tests
    cpp_files = glob.glob(os.path.join(args.src_dir, '**', '*.cpp'), recursive=True)
    cpp_files += glob.glob(os.path.join(args.src_dir, '**', '*.cc'), recursive=True)
    cpp_files += glob.glob(os.path.join(args.src_dir, '**', '*.h'), recursive=True)
    cpp_files += glob.glob(os.path.join(args.src_dir, '**', '*.hpp'), recursive=True)
    
    # Find all RST files that might contain requirements
    rst_files = glob.glob(os.path.join(args.req_dir, '**', '*.rst'), recursive=True)
    
    # Extract tests and requirements
    all_tests = []
    for cpp_file in cpp_files:
        try:
            tests = extract_tests_from_cpp(cpp_file)
            all_tests.extend(tests)
        except Exception as e:
            print(f"Error processing {cpp_file}: {e}", file=sys.stderr)
    
    all_requirements = {}
    for rst_file in rst_files:
        try:
            reqs = extract_requirements_from_rst(rst_file)
            for req in reqs:
                all_requirements[req.req_id] = req
        except Exception as e:
            print(f"Error processing {rst_file}: {e}", file=sys.stderr)
    
    # Analyze coherence
    issues = analyze_coherence(all_tests, all_requirements, args.verbose)
    
    # Report statistics
    print(f"Found {len(all_tests)} tests and {len(all_requirements)} requirements")
    print(f"Found {len(issues)} issues")
    
    # Print all issues
    for issue in issues:
        print(issue)
    
    # Return non-zero exit code if any issues found
    return 1 if issues else 0

if __name__ == "__main__":
    sys.exit(main())