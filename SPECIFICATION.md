# C Shell Project
## Kush Padalia

---

## Overview

In this project you will implement **`csh`** (Custom Shell) — a working command-line interpreter written in C. Your shell will replicate core behaviour of shells like `bash` and `sh`, handling process creation, I/O redirection, pipelines, and job control from scratch.

This is an **individual project**. You may discuss concepts with classmates, but all submitted code must be your own.

**Estimated effort:** 2–3 weeks (core), with optional extensions beyond.

---

## Learning Objectives

By the end of this project, you should be able to:

- Explain how a Unix shell interacts with the operating system kernel
- Use `fork`, `exec`, `wait`, and `pipe` correctly
- Implement I/O redirection at the file descriptor level
- Manage foreground and background processes
- Handle signals and terminal control for a group of processes

---

## Part 1 — Core Specification

### 1.1 Read-Eval-Print Loop (REPL)

Your shell must run a continuous REPL until the user exits. Each iteration must:

1. Print a prompt to `stdout` of the form:
   ```
   csh> 
   ```
   The prompt must appear **only** when stdin is a terminal (i.e., `isatty(STDIN_FILENO)`). When running non-interactively (e.g. piping a script in), no prompt should be printed.

2. Read a line of input from `stdin` using `fgets` or `getline`. A line is terminated by `\n` or `EOF`.

3. If `EOF` is received (e.g. Ctrl-D), the shell must exit gracefully with status 0.

4. Parse the line into a command and its arguments.

5. Execute the command and wait (or not, for background processes) for it to complete.

6. Return to step 1.

---

### 1.2 Parsing

Your parser must handle the following:

#### 1.2.1 Tokenisation

- Tokens are separated by one or more whitespace characters (spaces, tabs).
- Leading and trailing whitespace must be ignored.
- Empty lines (or lines containing only whitespace) must be silently ignored — no error, no execution.
- Lines beginning with `#` are **comments** and must be silently ignored.
- The maximum line length your shell is required to support is **4096 characters**.
- The maximum number of arguments per command is **128**.

#### 1.2.2 Quotes

- **Single quotes** (`'...'`): All characters inside are treated as literal. No special interpretation of `$`, `\`, `|`, `<`, `>`, or `&` inside single quotes.
- **Double quotes** (`"..."`): All characters are treated as literal **except** `\` (escape sequences) and `$` (variable expansion — see §1.5). Metacharacters like `|`, `<`, `>` lose their special meaning inside double quotes.
- An **unterminated quote** (e.g. `echo "hello`) must produce an error message and the command must not be executed:
  ```
  csh: syntax error: unterminated quote
  ```

#### 1.2.3 Escape Sequences

Inside double quotes and unquoted contexts, the following backslash escape sequences must be recognised:

| Sequence | Meaning          |
|----------|------------------|
| `\\`     | Literal backslash |
| `\"`     | Literal double quote |
| `\ `     | Literal space (prevents token split) |
| `\n`     | Newline |
| `\t`     | Tab |

---

### 1.3 Built-in Commands

The following commands must be implemented **inside** the shell process itself (i.e., without `fork`/`exec`). They are called **built-ins**.

| Command | Description |
|---------|-------------|
| `exit [status]` | Exit the shell. If `status` is provided it must be a non-negative integer; if omitted, exit with the status of the last executed command. |
| `cd [dir]` | Change the working directory. If `dir` is omitted, change to `$HOME`. If `dir` does not exist, print an error and do **not** change directory. |
| `pwd` | Print the absolute current working directory to `stdout`, followed by a newline. |
| `echo [args...]` | Print arguments separated by spaces, followed by a newline. Must support the `-n` flag to suppress the trailing newline. |
| `export NAME=VALUE` | Set an environment variable visible to child processes. Must also support `export NAME` to export a previously set shell variable without a value. |
| `unset NAME` | Remove a variable from the environment. |
| `history` | Print the command history (see §1.6). |
| `help` | Print a brief summary of all supported built-in commands. |

**Error handling for built-ins:**
- Argument count errors must print a usage line, e.g.:
  ```
  csh: cd: too many arguments
  ```
- All built-in error messages go to `stderr`.

---

