/* The file dialog on Android: the system document picker (Storage Access Framework) via CuttleActivity.
 * The app has no storage permission and the window works on file paths, so a picked document is copied to
 * cache/saf/<hash of its URI>/<name>. Copies remember their URI (documents.tsv in
 * the app's files folder) and are written back to it whenever the app changes them: Save replaces the copy,
 * "save as" targets a fresh copy, and a watcher exports any change once the file's size and time hold still.
 * Replaces file_dialog, file_dialogs_poll, file_dialog_busy and move_to_trash of dialogs.c (which renames its own
 * with a pt_desktop_ prefix on Android); split_command from there is used as is. */
#include "android_glue.h"
#include "ui.h"
#include <android/log.h>
#include <android_native_app_glue.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define LOG(...) __android_log_print(ANDROID_LOG_INFO, "cuttle", __VA_ARGS__)
#define FD_REEXPORT 100
#define FD_RENAME 101

char *pt_android_cache_dir;
int pt_desktop_move_to_trash(const char *path);

static void stamp(const char *path, long long *mtime, long long *size)
{
    struct stat st;
    if (stat(path, &st)) *mtime = *size = -1;
    else *mtime = (long long)st.st_mtim.tv_sec * 1000000000 + st.st_mtim.tv_nsec, *size = (long long)st.st_size;
}

static unsigned long long fnv(const char *s)
{
    unsigned long long h = 14695981039346656037ull;
    while (*s) h = (h ^ (unsigned char)*s++) * 1099511628211ull;
    return h;
}

static char *safe_name(const char *uri)
{
    char *name = pt_java("displayName", 1, uri);
    if (!name || !*name) free(name), name = pt_strdup("document");
    for (char *p = name; *p; p++)
        if (*p == '/' || *p == '\\' || *p == '\n' || *p == '\t') *p = '_';
    if (*name == '.') *name = '_';
    /* the reader is chosen by extension: a name without one gets its document type's */
    if (!strchr(name, '.')) {
        char *ext = pt_java("extensionOf", 1, uri), *named = pt_format("%s.%s", name, ext ? ext : "pdf");
        free(ext), free(name);
        name = named;
    }
    return name;
}

/* ---- copies and their URIs ---- */

typedef struct {
    char *path, *uri;
    long long mtime, size; /* the copy as last written back (or copied in); -2: never */
    long long seen_mtime, seen_size;
    double seen_at;
    int can_rename; /* -1: not asked yet */
} doc;

PT_VEC(doc_list, doc);
static doc_list docs;
static int docs_loaded;

static char *docs_file(void) { return pt_join(GetAndroidApp()->activity->internalDataPath, "documents.tsv"); }

static void save_docs(void)
{
    char *file = docs_file(), *part = pt_format("%s.part", file);
    FILE *f = fopen(part, "w");
    if (f) {
        for (int i = 0; i < docs.n; i++)
            fprintf(f, "%s\t%s\t%lld\t%lld\n", docs.v[i].path, docs.v[i].uri, docs.v[i].mtime, docs.v[i].size);
        if (!fclose(f)) rename(part, file);
    }
    free(file), free(part);
}

static doc *by_path(const char *path)
{
    for (int i = 0; i < docs.n; i++)
        if (!strcmp(docs.v[i].path, path)) return &docs.v[i];
    return NULL;
}

static doc *by_uri(const char *uri)
{
    for (int i = 0; i < docs.n; i++)
        if (!strcmp(docs.v[i].uri, uri)) return &docs.v[i];
    return NULL;
}

static doc *remember(const char *path, const char *uri)
{
    doc *d = by_path(path);
    if (!d) {
        pt_push(docs, (doc){pt_strdup(path), pt_strdup(uri), -2, -2, -1, -1, 0, -1});
        d = &docs.v[docs.n - 1];
    } else if (strcmp(d->uri, uri)) {
        free(d->uri);
        d->uri = pt_strdup(uri);
    }
    return d;
}

static int on_home_lists(const char *path)
{
    const pt_strs *recent = pt_settings_list(&app.settings, "recent_files");
    for (int i = 0; recent && i < recent->n; i++)
        if (!strcmp(recent->v[i], path)) return 1;
    return pt_settings_is_starred(&app.settings, path);
}

