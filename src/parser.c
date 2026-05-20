// Copyright (c) 2026 Kush Padalia. All Rights Reserved.

// -- Libraries and Headers ----------------------------------------------------
#include "../include/parser.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// -- Token types --------------------------------------------------------------
//
// These are private to the parser. Nothing outside this file should ever need
// to know that ">>" was a TOK_REDIR_APPEND — the public API only surfaces the
// finished struct pipeline.

typedef enum {
  TOK_WORD,             /* a string argument                               */
  TOK_PIPE,             /* |                                               */
  TOK_REDIR_IN,         /* <                                               */
  TOK_REDIR_OUT,        /* >                                               */
  TOK_REDIR_APPEND,     /* >>                                              */
  TOK_REDIR_ERR,        /* 2>                                              */
  TOK_REDIR_ERR_APPEND, /* 2>>                                             */
  TOK_REDIR_BOTH,       /* &>  (stdout + stderr)                           */
  TOK_BG,               /* &   (background)                                */
  TOK_EOF,              /* end of input or '#' comment                     */
  TOK_ERROR,            /* tokenisation error (e.g. unterminated quote)    */
} token_type;

typedef struct {
  token_type type;
  char *value; /* heap-allocated; set only for TOK_WORD, NULL otherwise */
} token;

// -- Growable string buffer ---------------------------------------------------
//
// A minimal dynamic char array used while building word tokens. Using a
// dedicated type avoids propagating realloc bookkeeping throughout read_word.

typedef struct {
  char *data;
  size_t len;
  size_t cap;
} strbuf;

static bool strbuf_init(strbuf *b) {
  b->cap = 64;
  b->len = 0;
  b->data = malloc(b->cap);
  return b->data != NULL;
}

/**
 * Append one character. Returns false on allocation failure; the buffer is
 * invalid after a false return and must be freed with strbuf_free().
 */
static bool strbuf_push(strbuf *b, char c) {
  if (b->len + 1 >= b->cap) {
    size_t new_cap = b->cap * 2;
    char *tmp = realloc(b->data, new_cap);
    if (!tmp) {
      return false;
    }
    b->data = tmp;
    b->cap = new_cap;
  }
  b->data[b->len++] = c;
  return true;
}

/**
 * NUL-terminate and transfer ownership of the buffer to the caller.
 * The strbuf is left empty and must not be used again without re-initialising.
 */
static char *strbuf_take(strbuf *b) {
  b->data[b->len] = '\0';
  char *result = b->data;
  b->data = nullptr;
  b->len = 0;
  b->cap = 0;
  return result;
}

static void strbuf_free(strbuf *b) {
  free(b->data);
  b->data = nullptr;
  b->len = 0;
  b->cap = 0;
}

// -- Escape sequence translation ----------------------------------------------

/**
 * Translate the character that follows a backslash into the character it
 * represents. Returns 0 for unrecognised sequences so the caller can handle
 * them by emitting the literal backslash + character instead.
 */
static char escape_char(char c) {
  switch (c) {
    case '\\':
      return '\\';
    case '"':
      return '"';
    case ' ':
      return ' ';
    case 'n':
      return '\n';
    case 't':
      return '\t';
    default:
      return 0;
  }
}

// -- Tokeniser ----------------------------------------------------------------

/**
 * Read one word token from line[*pos], consuming characters until an unquoted
 * metacharacter or end-of-input is reached. Handles:
 *   - Single quotes:  all content is literal.
 *   - Double quotes:  content is literal except backslash escapes.
 *   - Backslash:      escape sequences in unquoted and double-quoted contexts.
 *
 * Advances *pos past every consumed character.
 *
 * Returns a heap-allocated NUL-terminated string on success, or NULL on error
 * (sets *errmsg).
 */