### 1.4 External Command Execution

For any command that is not a built-in, your shell must:

1. `fork()` a child process.
2. In the child, call one of the `exec` family of functions to replace the process image with the target program.
3. In the parent, call `waitpid()` on the child (unless backgrounded — see §1.7).
4. Store the exit status of the child and make it available as `$?`.

#### 1.4.1 PATH Resolution

- If the command contains a `/` (e.g., `./myprog`, `/usr/bin/ls`), treat it as a path directly and attempt to `exec` it.
- Otherwise, search each directory in the `PATH` environment variable in order (colon-separated). Execute the first match found.
- If no match is found, print to `stderr`:
  ```
  csh: command not found: <name>
  ```

#### 1.4.2 Execution Errors

If `execve` fails for any reason other than "not found" (e.g., permission denied), print the system error using `perror` or `strerror(errno)` and exit the child with status 1.

---

### 1.5 Variable Expansion

Before a command is executed, your shell must perform variable expansion on each token.

- `$NAME` or `${NAME}` — expand to the value of the environment variable `NAME`. If `NAME` is not set, expand to an empty string.
- `$?` — expand to the decimal exit status of the most recently executed foreground command.
- `$$` — expand to the PID of the shell process itself.
- `$0` — expand to the name of the shell (`csh`).

Variable expansion must occur **after** parsing but **before** execution. It must respect quoting rules: `$` is not expanded inside single quotes.

---

### 1.6 Command History

Your shell must maintain an in-memory history of commands entered during the session.

- Store up to **500** entries.
- The `history` built-in prints entries in the format:
  ```
    1  ls -la
    2  cd /tmp
    3  echo hello
  ```
- Support history expansion with `!!` (repeat last command) and `!n` (repeat command number `n`). If `n` is out of range, print an error and do not execute anything:
  ```
  csh: !99: event not found
  ```
- History expansion must occur **before** variable expansion.

---

### 1.7 Background Execution

- If a command ends with `&`, the shell must **not** wait for it to complete. The shell must print the job number and PID immediately:
  ```
  [1] 3821
  ```
- The shell must continue to accept new commands while the background process runs.
- When a background process exits, the shell must print a notification **at the next prompt** (not asynchronously mid-input):
  ```
  [1]+  Done                    sleep 5
  ```
- Background processes must be placed in their own process group using `setpgid`.

---

### 1.8 I/O Redirection

Your shell must support the following redirection operators:

| Operator | Meaning |
|----------|---------|
| `< file` | Redirect `stdin` from `file` |
| `> file` | Redirect `stdout` to `file` (truncate) |
| `>> file` | Redirect `stdout` to `file` (append) |
| `2> file` | Redirect `stderr` to `file` (truncate) |
| `2>> file` | Redirect `stderr` to `file` (append) |
| `&> file` | Redirect both `stdout` and `stderr` to `file` |

Rules:
- Redirection operators may appear **anywhere** on the command line, not just at the end.
- Multiple redirections on the same file descriptor on the same command are a syntax error:
  ```
  csh: syntax error: duplicate redirection
  ```
- If an input file does not exist, print an error to `stderr` and do not execute the command:
  ```
  csh: no such file: foo.txt
  ```
- All file opens must use appropriate `open(2)` flags and `0644` permissions for output files.
- Redirections must be applied in the **child** process, after `fork()` but before `exec()`.

---

### 1.9 Pipelines

Your shell must support arbitrarily long pipelines of the form:

```
cmd1 | cmd2 | cmd3 | ... | cmdN
```

Requirements:
- Create `N-1` pipes using `pipe(2)`.
- Fork one child per command.
- Wire `stdout` of each command to `stdin` of the next using `dup2(2)`.
- The shell must close **all** pipe ends in the parent after forking all children.
- The shell must wait for **all** children in the pipeline to exit before returning.
- The exit status of a pipeline is the exit status of the **last** command in the pipeline.
- Pipelines may be combined with I/O redirection: `< input.txt cmd1 | cmd2 > out.txt`

---

### 1.10 Signal Handling