/* Copies the system dropped from the cache are fetched again for the Home lists, through the kept permission. */
static void load_docs(void)
{
    docs_loaded = 1;
    char *file = docs_file();
    FILE *f = fopen(file, "r");
    free(file);
    if (!f) return;
    char *line = NULL;
    size_t cap = 0;
    while (getline(&line, &cap, f) > 0) {
        char *save = NULL, *path = strtok_r(line, "\t\n", &save), *uri = strtok_r(NULL, "\t\n", &save);
        char *m = strtok_r(NULL, "\t\n", &save), *s = strtok_r(NULL, "\t\n", &save);
        if (!path || !uri || !m || !s || by_path(path)) continue;
        doc *d = remember(path, uri);
        d->mtime = atoll(m), d->size = atoll(s);
        stamp(d->path, &d->seen_mtime, &d->seen_size);
        if (d->seen_mtime < 0 && on_home_lists(d->path)) {
            char *dir = pt_dirname(d->path), *err;
            pt_mkdirs(dir);
            free(dir);
            if (!(err = pt_java("copyIn", 2, d->uri, d->path))) stamp(d->path, &d->mtime, &d->size);
            free(err);
        }
    }
    free(line);
    fclose(f);
}

static char *copy_path(const char *uri)
{
    doc *d = by_uri(uri);
    if (d) return pt_strdup(d->path);
    char *name = safe_name(uri);
    char *dir = pt_format("%s/saf/%016llx", pt_android_cache_dir, fnv(uri)), *path = pt_join(dir, name);
    pt_mkdirs(dir);
    free(dir), free(name);
    return path;
}

static int unsynced(const doc *d)
{
    long long m, s;
    stamp(d->path, &m, &s);
    return m >= 0 && (m != d->mtime || s != d->size);
}

char *pt_android_open_uri(const char *uri)
{
    char *path = copy_path(uri);
    doc *d = by_path(path);
    if (d && unsynced(d)) return path; /* edits not yet written back: keep them */
    char *err = pt_java("copyIn", 2, uri, path);
    if (err) {
        LOG("copy %s: %s", uri, err);
        free(err), free(path);
        return NULL;
    }
    d = remember(path, uri);
    stamp(path, &d->mtime, &d->size);
    d->seen_mtime = d->mtime, d->seen_size = d->size;
    save_docs();
    return path;
}

/* ---- renaming ---- */

/* tabs hold the resolved path (/data/data/...), the copies the one the cache folder was given (/data/user/0/...) */
static doc *by_file(const char *path)
{
    doc *d = by_path(path);
    for (int i = 0; !d && i < docs.n; i++)
        if (pt_same_file(docs.v[i].path, path)) d = &docs.v[i];
    return d;
}

/* Renaming in place (DocumentsContract.renameDocument) gives some providers' documents a new URI that the app's
 * grant does not follow (device storage names documents by path), so later saves could not reach it. A rename is
 * therefore Android's save dialog in the document's folder with its name filled in: the copy is written to the
 * document created there, and only then is the original deleted. */
int pt_android_can_rename(const char *path)
{
    doc *d = by_file(path);
    if (!d) return 0;
    if (d->can_rename < 0) {
        char *r = pt_java("canDelete", 1, d->uri);
        d->can_rename = r != NULL;
        free(r);
    }
    return d->can_rename;
}

static void pick_start(int mode, const char *mimes, const char *name, const char *initial, ui_files_done done,
                       void *ctx, const char *subject);

void pt_android_rename(const char *path, ui_files_done done, void *ctx)
{
    doc *d = by_file(path);
    if (!d) return;
    const pt_doc_format *format = pt_doc_format_of(d->path);
    const char *dot = strrchr(d->path, '.');
    const char *mime = dot && !strcasecmp(dot, ".pdf") ? "application/pdf"
                       : format                         ? format->mime
                                                        : "application/octet-stream";
    char *name = safe_name(d->uri);
    pick_start(FD_RENAME, mime, name, d->uri, done, ctx, d->path);
    free(name);
}

