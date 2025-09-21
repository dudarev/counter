// counter.c  (inc|dec|reset|get|set N  [--file PATH] [--speak] [--voice NAME])
// tiny, dynamically linked; prints the value to stdout; optional voice via /usr/bin/say

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <spawn.h>
extern char **environ;
#include <sys/stat.h>
#include <errno.h>

// Default storage path: $XDG_CONFIG_HOME/counter/counter or ~/.config/counter/counter
static const char* config_default_path(char *buf, size_t n) {
    const char *xdg = getenv("XDG_CONFIG_HOME");
    const char *home = getenv("HOME");
    char dir[1024];

    if (xdg && *xdg) {
        snprintf(dir, sizeof dir, "%s/counter", xdg);
    } else if (home && *home) {
        snprintf(dir, sizeof dir, "%s/.config/counter", home);
    } else {
        // Last resort: local ./counter directory
        snprintf(dir, sizeof dir, "./counter");
    }

    // Best-effort create parent (~/.config) when using HOME path
    if ((!xdg || !*xdg) && home && *home) {
        char cfg[1024];
        snprintf(cfg, sizeof cfg, "%s/.config", home);
        if (mkdir(cfg, 0700) != 0 && errno != EEXIST) { /* ignore */ }
    }
    // Ensure the counter directory exists
    if (mkdir(dir, 0700) != 0 && errno != EEXIST) { /* ignore */ }

    // File inside that directory
    snprintf(buf, n, "%s/%s", dir, "counter");
    return buf;
}

static long read_val(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return 0;
    long v = 0; if (fscanf(f, "%ld", &v) != 1) v = 0;
    fclose(f); return v;
}

static void write_val(const char *path, long v) {
    FILE *f = fopen(path, "w");
    if (!f) return;
    fprintf(f, "%ld", v);
    fclose(f);
}

static void speak_num(long v, const char *voice, int doSpeak) {
    if (!doSpeak) return;
    const char *prog = "/usr/bin/say";
    if (access(prog, X_OK) != 0) return; // silent if say(1) missing
    char num[32]; snprintf(num, sizeof num, "%ld", v);
    char *argv[6]; int i = 0;
    argv[i++] = "say";
    if (voice && *voice) { argv[i++] = "-v"; argv[i++] = (char*)voice; }
    argv[i++] = num; argv[i] = NULL;
    pid_t pid; (void)posix_spawn(&pid, prog, NULL, NULL, argv, environ); // ignore errors
}

int main(int argc, char **argv) {
    // Print version when asked early
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--version") || !strcmp(argv[i], "-V")) {
            const char *v =
#ifdef COUNTER_VERSION
                COUNTER_VERSION;
#else
                "0.0.0";
#endif
            printf("%s\n", v);
            return 0;
        }
    }

    // Print usage when asked
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h") || !strcmp(argv[i], "help")) {
            fprintf(stderr, "usage: %s [inc|dec|reset|get|set N] [--file PATH] [--speak] [--voice NAME]\n", argv[0]);
            return 0;
        }
    }

    char pathbuf[1024]; const char *file = config_default_path(pathbuf, sizeof pathbuf);
    int doSpeak = 0; const char *voice = NULL;
    // collect non-flag args
    char *pos[3]; int pn = 0;
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--file") && i+1 < argc) { file = argv[++i]; }
        else if (!strcmp(argv[i], "--speak")) { doSpeak = 1; }
        else if (!strcmp(argv[i], "--voice") && i+1 < argc) { voice = argv[++i]; }
        else if (pn < 3) { pos[pn++] = argv[i]; }
    }
    const char *cmd = pn ? pos[0] : "get";
    long val = read_val(file);

    if (!strcmp(cmd, "inc")) val++;
    else if (!strcmp(cmd, "dec")) val = val > 0 ? val - 1 : 0;
    else if (!strcmp(cmd, "reset")) val = 0;
    else if (!strcmp(cmd, "set")) { long n = (pn >= 2) ? strtol(pos[1], NULL, 10) : 0; if (n < 0) n = 0; val = n; }
    else if (!strcmp(cmd, "get")) { /* no change */ }
    else {
        fprintf(stderr, "unknown cmd\n");
        fprintf(stderr, "usage: %s [inc|dec|reset|get|set N] [--file PATH] [--speak] [--voice NAME]\n", argv[0]);
        return 2;
    }

    if (strcmp(cmd, "get")) write_val(file, val);
    printf("%ld\n", val);
    speak_num(val, voice, doSpeak);
    return 0;
}