| Signal | Required Behaviour |
|--------|-------------------|
| `SIGINT` (Ctrl-C) | Must **not** terminate the shell. If a foreground process is running, deliver `SIGINT` to it. If no foreground process, reprint the prompt on a new line. |
| `SIGQUIT` (Ctrl-\\) | Same as `SIGINT` — never terminate the shell. |
| `SIGTSTP` (Ctrl-Z) | If a foreground process is running, suspend it and report it as a stopped job. If no foreground process, ignore. |
| `SIGCHLD` | Use to reap background children. Do **not** block or ignore this signal. |

All signal handlers must be installed using `sigaction(2)`. The use of `signal()` is **not** permitted.

---

### 1.11 Error Handling

- All system call return values must be checked. On failure, your shell must print a meaningful error to `stderr` and continue running (unless the failure is unrecoverable).
- Your shell must never `exit()` from the parent shell process except in response to `exit` or `EOF`.
- Memory returned by `malloc`/`calloc` must be freed before a command cycle completes. Your shell must not leak memory across iterations of the REPL. You will be tested with Valgrind.

---

### 1.12 Testing Requirements

You must include a `tests/` directory containing:

- At least **20 test cases** as plain shell scripts that can be run against your `csh` binary.
- A `run_tests.sh` driver script that runs all tests and reports pass/fail.
- Tests must cover: basic execution, redirection, pipelines, built-ins, quoting, variable expansion, background jobs, and error conditions.

---

### 1.13 Build Requirements

- Your project must build with `make` in the root directory.
- Your `Makefile` must support the targets: `all`, `clean`, and `test`.
- Compile with: `gcc -Wall -Wextra -Werror -std=c11 -g`
- No warnings may be present in the final submission.
- The binary produced must be named `csh`.

---

### 1.14 Prohibited Functions and Shortcuts

You may **not** use any of the following:

- `system()` — defeats the purpose of the project
- `popen()` — same reason
- `getline()` from GNU `<stdio.h>` — implement your own line reader
- `wordexp()` or `glob()` for parsing — implement tokenisation yourself
- Any existing shell-parsing library

---

### 1.15 Deliverables

| Item | Location |
|------|----------|
| Source code | `src/` |
| Makefile | Root directory |
| Test suite | `tests/` |
| Design document | `DESIGN.md` — max 2 pages, covering architecture and notable design decisions |
| README | `README.md` — build and run instructions |

---

### 1.16 Grading Rubric (Core)

| Component | Weight |
|-----------|--------|
| REPL, prompt, EOF handling | 5% |
| Parsing (whitespace, quotes, escapes) | 10% |
| Built-in commands | 15% |
| External command execution + PATH search | 10% |
| Variable expansion | 10% |
| I/O redirection | 15% |
| Pipelines | 15% |
| Background jobs | 10% |
| Signal handling | 5% |
| Error handling + no memory leaks | 5% |

---

---

## Part 2 — Extension Specification (Open Source / Portfolio Track)

The following features are **optional** but transform this project from a standard assignment into a production-quality, portfolio-worthy tool. Tackle them in any order. Each is independent unless noted.

---

### 2.1 Interactive Line Editing (Readline-like)

Implement your own terminal line editor **without** linking `libreadline` or `libedit`. This means dropping into raw terminal mode and handling keypresses manually.

Required keybindings:

| Key | Action |
|-----|--------|
| Left / Right arrow | Move cursor within line |
| Ctrl-A / Ctrl-E | Jump to start / end of line |
| Ctrl-W | Delete word before cursor |
| Ctrl-U | Clear from cursor to start of line |
| Ctrl-K | Clear from cursor to end of line |
| Ctrl-L | Clear screen, reprint prompt and current input |
| Up / Down arrow | Navigate history (see §2.2) |
| Tab | Trigger completion (see §2.3) |
| Backspace / Delete | Delete character before/at cursor |

You must save and restore terminal attributes using `tcgetattr`/`tcsetattr` with `TCSAFLUSH`, and restore them on `SIGWINCH`, `exit`, and any fatal signal.

---

### 2.2 Persistent History with Fuzzy Search