/* the copy of d to the new document uri, then the original deleted; the copy's new path, or NULL */
static char *rename_to(doc *d, const char *uri)
{
    char *same = pt_java("sameDocument", 2, d->uri, uri);
    if (same) {
        free(same);
        return NULL;
    }
    char *err = pt_java("copyOut", 2, d->path, uri);
    if (err) {
        LOG("rename %s to %s: %s", d->uri, uri, err);
        free(err);
        free(pt_java("deleteDocument", 1, uri));
        char *msg = pt_format("Could not save %s under the new name; nothing was renamed.", pt_basename(d->path));
        message_box(MSG_ERROR, APP_NAME, msg, NULL, NULL);
        free(msg);
        return NULL;
    }
    char *deleted = pt_java("deleteDocument", 1, d->uri);
    char *old_name = pt_strdup(pt_basename(d->path));

    /* the copy moves to where opening the new document would put it */
    char *name = safe_name(uri);
    char *dir = pt_format("%s/saf/%016llx", pt_android_cache_dir, fnv(uri)), *target = pt_join(dir, name);
    pt_mkdirs(dir);
    if (rename(d->path, target)) {
        LOG("move copy %s: %s", d->path, strerror(errno));
        free(target);
        target = pt_strdup(d->path);
    } else {
        char *old_dir = pt_dirname(d->path);
        if (strcmp(old_dir, dir)) rmdir(old_dir);
        free(old_dir);
    }
    free(d->path), free(d->uri);
    d->path = pt_strdup(target), d->uri = pt_strdup(uri);
    stamp(d->path, &d->mtime, &d->size);
    d->seen_mtime = d->mtime, d->seen_size = d->size;
    d->can_rename = -1;
    save_docs();
    if (!deleted) {
        char *msg = pt_format("Saved as %s, but %s could not be deleted, so both are there now.", name, old_name);
        message_box(MSG_INFO, APP_NAME, msg, NULL, NULL);
        free(msg);
    }
    free(deleted), free(old_name), free(name), free(dir);
    return target;
}

/* ---- writing back ---- */

static void reexport_answer(void *ctx, int yes)
{
    char *path = ctx;
    if (yes) pick_start(FD_REEXPORT, "application/pdf", pt_basename(path), NULL, NULL, NULL, path);
    free(path);
}

/* Write a changed copy back to its document; force: now, else once it has been still for half a second. */
static void sync_doc(doc *d, int force)
{
    long long m, s;
    stamp(d->path, &m, &s);
    if (m < 0 || (m == d->mtime && s == d->size)) return;
    if (!force) {
        if (m != d->seen_mtime || s != d->seen_size) {
            d->seen_mtime = m, d->seen_size = s, d->seen_at = GetTime();
            return;
        }
        if (GetTime() - d->seen_at < 0.5) return;
    }
    char *err = pt_java("copyOut", 2, d->path, d->uri);
    d->mtime = m, d->size = s;
    save_docs();
    if (!err) {
        char *msg = pt_format("Saved %s to its original location.", pt_basename(d->path));
        status(msg, 5);
        free(msg);
        return;
    }
    LOG("write back %s: %s", d->uri, err);
    free(err);
    char *msg = pt_format("%s could not be written back where it came from (no write access).\n\n"
                          "The saved version is kept in the app. Choose a place for it?",
                          pt_basename(d->path));
    question_box(APP_NAME, msg, "Choose...", "Not now", reexport_answer, pt_strdup(d->path));
    free(msg);
}

/* ---- folders picked for "Save N files in" ---- */

typedef struct {
    char *name;
    long long mtime, size;
    double seen_at;
    int exported;
} out_file;

PT_VEC(out_files, out_file);

typedef struct {
    char *dir, *tree;
    out_files files;
} out_dir;

PT_VEC(out_dirs, out_dir);
static out_dirs outs;

