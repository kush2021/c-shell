#!/bin/sh
# Tests Against the cd Built-in

BINARY="./build/csh"

if [ ! -x "$BINARY" ]; then
  echo "FAIL $0: binary not found or not executable: $BINARY"
  exit 1
fi

# -- Setup --------------------------------------------------------------------

TESTDIR=$(mktemp -d /tmp/csh_test_cd.XXXXXX)
TESTDIR=$(cd "$TESTDIR" && pwd -P)   # resolve symlinks before any comparisons
SUBDIR="$TESTDIR/subdir"
NESTEDDIR="$TESTDIR/subdir/nested"
TESTFILE="$TESTDIR/not_a_dir.txt"

mkdir -p "$NESTEDDIR"
touch "$TESTFILE"

# -- Helpers ------------------------------------------------------------------

fail() {
  echo "FAIL $0: $1"
  rm -rf "$TESTDIR"
  exit 1
}

run() {
  # run <input> — captures stdout into $OUT, stderr into $ERR, status into $STATUS
  OUT=$(printf "%s\n" "$1" | "$BINARY" 2>/tmp/csh_test_stderr)
  STATUS=$?
  ERR=$(cat /tmp/csh_test_stderr)
}

# Run multiple lines, capturing the last command's output
run_lines() {
  OUT=$(printf "%s\n" "$1" | "$BINARY" 2>/tmp/csh_test_stderr)
  STATUS=$?
  ERR=$(cat /tmp/csh_test_stderr)
}

# -- Assertion 1: cd with no arguments goes to $HOME --------------------------

run_lines "cd
pwd"
[ "$OUT" = "$HOME" ] || fail "assertion 1: expected cd with no args to go to HOME ('$HOME'), got: '$OUT'"

# -- Assertion 2: cd to an absolute path --------------------------------------

run_lines "cd $TESTDIR
pwd"
[ "$OUT" = "$TESTDIR" ] || fail "assertion 2: expected pwd to show '$TESTDIR' after cd, got: '$OUT'"

# -- Assertion 3: cd to a relative path ---------------------------------------

run_lines "cd $TESTDIR
cd subdir
pwd"
[ "$OUT" = "$SUBDIR" ] || fail "assertion 3: expected pwd to show '$SUBDIR' after relative cd, got: '$OUT'"

# -- Assertion 4: cd into a nested subdirectory -------------------------------

run_lines "cd $NESTEDDIR
pwd"
[ "$OUT" = "$NESTEDDIR" ] || fail "assertion 4: expected pwd to show '$NESTEDDIR', got: '$OUT'"

# -- Assertion 5: cd to a non-existent directory prints to stderr -------------

run "cd /this/path/does/not/exist/xyz123"
[ -n "$ERR" ] || fail "assertion 5: expected an error on stderr for non-existent directory, got nothing"

# -- Assertion 6: cd to non-existent directory does not change the cwd --------

run_lines "cd $TESTDIR
cd /this/path/does/not/exist/xyz123
pwd"
[ "$OUT" = "$TESTDIR" ] || fail "assertion 6: expected cwd to remain '$TESTDIR' after failed cd, got: '$OUT'"

# -- Assertion 7: cd to a file (not a directory) prints to stderr -------------

run "cd $TESTFILE"
[ -n "$ERR" ] || fail "assertion 7: expected an error on stderr when cd-ing to a file, got nothing"

# -- Assertion 8: cd to a file does not change the cwd -----------------------

run_lines "cd $TESTDIR
cd $TESTFILE
pwd"
[ "$OUT" = "$TESTDIR" ] || fail "assertion 8: expected cwd to remain '$TESTDIR' after cd to a file, got: '$OUT'"

# -- Assertion 9: too many arguments is rejected ------------------------------

run "cd /tmp /tmp"
[ "$STATUS" -eq 2 ] || fail "assertion 9: expected status 2 for too many arguments, got: $STATUS"
[ -n "$ERR" ]       || fail "assertion 9: expected an error on stderr for too many arguments, got nothing"

# -- Assertion 10: too many arguments does not change cwd ---------------------

run_lines "cd $TESTDIR
cd /tmp /tmp
pwd"
[ "$OUT" = "$TESTDIR" ] || fail "assertion 10: expected cwd to remain '$TESTDIR' after rejected cd, got: '$OUT'"

# -- Assertion 11: error message from cd goes to stderr, not stdout -----------

run "cd /this/path/does/not/exist/xyz123"
[ -z "$OUT" ] || fail "assertion 11: expected no stdout output on cd error, got: '$OUT'"
[ -n "$ERR" ] || fail "assertion 11: expected error on stderr, got nothing"

# -- Assertion 12: shell continues after a failed cd --------------------------

OUT=$(printf "cd /this/path/does/not/exist/xyz123\necho still_running\n" | "$BINARY" 2>/dev/null)
[ "$OUT" = "still_running" ] || fail "assertion 12: expected shell to continue after failed cd, got: '$OUT'"

# -- Assertion 13: cd to / works ----------------------------------------------

run_lines "cd /
pwd"
[ "$OUT" = "/" ] || fail "assertion 13: expected pwd to show '/' after cd /, got: '$OUT'"

# -- Assertion 14: permission denied prints to stderr (skipped if root) -------

if [ "$(id -u)" -ne 0 ]; then
  RESTRICTED="$TESTDIR/restricted"
  mkdir "$RESTRICTED"
  chmod 000 "$RESTRICTED"

  run "cd $RESTRICTED"
  [ -n "$ERR" ] || fail "assertion 14: expected an error on stderr for permission denied, got nothing"

  chmod 755 "$RESTRICTED"
else
  echo "  (assertion 14 skipped: running as root)"
fi

# -- Cleanup ------------------------------------------------------------------

rm -rf "$TESTDIR"
rm -f /tmp/csh_test_stderr

echo "PASS $0"
exit 0
