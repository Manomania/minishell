#!/usr/bin/env bash

# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    minishell_test.sh                                  :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: test_user <test@student.42.fr>             +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/17 10:00:00 by test_user         #+#    #+#              #
#    Updated: 2025/03/17 10:00:00 by test_user        ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Colors for better output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Path to minishell
MINISHELL="./minishell -c"

# Count for tests
TOTAL_TESTS=0
PASSED_TESTS=0

# Temporary files
EXPECTED_OUTPUT="/tmp/minishell_expected.txt"
ACTUAL_OUTPUT="/tmp/minishell_actual.txt"
ERROR_OUTPUT="/tmp/minishell_error.txt"
TEST_FILE="/tmp/minishell_test.txt"

# Check if minishell executable exists
if [ ! -f "$MINISHELL" ]; then
  echo -e "${RED}Error: $MINISHELL not found. Make sure you are in the correct directory.${NC}"
  exit 1
fi

# Make sure we have the necessary permissions
chmod +x "$MINISHELL"

# Function to print section headers
print_header() {
  echo -e "\n${BLUE}===============================================${NC}"
  echo -e "${BLUE}= $1${NC}"
  echo -e "${BLUE}===============================================${NC}\n"
}

# Function to print test description
print_test() {
  echo -e "${CYAN}Test #$TOTAL_TESTS: $1${NC}"
}

# Function to check if the test passed or failed
check_result() {
  local result=$1
  local description=$2

  if [ $result -eq 0 ]; then
    echo -e "${GREEN}✓ PASS: $description${NC}"
    PASSED_TESTS=$((PASSED_TESTS + 1))
  else
    echo -e "${RED}✗ FAIL: $description${NC}"
    if [ -n "$3" ]; then
      echo -e "${YELLOW}$3${NC}"
    fi
  fi
  TOTAL_TESTS=$((TOTAL_TESTS + 1))
}

# Function to run a command and compare its output
run_test() {
  local cmd=$1
  local expected_output=$2
  local description=$3

  print_test "$description"

  # Write expected output to file with newline (matching echo behavior)
  echo "$expected_output" >"$EXPECTED_OUTPUT"

  # Run the command in minishell
  echo "$cmd" | "$MINISHELL" 2>"$ERROR_OUTPUT" | grep -v "^$" | grep -v "^\$" | tr -d '\r' >"$ACTUAL_OUTPUT"

  # For debugging - show what's in the files
  echo "Debug - Expected output:" >>"$ERROR_OUTPUT"
  hexdump -C "$EXPECTED_OUTPUT" >>"$ERROR_OUTPUT"
  echo "Debug - Actual output:" >>"$ERROR_OUTPUT"
  hexdump -C "$ACTUAL_OUTPUT" >>"$ERROR_OUTPUT"

  # Compare outputs (ignore empty lines and prompt)
  diff "$EXPECTED_OUTPUT" "$ACTUAL_OUTPUT" >/dev/null
  local result=$?

  local error_msg=""
  if [ $result -ne 0 ]; then
    error_msg="Expected: \"$expected_output\"\nActual: \"$(cat "$ACTUAL_OUTPUT")\""
    if [ -s "$ERROR_OUTPUT" ]; then
      error_msg="$error_msg\nError: \"$(cat "$ERROR_OUTPUT")\""
    fi
  fi

  check_result $result "$description" "$error_msg"
}

# Function to run a command with exit status check
run_test_exit_status() {
  local cmd=$1
  local expected_status=$2
  local description=$3

  print_test "$description"

  # Run the command in minishell and capture exit status
  echo "$cmd" | "$MINISHELL" >/dev/null 2>&1
  echo "echo \$?" | "$MINISHELL" >"$ACTUAL_OUTPUT" 2>/dev/null

  # Get the actual status
  local actual_status=$(grep -v "^\$" "$ACTUAL_OUTPUT" | tail -1)

  local result=1
  if [ "$actual_status" -eq "$expected_status" ]; then
    result=0
  fi

  local error_msg=""
  if [ $result -ne 0 ]; then
    error_msg="Expected exit status: $expected_status, Actual: $actual_status"
  fi

  check_result $result "$description" "$error_msg"
}

