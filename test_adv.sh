#!/usr/bin/env bash

# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    minishell_advanced_test.sh                         :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: test_user <test@student.42.fr>             +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/17 10:00:00 by test_user         #+#    #+#              #
#    Updated: 2025/03/17 10:00:00 by test_user        ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# This script contains more advanced test cases for minishell
# It's designed to be used after the basic test script passes

# Colors for better output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Path to minishell
MINISHELL="./minishell_test"

# Count for tests
TOTAL_TESTS=0
PASSED_TESTS=0

# Function to print test description
print_test() {
  echo -e "${CYAN}Test #$TOTAL_TESTS: $1${NC}"
}

# Function to run a command manually with prompt
run_manual_test() {
  local cmd=$1
  local description=$2

  echo -e "\n${MAGENTA}=== MANUAL TEST ===${NC}"
  print_test "$description"
  echo -e "${YELLOW}Command to test: ${NC}$cmd"
  echo -e "${YELLOW}Press ENTER to run this test in minishell, then verify the behavior manually${NC}"
  read

  echo -e "${YELLOW}Running: $cmd${NC}"
  echo "$cmd" | "$MINISHELL"

  echo -e "${YELLOW}Did the test pass? (y/n)${NC}"
  read answer

  if [[ "$answer" == "y" || "$answer" == "Y" ]]; then
    echo -e "${GREEN}✓ PASS: $description${NC}"
    PASSED_TESTS=$((PASSED_TESTS + 1))
  else
    echo -e "${RED}✗ FAIL: $description${NC}"
  fi
  TOTAL_TESTS=$((TOTAL_TESTS + 1))
}

# Function to print section headers
print_header() {
  echo -e "\n${BLUE}======================================${NC}"
  echo -e "${BLUE}= $1${NC}"
  echo -e "${BLUE}======================================${NC}\n"
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

# Check if minishell executable exists
if [ ! -f "$MINISHELL" ]; then
  echo -e "${RED}Error: $MINISHELL not found. Make sure you are in the correct directory.${NC}"
  exit 1
fi

# Make sure we have the necessary permissions
chmod +x "$MINISHELL"

print_header "ADVANCED MINISHELL TESTING SCRIPT"
echo -e "${YELLOW}This script contains tests that require manual verification.${NC}"
echo -e "${YELLOW}Follow the instructions for each test carefully.${NC}"

# ===== INTERACTIVE SIGNAL TESTS =====
print_header "SIGNAL HANDLING TESTS"

echo -e "${YELLOW}The following tests will check signal handling in interactive mode.${NC}"
echo -e "${YELLOW}For each test, observe the behavior and compare it with bash.${NC}"

# Ctrl+C test (SIGINT)
run_manual_test "sleep 10" "Ctrl+C test: Run sleep 10, then press Ctrl+C - should interrupt and show a new prompt"

# Ctrl+D test (EOF)
run_manual_test "# Just press Ctrl+D when minishell starts" "Ctrl+D test: Press Ctrl+D at the prompt - should exit the shell"

# Ctrl+\ test (SIGQUIT)
run_manual_test "sleep 10" "Ctrl+\\ test: Run sleep 10, then press Ctrl+\\ - should do nothing"

# ===== EDGE CASES =====
print_header "EDGE CASES"

# Multiple pipes
run_manual_test "echo hello | cat | grep hello | tr 'a-z' 'A-Z'" "Multiple pipes: Should print HELLO"

# Multiple redirections
run_manual_test "echo hello > file1 > file2 > file3 && cat file3" "Multiple redirections: Should create 3 files and file3 should contain 'hello'"

# Nested quotes
run_manual_test "echo \"This is a 'nested' quote\"" "Nested quotes: Should print \"This is a 'nested' quote\""

# Empty command
run_manual_test "" "Empty command: Should just show a new prompt"

# Command with just spaces
run_manual_test "     " "Command with spaces: Should just show a new prompt"

# Redirections with no command
run_manual_test "> file.txt" "Redirection with no command: Should create an empty file"

# Heredoc with variable expansion
run_manual_test "cat << EOF\nHello \$USER\nEOF" "Heredoc with variable expansion: Should expand \$USER"

# Pipe with no command on right
run_manual_test "echo hello | " "Pipe with no right command: Should show an error"

# Pipe with no command on left
run_manual_test "| echo hello" "Pipe with no left command: Should show an error"

# ===== ERROR HANDLING =====
print_header "ERROR HANDLING"

# Command not found
run_manual_test "nonexistent_command" "Command not found: Should show an appropriate error message"

# Permission denied
run_manual_test "touch test_file.txt && chmod -x test_file.txt && ./test_file.txt" "Permission denied: Should show an appropriate error message"

# Directory as command
run_manual_test "mkdir -p test_dir && test_dir" "Directory as command: Should show an appropriate error message"

# Syntax error in quotes
run_manual_test "echo \"unclosed quote" "Unclosed quote: Should show an appropriate error message"

# ===== BONUS FEATURES =====
print_header "BONUS FEATURES"

# AND/OR operators
run_manual_test "true && echo success || echo failure" "AND/OR operators: Should print 'success'"
run_manual_test "false && echo success || echo failure" "AND/OR operators: Should print 'failure'"

# Parentheses for priorities
run_manual_test "(false || true) && echo success" "Parentheses priority: Should print 'success'"
run_manual_test "false || (true && echo success)" "Parentheses priority: Should print 'success'"

# Wildcards
run_manual_test "touch a.txt b.txt c.txt && echo *.txt" "Wildcard expansion: Should list all .txt files"

print_summary

# Clean up any files created during tests
rm -f test_file.txt file1 file2 file3 a.txt b.txt c.txt
rm -rf test_dir

exit 0