static void export_file(out_dir *o, out_file *f)
{
    char *path = pt_join(o->dir, f->name);
    const char *dot = strrchr(f->name, '.');
    char *uri = pt_java("createIn", 3, o->tree, f->name,
                        dot && !strcasecmp(dot, ".pdf") ? "application/pdf" : "application/octet-stream");
    char *err = uri ? pt_java("copyOut", 2, path, uri) : pt_strdup("could not create the file");
    f->exported = 1;
    if (err) {
        char *msg = pt_format("Could not save %s in the chosen folder.", f->name);
        message_box(MSG_ERROR, APP_NAME, msg, NULL, NULL);
        free(msg);
        LOG("export %s: %s", f->name, err);
    } else {
        char *msg = pt_format("Saved %s in the chosen folder.", f->name);
        status(msg, 5);
        free(msg);
    }
    free(path), free(uri), free(err);
}

static void scan_out(out_dir *o, int force)
{
    DIR *dir = opendir(o->dir);
    if (!dir) return;
    struct dirent *e;
    while ((e = readdir(dir))) {
        const char *name = e->d_name, *dot = strrchr(name, '.');
        if (name[0] == '.' || (dot && !strcmp(dot, ".part"))) continue;
        char *path = pt_join(o->dir, name);
        long long m, s;
        stamp(path, &m, &s);
        free(path);
        if (m < 0) continue;
        out_file *f = NULL;
        for (int i = 0; i < o->files.n; i++)
            if (!strcmp(o->files.v[i].name, name)) f = &o->files.v[i];
        if (!f) {
            pt_push(o->files, (out_file){pt_strdup(name), m, s, GetTime(), 0});
            f = &o->files.v[o->files.n - 1];
        } else if (f->mtime != m || f->size != s) {
            f->mtime = m, f->size = s, f->seen_at = GetTime(), f->exported = 0;
            continue;
        }
        if (!f->exported && (force || GetTime() - f->seen_at >= 0.5)) export_file(o, f);
    }
    closedir(dir);
}

/* ---- the picker ---- */

static struct {
    int active, mode;
    ui_files_done done;
    void *ctx;
    char *subject; /* FD_REEXPORT, FD_RENAME: the copy the pick is for */
} pick;

static void pick_start(int mode, const char *mimes, const char *name, const char *initial, ui_files_done done,
                       void *ctx, const char *subject)
{
    if (pick.active) return;
    const char *kind = mode == FD_OPEN ? "open" : mode == FD_OPEN_MANY ? "many" : mode == FD_DIR ? "dir" : "save";
    free(pt_java("pick", 4, kind, mimes, name, initial));
    free(pick.subject);
    pick.active = 1, pick.mode = mode, pick.done = done, pick.ctx = ctx;
    pick.subject = subject ? pt_strdup(subject) : NULL;
}

/* room for every type of pt_doc_formats */
#define MIME_TYPES_MAX 1024

static void add_type(char *types, const char *type)
{
    if (strstr(types, type)) return;
    if (*types) strcat(types, " ");
    strcat(types, type);
}

/* MIME types for patterns such as "*.pdf *.png"; "" (anything) when one is unknown */
static void mime_types(const char *patterns, char types[MIME_TYPES_MAX])
{
    static const char *images[] = {".png", ".jpg", ".jpeg", ".webp", ".bmp", ".gif", ".tif", ".tiff"};
    types[0] = 0;
    if (!patterns) return;
    char *copy = pt_strdup(patterns), *save = NULL;
    for (char *p = strtok_r(copy, " ", &save); p; p = strtok_r(NULL, " ", &save)) {
        const char *dot = strrchr(p, '.');
        int image = 0;
        for (size_t i = 0; dot && i < sizeof images / sizeof *images; i++) image |= !strcasecmp(dot, images[i]);
        const pt_doc_format *format = pt_doc_format_of(p);
        if (dot && !strcasecmp(dot, ".pdf")) add_type(types, "application/pdf");
        else if (image) add_type(types, "image/*");
        else if (format) add_type(types, format->mime);
        else {
            types[0] = 0;
            break;
        }
    }
    free(copy);
}

void file_dialog(int mode, const char *title, const char *start, const char *filter_name, const char *patterns,
                 ui_files_done done, void *ctx)
{
    (void)title, (void)filter_name;
    if (pick.active) return;
    char types[MIME_TYPES_MAX];
    mime_types(patterns, types);
    if (mode == FD_SAVE)
        snprintf(types, sizeof types, "%s",
                 strstr(types, "application/pdf") ? "application/pdf" : "application/octet-stream");
    pick_start(mode, types, mode == FD_SAVE && start ? pt_basename(start) : NULL, NULL, done, ctx, NULL);
}