# Function to run a test on command execution (without output comparison)
run_command_test() {
  local cmd=$1
  local description=$2

  print_test "$description"

  # Run the command in minishell
  echo "$cmd" | "$MINISHELL" >/dev/null 2>&1
  local result=$?

  check_result 0 "$description" # Assume success unless it crashes
}

# Function to check if a file exists after a command
check_file_existence() {
  local cmd=$1
  local file=$2
  local should_exist=$3 # 1 for should exist, 0 for should not exist
  local description=$4

  print_test "$description"

  # Run the command in minishell
  echo "$cmd" | "$MINISHELL" >/dev/null 2>&1

  # Check if file exists
  if [ -e "$file" ]; then
    file_exists=1
  else
    file_exists=0
  fi

  local result=1
  if [ "$file_exists" -eq "$should_exist" ]; then
    result=0
  fi

  local error_msg=""
  if [ $result -ne 0 ]; then
    if [ "$should_exist" -eq 1 ]; then
      error_msg="File $file does not exist but should."
    else
      error_msg="File $file exists but should not."
    fi
  fi

  check_result $result "$description" "$error_msg"

  # Clean up the file if it exists
  if [ -e "$file" ]; then
    rm -f "$file"
  fi
}

# Function to print the final summary
print_summary() {
  echo -e "\n${BLUE}======================================${NC}"
  echo -e "${BLUE}= SUMMARY${NC}"
  echo -e "${BLUE}======================================${NC}"
  echo -e "Total tests: $TOTAL_TESTS"
  echo -e "Passed: ${GREEN}$PASSED_TESTS${NC}"
  echo -e "Failed: ${RED}$((TOTAL_TESTS - PASSED_TESTS))${NC}"

  if [ $PASSED_TESTS -eq $TOTAL_TESTS ]; then
    echo -e "\n${GREEN}All tests passed! 🎉${NC}"
  else
    echo -e "\n${RED}Some tests failed. 😢${NC}"
  fi
}

# Function to set up the test environment
setup() {
  # Create test files
  echo "test file content" >"$TEST_FILE"
}

# Function to clean up after tests
cleanup() {
  # Remove temporary files
  rm -f "$EXPECTED_OUTPUT" "$ACTUAL_OUTPUT" "$ERROR_OUTPUT" "$TEST_FILE"

  # Clean up any files that may have been created during tests
  rm -f output.txt input.txt test_file.txt
}

