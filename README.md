# csh — a Unix shell written from scratch in C

`csh` ("Custom Shell") is a POSIX-ish command-line shell built entirely from
first principles in **C23** — no `system()`, no `popen()`, no third-party
parsing or line-editing libraries. Process management, tokenising, parsing,
redirection, pipelines, and job control are all hand-rolled on top of raw
`fork`/`exec`/`wait`/`pipe` and friends.

> **Status: early development.** The project is not yet feature-complete —
> see [Progress](#progress) below. Expect breaking changes, missing
> features, and rough edges while core execution and parsing infrastructure
> are still being built out.

---

## Why

Most people use a shell every day without ever seeing what's underneath it.
This project is an attempt to actually build one — fork/exec semantics,
redirection at the file-descriptor level, pipeline plumbing with `dup2`,
signal handling, and (eventually) a small scripting language with control
flow and functions — rather than just reading about how they work.

## Goals

- A correct, POSIX-influenced core shell: REPL, quoting/escaping, built-ins,
  variable expansion, redirection, pipelines, background jobs, and proper
  signal handling.
- A clean architecture (lexer → parser → AST → executor) that can support
  real scripting constructs (`if`/`for`/`while`, functions, `local`
  variables) later without a rewrite.
- Extensions beyond a typical shell project: interactive line editing,
  persistent/fuzzy history search, tab completion, full job control, prompt
  customisation, arithmetic and command substitution, globbing, and a small
  plugin/hook system.

## Progress

| Area | Status |
|---|---|
| REPL / prompt / EOF handling | 🚧 in progress |
| Tokeniser | 🚧 in progress |
| Lexer / parser / AST | ⏳ planned |
| External command execution (`fork`/`exec`/`wait`) | ✅ working |
| PATH resolution | ✅ working |
| Built-in commands (`cd`, `pwd`, `echo`, `export`, …) | ✅ working |
| I/O redirection | ⏳ planned |
| Pipelines | ⏳ planned |
| Variable expansion | ⏳ planned |
| Background jobs & `SIGCHLD` reaping | ⏳ planned |
| Signal handling (`SIGINT`, `SIGTSTP`, …) | ⏳ planned |
| Command history | ⏳ planned |
| Scripting engine (control flow, functions) | ⏳ planned |
| Line editing / tab completion / fuzzy history | ⏳ planned |

Legend: ✅ implemented · 🚧 actively being worked on · ⏳ not started

This table will be kept up to date as features land. Until the core spec
(built-ins, redirection, pipelines, expansion, jobs, signals) is complete,
treat this as a work-in-progress rather than a usable daily-driver shell.

## Design

The shell follows a fairly standard pipeline:

```
input line → tokeniser → recursive-descent parser → AST → executor
```

Reserved words (`if`, `for`, `while`, …) are only recognised in *command
position*, matching real shell grammar rather than treating them as global
keywords. The grammar and executor are being built with the later scripting
extensions in mind, so control flow, functions, and local variables can be
added at the natural point in the AST rather than bolted on afterward.

## Building

```sh
make        # build the csh binary
make clean  # remove build artifacts
make test   # run the test suite (once tests/ exists)
```

Requires a C23-capable compiler (recent GCC or Clang).

## Usage

```sh
./csh
csh> echo hello world
hello world
```

Non-interactive scripts (`csh script.csh` or piped input) are also
supported once scripting features land.

## Project layout

```
src/            shell source code
tests/          shell-script test cases + test runner
README.md        this file
```

## Contributing

This is primarily a hobby project, so it's not actively seeking
outside contributions right now, but issues and suggestions are welcome.

## License

MIT
