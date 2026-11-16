/* The dark and light themes: the colours every widget draws with (pal), which app.c sets from the theme settings;
 * and the colour helpers of ui.h.
 * As in jackdaw and ferret: Nord (nordtheme.com) by default; on a Linux desktop the colours of
 * ~/.local/state/omarchy/current/theme/colors.toml when it is there and as light or dark as asked (the roles folio,
 * hyprlock and waybar read); the [theme] table of settings.toml overrides both, role by role. */
#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HEX(v)                                                                         \
    {                                                                                  \
        (unsigned char)((v) >> 16), (unsigned char)((v) >> 8), (unsigned char)(v), 255 \
    }
#define HEXA(v, alpha)                                                                   \
    {                                                                                    \
        (unsigned char)((v) >> 16), (unsigned char)((v) >> 8), (unsigned char)(v), alpha \
    }
/* raylib's WHITE and BLACK are compound literals, which MSVC does not take in a static initializer */
#define C_WHITE HEX(0xffffff)
#define C_BLACK HEX(0x000000)

/* drawn over pages and thumbnails, the same in every theme (the see-through black and white ones: theme_apply) */
static const ui_palette PAGE = {
    .selection = HEX(0x5e81ac), .find_match = HEXA(0xebcb8b, 120), .find_current = HEXA(0xd08770, 140),
    .sync_mark = HEXA(0xebcb8b, 120), .field_bg = HEX(0xdbe5f1), .field_mark = HEX(0x2e3440),
    .crop_dim = HEXA(0x2e3440, 150), .note_fill = HEX(0xebcb8b), .note_line = HEX(0x8a6416),
    .thumb_bg = HEX(0xe5e9f0), .thumb_frame = HEXA(0x4c566a, 120), .check_border = HEX(0x4c566a),
    .pad_guide = HEX(0xd8dee9), .paper = C_WHITE, .shadow = C_BLACK, .scrim = C_BLACK, .field_text = C_BLACK,
    .field_frame = HEX(0x4c566a), .on_danger = C_WHITE, .on_backdrop = HEX(0xeceff4), .present_bg = C_BLACK,
    .swatch_line = C_BLACK,
};

enum { R_ACCENT, R_BACKGROUND, R_DARK_BACKGROUND, R_LIGHTER_BACKGROUND, R_FOREGROUND, R_BRIGHT_FOREGROUND, R_RED,
       R_YELLOW, R_GREEN, R_BLUE, R_MAGENTA, R_CYAN, R_ORANGE, ROLES };
static const char *const ROLE_NAMES[ROLES] = {"accent", "background", "dark_background", "lighter_background",
                                              "foreground", "bright_foreground", "red", "yellow", "green", "blue",
                                              "magenta", "cyan", "orange"};

ui_palette pal;

Color with_alpha(Color c, int a)
{
    c.a = (unsigned char)a;
    return c;
}

Color rgb3(const unsigned char rgb[3]) { return (Color){rgb[0], rgb[1], rgb[2], 255}; }

static Color hex(unsigned v) { return (Color){(unsigned char)(v >> 16), (unsigned char)(v >> 8), (unsigned char)v, 255}; }

/* a moved t of the way to b */
static Color mix(Color a, Color b, float t)
{
    return (Color){(unsigned char)(a.r + (b.r - a.r) * t), (unsigned char)(a.g + (b.g - a.g) * t),
                   (unsigned char)(a.b + (b.b - a.b) * t), 255};
}

/* Rec. 709 luma of the gamma-encoded channels: enough to tell a light colour from a dark one */
static float luma(Color c) { return (0.2126f * c.r + 0.7152f * c.g + 0.0722f * c.b) / 255.0f; }

static int parse_hex(const char *s, Color *out)
{
    while (*s == ' ' || *s == '"' || *s == '\'') s++;
    if (*s == '#') s++;
    unsigned v;
    if (sscanf(s, "%6x", &v) != 1) return 0;
    *out = hex(v);
    return 1;
}

/* the roles the desktop's colors.toml sets into c and set[]; 0 when there is none */
static int read_desktop(Color *c, int *set)
{
#if defined(__linux__) && !defined(__ANDROID__)
    const char *home = getenv("HOME");
    if (!home) return 0;
    char *path = pt_format("%s/.local/state/omarchy/current/theme/colors.toml", home);
    FILE *f = fopen(path, "r");
    free(path);
    if (!f) return 0;
    char line[256];
    while (fgets(line, sizeof line, f)) {
        char *eq = strchr(line, '=');
        if (!eq || line[0] == '#') continue;
        char *end = eq;
        while (end > line && (end[-1] == ' ' || end[-1] == '\t')) end--;
        for (int i = 0; i < ROLES; i++)
            if ((size_t)(end - line) == strlen(ROLE_NAMES[i]) && !strncmp(line, ROLE_NAMES[i], (size_t)(end - line)))
                set[i] |= parse_hex(eq + 1, &c[i]);
    }
    fclose(f);
    return 1;
#else
    (void)c, (void)set;
    return 0;
#endif
}

/* "system": as light or dark as the desktop theme's background; dark when there is none */
static int wants_light(const char *mode)
{
    if (mode && !strcmp(mode, "light")) return 1;
    if (!mode || strcmp(mode, "system")) return 0;
    Color c[ROLES] = {0};
    int set[ROLES] = {0};
    return read_desktop(c, set) && set[R_BACKGROUND] && luma(c[R_BACKGROUND]) > 0.5f;
}

