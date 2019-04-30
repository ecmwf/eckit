#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "eckit/cmd/UserInput.h"


namespace eckit {

struct termios save;
static bool inited = false;


enum
{
    ESC       = 0x1B,
    BACKSPACE = 0x7F,
    TAB       = 0x9,
    CR        = 0XD,
    CONTROL_A = 0x1,
    CONTROL_B = 0x2,
    CONTROL_C = 0x3,
    CONTROL_D = 0x4,
    CONTROL_E = 0x5,
    CONTROL_F = 0x6,
    CONTROL_G = 0x7,
    CONTROL_H = 0x8,
    CONTROL_I = 0x9,
    CONTROL_J = 0xA,
    CONTROL_K = 0xB,
    CONTROL_L = 0xC,
    CONTROL_M = 0xD,
    CONTROL_N = 0xE,
    CONTROL_O = 0xF,
    CONTROL_P = 0x10,
    CONTROL_Q = 0x11,
    CONTROL_R = 0x12,
    CONTROL_S = 0x13,
    CONTROL_T = 0x14,
    CONTROL_U = 0x15,
    CONTROL_V = 0x16,
    CONTROL_W = 0x17,
    CONTROL_X = 0x18,
    CONTROL_Y = 0x19,
    CONTROL_Z = 0x1A,