- Persist history to `~/.csh_history` on exit and reload it on startup (up to 10,000 entries).
- Implement **Ctrl-R reverse-incremental search**: as the user types, highlight the most recent matching history entry in-place. Pressing Enter executes it; pressing Escape cancels.
- Implement **Ctrl-F fuzzy history search**: open a full-screen TUI panel (rendered in the terminal) showing all history entries scored by fuzzy-match quality against the current query. The user can navigate entries with arrow keys.
- Deduplicate consecutive identical commands before storing.
- Support `HISTFILE` and `HISTSIZE` environment variables to override defaults.

---

### 2.3 Tab Completion

Implement context-aware tab completion:

- **Command completion**: Complete executable names from `PATH` and built-in names.
- **Path completion**: If the token begins with `/`, `./`, or `~/`, complete filesystem paths using `opendir`/`readdir`. Append `/` for directories, ` ` for files.
- **Environment variable completion**: After `$`, complete variable names from the environment.
- If there is exactly one match, complete immediately.
- If there are multiple matches, on the first Tab press show a menu below the prompt listing all matches (formatted in columns). On the second Tab press, cycle through completions. Pressing any non-Tab key accepts the current selection.

---

### 2.4 Job Control (Full `bash`-style)

Extend background job support (§1.7) to a complete job control subsystem:

- `jobs [-l]` built-in: list all jobs with status (`Running`, `Stopped`, `Done`). `-l` includes PIDs.
- `fg [%n]` built-in: bring job `n` to the foreground (or most recent if omitted). Deliver `SIGCONT` and call `tcsetpgrp` to hand over terminal control.
- `bg [%n]` built-in: resume a stopped job in the background.
- `kill %n` / `kill -SIGNAL %n`: send a signal to a job by job number.
- Pipelines must be a single job (the entire pipeline shares a process group).
- Stopped jobs must be reported when the user runs `jobs`, not lost.
- On `exit`, warn if there are stopped jobs: `csh: you have stopped jobs`.

---

### 2.5 Scripting Engine

Allow your shell to be used as a scripting language: `csh script.csh`.

Implement the following scripting constructs:

#### Conditionals
```sh
if condition; then
    ...
elif condition; then
    ...
else
    ...
fi
```
`condition` is any command; its exit status determines truth (0 = true).

#### Loops
```sh
for var in word1 word2 ...; do
    ...
done

while condition; do
    ...
done
```

#### Functions
```sh
greet() {
    echo "Hello, $1"
}
greet world
```
- Functions are stored in a separate namespace from variables.
- `$1`, `$2`, ... `$N` are positional parameters within function scope.
- `return [n]` exits a function with status `n`.
- `local NAME=VALUE` declares a function-local variable.

#### Other scripting features
- `[ expr ]` / `test expr`: evaluate expressions (string comparison, integer comparison, file tests `-e`, `-f`, `-d`, `-r`, `-w`, `-x`).
- `source file` / `. file`: execute a script in the current shell environment.
- `trap COMMAND SIGNAL [...]`: execute `COMMAND` when the listed signals are received.

---

### 2.6 Rich Prompt Customisation

Support a `PS1` environment variable with prompt escape codes:

| Escape | Expands To |
|--------|-----------|
| `\u` | Username |
| `\h` | Hostname (short) |
| `\H` | Hostname (full) |
| `\w` | Current working directory (with `~` for home) |
| `\W` | Basename of current directory |
| `\d` | Date in `Mon Mar 05` format |
| `\t` | Time in 24h `HH:MM:SS` |
| `\j` | Number of currently managed jobs |
| `\$` | `#` if root, `$` otherwise |
| `\[...\]` | Non-printing sequence (for embedding ANSI codes) |

Example: `PS1='\[\e[1;32m\]\u@\h\[\e[0m\]:\[\e[1;34m\]\w\[\e[0m\]\$ '` produces a coloured bash-style prompt.

