# Changelog

What changed in each release of cuttle (named PDF Tools before 0.6.0). The format follows
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/) and versions follow [Semantic Versioning](https://semver.org/).
A release's notes on GitHub are its section here, taken when its tag is pushed.

## [Unreleased]

## [0.6.3] - 2026-11-24

### Added
- **Rename on Android** (File > Rename...): Android's save dialog opens in the file's folder with its name filled in;
  the file is saved under the new name and the old one is deleted, so later saves still reach it. Offered where the
  app the file came from allows deleting it.
- **One Search** (the 🔍 at the end of the tab bar, Ctrl+Shift+K): find what you type in this document or in
  all tabs; matching tabs are listed below.

### Changed
- The toolbar's ⋮ menu and Find button are gone: their items are in the main menu (File, View), and Find is in
  Search (Ctrl+F still opens the find bar).
- Android: no vim keys, key hints or Keyboard shortcuts dialog; a hardware keyboard still has the default shortcuts.

### Fixed
- About: the licences and Close no longer run past the bottom of the dialog.

## [0.6.2] - 2026-11-23

### Added
- **Rename** (File > Rename..., F2): renames the open file in its folder. Unsaved edits stay in the tab, and the file
  keeps its reading position, bookmarks, star and place in Recent. Not on Android.

## [0.6.1] - 2026-11-16

### Fixed
- Home on a phone: the Open, New blank PDF and Combine & split buttons stack in one column when a row does not fit (the last one ran off the screen)
- About on a phone: the licences no longer sit under the divider line

## [0.6.0] - 2026-11-16

PDF Tools is now **cuttle**.

### What's new
- New name, new logo (file-pen-line on a dark tile), adaptive Android icon
- Nord theme, as in jackdaw and ferret: Dark / Light / System, colours from the desktop theme, every colour role settable in `[theme]` of settings.toml
- Text one size up; secondary text easier to read
- Menu > About cuttle: version, links, Check for updates and the startup check

### Moving from PDF Tools 0.5.7
PDF Tools cannot update itself to cuttle (new package names and Android app ID). Install cuttle as described in the [README](https://github.com/vamsikalagaturu/cuttle#install), then remove PDF Tools. Settings, signatures, reading positions and bookmarks move over on the first start (desktop).

## [0.5.7] - 2026-11-13

PDF Tools 0.5.7 makes fillable PDF forms: start from a blank PDF or any PDF, add fields, and share a form anyone can fill in their PDF viewer.

### New
- **Form tool (F):** drag a box, or click, to add a text field, checkbox, radio button, dropdown or signature field. A dialog sets its name, value, choices, font size (11 pt by default) and whether it is required or read-only; click it again to change these, drag it or its corner to move or resize it. Radio buttons with the same group name are one group.
- **Delete form fields:** with the Form tool, click a field already in the file and press Delete (or right-click > Delete field).
- **New blank PDF:** File > New blank PDF... and a button on Home make an empty A4, Letter or A5 PDF with any number of pages, opened with the Form tool.
- **Commands:** `pdf-tools new -o form.pdf` writes blank pages; `pdf-tools add-field` adds a field (`--type text|checkbox|radio|dropdown|signature`).

### Fixed
- **Forms saved from the window:** saving dropped the form's field list, so other viewers might not let anyone fill the fields. Forms keep it now; open an affected file and save it again to repair it.
- **Radio buttons:** filling a radio group (`fill` or the window) turned every button of the group on; now only the chosen one is. `fill` lists a group's choices when given another value.
- **Checkboxes in other viewers:** a ticked checkbox could show no tick outside PDF Tools.
- **Crop:** the dragged area stays visible while the crop menu asks what to crop.

## [0.5.6] - 2026-11-04

PDF Tools 0.5.6 fixes the window on scaled Wayland displays, where 0.5.5 could draw it in only part of its area, gives
it its name and icon in the GNOME dock, and adds `pdf-tools update` to update from the command line.

### New

- **`pdf-tools update`** installs the latest release from the command line, the way the window's Update does. `--check`
  only reports what is available, and `--force` also replaces a development build. Open windows keep the old version
  until restarted. It is the only command that uses the network.

### Changed

- The README's install steps now have download commands for Linux, macOS and Windows, and the Linux ones include
  `--install-desktop`.

### Fixed

- **Window on scaled Wayland displays** (for example GNOME at 125%, new in 0.5.5): a new window drew its interface in
  the bottom-left part of its area, with an empty band along the top and right, until it was resized. It now fills the
  window.
- **Name and icon on GNOME:** on Wayland the dock showed "Unknown" and a gear for the window. With the launcher entry
  from `--install-desktop` it now shows PDF Tools and its icon.

### Known limits

- A new window's monitor isn't known before it shows, so its size is limited by the smallest monitor: with monitors of
  different sizes it can open smaller than it needed to.
- On Windows, `pdf-tools update` swaps the folder just after it exits, so an open window can block the swap even though
  the command reported success.

## [0.5.5] - 2026-11-04

PDF Tools 0.5.5 fixes the window on scaled Wayland displays: the pointer now hits what it is over at any display
scale, and the window opens at a size that fits the screen.

### Fixed

- **Pointer on scaled Wayland displays** (for example GNOME at 125%): hover and clicks landed away from the pointer, by
  more the further right and down it was, so the highlight sat on the wrong button. They now land under it. The point a
  touchpad pinch zooms around gets the same correction.
- **Window size on scaled Wayland displays:** a new window no longer opens larger than a small scaled screen, and the
  minimum window size is the intended one.
- The interface follows a change of display scale at once, not up to a second later.

### Known limits

- A new window's monitor isn't known before it shows, so its size is limited by the smallest monitor: with monitors of
  different sizes it can open smaller than it needed to.

## [0.5.4] - 2026-11-04

PDF Tools 0.5.4 adds starred files, a reading mode, full screen and a dark mode for documents, fits the width to what a
page shows, and numbers lines closer to how LaTeX's lineno sets them.

### New

- **Starred files:** star a file from the toolbar, the menus or its row on Home. Home has a search field over one list,
  Starred above Recent.
- **Reading mode** (F8, View and More menus): hides the toolbar, tabs, status bar, sidebar and tool options with a short
  slide. Moving to the top edge brings the bars back over the page, a tap does on touch; Esc or Back leaves. Messages
  show as a floating note meanwhile.
- **Full screen** (F11, View and More menus).
- **Dark document mode** (F7, View and More menus; off at each start): pages are recoloured as they are drawn, by
  perceived brightness, not inverted. Photos keep their colours, slightly dimmed; scans are recoloured; yellow fills and
  blue text stay readable; highlights still show. What the tools draw follows the page's colours while you draw. The
  file keeps its real colours.
- **Vim keys:** `]c` and `[c` go to the next and previous column.
- **Go to tab:** `:tabs` or Ctrl+Shift+K.

### Changed

- **Fit width** fits what a document shows (text, images, drawings, comments), not the page, and leaves room for line
  numbers. The gaps beside the page are even, the scrollbar taken into account.
- **Line numbers**, compared with LaTeX's lineno over 16 layouts: running heads, feet and page numbers are skipped; a
  column cut short by figures is numbered as a column, not taken for margin notes; a middle column's numbers fit in its
  gutter. Captions, footnotes, table rows and titles are numbered.
- **Toolbar:** the Save button becomes a More menu that holds Save, and the zoom buttons sit closer to the percentage.

### Fixed

- Toggling full screen no longer makes the interface grow each time.
- Changing the interface size now works when `PDF_TOOLS_SCALE` is set.
- A minimum window size keeps the app usable. On small windows, menus are capped, long names are shortened in the middle,
  and submenus that would cover their menu wait for a press instead of opening on hover.
- File > Open recent opens in the right place.
- Even gaps in the status bar, and a clear highlight on the current tab.

### Android

- Scrolling while zoomed in keeps to its direction when the swipe is mostly straight, and flings are calmer.
- Back shows its question at once, without a touch on the screen.
- New window and Move to new window are gone.

## [0.5.3] - 2026-11-03

Android: File and other submenus open on tap; Back and a back row step out of a submenu.

## [0.5.2] - 2026-11-02

PDF Tools 0.5.2 makes zooming and paging through documents look sharp: zoom changes sharpen in one step instead of
tile by tile, page steps land on a sharp page, and line numbers handle equations, two columns and figures the way
LaTeX's lineno sets them.

### Changed

- **Zooming** (pinch, Ctrl+wheel, zoom buttons and keys) shows the page at the zoom it had, scaled, until the sharp
  page for the new zoom is ready, then switches in one step instead of sharpening tile by tile. While you pinch, a
  sharper page follows every small zoom step without waiting for you to stop.
- **Page steps** (Left/Right, Page Up/Down) land on a sharp page: the pages a step either way would show are prepared
  in advance. Flipping through pages quickly shows each page whole, and the page you stop on sharpens straight away.
- A page scrolling into view switches to its sharp version at once, not in patches.
- Zooming out no longer shimmers.
- **Line numbers:** a displayed equation (fractions, limits, sub- and superscripts, matrices) gets one number per line,
  and an equation number on its own line gets the next. On two-column pages each column is numbered from top to
  bottom, left column first, each number beside its own column. Text inside figures (plots, diagrams, images) and
  their labels is no longer numbered.

### Fixes

- Renders made in advance were thrown away when the window had been idle for a second, so prepared pages were often
  blurry after all.
- The Edits sidebar list no longer passes an empty list to the C library in a way the C standard leaves undefined
  (documents with no deleted annotations or no comments).

## [0.5.1] - 2026-11-02

PDF Tools 0.5.1 lets you recolour and re-widen drawings and other annotations already saved in a PDF, opens an
annotation's menu from the sidebar and with a long press while drawing, and shows unsaved edits on the page thumbnails.

### New

- **Colour and width of saved annotations**: drawings, shapes, lines, highlights, underlines and notes already in the
  PDF now have Colour in their menu, and drawings and shapes have Width. The change shows at once, can be undone, is
  listed under Edits as "Restyled" and is written when you save.
- **⋮ on each row of the Annotations sidebar** opens the annotation's menu (also right-click, or a long press on a
  phone).

### Changed

- Android: with the pen or another drawing tool on, holding a finger still on a drawing opens its menu (Delete,
  Colour, Width). Moving the finger draws as before.
- An annotation's menu no longer includes the Page submenu; right-click empty page space or use the Pages panel for
  page actions.

### Fixes

- Page thumbnails show drawings, shapes, text and other edits as soon as they are made, not only after saving.

## [0.5.0] - 2026-11-02

PDF Tools 0.5.0 adds vim keys, a command palette and editable shortcuts, numbers lines per page or paragraph and
across tables, zooms with a touchpad pinch, and exports comments with their line numbers.

### New

- **Vim keys** (View > Vim keys): a caret on the text moved by h/j/k/l, w/b/e, 0/$, gg/G; `v`/`V` select by
  character or line (a table's row across its cells) to copy or mark; `/`, `n`/`N`, `*`/`#`; `zz`/`zt`/`zb`;
  `viw`/`vip`; marks (`ma`, `'a`, `` `a ``; capitals across files); Enter or Ctrl+] follows a link, Ctrl+O/Ctrl+I
  go back and forward, also across files; `:` runs any action (`:w`, `:42`, `:zoom 150`). After a prefix a box
  lists the keys that can follow. A mouse selection becomes a visual one. `g?` lists every key.
- **Command palette** (Ctrl+K) runs any action by name.
- **Keyboard shortcuts** (Menu > Keyboard shortcuts...): every shortcut can be changed, with search and clash
  warnings; kept in `keys.toml`.
- **Figures, tables and equations** list in the sidebar.
- **Line numbers** through the document, per page or per paragraph (View > Line numbers). Tables are numbered
  once per row; margin notes are left out; a section number and its title are one line.
- **Shift+drag** selects whole lines.
- **Touchpad pinch** zooms on Linux (Wayland and X11) and macOS; on Windows a precision touchpad zooms smoothly.
- **Back and Forward** (Alt+Left/Right, mouse side buttons) after following a link, an outline entry or Go to page.
- **Link preview**: resting on a link into the document shows where it leads.
- **Search all open tabs** from the find bar, and **ignore accents** (Struber finds Strüber).
- **Export comments** (File > Export comments..., `pdf-tools comments`): highlights with their text, notes, text
  boxes and stamps, with page, line numbers, author, date and replies, as Markdown or JSON.
- The last session's tabs reopen at startup (`reopen_tabs`; not on Android). Zoom goes to 1000%.
- `search` and `redact` take `--ignore-accents`.

### Changed

- Text, reading order, search and regular expressions come from MuPDF itself: case folding across scripts,
  matches across line breaks and hyphenation. `--regex` uses JavaScript syntax (no lookbehind).
- Ruled (booktabs) tables are recognised by their rules, so selecting and numbering follow their rows.
- Office files (DOCX, XLSX, PPTX) are for the commands only: MuPDF reads their text but not their layout. Convert
  them with LibreOffice to view them.
- Linux installs made with `build.sh install PREFIX` into a folder you can write update themselves.
- Switching between two documents shows the other one at once, already sharp.
- Menus, the palette and popovers share a soft shadow; the tab close cross is lighter and turns red under the
  pointer; the status line keeps line : column and page / total in fixed places.

### Fixes

- A tab closes only when the click starts and ends on its cross.
- `redact --ignore-accents` also matches accented text in annotations.

## [0.4.2] - 2026-11-01

PDF Tools 0.4.2 makes pages sharp the moment you reach them, adds line numbers and double-click word selection, and
fixes the oversized cursor on macOS.

### New

- **Line numbers**: View > Line numbers numbers the text lines through the whole document in reading order, the
  way LaTeX's `lineno` package does. In two-column layouts the left column is numbered in the left margin and the
  right column in the right margin. The choice is remembered (`line_numbers` in `settings.toml`).
- **Double-click a word** to select it; with the Highlight or Redact tool, double-clicking marks that word.

### Smoother

- Pages are sharp as soon as you reach them, also after reading through many pages: on large screens the app used
  to stop preparing upcoming pages once its cache was full, so text took a moment to sharpen.
- Deleting an annotation, other edits and saving no longer flash blurred or white pages: the view changes in one
  step, already sharp.

### Fixes

- macOS: the mouse pointer was twice the normal size. The app now uses the system's own pointers (arrow, text,
  link hand, resize and others), which also follow the pointer size set in System Settings > Accessibility.

## [0.4.1] - 2026-11-01

PDF Tools 0.4.1 updates itself, scrolls and zooms more smoothly, and signs the Android app with a permanent key.

**Android: uninstall 0.4.0 (or older) before installing 0.4.1.** Earlier APKs were each signed with a throwaway
key, so Android refuses to install a new version over them ("package conflicts with an existing package").
From 0.4.1 on, every release uses the same key and installs over the previous one, from the app itself too.

### New

- **Updates**: at startup, at most once a day, the window checks GitHub for a newer release and offers
  **Update** / **Later**. Update downloads the package for your install, checks its SHA-256 against the digest
  GitHub lists, and installs it: the AppImage, the tarball's folder and the macOS app are replaced, the Windows
  folder is swapped when the app closes, and Android opens the system installer. Installs the app cannot replace
  itself (distribution packages, `build.sh install`) get a link to the release page instead.
  Menu > **Check for updates...** checks now; Menu > **Check for updates at startup** (`check_updates` in
  `settings.toml`) turns the check off. The request carries no data beyond a `pdf-tools/VERSION` user agent, and
  the commands (`pdf-tools info`, `redact`, ...) never use the network.
- Android asks for two new permissions for this, and only uses them for it: internet access and installing apps
  (the "Install unknown apps" switch, asked for the first time you tap Update).

### Smoother

- Pages are sharp as soon as scrolling or a zoom stops; before, text could stay blurry for up to a third of a
  second.
- Fit width / Fit height glide to their end without a jump.
- No more white pages while scrolling fast through long documents.
- Mouse-wheel notches glide instead of jumping; touchpads scroll directly as before.
- The window draws at the monitor's refresh rate (e.g. 120 Hz) instead of 60 fps.

### Build and release

- Pull requests build and test on Linux, macOS, Windows and Android; packages are built only when a release is
  published, and attached to it.
- Each platform's libraries are built by `platform/<os>/deps.sh` (run by `build.sh`); `build.sh check` builds and
  runs the tests without packing.

## [0.4.0] - 2026-10-31

PDF Tools 0.4.0 adds Windows, opens e-books, Office files, comics and images, makes redaction much stricter and cuts
the download size by about two thirds. **Updating is recommended**: 0.3.x could leave form values, comments and
stamps under a redaction in the saved file.

### New

- **Windows**: first release, built with MSVC. Window, command line, file dialogs, printing, Recycle Bin and
  "Open with". The command-line test suite passes on Windows; the window is new there, so please report problems.
- **More formats**: XPS/OXPS, comics (CBZ), images (PNG, JPEG, multi-page TIFF, GIF, BMP), SVG, e-books (EPUB,
  MOBI, FB2) and Office files (DOCX, XLSX, PPTX) open like PDFs: search, select, print, OCR, export. E-books lay
  out at a page and font size you choose (View > E-book layout...). Editing one makes a PDF copy; Save writes
  `name.pdf` next to the original, which is never changed.
- **`pdf-tools convert in.epub -o out.pdf`** turns any of those into a PDF (`--page-size`, `--font-size`).
- **Merge encrypted PDFs**: `merge --password-for FILE=PASSWORD`.
- **Passwords from files**: `--password-file`, `--current-password-file`, `--cert-password-file`, so passwords
  stay out of process listings.
- **Faster and lighter window**: it redraws only when something changes (0% CPU when idle, longer battery life
  on phones); pages render in the background without redrawing the whole window.
- **Toolbar**: fits narrow windows and phones in landscape; a tool's options get their own row, which scrolls.
- **Smaller downloads**: AppImage 35 → 11 MB, APK 38 → 12.5 MB. The large Chinese/Japanese/Korean and other
  fallback fonts are no longer bundled; the app uses the fonts installed on the system instead
  (build with `PT_FONTS=full` to bundle them).

### Redaction and privacy

- Redaction now deletes every annotation and form field under the box (comments, stamps, field values,
  authors), and removes the redacted text from bookmarks, page labels, link destinations and accessibility
  text. The tagged-PDF structure is dropped from redacted files.
- Text search and redaction also find text in form fields and comments: after `fill`, `redact --text` finds the
  filled-in value.
- Regular expressions work per line (`^`, `$`, `\b`) and no longer stop at non-ASCII text.
- Scrub (and every redaction) also removes text in hidden layers and outside the page, attached files linked from
  the catalog or pages, private application data and XFA form data.
- Visible text overlapping an invisible OCR layer is no longer deleted along with it.
- `merge` of selected pages no longer carries the left-out pages along through links.
- Saving an encrypted PDF keeps its encryption and permissions; when only the user password is known, the
  output gets a new owner password (reported as `owner_password_replaced`).

### Command line changes (check your scripts)

- Usage errors now exit **1** with `{"error": ...}` (they exited 2, which also meant "text left under a
  redaction"). Every command prints exactly one JSON object, also when it fails partway.
- An unknown command (`pdf-tools serach ...`) is an error instead of opening the window.
- An output can no longer replace any input, including through a hard link or symlink; outputs are written to a
  temporary file and renamed.
- `--help` lists every command.

### Android

- No storage permission any more: files open through the system picker or "Open with" and edits are written back
  to the same file.
- The status-bar strip at the top takes the app's colour instead of black.
- Links in PDFs open (they failed before).

### Fixes

- Menu actions (Draw, Shape, Fit width/height) take effect immediately on Android instead of after the next
  touch.
- Crashes and memory errors: closing a tab with Ctrl+W, pressing Enter with several dialogs open, malformed
  messages between windows, file dialogs answering after their tab closed, a cyclic form field in `merge`, a tiny
  `.synctex` file allocating gigabytes, `flatten --dpi 0`.
- Messages between windows are accepted only from your own user.
- A bad line in `settings.toml` no longer resets all settings, and settings are saved safely.
- A file that fails to reload no longer retries forever; documents rewritten on disk (LaTeX) cannot break the
  open view.
- Links with schemes other than http, https and mailto ask before opening.

### Known limitations

- ODT, WebP and CBR files are not supported.
- Arabic text in a PDF whose page text uses a non-embedded Latin-named font can show wrong letters (as before).
- Windows, macOS: unsigned builds.

## [0.3.1] - 2026-10-30

No notes were published for this release.

## [0.3.0] - 2026-10-30

No notes were published for this release.

## [0.2.0] - 2026-10-28

Smoother, more precise editing.

### Changes

- **Select letter by letter**: a text selection now starts and ends at the character
  under the pointer, so copy, mark and redact can take part of a word.
- **Smooth LaTeX preview**: a rebuilt PDF is swapped in place; page, zoom and scroll
  stay put and the view no longer blanks out.
- **Cursors that always work**: the app draws its own cursors (text, crosshair, hand,
  pencil, move, resize), independent of the desktop's cursor theme.
- **Pages sidebar**: larger, sharp thumbnails with the page number below, a tick box
  and an edit-count badge on each thumbnail, and a clearer current-page highlight.
  Page commands are in the right-click menus and the Pages menu.
- **Sidebar** slides in and out (F9) and remembers its width.
- **Status bar**: page count on the right; edit and tick counts appear on the left
  only when there are any, and no longer overlap messages.
- Docstrings throughout the code.

## [0.1.0] - 2026-10-28

First release of PDF Tools, a PDF viewer and editor for Linux.

### Highlights

- **Redact for real**: text, images and drawings under a box are removed, hidden data
  (metadata, attachments, scripts, hidden text) is stripped, and leftovers are reported.
- **Edit**: type text on the page (fonts, bold, italic), marks and comments, notes,
  signatures, images, freehand drawing, form filling; everything undoable and listed
  per page in the Edits tab, where single edits can be discarded.
- **Pages**: rotate, remove, reorder, insert, crop (content outside is deleted).
- **Combine & split**: one workspace to merge PDFs and images, split by cuts or every
  N pages, and extract pages.
- **Passwords**: open protected PDFs, keep or add AES-256 protection.
- **Print** with preview, export pages, flatten to images.
- **LaTeX**: auto-reload on rebuild, SyncTeX Ctrl+click to source and
  `pdf-tools --forward` from the editor.
- **Windows and tabs**: several documents and windows in one running app.
- **Command line for scripts and agents**: `info`, `redact`, `fill`, `pages`, `merge`,
  `split`, `protect`, `unprotect`, `flatten`, `synctex` with JSON output
  (`pdf-tools help`); the manual opens with `pdf-tools --help`.

License: AGPL-3.0-or-later.

[Unreleased]: https://github.com/vamsikalagaturu/cuttle/compare/v0.6.2...HEAD
[0.6.2]: https://github.com/vamsikalagaturu/cuttle/compare/v0.6.1...v0.6.2
[0.6.1]: https://github.com/vamsikalagaturu/cuttle/compare/v0.6.0...v0.6.1
[0.6.0]: https://github.com/vamsikalagaturu/cuttle/compare/v0.5.7...v0.6.0
[0.5.7]: https://github.com/vamsikalagaturu/cuttle/compare/v0.5.6...v0.5.7
[0.5.6]: https://github.com/vamsikalagaturu/cuttle/compare/v0.5.5...v0.5.6
[0.5.5]: https://github.com/vamsikalagaturu/cuttle/compare/v0.5.4...v0.5.5
[0.5.4]: https://github.com/vamsikalagaturu/cuttle/compare/v0.5.3...v0.5.4
[0.5.3]: https://github.com/vamsikalagaturu/cuttle/compare/v0.5.2...v0.5.3
[0.5.2]: https://github.com/vamsikalagaturu/cuttle/compare/v0.5.1...v0.5.2
[0.5.1]: https://github.com/vamsikalagaturu/cuttle/compare/v0.5.0...v0.5.1
[0.5.0]: https://github.com/vamsikalagaturu/cuttle/compare/v0.4.2...v0.5.0
[0.4.2]: https://github.com/vamsikalagaturu/cuttle/compare/v0.4.1...v0.4.2
[0.4.1]: https://github.com/vamsikalagaturu/cuttle/compare/v0.4.0...v0.4.1
[0.4.0]: https://github.com/vamsikalagaturu/cuttle/compare/v0.3.1...v0.4.0
[0.3.1]: https://github.com/vamsikalagaturu/cuttle/compare/v0.3.0...v0.3.1
[0.3.0]: https://github.com/vamsikalagaturu/cuttle/compare/v0.2.0...v0.3.0
[0.2.0]: https://github.com/vamsikalagaturu/cuttle/compare/v0.1.0...v0.2.0
[0.1.0]: https://github.com/vamsikalagaturu/cuttle/releases/tag/v0.1.0