    // Virtual codes
    UP_ARROW    = 1000,
    DOWN_ARROW  = 1001,
    LEFT_ARROW  = 1002,
    RIGHT_ARROW = 1003,
    HOME        = 1004,
    END         = 1005,
    DELETE      = 1006,
    INSERT      = 1007,
    PAGE_UP     = 1008,
    PAGE_DOWN   = 1009
};


static void exitRaw() {
    if (inited) {
        tcsetattr(0, TCSAFLUSH, &save);
    }
}

static void enterRaw() {

    if (!isatty(0)) {
        return;
    }

    if (tcgetattr(0, &save) < 0) {
        perror("tcgetattr");
    }

    struct termios raw = save;

    cfmakeraw(&raw);

    // Re-enable ^C and ^Z

    // raw.c_lflag |= ISIG;

    raw.c_cc[VMIN]  = 1;  // On char at a time
    raw.c_cc[VTIME] = 0;

    if (!inited) {
        atexit(&exitRaw);
        inited = true;
    }

    if (tcsetattr(0, TCSAFLUSH, &raw) < 0) {
        perror("tcsetattr");
    }
}

typedef struct entry {
    struct entry* next;
    struct entry* prev;
    char* line;
    char* edit;
    int len;
} entry;

static entry* history = NULL;

typedef struct context {
    const char* prompt;
    char* clipboard;
    entry* curr;
    int pos;
    int mark;
    bool overwrite;
    bool eof;
    bool tab;
    UserInput::completion_proc completion;
} context;

static bool processCode(int c, context* s);

static void output(const context* s) {
    char* buffer = (char*)malloc(strlen(s->prompt) + strlen(s->curr->edit) + 20);
    sprintf(buffer, "\r%s%s\033[0K\r\033[%luC", s->prompt, s->curr->edit, strlen(s->prompt) + s->pos);
    write(1, buffer, strlen(buffer));
    free(buffer);
}

static entry* current(entry* e) {
    if (!e->edit) {
        e->edit = (char*)malloc(e->len);
        strcpy(e->edit, e->line);
    }
    return e;
}

static void del(context* s) {
    if (s->pos > 0) {
        int i;
        char* line = s->curr->edit;
        int len    = strlen(line);
        for (i = s->pos - 1; i < len; i++) {
            line[i] = line[i + 1];
        }
        s->pos--;
        line[len] = 0;
    }
}

static void ins(context* s, char c) {
    char* line = s->curr->edit;
    int len    = strlen(line);

    if (len + 2 <= s->curr->len) {
        char* old = s->curr->edit;
        s->curr->len += 80;
        s->curr->edit = (char*)calloc(s->curr->len, 1);
        strcpy(s->curr->edit, old);
        free(old);
        line = s->curr->edit;
    }

    if (!s->overwrite) {
        int i;
        int len = strlen(line);
        for (i = len; i >= s->pos; i--) {
            line[i + 1] = line[i];
        }
    }
    line[s->pos++] = c;
}

static char nextChar() {
    char c;
    if (read(0, &c, 1) != 1) {
        return 0;
    }
    return c;
}

static struct {
    const char* sequence;
    int code;
} escapes[] = {
    {
        "[A",
        UP_ARROW,
    },
    {
        "[B",
        DOWN_ARROW,
    },
    {
        "[C",
        RIGHT_ARROW,
    },
    {
        "[D",
        LEFT_ARROW,
    },
    {
        "OH",
        HOME,
    },
    {
        "OF",
        END,
    },
    {
        "[1~",
        HOME,
    },
    {
        "[2~",
        INSERT,
    },
    {
        "[3~",
        DELETE,
    },
    {
        "[4~",
        END,
    },
    {
        "[5~",
        PAGE_UP,
    },
    {
        "[6~",
        PAGE_DOWN,
    },
};

static void esc(context* s) {
    int len = sizeof(escapes) / sizeof(escapes[0]);
    int i   = 0;
    int j   = 0;
    char buf[80];
    memset(buf, 0, sizeof(buf));
    while (1) {
        int partial = 0;
        buf[j++]    = nextChar();
        for (i = 0; i < len; i++) {
            if (strncmp(escapes[i].sequence, buf, j) == 0) {
                partial++;
            }
            if (strcmp(escapes[i].sequence, buf) == 0) {
                processCode(escapes[i].code, s);
                return;
            }
        }

        if (partial == 0) {
            break;
        }
    }

    /* No match: output all charaters */
    for (i = 0; i < j; i++) {
        processCode(buf[i], s);
    }
}

static bool processCode(int c, context* s) {

    char *p, *q;

    if (c != TAB) {
        s->tab = false;
    }

    switch (c) {

        case 0:
            s->curr->edit[0] = 0;
            s->eof           = true;
            return true;

        case CONTROL_D:
            if (strlen(s->curr->edit)) {
                return processCode(BACKSPACE, s);
            }
            else {
                return processCode(0, s);
            }
            break;

        case BACKSPACE:
        case CONTROL_H:
            del(s);
            break;

        case DELETE:
            s->pos++;
            del(s);
            break;

        case TAB:
            if (s->completion) {
                char insert[10240];
                char* p = insert;
                memset(insert, 0, sizeof(insert));

                if (s->completion(s->curr->edit, s->pos, insert, sizeof(insert) - 1)) {
                    while (*p) {
                        ins(s, *p);
                        p++;
                    }
                    if (insert[0] && (size_t(s->pos) == strlen(s->curr->edit))) {
                        ins(s, ' ');
                    }
                }
                else {
                    if (s->tab) {  // Second TAB
                        write(1, "\r\n", 2);
                        write(1, insert, strlen(insert));
                        write(1, "\r\n", 2);
                        s->tab = false;
                    }
                    else {
                        s->tab = true;
                    }
                }
            }
            break;

        case CONTROL_U:
            p = s->curr->edit + s->pos;
            q = s->curr->edit;

            if (s->clipboard) {
                free(s->clipboard);
            }
            s->clipboard = strdup(s->curr->edit);
            strncpy(s->clipboard, s->curr->edit, s->pos);
            s->clipboard[s->pos] = 0;

            while (*p) {
                *q++ = *p++;
            }
            *q     = 0;
            s->pos = 0;
            break;

        case ESC:
            esc(s);
            break;

        case CONTROL_L:
            write(1, "\033[2J\033[H", 7);
            break;

        case UP_ARROW:
        case CONTROL_P:
            if (s->curr->prev) {
                s->curr = current(s->curr->prev);
                s->pos  = strlen(s->curr->edit);
            }
            break;

        case DOWN_ARROW:
        case CONTROL_N:
            if (s->curr->next) {
                s->curr = current(s->curr->next);
                s->pos  = strlen(s->curr->edit);
            }
            break;

        case RIGHT_ARROW:
        case CONTROL_F:
            s->pos++;
            if (size_t(s->pos) > strlen(s->curr->edit)) {
                s->pos = strlen(s->curr->edit);
            }
            break;

        case LEFT_ARROW:
        case CONTROL_B:
            s->pos--;
            if (s->pos < 0) {
                s->pos = 0;
            }
            break;

        case CONTROL_K:
            if (s->clipboard) {
                free(s->clipboard);
            }
            s->clipboard = strdup(s->curr->edit);
            strcpy(s->clipboard, s->curr->edit + s->pos);
            s->curr->edit[s->pos] = 0;
            break;

        case CONTROL_A:
        case HOME:
            s->pos = 0;
            break;

        case CONTROL_E:
        case END:
            s->pos = strlen(s->curr->edit);
            break;

        case PAGE_UP:
            break;

        case PAGE_DOWN:
            break;

        case INSERT:
            s->overwrite = !s->overwrite;
            break;

        case CONTROL_J:
        case CR:
            write(1, "\r\n", 2);
            return true;
            break;

        case CONTROL_C:
            write(1, "\r\n", 2);
            s->pos = 0;
            return processCode(0,
                               s);  // CONTROL_C behaves as CONTROL_D -- for backward compatibility to previous marsadm
            break;

        case CONTROL_G:
            break;

        case CONTROL_O:
            break;

        case CONTROL_Q:
            break;

        case CONTROL_R:
            // TODO: backward search history
            break;

        case CONTROL_S:
            // TODO: foreward search history
            break;

        case CONTROL_T:
            if (strlen(s->curr->edit) > 1) {
                int n = s->pos;
                if (size_t(s->pos) == strlen(s->curr->edit)) {
                    n--;
                }
                if (n >= 1) {
                    char c               = s->curr->edit[n];
                    s->curr->edit[n]     = s->curr->edit[n - 1];
                    s->curr->edit[n - 1] = c;
                }
            }
            break;

        case CONTROL_V:
            // TODO: escape next control char
            break;

        case CONTROL_W:
            // TODO: delete word before
            break;

        case CONTROL_X:
            switch (nextChar()) {
                case CONTROL_X: {
                    int tmp = s->pos;
                    s->pos  = s->mark;
                    s->mark = tmp;

                    if (size_t(s->pos) > strlen(s->curr->edit)) {
                        s->pos = strlen(s->curr->edit);
                    }
                } break;
            }
            break;

        case CONTROL_Y:
            if (s->clipboard) {
                char* p = s->clipboard;
                while (*p) {
                    ins(s, *p++);
                }
            }
            break;

        case CONTROL_Z:
            exitRaw();
            kill(0, SIGTSTP);
            enterRaw();
            break;

        default:
            if (isprint(c)) {
                ins(s, c);
            }
            break;
    }
    return false;
}

void UserInput::printHistory(int max) {
    entry* last = NULL;
    entry* e;

    if (max == 0) {
        max = INT_MAX;
    }

    e = history;
    while (e && max-- > 0) {
        last = e;
        e    = e->prev;
    }

    while (last) {
        printf("%s\n", last->line);
        last = last->next;
    }
}

void UserInput::saveHistory(const char* path, int max) {

    entry* last = NULL;
    entry* e;

    FILE* f = ::fopen(path, "w");

    if (!f) {
        perror(path);
        return;
    }

    if (max == 0) {
        max = INT_MAX;
    }

    e = history;
    while (e && max-- > 0) {
        last = e;
        e    = e->prev;
    }

    while (last) {
        fprintf(f, "%s\n", last->line);
        last = last->next;
    }

    fclose(f);
}

void UserInput::loadHistory(const char* path) {

    char line[10240];
    FILE* f = ::fopen(path, "r");

    if (!f) {
        perror(path);
        return;
    }

    memset(line, 0, sizeof(line));
    while (fgets(line, sizeof(line) - 1, f)) {

        entry* h = (entry*)calloc(sizeof(entry), 1);

        int len = strlen(line);
        while (len) {
            if (line[len - 1] == '\n' || line[len - 1] == '\r') {
                line[len - 1] = 0;
                len--;
            }
            else {
                break;
            }
        }

        if (len) {

            h->len  = len + 1;
            h->line = strdup(line);
            h->prev = history;

            if (history) {
                history->next = h;
            }

            history = h;
        }
    }

    fclose(f);
}

static void cleanup_history() {
    entry* h = history;
    entry* n = NULL;

    char* prev = strdup("");

    while (h) {
        n = h->prev;
        if (h->edit) {
            free(h->edit);
            h->edit = NULL;
        }

        if (strlen(h->line) == 0 /*|| strcmp(h->line, prev) == 0*/) {
            free(h->line);

            if (history == h) {
                history = h->prev;
            }

            if (h->next) {
                h->next->prev = h->prev;
            }

            if (h->prev) {
                h->prev->next = h->next;
            }

            free(h);
        }
        else {
            free(prev);
            prev = strdup(h->line);
        }

        h = n;
    }

    free(prev);
}

const char* UserInput::getUserInput(const char* prompt, completion_proc completion) {

    bool done = false;
    context s;
    entry* h = (entry*)calloc(sizeof(entry), 1);
    h->len   = 80;
    h->line  = (char*)calloc(h->len, 1);
    h->prev  = history;

    if (history) {
        history->next = h;
    }

    history = h;

    memset(&s, 0, sizeof(s));
    s.prompt     = prompt;
    s.curr       = current(h);
    s.completion = completion;

    enterRaw();

    output(&s);

    while (!done) {
        char c = nextChar();
        done   = processCode(c, &s);
        if (!done) {
            output(&s);
        }
    }

    exitRaw();

    if (s.clipboard) {
        free(s.clipboard);
    }

    history->len = s.curr->len;
    free(history->line);
    history->line = strdup(s.curr->edit);

    if (strlen(history->line) == 0) {
        cleanup_history();
        return s.eof ? NULL : "";
    }

    cleanup_history();

    return s.eof ? NULL : (history ? history->line : "");
}

}  // namespace eckit
