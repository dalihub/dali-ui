#!/usr/bin/env python3
"""
Convert TCT test result XML to GTest XML format.

This script reads the TCT test result XML files and converts them to
GTest-compatible XML format for better integration with CI/CD tools.

Usage:
    python3 convert-to-gtest.py [output_file]

If output_file is not specified, outputs to stdout.
"""

import xml.etree.ElementTree as ET
import sys
from datetime import datetime
from pathlib import Path


def parse_timestamp(timestamp_str):
    """Parse TCT timestamp format to datetime."""
    if not timestamp_str:
        return datetime.now()

    timestamp_str = timestamp_str.strip()

    try:
        # Format: "Thu Feb  5 20:44:19 2026" (note: double space for single digit day)
        # Try with double space first
        return datetime.strptime(timestamp_str, "%a %b  %d %H:%M:%S %Y")
    except ValueError:
        try:
            # Try with single space
            return datetime.strptime(timestamp_str, "%a %b %d %H:%M:%S %Y")
        except ValueError:
            try:
                # Try alternative format: "2026-02-05_20_44_19"
                return datetime.strptime(timestamp_str, "%Y-%m-%d_%H_%M_%S")
            except ValueError:
                return datetime.now()


def calculate_time_ms(start_str, end_str):
    """Calculate time in milliseconds between start and end timestamps."""
    start = parse_timestamp(start_str)
    end = parse_timestamp(end_str)
    return int((end - start).total_seconds() * 1000)


def convert_testcase(testcase_elem):
    """Convert a TCT testcase element to GTest format."""
    test_id = testcase_elem.get('id', 'Unknown')
    result = testcase_elem.get('result', 'UNKNOWN')

    # Find result_info
    result_info = testcase_elem.find('result_info')
    if result_info is None:
        return None

    # Get timestamps
    start_elem = result_info.find('start')
    end_elem = result_info.find('end')

    start_time = start_elem.text if start_elem is not None else ''
    end_time = end_elem.text if end_elem is not None else ''

    # Calculate time in milliseconds
    time_ms = calculate_time_ms(start_time, end_time)

    # Create testcase element
    gtest_testcase = ET.Element('testcase')
    gtest_testcase.set('name', test_id)
    gtest_testcase.set('time', f'{time_ms}')

    # Add failure element if test failed
    if result == 'FAIL':
        failure_elem = ET.SubElement(gtest_testcase, 'failure')

        # Get stderr for failure message
        stderr_elem = result_info.find('stderr')
        if stderr_elem is not None and stderr_elem.text:
            failure_elem.set('message', stderr_elem.text.strip())
            failure_elem.text = stderr_elem.text
        else:
            failure_elem.set('message', 'Test failed')

    return gtest_testcase


def convert_suite(suite_file):
    """Convert a TCT suite XML file to GTest format."""
    try:
        tree = ET.parse(suite_file)
        root = tree.getroot()
    except Exception as e:
        print(f"Error parsing {suite_file}: {e}", file=sys.stderr)
        return None

    # Find suite element (try multiple possible paths)
    suite_elem = root.find('.//suite')
    if suite_elem is None:
        print(f"Warning: No suite element found in {suite_file.name}", file=sys.stderr)
        return None

    suite_name = suite_elem.get('name', 'Unknown')

    # Get suite timing from summary
    summary = root.find('.//summary')
    suite_time = 0
    if summary is not None:
        start_at = summary.find('start_at')
        end_at = summary.find('end_at')
        if start_at is not None and end_at is not None:
            # Parse timestamps: "2026-02-05_20_44_19"
            try:
                start_dt = datetime.strptime(start_at.text, "%Y-%m-%d_%H_%M_%S")
                end_dt = datetime.strptime(end_at.text, "%Y-%m-%d_%H_%M_%S")
                suite_time = int((end_dt - start_dt).total_seconds() * 1000)
            except ValueError:
                pass

    # Create testsuite element
    gtest_testsuite = ET.Element('testsuite')
    gtest_testsuite.set('name', suite_name)
    gtest_testsuite.set('tests', '0')  # Will count actual tests
    gtest_testsuite.set('failures', '0')  # Will count actual failures
    gtest_testsuite.set('disabled', '0')
    gtest_testsuite.set('errors', '0')
    gtest_testsuite.set('time', str(suite_time))

    # Convert all testcases and count tests/failures
    test_count = 0
    failure_count = 0
    for testcase in root.findall('.//testcase'):
        gtest_testcase = convert_testcase(testcase)
        if gtest_testcase is not None:
            gtest_testsuite.append(gtest_testcase)
            test_count += 1
            # Count failures
            if testcase.get('result') == 'FAIL':
                failure_count += 1

    # Update test and failure counts
    gtest_testsuite.set('tests', str(test_count))
    gtest_testsuite.set('failures', str(failure_count))

    return gtest_testsuite


def main():
    """Main function."""
    # Get script directory
    script_dir = Path(__file__).parent
    automated_tests_dir = script_dir.parent

    # Find all TCT test result XML files
    suite_files = [
        automated_tests_dir / 'tct-dali-ui-foundation-core-tests.xml',
        automated_tests_dir / 'tct-dali-ui-components-core-tests.xml',
    ]

    # Create root testsuites element
    gtest_root = ET.Element('testsuites')

    total_tests = 0
    total_failures = 0
    total_time = 0

    # Convert each suite
    for suite_file in suite_files:
        if not suite_file.exists():
            print(f"Warning: {suite_file} not found, skipping", file=sys.stderr)
            continue

        gtest_testsuite = convert_suite(suite_file)
        if gtest_testsuite is not None:
            gtest_root.append(gtest_testsuite)
            tests = int(gtest_testsuite.get('tests', 0))
            failures = int(gtest_testsuite.get('failures', 0))
            total_tests += tests
            total_failures += failures
            print(f"Converted {suite_file.name}: {tests} tests, {failures} failures")

    # Set root attributes
    gtest_root.set('tests', str(total_tests))
    gtest_root.set('failures', str(total_failures))
    gtest_root.set('disabled', '0')
    gtest_root.set('errors', '0')
    gtest_root.set('time', str(total_time))
    gtest_root.set('timestamp', datetime.now().isoformat())

    # Create tree and output
    tree = ET.ElementTree(gtest_root)

    # Output to file or stdout
    if len(sys.argv) > 1:
        output_file = sys.argv[1]
        tree.write(output_file, encoding='utf-8', xml_declaration=True)
        print(f"GTest XML written to {output_file}")
    else:
        tree.write(sys.stdout, encoding='unicode', xml_declaration=True)


if __name__ == '__main__':
    main()