int file_dialog_busy(void) { return pick.active; }

static void picked(int mode, ui_files_done done, void *ctx, char *subject, pt_strs *uris)
{
    pt_strs paths = {0};
    doc *d = NULL;
    out_dir *o = NULL;
    if (mode == FD_OPEN || mode == FD_OPEN_MANY) {
        for (int i = 0; i < uris->n && (mode == FD_OPEN_MANY || !paths.n); i++) {
            char *path = pt_android_open_uri(uris->v[i]);
            if (path) pt_push(paths, path);
            else {
                char *name = safe_name(uris->v[i]), *msg = pt_format("Could not open %s.", name);
                message_box(MSG_ERROR, APP_NAME, msg, NULL, NULL);
                free(name), free(msg);
            }
        }
    } else if (mode == FD_SAVE && uris->n) {
        char *path = copy_path(uris->v[0]);
        d = remember(path, uris->v[0]);
        d->mtime = d->size = -2;
        save_docs();
        pt_push(paths, path);
    } else if (mode == FD_DIR && uris->n) {
        char *dir = pt_format("%s/saf-out/%016llx-%lld", pt_android_cache_dir, fnv(uris->v[0]), (long long)time(NULL));
        pt_mkdirs(dir);
        pt_push(outs, (out_dir){pt_strdup(dir), pt_strdup(uris->v[0]), {0}});
        o = &outs.v[outs.n - 1];
        pt_push(paths, dir);
    } else if (mode == FD_REEXPORT && uris->n && subject && (d = by_path(subject))) {
        free(d->uri);
        d->uri = pt_strdup(uris->v[0]);
        d->mtime = d->size = -2;
        save_docs();
        sync_doc(d, 1);
        return;
    } else if (mode == FD_RENAME && uris->n && subject) {
        doc *r = by_path(subject);
        char *path = r ? rename_to(r, uris->v[0]) : NULL;
        if (path) pt_push(paths, path);
    }
    if (done) done(ctx, &paths);
    pt_strs_free(&paths);
    /* the window writes saved files while `done` runs; anything written later is caught by the watcher */
    if (d && (d = by_uri(uris->v[0]))) sync_doc(d, 1);
    if (o) scan_out(&outs.v[outs.n - 1], 1);
}

void file_dialogs_poll(void)
{
    static double watched;
    if (!docs_loaded) load_docs();
    if (pick.active) {
        char *result = pt_java("pickResult", 0);
        if (result) {
            pt_strs uris = {0};
            char *save = NULL;
            for (char *u = strtok_r(result, "\n", &save); u; u = strtok_r(NULL, "\n", &save))
                pt_push(uris, pt_strdup(u));
            free(result);
            int mode = pick.mode;
            ui_files_done done = pick.done;
            void *ctx = pick.ctx;
            char *subject = pick.subject;
            pick.active = 0, pick.subject = NULL;
            picked(mode, done, ctx, subject, &uris);
            free(subject);
            pt_strs_free(&uris);
            ui.dirty = 1;
        }
    }
    if (GetTime() - watched < 0.25) return;
    watched = GetTime();
    for (int i = 0; i < docs.n; i++) sync_doc(&docs.v[i], 0);
    for (int i = 0; i < outs.n; i++) scan_out(&outs.v[i], 0);
}

/* No trash on shared storage or behind a URI: Save replaces the file (or the copy, which is then written back)
 * in place. Files of the app itself (signatures) still go to the app's freedesktop trash. */
int move_to_trash(const char *path)
{
    if (by_file(path)) return 0;
    /* resolved on both sides: /data/data/... and /data/user/0/... name the same folder */
    char *own = pt_realpath(GetAndroidApp()->activity->internalDataPath), *real = pt_realpath(path);
    size_t n = strlen(own);
    int inside = !strncmp(real, own, n) && (!real[n] || real[n] == '/');
    free(own), free(real);
    return inside ? pt_desktop_move_to_trash(path) : 0;
}