static char *read_word(const char *line, int *pos, const char **errmsg) {
  strbuf buf;
  if (!strbuf_init(&buf)) {
    *errmsg = "out of memory";
    return nullptr;
  }

  bool in_single = false;
  bool in_double = false;

  while (true) {
    char c = line[*pos];

    /* End of input — check for unclosed quotes. */
    if (c == '\0') {
      if (in_single || in_double) {
        *errmsg = "syntax error: unterminated quote";
        strbuf_free(&buf);
        return nullptr;
      }
      break;
    }

    /* ── Inside single quotes: everything is literal. ── */
    if (in_single) {
      if (c == '\'') {
        in_single = false;
      } else {
        if (!strbuf_push(&buf, c)) goto oom;
      }
      (*pos)++;
      continue;
    }

    /* ── Inside double quotes: literal except \ and (later) $. ── */
    if (in_double) {
      if (c == '"') {
        in_double = false;
        (*pos)++;
        continue;
      }
      if (c == '\\') {
        (*pos)++;
        char next = line[*pos];
        char esc = escape_char(next);
        if (esc) {
          if (!strbuf_push(&buf, esc)) goto oom;
        } else {
          if (!strbuf_push(&buf, '\\')) goto oom;
          if (next && !strbuf_push(&buf, next)) goto oom;
        }
        if (next) (*pos)++;
        continue;
      }
      if (!strbuf_push(&buf, c)) goto oom;
      (*pos)++;
      continue;
    }

    /* ── Unquoted context. ── */
    if (c == '\'') {
      in_single = true;
      (*pos)++;
      continue;
    }
    if (c == '"') {
      in_double = true;
      (*pos)++;
      continue;
    }

    if (c == '\\') {
      (*pos)++;
      char next = line[*pos];
      if (next == '\0') {
        break; /* trailing backslash — consume and stop */
      }
      char esc = escape_char(next);
      if (esc) {
        if (!strbuf_push(&buf, esc)) {
          goto oom;
        }
      } else {
        if (!strbuf_push(&buf, '\\')) {
          goto oom;
        }
        if (!strbuf_push(&buf, next)) {
          goto oom;
        }
      }
      (*pos)++;
      continue;
    }

    /* Stop at an unquoted metacharacter; next_token will handle it. */
    if (isspace((unsigned char) c) || c == '|' || c == '<' || c == '>' ||
        c == '&' || c == '#') {
      break;
    }

    if (!strbuf_push(&buf, c)) {
      goto oom;
    }
    (*pos)++;
  }

  return strbuf_take(&buf);

oom:
  *errmsg = "out of memory";
  strbuf_free(&buf);
  return nullptr;
}

/**
 * Return the next token from line[*pos], advancing *pos past it.
 *
 * The caller owns tok.value (when non-NULL) and is responsible for freeing it.
 * All other token types have tok.value == NULL.
 *
 * Operator precedence / disambiguation rules:
 *   >>  is checked before  >   to avoid consuming the first '>' prematurely.
 *   2>  is only recognised at the start of a new token (after whitespace),
 *       which is guaranteed by the whitespace skip at the top of this function.
 *   &>  is recognised before a lone &.
 */
static token next_token(const char *line, int *pos, const char **errmsg) {
  token tok = {TOK_EOF, nullptr};

  /* Skip horizontal whitespace. */
  while (line[*pos] != '\0' && isspace((unsigned char) line[*pos])) {
    (*pos)++;
  }

  char c = line[*pos];

  /* End-of-input and comments both terminate the token stream. */
  if (c == '\0' || c == '#') {
    return tok; /* TOK_EOF */
  }

  if (c == '|') {
    (*pos)++;
    tok.type = TOK_PIPE;
    return tok;
  }
  if (c == '<') {
    (*pos)++;
    tok.type = TOK_REDIR_IN;
    return tok;
  }

  if (c == '>') {
    (*pos)++;
    if (line[*pos] == '>') {
      (*pos)++;
      tok.type = TOK_REDIR_APPEND;
    } else {
      tok.type = TOK_REDIR_OUT;
    }
    return tok;
  }

  if (c == '&') {
    (*pos)++;
    if (line[*pos] == '>') {
      (*pos)++;
      tok.type = TOK_REDIR_BOTH;
    } else {
      tok.type = TOK_BG;
    }
    return tok;
  }

  /*
   * 2> / 2>>: stderr redirection.
   *
   * We recognise this only when '2' appears at the very start of a new
   * token (enforced by the whitespace skip above). This matches sh/bash
   * behaviour: `echo 2> err.txt` redirects stderr; `echo 2 > out.txt`
   * prints the literal "2" and redirects stdout.
   */
  if (c == '2' && line[*pos + 1] == '>') {
    (*pos) += 2; /* consume '2' and '>' */
    if (line[*pos] == '>') {
      (*pos)++;
      tok.type = TOK_REDIR_ERR_APPEND;
    } else {
      tok.type = TOK_REDIR_ERR;
    }
    return tok;
  }

  /* Anything else is a word. */
  tok.value = read_word(line, pos, errmsg);
  tok.type = tok.value ? TOK_WORD : TOK_ERROR;
  return tok;
}

// -- Internal helpers ---------------------------------------------------------

/**
 * Consume the next token and verify it is a filename word. On success,
 * *filename receives a heap-allocated string the caller must free. On failure,
 * sets *errmsg and returns false.
 */
static bool expect_filename(const char *line, int *pos, char **filename,
                            const char **errmsg) {
  token fn = next_token(line, pos, errmsg);
  if (fn.type == TOK_WORD) {
    *filename = fn.value;
    return true;
  }
  /* TOK_ERROR already set *errmsg via read_word; for all other bad tokens
     (EOF, another operator) we provide our own message. */
  if (fn.type != TOK_ERROR)
    *errmsg = "syntax error: expected filename after redirection operator";
  return false;
}

// -- Public API ---------------------------------------------------------------