void theme_apply(const char *mode)
{
    int light = wants_light(mode);
    /* Nord, as ferret: the light accent is Frost's nord10, since nord8 on Snow Storm is too faint for text */
    Color c[ROLES] = {[R_ACCENT] = hex(0x88c0d0),     [R_BACKGROUND] = hex(0x2e3440),
                      [R_DARK_BACKGROUND] = hex(0x242933), [R_LIGHTER_BACKGROUND] = hex(0x3b4252),
                      [R_FOREGROUND] = hex(0xd8dee9), [R_BRIGHT_FOREGROUND] = hex(0xeceff4),
                      [R_RED] = hex(0xbf616a),        [R_YELLOW] = hex(0xebcb8b),
                      [R_GREEN] = hex(0xa3be8c),      [R_BLUE] = hex(0x81a1c1),
                      [R_MAGENTA] = hex(0xb48ead),    [R_CYAN] = hex(0x8fbcbb),
                      [R_ORANGE] = hex(0xd08770)};
    if (light) {
        c[R_ACCENT] = hex(0x5e81ac);
        c[R_BACKGROUND] = hex(0xeceff4);
        c[R_DARK_BACKGROUND] = hex(0xe5e9f0);
        c[R_LIGHTER_BACKGROUND] = hex(0xf8f9fb);
        c[R_FOREGROUND] = hex(0x434c5e);
        c[R_BRIGHT_FOREGROUND] = hex(0x2e3440);
        c[R_BLUE] = hex(0x5e81ac);
    }
    /* the desktop's palette, when it is as light or dark as asked */
    Color d[ROLES];
    int set[ROLES] = {0};
    memcpy(d, c, sizeof d);
    if (read_desktop(d, set) && (!set[R_BACKGROUND] || (luma(d[R_BACKGROUND]) > 0.5f) == !!light)) memcpy(c, d, sizeof c);
    for (int i = 0; i < ROLES; i++) {
        char key[64];
        snprintf(key, sizeof key, "theme.%s", ROLE_NAMES[i]);
        const char *v = pt_settings_str(&app.settings, key);
        if (v && *v) parse_hex(v, &c[i]);
    }

    Color bg = c[R_BACKGROUND], text = light ? c[R_BRIGHT_FOREGROUND] : c[R_FOREGROUND];
    /* the chrome: raised above the content when dark, a shade below it when light */
    Color window = light ? c[R_DARK_BACKGROUND] : c[R_LIGHTER_BACKGROUND], fg = c[R_FOREGROUND];
    /* Aurora is made for dark backgrounds: on light ones it takes some of the text colour to keep its contrast */
    float deepen = light ? 0.25f : 0;
    pal = PAGE;
    pal.window = window;
    pal.window_text = text;
    pal.base = bg;
    pal.text = text;
    pal.button = mix(window, fg, 0.10f);
    pal.button_text = c[R_BRIGHT_FOREGROUND];
    pal.mid = mix(window, fg, 0.06f);
    pal.light = light ? c[R_LIGHTER_BACKGROUND] : mix(window, fg, 0.15f);
    pal.dark = light ? mix(window, fg, 0.30f) : c[R_DARK_BACKGROUND];
    /* filled areas (current tab, chosen tool, buttons): a dark theme's accent is a light colour made for text and
     * lines, too loud as a fill, so it is taken 40% toward the background there; links keep it whole */
    pal.highlight = light ? c[R_ACCENT] : mix(c[R_ACCENT], bg, 0.40f);
    /* whichever of the theme's darkest and lightest text reads on that fill */
    Color darkest = light ? c[R_BRIGHT_FOREGROUND] : bg, lightest = light ? (Color)C_WHITE : c[R_BRIGHT_FOREGROUND];
    pal.highlighted_text = luma(pal.highlight) > 0.55f ? darkest : lightest;
    /* also the secondary text everywhere: about 4.5:1 on the chrome with Nord */
    pal.disabled_text = mix(text, window, 0.30f);
    pal.placeholder = pal.disabled_text;
    pal.link = light ? mix(c[R_ACCENT], c[R_BRIGHT_FOREGROUND], 0.25f) : c[R_ACCENT];
    pal.tooltip = light ? bg : c[R_LIGHTER_BACKGROUND];
    pal.page_bg = light ? mix(c[R_BRIGHT_FOREGROUND], bg, 0.15f) : c[R_DARK_BACKGROUND];
    pal.border = mix(window, fg, 0.18f);
    pal.scroll_lane = light ? window : bg;
    pal.scroll_thumb = mix(window, fg, 0.25f);
    pal.scroll_thumb_hot = mix(window, fg, 0.40f);
    pal.danger = c[R_RED];
    pal.warning = mix(c[R_ORANGE], c[R_BRIGHT_FOREGROUND], deepen);
    pal.success = mix(c[R_GREEN], c[R_BRIGHT_FOREGROUND], light ? 0.35f : 0);
    pal.source[0] = c[R_BLUE], pal.source[1] = c[R_RED], pal.source[2] = c[R_GREEN];
    pal.source[3] = c[R_YELLOW], pal.source[4] = c[R_MAGENTA], pal.source[5] = c[R_CYAN];
    pal.dark_mode = !light;
    pal.redact_preview = with_alpha(BLACK, 90);
    pal.present_label = with_alpha(BLACK, 160);
    pal.check_bg = with_alpha(WHITE, 230);
}