Additionally, implement a `PS2` prompt for multi-line continuation (when a line ends with `\` or an open quote/brace is not yet closed):
```
csh> echo "hello \
> world"
```

---

### 2.7 Arithmetic Expansion

Support `$(( expr ))` arithmetic expansion, where `expr` is an integer arithmetic expression. Required operators:

`+`, `-`, `*`, `/`, `%`, `**` (power), `==`, `!=`, `<`, `>`, `<=`, `>=`, `&&`, `||`, `!`, `~`, `&`, `|`, `^`, `<<`, `>>`, `( )` for grouping.

Variable names inside `$(( ))` are expanded as integers without needing `$` prefix:

```sh
x=5
echo $(( x * 2 + 1 ))   # prints 11
```

---

### 2.8 Command Substitution

Support two syntaxes:

- `$(command)` — recommended form
- `` `command` `` — legacy backtick form

The shell executes `command` in a subshell, captures its `stdout`, trims trailing newlines, and substitutes the result inline. Command substitutions may be nested (backtick form may not nest).

---

### 2.9 Glob / Wildcard Expansion

Before execution, expand unquoted tokens containing glob metacharacters using your own implementation (not `glob(3)`):

| Pattern | Meaning |
|---------|---------|
| `*` | Match any sequence of characters (not `/`) |
| `?` | Match any single character (not `/`) |
| `[abc]` | Match any character in the set |
| `[a-z]` | Match any character in the range |
| `[!abc]` | Match any character not in the set |
| `**` | Match any path component including `/` (recursive glob) |

If a glob matches no files, the glob pattern must be passed as a literal argument (do not error). This matches the `nullglob` off / `failglob` off behaviour of bash by default. Support `setopt nullglob` and `setopt failglob` to change this.

---

### 2.10 Plugin / Hook System

Implement a lightweight event hook system usable from shell scripts:

```sh
# Runs before every command
precmd() { echo "[$(date +%T)] About to run: $LAST_CMD"; }

# Runs after every command (receives exit status as $1)
postcmd() { [ $1 -ne 0 ] && echo "Command failed with $1"; }

# Runs when directory changes
chpwd() { ls; }
```

Functions with these reserved names are automatically called at the relevant points in the REPL, if defined.

Additionally, support loading **plugin scripts** from `~/.csh/plugins/`:
```sh
csh> plugin load myplugin     # sources ~/.csh/plugins/myplugin.csh
csh> plugin list              # lists loaded plugins
csh> plugin unload myplugin
```

---

### 2.11 Configuration File

On startup (interactive mode only), source `~/.cshrc` if it exists. The config file is a normal `csh` script and can contain any command, variable assignment, function definition, or alias.

Support `alias`:
```sh
alias ll='ls -la'
alias gs='git status'
```
Aliases are expanded as the first token of a command before any other processing. Alias expansion is not recursive (an alias cannot call itself).

Support `unalias NAME` and `alias` (with no args) to list all aliases.

---

### 2.12 Structured Error Messages and Debugging

- `set -x` (or `csh -x script.csh`): print each command to `stderr` with a `+ ` prefix before executing it (xtrace mode).
- `set -e`: exit the shell immediately if any command exits with a non-zero status (errexit mode).
- `set -u`: treat unset variables as an error (nounset mode).
- Error messages must include the source file name and line number when running a script:
  ```
  csh: script.csh:14: command not found: frobnicate
  ```

---

## Appendix A — Suggested Implementation Order

If you are unsure where to begin, the following order is recommended:

1. REPL skeleton (prompt, `fgets`, `exit`)
2. Tokeniser (whitespace splitting, quote handling)
3. `fork`/`exec`/`wait` for external commands
4. Built-in commands
5. I/O redirection
6. Pipelines
7. Variable expansion
8. Background jobs + `SIGCHLD`
9. Signal handling (`SIGINT`, `SIGTSTP`)
10. History
11. Extensions (any order)

---

## Appendix B — Useful System Calls and Functions

You are expected to consult the man pages for all of the following:

`fork(2)`, `execve(2)`, `waitpid(2)`, `pipe(2)`, `dup2(2)`, `open(2)`, `close(2)`, `read(2)`, `write(2)`, `signal(2)`, `sigaction(2)`, `kill(2)`, `setpgid(2)`, `tcsetpgrp(3)`, `tcgetattr(3)`, `tcsetattr(3)`, `getenv(3)`, `setenv(3)`, `unsetenv(3)`, `getcwd(3)`, `chdir(2)`, `stat(2)`, `opendir(3)`, `readdir(3)`, `isatty(3)`, `getpid(2)`, `strtol(3)`

---

*End of Specification*