int parse_line(const char *line, struct pipeline *out, const char **errmsg) {
  *errmsg = nullptr;
  memset(out, 0, sizeof *out);

  /*
   * Fast path: blank lines and comment lines produce an empty pipeline
   * (count == 0). This is not an error — the REPL should simply continue.
   */
  int pos = 0;
  while (isspace((unsigned char) line[pos])) pos++;
  if (line[pos] == '\0' || line[pos] == '#') return 0;

  /* Allocate the initial commands array. It grows on demand as pipes are
     encountered. calloc() zeroes all slots, which is relied upon below. */
  int cmd_cap = 4;
  out->commands = calloc((size_t) cmd_cap, sizeof(struct command));
  if (!out->commands) {
    *errmsg = "out of memory";
    return -1;
  }
  out->count = 1;

  /* Allocate argv for the first command. MAX_ARGS + 1 gives room for the
     mandatory NULL sentinel at argv[argc]. */
  out->commands[0].argv = (char **) calloc(MAX_ARGS + 1, sizeof(char *));
  if (!out->commands[0].argv) {
    *errmsg = "out of memory";
    goto fail;
  }

  int ci = 0; /* index of the command currently being built */

  for (;;) {
    token tok = next_token(line, &pos, errmsg);

    if (tok.type == TOK_ERROR) goto fail;
    if (tok.type == TOK_EOF) break;

    struct command *cmd = &out->commands[ci];

    /* ── Word argument ─────────────────────────────────────────────── */
    if (tok.type == TOK_WORD) {
      if (cmd->argc >= MAX_ARGS) {
        *errmsg = "syntax error: too many arguments";
        free(tok.value);
        goto fail;
      }
      cmd->argv[cmd->argc++] = tok.value;
      /* argv[argc] is already NULL (calloc'd). */
      continue;
    }

    /* ── Pipe: finish the current command, start the next one ──────── */
    if (tok.type == TOK_PIPE) {
      ci++;
      if (ci >= cmd_cap) {
        /* Double the capacity and zero the new slots. */
        int new_cap = cmd_cap * 2;
        struct command *tmp =
            realloc(out->commands, (size_t) new_cap * sizeof(struct command));
        if (!tmp) {
          *errmsg = "out of memory";
          goto fail;
        }
        out->commands = tmp;
        memset(&out->commands[ci], 0,
               (size_t) (new_cap - ci) * sizeof(struct command));
        cmd_cap = new_cap;
      }
      out->count = ci + 1;
      out->commands[ci].argv = (char **) calloc(MAX_ARGS + 1, sizeof(char *));
      if (!out->commands[ci].argv) {
        *errmsg = "out of memory";
        goto fail;
      }
      continue;
    }

    /* ── Background ─────────────────────────────────────────────────── */
    if (tok.type == TOK_BG) {
      out->background = true;
      continue;
    }

    /* ── &> (redirect both stdout and stderr) ────────────────────────
     * Handled separately because it uses redir_out as the filename but
     * also sets the redir_both flag, and the duplicate-check must cover
     * both the standalone > and the &> case. */
    if (tok.type == TOK_REDIR_BOTH) {
      if (cmd->redir_out) {
        *errmsg = "syntax error: duplicate redirection";
        goto fail;
      }
      if (!expect_filename(line, &pos, &cmd->redir_out, errmsg)) {
        goto fail;
      }
      cmd->redir_both = true;
      continue;
    }

    /* ── All other redirections ──────────────────────────────────────── */
    char **target = nullptr;
    switch (tok.type) {
      case TOK_REDIR_IN:
        target = &cmd->redir_in;
        break;
      case TOK_REDIR_OUT:
        target = &cmd->redir_out;
        break;
      case TOK_REDIR_APPEND:
        target = &cmd->redir_append;
        break;
      case TOK_REDIR_ERR:
        target = &cmd->redir_err;
        break;
      case TOK_REDIR_ERR_APPEND:
        target = &cmd->redir_err_append;
        break;
      default:
        break; /* unreachable: all non-word types are handled above */
    }

    if (!target) {
      continue; /* defensive; should never be reached */
    }

    if (*target) {
      *errmsg = "syntax error: duplicate redirection";
      goto fail;
    }
    if (!expect_filename(line, &pos, target, errmsg)) {
      goto fail;
    }
  }

  for (int i = 0; i < out->count; i++) {
    if (out->commands[i].argc == 0) {
      *errmsg = "syntax error: empty command in pipeline";
      goto fail;
    }
  }

  return 0;

fail:
  pipeline_free(out);
  return -1;
}

void pipeline_free(struct pipeline *p) {
  if (!p || !p->commands) {
    return;
  }

  for (int i = 0; i < p->count; i++) {
    struct command *cmd = &p->commands[i];
    if (cmd->argv) {
      for (int j = 0; j < cmd->argc; j++) {
        free(cmd->argv[j]);
      }
      free((void *) cmd->argv);
    }
    free(cmd->redir_in);
    free(cmd->redir_out);
    free(cmd->redir_append);
    free(cmd->redir_err);
    free(cmd->redir_err_append);
  }

  free(p->commands);

  memset(p, 0, sizeof *p);
}
