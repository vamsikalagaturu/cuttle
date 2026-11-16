/* The dark and light themes: the colours every widget draws with (pal), which app.c sets from the theme and accent
 * colour settings; and the colour helpers of ui.h. */
#include "ui.h"
#include <stdio.h>
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

/* Nord (nordtheme.com), as jackdaw and ferret: Polar Night surfaces and Snow Storm text when dark, the other way
 * round when light, Frost for the accent and links, Aurora for danger, warning and success. The light accents are
 * darkened until text on them stays above 4.5:1 (WCAG 2.2 SC 1.4.3), as in jackdaw. */

/* drawn over pages and thumbnails, the same in both themes (the see-through black and white ones: theme_apply) */
#define PAGE_COLOURS                                                                                                   \
    .selection = HEX(0x5e81ac), .find_match = HEXA(0xebcb8b, 120), .find_current = HEXA(0xd08770, 140),                \
    .sync_mark = HEXA(0xebcb8b, 120), .field_bg = HEX(0xdbe5f1), .field_mark = HEX(0x2e3440),                          \
    .crop_dim = HEXA(0x2e3440, 150), .note_fill = HEX(0xebcb8b), .note_line = HEX(0x8a6416), .danger = HEX(0xbf616a),  \
    .warning = HEX(0xd08770), .success = HEX(0x4f7a3a), .thumb_bg = HEX(0xe5e9f0), .thumb_frame = HEXA(0x4c566a, 120), \
    .check_border = HEX(0x4c566a), .pad_guide = HEX(0xd8dee9), .paper = C_WHITE, .shadow = C_BLACK, .scrim = C_BLACK,  \
    .field_text = C_BLACK, .field_frame = HEX(0x4c566a), .on_danger = C_WHITE, .on_backdrop = HEX(0xeceff4),          \
    .present_bg = C_BLACK, .swatch_line = C_BLACK,                                                                     \
    .source = {HEX(0x5e81ac), HEX(0xbf616a), HEX(0xa3be8c), HEX(0xebcb8b), HEX(0xb48ead), HEX(0x88c0d0)}

static const ui_palette LIGHT = {
    .window = HEX(0xe5e9f0),
    .window_text = HEX(0x2e3440),
    .base = HEX(0xeceff4),
    .text = HEX(0x2e3440),
    .button = HEX(0xd8dee9),
    .button_text = HEX(0x2e3440),
    .mid = HEX(0xc5cddb),
    .light = C_WHITE,
    .dark = HEX(0xa3adbf),
    .highlight = HEX(0x5e81ac),
    .highlighted_text = C_WHITE,
    .disabled_text = HEX(0x7b8394),
    .placeholder = HEX(0x5c6577),
    .link = HEX(0x3b6a99),
    .tooltip = HEX(0xeceff4),
    .page_bg = HEX(0x4c566a),
    .border = HEX(0xc5cddb),
    .scroll_lane = HEX(0xe5e9f0),
    .scroll_thumb = HEX(0xa3adbf),
    .scroll_thumb_hot = HEX(0x7b8394),
    PAGE_COLOURS,
    .dark_mode = 0,
};

static const ui_palette DARK = {
    .window = HEX(0x3b4252),
    .window_text = HEX(0xd8dee9),
    .base = HEX(0x2e3440),
    .text = HEX(0xd8dee9),
    .button = HEX(0x434c5e),
    .button_text = HEX(0xeceff4),
    .mid = HEX(0x3b4252),
    .light = HEX(0x4c566a),
    .dark = HEX(0x272c36),
    .highlight = HEX(0x5e81ac),
    .highlighted_text = C_WHITE,
    .disabled_text = HEX(0x6f7a8f),
    .placeholder = HEX(0xa8b1c2),
    .link = HEX(0x88c0d0),
    .tooltip = HEX(0x3b4252),
    .page_bg = HEX(0x272c36),
    .border = HEX(0x4c566a),
    .scroll_lane = HEX(0x2e3440),
    .scroll_thumb = HEX(0x4c566a),
    .scroll_thumb_hot = HEX(0x616e88),
    PAGE_COLOURS,
    .dark_mode = 1,
};

/* text on the accent: whichever of these contrasts more (Rec. 709 luma) */
static const Color ON_LIGHT_ACCENT = HEX(0x111111), ON_DARK_ACCENT = C_WHITE;

ui_palette pal;

Color with_alpha(Color c, int a)
{
    c.a = (unsigned char)a;
    return c;
}

Color rgb3(const unsigned char rgb[3]) { return (Color){rgb[0], rgb[1], rgb[2], 255}; }

void theme_apply(const char *mode)
{
    pal = mode && !strcmp(mode, "light") ? LIGHT : DARK;
    pal.redact_preview = with_alpha(BLACK, 90);
    pal.present_label = with_alpha(BLACK, 160);
    pal.check_bg = with_alpha(WHITE, 230);
    const char *accent = pt_settings_str(&app.settings, pal.dark_mode ? "accent_colour_dark" : "accent_colour_light");
    unsigned v;
    if (accent && accent[0] == '#' && strlen(accent) == 7 && sscanf(accent + 1, "%6x", &v) == 1) {
        pal.highlight = (Color)HEX(v);
        float luma = 0.2126f * pal.highlight.r + 0.7152f * pal.highlight.g + 0.0722f * pal.highlight.b;
        pal.highlighted_text = luma > 150 ? ON_LIGHT_ACCENT : ON_DARK_ACCENT;
    }
}