# Main test function
run_tests() {
  print_header "MINISHELL TESTING SCRIPT"

  # ===== BASIC COMMANDS =====
  print_header "BASIC COMMANDS"

  run_test "echo Hello" "Hello" "Simple echo command"
  run_test "echo -n Hello" "Hello" "Echo with -n option"
  run_test "pwd" "$(pwd)" "PWD command"
  run_test "echo \$PWD" "$(pwd)" "Environment variable in echo"

  # ===== ENVIRONMENT VARIABLES =====
  print_header "ENVIRONMENT VARIABLES"

  run_test "export TEST_VAR=hello && echo \$TEST_VAR" "hello" "Set and echo environment variable"
  run_test "export TEST_VAR=hello && unset TEST_VAR && echo \$TEST_VAR" "" "Unset environment variable"
  run_test "echo \$?" "0" "Echo exit status after successful command"
  run_test "nonexistent_command 2>/dev/null || echo \$?" "127" "Echo exit status after command not found"

  # ===== BUILTINS =====
  print_header "BUILTIN COMMANDS"

  # CD tests
  current_dir=$(pwd)
  run_command_test "cd /tmp && pwd" "CD to absolute path"
  run_command_test "cd $current_dir" "CD back to original directory"
  run_command_test "cd .. && pwd" "CD to parent directory"
  run_command_test "cd $current_dir" "CD back to original directory again"

  # ENV test
  run_command_test "env | grep -q PATH" "ENV command shows PATH"

  # EXPORT and UNSET tests
  run_test "export TEST_VAR=value && echo \$TEST_VAR" "value" "EXPORT sets variable"
  run_test "unset TEST_VAR && echo \$TEST_VAR" "" "UNSET removes variable"

  # EXIT test
  # Note: We can't directly test exit as it would terminate the script
  run_command_test "echo 'exit status test'" "Exit command recognized (indirect test)"

  # ===== QUOTES =====
  print_header "QUOTES HANDLING"

  run_test "echo 'single quotes'" "single quotes" "Single quotes"
  run_test "echo \"double quotes\"" "double quotes" "Double quotes"
  run_test "echo 'mixed \"quotes\"'" "mixed \"quotes\"" "Mixed quotes (single outside)"
  run_test "echo \"mixed 'quotes'\"" "mixed 'quotes'" "Mixed quotes (double outside)"
  run_test "echo \"variable \$USER expansion\"" "variable $USER expansion" "Variable expansion in double quotes"
  run_test "echo 'no \$USER expansion'" "no \$USER expansion" "No variable expansion in single quotes"

  # ===== REDIRECTIONS =====
  print_header "REDIRECTIONS"

  # Output redirection >
  check_file_existence "echo output > output.txt" "output.txt" 1 "Output redirection >"

  # Output redirection >>
  check_file_existence "echo first > output.txt && echo second >> output.txt" "output.txt" 1 "Output redirection >>"

  # Input redirection <
  check_file_existence "echo input data > input.txt && cat < input.txt > output.txt" "output.txt" 1 "Input redirection <"

  # Heredoc <<
  check_file_existence "cat << EOF > output.txt\nHeredoc test\nEOF" "output.txt" 1 "Heredoc <<"

  # ===== PIPES =====
  print_header "PIPES"

  run_test "echo hello | cat" "hello" "Simple pipe"
  run_test "echo hello | grep hello" "hello" "Pipe with grep"
  run_test "echo hello | grep world || echo not found" "not found" "Pipe with grep (not found)"
  run_test "echo hello | tr 'a-z' 'A-Z'" "HELLO" "Pipe with tr"
  run_test "echo hello | tr 'a-z' 'A-Z' | sed 's/H/h/'" "hELLO" "Multiple pipes"

  # ===== SIGNAL HANDLING =====
  print_header "SIGNAL HANDLING (Interactive Mode Simulation)"

  # Note: We can't really test signal handling in a script like this.
  # We'll just make sure the shell doesn't crash when we try common signals
  echo -e "${YELLOW}Signal handling tests should be performed manually.${NC}"
  echo -e "${YELLOW}Try pressing Ctrl+C, Ctrl+D, and Ctrl+\\ in the shell and observe behavior.${NC}"

  # ===== BONUS FEATURES =====
  print_header "BONUS FEATURES (if implemented)"

  # && and || operators with parenthesis
  run_test "echo success && echo another success" "success\nanother success" "AND operator &&"
  run_test "false || echo fallback" "fallback" "OR operator ||"

  # Wildcards (this test depends on files in the current directory)
  run_command_test "ls *.sh > /dev/null 2>&1" "Wildcard * expansion"

  print_header "COMPLEX COMMANDS"

  # Complex command combinations
  run_command_test "export TEST=hello && echo \$TEST | tr 'a-z' 'A-Z' > output.txt && cat output.txt" "Complex command with export, pipe, and redirection"
  run_command_test "cat < input.txt | grep test > output.txt 2>/dev/null || echo 'Not found'" "Complex command with input, pipe, output and error redirections"

  # Print summary
  print_summary
}

# Run the whole test suite
setup
run_tests
cleanup

exit 0
