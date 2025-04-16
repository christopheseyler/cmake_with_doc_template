# Test-Requirement Coherence Checker

This tool helps verify the coherence between test case descriptions and the requirements they claim to check.

## Usage

```bash
python check_test_requirement_coherence.py --src-dir <path_to_source_code> --req-dir <path_to_requirements>
```

Optional flags:
- `--verbose`: Show more detailed information about mismatches

## What it checks

1. Tests referencing non-existent requirements
2. Requirements not covered by any test
3. Potential misalignment between test cases and the requirements they check
4. Test cases that don't follow the GIVEN/WHEN/THEN format

## How to interpret results

- **ERROR**: Critical issues that should be fixed (e.g., referencing non-existent requirements)
- **WARNING**: Potential issues that should be reviewed (e.g., misalignment or missing coverage)

## Integration with CI/CD

You can add this script to your CI/CD pipeline to ensure continued alignment between tests and requirements:

```yaml
test-requirement-coherence:
  script:
    - python tools/check_test_requirement_coherence.py --src-dir src --req-dir docs/requirements
  allow_failure: true  # Initially allow failures until all issues are fixed
```
