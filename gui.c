#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define chdir _chdir
#else
#include <unistd.h>
#include <libgen.h>
#endif
#ifdef _WIN32
#define EXE_NAME "alsdd.exe"
#else
#define EXE_NAME "./alsdd"
#endif

#define SIDEBAR_W  290
#define WIN_W      1100
#define WIN_H      700

/* ── Refined Dark Palette ── */
#define C_BG        "#0a0c10"
#define C_SURF      "#10141c"
#define C_SURF2     "#161d2a"
#define C_SURF3     "#1c2538"
#define C_BORDER    "#1e2d42"
#define C_BORDER2   "#243347"
#define C_ACCENT    "#4d9fff"
#define C_ACCENT2   "#2b7de9"
#define C_ACCENT_DIM "#1a3d6b"
#define C_TEXT      "#dce8f5"
#define C_TEXT2     "#a8bdd4"
#define C_MUTED     "#5a7a9b"
#define C_SUCCESS   "#38c96b"
#define C_WARN      "#f0a520"
#define C_ERROR     "#f05b5b"
#define C_HEADER    "#080a0f"
#define C_GLOW      "rgba(77,159,255,0.08)"

/* Module colors */
#define MOD_LL_COL    "#4d9fff"
#define MOD_STK_COL   "#38c96b"
#define MOD_BST_COL   "#f0a520"
#define MOD_REC_COL   "#c57cff"

static char g_exe_dir[4096] = {0};

static void wcss(GtkWidget *w, const char *css)
{
    GtkCssProvider *p = gtk_css_provider_new();
    gtk_css_provider_load_from_data(p, css, -1, NULL);
    gtk_style_context_add_provider(gtk_widget_get_style_context(w),
        GTK_STYLE_PROVIDER(p), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(p);
}

static void wcss_screen(const char *css)
{
    GtkCssProvider *p = gtk_css_provider_new();
    gtk_css_provider_load_from_data(p, css, -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(p), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(p);
}

static void show_main_menu(void);
static void show_sub_menu(int module, const char *title);
static void run_action(int module, int sub, const char **labels, int n);

static GtkWidget *g_window  = NULL;
static GtkWidget *g_sidebar = NULL;
static GtkWidget *g_output  = NULL;
static GtkWidget *g_status  = NULL;
static GtkWidget *g_status_icon = NULL;

typedef struct { const char *label; const char *fields[6]; } Action;

static const Action LL_ACTIONS[] = {
    { "Display all personalities",          { NULL } },
    { "Sort alphabetically by name",        { NULL } },
    { "Sort by name length",                { NULL } },
    { "Sort by birth year",                 { NULL } },
    { "Search by Date of Birth",            { "Date of birth (e.g. 14/12/1922):", NULL } },
    { "Search by Date of Death",            { "Date of death (e.g. 18/10/1970):", NULL } },
    { "Find palindrome names",              { NULL } },
    { "Find similar by date string",        { "Date string:", NULL } },
    { "Find by exact date",                 { "Day:", "Month:", "Year:", NULL } },
    { "Add a personality",                  { "Name:", "Date of birth (DD/MM/YYYY):", "Date of death (DD/MM/YYYY):", NULL } },
    { "Add an event",                       { "Event name:", "Date:", NULL } },
    { "Update a personality",               { "Name:", "New definition:", "New DoB:", "New DoD:", NULL } },
    { "Delete a personality",               { "Name to delete:", NULL } },
    { "Merge first nodes (bidirectional)",  { NULL } },
    { "Name queue (sName)",                 { NULL } },
    { "Age queue (ageP)",                   { NULL } },
    { "Full queue (toQueue)",               { NULL } },
};
#define LL_N  (int)(sizeof(LL_ACTIONS)/sizeof(LL_ACTIONS[0]))

static const Action STK_ACTIONS[] = {
    { "Display stack",                        { NULL } },
    { "Search personality by name",           { "Name:", NULL } },
    { "Sort stack alphabetically",            { NULL } },
    { "Sort by definition word count",        { NULL } },
    { "Split short / long events",            { NULL } },
    { "Add a personality",                    { "Name:", "Definition:", "Date of birth (DD/MM/YYYY):", "Date of death (DD/MM/YYYY):", NULL } },
    { "Delete a personality",                 { "Name to delete:", NULL } },
    { "Update a personality",                 { "Name:", "New definition:", "New DoB:", "New DoD:", NULL } },
    { "Get shortest definition",              { NULL } },
    { "Continuous (overlapping) events",      { NULL } },
    { "Check if personality was killed",      { "Name:", NULL } },
    { "Reverse stack (recursive)",            { NULL } },
    { "Convert stack to sorted queue",        { NULL } },
    { "Convert stack to bidirectional list",  { NULL } },
};
#define STK_N (int)(sizeof(STK_ACTIONS)/sizeof(STK_ACTIONS[0]))

static const Action BST_ACTIONS[] = {
    { "Display tree (in-order)",             { NULL } },
    { "Pre-order traversal",                 { NULL } },
    { "Post-order traversal",                { NULL } },
    { "Search by name",                      { "Name:", NULL } },
    { "Add a name",                          { "Name:", "Date of birth:", "Date of death:", NULL } },
    { "Delete a name",                       { "Name:", NULL } },
    { "Update a name",                       { "Name:", "New definition:", "New DoB:", "New DoD:", NULL } },
    { "Height and size",                     { NULL } },
    { "Lowest Common Ancestor",              { "First name:", "Second name:", NULL } },
    { "Count nodes in birth-year range",     { "Low year:", "High year:", NULL } },
    { "In-order successor",                  { "Name:", NULL } },
    { "Mirror the tree",                     { NULL } },
    { "Check if tree is balanced",           { NULL } },
    { "Merge with a fresh copy of the tree", { NULL } },
};
#define BST_N (int)(sizeof(BST_ACTIONS)/sizeof(BST_ACTIONS[0]))

static const Action REC_ACTIONS[] = {
    { "Count occurrences of a name",         { "Name:", NULL } },
    { "Remove all occurrences of a name",    { "Name/word:", NULL } },
    { "Replace occurrences (update dates)",  { "Name:", "New DoB:", "New DoD:", NULL } },
    { "Print all permutations of a name",    { "Name:", NULL } },
    { "Generate all subsequences",           { "Word:", NULL } },
    { "Print overlapping date range",        { "Start (DD/MM/YYYY):", "End (DD/MM/YYYY):", NULL } },
    { "Count distinct subsequences",         { "String:", NULL } },
    { "Check if a string is a palindrome",   { "String:", NULL } },
};
#define REC_N (int)(sizeof(REC_ACTIONS)/sizeof(REC_ACTIONS[0]))

/* ── Global CSS ── */
static void apply_global_css(void)
{
    wcss_screen(
        "* { -gtk-outline-radius: 0; }"
        "scrollbar { background: #080a0f; border: none; min-width: 6px; }"
        "scrollbar slider { background: #1e2d42; border-radius: 3px; min-width: 6px; }"
        "scrollbar slider:hover { background: #243347; }"
        "scrollbar.vertical { border-left: 1px solid #0d1520; }"
    );
}

static void output_set(const char *text)
{
    GtkTextBuffer *buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(g_output));
    gtk_text_buffer_set_text(buf, text ? text : "", -1);
    GtkTextIter it;
    gtk_text_buffer_get_start_iter(buf, &it);
    gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(g_output), &it, 0.0, FALSE, 0, 0);
}

static void status_set(const char *msg, const char *color, const char *icon)
{
    gtk_label_set_text(GTK_LABEL(g_status), msg);
    if (g_status_icon) gtk_label_set_text(GTK_LABEL(g_status_icon), icon ? icon : "●");

    char css[512];
    snprintf(css, sizeof(css),
        "label { font-size: 11px; font-family: 'JetBrains Mono', 'Fira Code', monospace;"
        "        color: %s; letter-spacing: 0.3px; }",
        color ? color : C_MUTED);
    wcss(g_status, css);

    if (g_status_icon) {
        char icon_css[256];
        snprintf(icon_css, sizeof(icon_css),
            "label { font-size: 10px; color: %s; padding-right: 6px; }",
            color ? color : C_MUTED);
        wcss(g_status_icon, icon_css);
    }
}

static void do_run(const char *stdin_str)
{
    if (g_exe_dir[0]) chdir(g_exe_dir);
    status_set("Running process...", C_WARN, "◌");

    while (gtk_events_pending()) gtk_main_iteration();

    GString *result = g_string_new(NULL);
#ifdef _WIN32
    FILE *fp = _popen(EXE_NAME, "r+");
    if (!fp) {
        output_set("  ✗  Error: Could not launch " EXE_NAME "\n\n"
                   "  Make sure alsdd.exe is in the same folder as gui.exe");
        g_string_free(result, TRUE);
        status_set("Launch failed", C_ERROR, "✗");
        return;
    }
    fwrite(stdin_str, 1, strlen(stdin_str), fp);
    fflush(fp);
    char buf[512];
    while (fgets(buf, sizeof(buf), fp))
        g_string_append(result, buf);
    _pclose(fp);
#else
    char tmppath[] = "/tmp/alsdd_in_XXXXXX";
    int fd = mkstemp(tmppath);
    if (fd < 0) {
        output_set("  ✗  Error: Could not create temporary file.");
        g_string_free(result, TRUE);
        status_set("Error", C_ERROR, "✗");
        return;
    }
    write(fd, stdin_str, strlen(stdin_str));
    close(fd);
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), EXE_NAME " < %s 2>&1", tmppath);
    FILE *fp = popen(cmd, "r");
    if (!fp) {
        unlink(tmppath);
        output_set("  ✗  Error: Could not launch " EXE_NAME "\n\n"
                   "  Ensure the 'alsdd' binary is in the same folder as 'gui'\n"
                   "  and has execute permission:  chmod +x alsdd");
        g_string_free(result, TRUE);
        status_set("Launch failed", C_ERROR, "✗");
        return;
    }
    char buf[512];
    while (fgets(buf, sizeof(buf), fp))
        g_string_append(result, buf);
    pclose(fp);
    unlink(tmppath);
#endif

    /* Prepend a small spacer line for readability */
    GString *formatted = g_string_new("\n");
    g_string_append(formatted, result->str);
    g_string_free(result, TRUE);

    output_set(formatted->str);
    g_string_free(formatted, TRUE);
    status_set("Completed successfully", C_SUCCESS, "✓");
}

/* ── Input Dialog ── */
static gboolean input_dialog(const char *title,
                              const char **labels, int n, char **answers)
{
    GtkWidget *dlg = gtk_dialog_new_with_buttons(
        title, GTK_WINDOW(g_window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Execute", GTK_RESPONSE_OK, NULL);
    gtk_dialog_set_default_response(GTK_DIALOG(dlg), GTK_RESPONSE_OK);
    gtk_window_set_default_size(GTK_WINDOW(dlg), 440, -1);

    wcss(dlg,
        "dialog {"
        "  background: " C_SURF "; border-radius: 10px;"
        "}"
        ".dialog-vbox { padding: 0; }"
        ".dialog-action-area {"
        "  padding: 12px 20px 16px;"
        "  border-top: 1px solid " C_BORDER ";"
        "  background: " C_SURF2 ";"
        "}"
        ".dialog-action-area button {"
        "  border-radius: 7px; padding: 8px 22px;"
        "  font-size: 12px; font-family: 'JetBrains Mono', monospace;"
        "  border: 1px solid " C_BORDER2 "; color: " C_TEXT2 ";"
        "  background: " C_SURF3 "; transition: all 120ms ease;"
        "}"
        ".dialog-action-area button:last-child {"
        "  background: " C_ACCENT2 "; color: #ffffff;"
        "  border-color: " C_ACCENT "; font-weight: bold;"
        "}"
        ".dialog-action-area button:hover { opacity: 0.88; }"
        ".dialog-action-area button:active { opacity: 0.75; }");

    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dlg));
    gtk_container_set_border_width(GTK_CONTAINER(content), 0);
    gtk_box_set_spacing(GTK_BOX(content), 0);

    /* Dialog header */
    GtkWidget *dlg_header = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    wcss(dlg_header,
        "box { background: " C_SURF2 "; border-bottom: 1px solid " C_BORDER ";"
        "      padding: 18px 22px 14px; }");
    GtkWidget *dlg_title = gtk_label_new(title);
    gtk_label_set_xalign(GTK_LABEL(dlg_title), 0.0f);
    wcss(dlg_title,
        "label { font-size: 14px; font-weight: bold; color: " C_TEXT ";"
        "        font-family: 'JetBrains Mono', monospace; }");
    GtkWidget *dlg_sub = gtk_label_new("Fill in the fields below and click Execute");
    gtk_label_set_xalign(GTK_LABEL(dlg_sub), 0.0f);
    wcss(dlg_sub, "label { font-size: 11px; color: " C_MUTED "; margin-top: 2px; }");
    gtk_box_pack_start(GTK_BOX(dlg_header), dlg_title, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(dlg_header), dlg_sub,   FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(content), dlg_header, FALSE, FALSE, 0);

    /* Fields area */
    GtkWidget *fields_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_set_border_width(GTK_CONTAINER(fields_box), 22);
    gtk_box_set_spacing(GTK_BOX(fields_box), 4);
    gtk_box_pack_start(GTK_BOX(content), fields_box, FALSE, FALSE, 0);

    GtkWidget *entries[6] = {0};
    for (int i = 0; i < n; i++) {
        GtkWidget *lbl = gtk_label_new(labels[i]);
        gtk_widget_set_halign(lbl, GTK_ALIGN_START);
        wcss(lbl,
            "label { font-size: 10px; color: " C_MUTED ";"
            "        font-family: 'JetBrains Mono', monospace;"
            "        text-transform: uppercase; letter-spacing: 0.8px;"
            "        margin-top: 14px; margin-bottom: 4px; }");
        gtk_box_pack_start(GTK_BOX(fields_box), lbl, FALSE, FALSE, 0);

        entries[i] = gtk_entry_new();
        gtk_entry_set_activates_default(GTK_ENTRY(entries[i]), TRUE);
        wcss(entries[i],
             "entry {"
             "  font-size: 13px; font-family: 'JetBrains Mono', monospace;"
             "  border: 1px solid " C_BORDER2 "; border-radius: 7px;"
             "  padding: 9px 14px; background: " C_BG ";"
             "  color: " C_TEXT "; transition: border-color 150ms;"
             "  caret-color: " C_ACCENT ";"
             "}"
             "entry:focus {"
             "  border-color: " C_ACCENT ";"
             "  background: #0c1018;"
             "}");
        gtk_box_pack_start(GTK_BOX(fields_box), entries[i], FALSE, FALSE, 0);
    }

    if (n > 0 && entries[0]) gtk_widget_grab_focus(entries[0]);
    gtk_widget_show_all(dlg);

    gint resp = gtk_dialog_run(GTK_DIALOG(dlg));
    if (resp != GTK_RESPONSE_OK) { gtk_widget_destroy(dlg); return FALSE; }
    for (int i = 0; i < n; i++)
        answers[i] = g_strdup(gtk_entry_get_text(GTK_ENTRY(entries[i])));
    gtk_widget_destroy(dlg);
    return TRUE;
}

static void run_action(int mod, int sub, const char **flabels, int nf)
{
    char *answers[6] = {0};
    if (nf > 0 && !input_dialog("Parameters", flabels, nf, answers))
        return;
    GString *s = g_string_new(NULL);
    g_string_append_printf(s, "%d\n%d\n", mod, sub);
    for (int i = 0; i < nf; i++) {
        g_string_append_printf(s, "%s\n", answers[i] ? answers[i] : "");
        g_free(answers[i]);
    }
    g_string_append(s, "0\n0\n");
    do_run(s->str);
    g_string_free(s, TRUE);
}

/* ── Module palette ── */
static const char *MODULE_COLORS[] = {
    MOD_LL_COL, MOD_STK_COL, MOD_BST_COL, MOD_REC_COL
};
static const char *MODULE_ICONS[] = { "⊞", "⊟", "⊿", "↺" };
static const char *MODULE_TAGS[]  = { "LL", "STK", "BST", "REC" };

/* ── Widget builders ── */
static GtkWidget *make_lbl(const char *text, const char *css)
{
    GtkWidget *w = gtk_label_new(text);
    gtk_label_set_xalign(GTK_LABEL(w), 0.0f);
    if (css) wcss(w, css);
    return w;
}

static GtkWidget *make_sep(void)
{
    GtkWidget *s = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    wcss(s, "separator { min-height: 1px; background: " C_BORDER ";"
            "            margin: 10px 0; opacity: 0.6; }");
    return s;
}

static GtkWidget *make_module_btn(const char *icon, const char *text,
                                   const char *tag, int idx)
{
    GtkWidget *btn = gtk_button_new();
    gtk_button_set_relief(GTK_BUTTON(btn), GTK_RELIEF_NONE);
    gtk_widget_set_hexpand(btn, TRUE);

    const char *col = (idx >= 0 && idx < 4) ? MODULE_COLORS[idx] : C_ACCENT;

    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    /* Icon badge */
    GtkWidget *icon_lbl = gtk_label_new(icon);
    char icon_css[200];
    snprintf(icon_css, sizeof(icon_css),
        "label { font-size: 16px; color: %s; min-width: 28px; }", col);
    wcss(icon_lbl, icon_css);
    gtk_box_pack_start(GTK_BOX(hbox), icon_lbl, FALSE, FALSE, 0);

    /* Text block */
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
    gtk_widget_set_hexpand(vbox, TRUE);

    GtkWidget *name_lbl = gtk_label_new(text);
    gtk_label_set_xalign(GTK_LABEL(name_lbl), 0.0f);
    wcss(name_lbl,
        "label { font-size: 12px; color: " C_TEXT ";"
        "        font-family: 'JetBrains Mono', monospace;"
        "        font-weight: 600; }");

    GtkWidget *tag_lbl = gtk_label_new(tag);
    gtk_label_set_xalign(GTK_LABEL(tag_lbl), 0.0f);
    char tag_css[200];
    snprintf(tag_css, sizeof(tag_css),
        "label { font-size: 9px; color: %s; letter-spacing: 1px;"
        "        font-family: 'JetBrains Mono', monospace; opacity: 0.8; }", col);
    wcss(tag_lbl, tag_css);

    gtk_box_pack_start(GTK_BOX(vbox), name_lbl, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), tag_lbl,  FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, TRUE, 6);

    /* Arrow */
    GtkWidget *arrow = gtk_label_new("›");
    wcss(arrow, "label { font-size: 16px; color: " C_MUTED "; }");
    gtk_box_pack_end(GTK_BOX(hbox), arrow, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(btn), hbox);

    char full[900];
    snprintf(full, sizeof(full),
        "button {"
        "  background: " C_SURF2 ";"
        "  border: 1px solid " C_BORDER ";"
        "  border-left: 3px solid %s;"
        "  border-radius: 8px;"
        "  padding: 12px 14px;"
        "  margin: 3px 0;"
        "  transition: background 120ms ease;"
        "}"
        "button:hover {"
        "  background: " C_SURF3 ";"
        "  border-color: %s;"
        "}",
        col, col);
    wcss(btn, full);
    return btn;
}

static GtkWidget *make_action_btn(const char *text, int number, int mod_idx)
{
    GtkWidget *btn = gtk_button_new();
    gtk_button_set_relief(GTK_BUTTON(btn), GTK_RELIEF_NONE);
    gtk_widget_set_hexpand(btn, TRUE);

    const char *col = (mod_idx >= 0 && mod_idx < 4) ? MODULE_COLORS[mod_idx] : C_ACCENT;

    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);

    /* Number badge */
    char num_str[8];
    snprintf(num_str, sizeof(num_str), "%02d", number);
    GtkWidget *num_lbl = gtk_label_new(num_str);
    char num_css[200];
    snprintf(num_css, sizeof(num_css),
        "label { font-size: 9px; color: %s; font-family: 'JetBrains Mono', monospace;"
        "        min-width: 22px; font-weight: bold; opacity: 0.7; }", col);
    wcss(num_lbl, num_css);
    gtk_box_pack_start(GTK_BOX(hbox), num_lbl, FALSE, FALSE, 0);

    /* Label */
    GtkWidget *lbl = gtk_label_new(text);
    gtk_label_set_xalign(GTK_LABEL(lbl), 0.0f);
    gtk_label_set_ellipsize(GTK_LABEL(lbl), PANGO_ELLIPSIZE_END);
    gtk_widget_set_hexpand(lbl, TRUE);
    wcss(lbl,
        "label { font-size: 12px; color: " C_TEXT2 ";"
        "        font-family: 'JetBrains Mono', monospace; }");
    gtk_box_pack_start(GTK_BOX(hbox), lbl, TRUE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(btn), hbox);

    char css[512];
    snprintf(css, sizeof(css),
        "button {"
        "  background: transparent;"
        "  border: none; border-radius: 6px;"
        "  padding: 8px 10px; margin: 1px 0;"
        "  transition: background 100ms ease;"
        "}"
        "button:hover {"
        "  background: " C_SURF3 ";"
        "}"
        "button:hover label { color: %s; }"
        "button:active { background: " C_BORDER "; }",
        col);
    wcss(btn, css);
    return btn;
}

/* ── Data structures ── */
typedef struct { int module; const char *title; int color_idx; } SubMenuData;
static SubMenuData g_sub_data[4];

typedef struct { int module; int sub; const char **fields; int nf; int mod_idx; } ActionData;
static ActionData g_ad_ll[LL_N], g_ad_stk[STK_N], g_ad_bst[BST_N], g_ad_rec[REC_N];

static int nfields(const char * const *f) { int n=0; while(f[n]) n++; return n; }

static void build_ad(ActionData *arr, const Action *a, int n, int mod, int midx) {
    for (int i=0;i<n;i++){
        arr[i].module=mod; arr[i].sub=i+1;
        arr[i].fields=(const char**)a[i].fields;
        arr[i].nf=nfields(a[i].fields);
        arr[i].mod_idx=midx;
    }
}

static void on_module_clicked(GtkButton *b, gpointer d)
{
    (void)b;
    SubMenuData *sd = (SubMenuData *)d;
    show_sub_menu(sd->module, sd->title);
}

static void on_action_clicked(GtkButton *b, gpointer d)
{
    (void)b;
    ActionData *ad = (ActionData *)d;
    run_action(ad->module, ad->sub, ad->fields, ad->nf);
}

static void on_back_clicked(GtkButton *b, gpointer d) { (void)b; (void)d; show_main_menu(); }

/* ── Main menu ── */
static void show_main_menu(void)
{
    GList *ch = gtk_container_get_children(GTK_CONTAINER(g_sidebar));
    for (GList *l = ch; l; l = l->next) gtk_widget_destroy(GTK_WIDGET(l->data));
    g_list_free(ch);

    /* Branding */
    GtkWidget *brand = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
    gtk_widget_set_margin_bottom(brand, 4);

    GtkWidget *brand_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *brand_icon = gtk_label_new("⬡");
    wcss(brand_icon,
        "label { font-size: 28px; color: " C_ACCENT ";"
        "        font-family: monospace; padding-top: 2px; }");
    gtk_box_pack_start(GTK_BOX(brand_row), brand_icon, FALSE, FALSE, 0);

    GtkWidget *brand_text = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
    gtk_box_pack_start(GTK_BOX(brand_text),
        make_lbl("ALSDD",
            "label { font-size: 20px; font-weight: bold; color: " C_ACCENT ";"
            "        font-family: 'JetBrains Mono', monospace; line-height: 1; }"),
        FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(brand_text),
        make_lbl("History Database",
            "label { font-size: 9px; color: " C_MUTED ";"
            "        letter-spacing: 1.5px; text-transform: uppercase; }"),
        FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(brand_row), brand_text, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(brand), brand_row, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(g_sidebar), brand, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(g_sidebar), make_sep(), FALSE, FALSE, 0);

    /* Section label */
    gtk_box_pack_start(GTK_BOX(g_sidebar),
        make_lbl("MODULES",
            "label { font-size: 9px; color: " C_MUTED ";"
            "        letter-spacing: 2.5px; padding: 6px 2px 10px; }"),
        FALSE, FALSE, 0);

    static const struct { const char *icon; const char *t; const char *tag; } M[] = {
        { "⊞", "Linked Lists & Queues", "MODULE · LL" },
        { "⊟", "Stacks",               "MODULE · STK" },
        { "⊿", "Binary Search Tree",   "MODULE · BST" },
        { "↺",  "Recursion",            "MODULE · REC" },
    };
    for (int i = 0; i < 4; i++) {
        GtkWidget *btn = make_module_btn(M[i].icon, M[i].t, M[i].tag, i);
        g_sub_data[i].module    = i + 1;
        g_sub_data[i].title     = M[i].t;
        g_sub_data[i].color_idx = i;
        g_signal_connect(btn, "clicked", G_CALLBACK(on_module_clicked), &g_sub_data[i]);
        gtk_box_pack_start(GTK_BOX(g_sidebar), btn, FALSE, FALSE, 0);
    }

    /* Spacer */
    gtk_box_pack_start(GTK_BOX(g_sidebar),
        gtk_box_new(GTK_ORIENTATION_VERTICAL, 0), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(g_sidebar), make_sep(), FALSE, FALSE, 0);

    /* Footer row */
    GtkWidget *footer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget *ver = gtk_label_new("v1.0.0");
    wcss(ver, "label { font-size: 10px; color: " C_BORDER2 ";"
              "        font-family: 'JetBrains Mono', monospace; }");
    gtk_box_pack_start(GTK_BOX(footer), ver, TRUE, TRUE, 0);

    GtkWidget *ex = gtk_button_new_with_label("Quit");
    gtk_button_set_relief(GTK_BUTTON(ex), GTK_RELIEF_NONE);
    wcss(ex,
        "button { background: transparent; color: " C_MUTED ";"
        "         border: 1px solid " C_BORDER2 "; border-radius: 6px;"
        "         padding: 5px 14px; font-size: 11px;"
        "         font-family: 'JetBrains Mono', monospace; }"
        "button:hover { color: " C_ERROR "; border-color: " C_ERROR ";"
        "               background: rgba(240,91,91,0.06); }");
    g_signal_connect(ex, "clicked", G_CALLBACK(gtk_main_quit), NULL);
    gtk_box_pack_end(GTK_BOX(footer), ex, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(g_sidebar), footer, FALSE, FALSE, 0);

    gtk_widget_show_all(g_sidebar);

    output_set(
        "  Welcome to ALSDD — Algeria History Database\n"
        "  ─────────────────────────────────────────────\n\n"
        "  Select a module from the sidebar to begin.\n\n"
        "  Available Modules:\n\n"
        "    ⊞  Linked Lists & Queues    — LL\n"
        "    ⊟  Stacks                   — STK\n"
        "    ⊿  Binary Search Tree       — BST\n"
        "    ↺  Recursion                — REC\n\n"
        "  ─────────────────────────────────────────────\n"
        "  Algeria · Data Structures & Algorithms\n"
    );
    status_set("Ready — select a module", C_MUTED, "●");
}

/* ── Sub-menu ── */
static void show_sub_menu(int module, const char *title)
{
    GList *ch = gtk_container_get_children(GTK_CONTAINER(g_sidebar));
    for (GList *l = ch; l; l = l->next) gtk_widget_destroy(GTK_WIDGET(l->data));
    g_list_free(ch);

    int cidx = module - 1;
    const char *col = (cidx >= 0 && cidx < 4) ? MODULE_COLORS[cidx] : C_ACCENT;

    /* Back button */
    GtkWidget *back = gtk_button_new_with_label("← Back to Modules");
    gtk_button_set_relief(GTK_BUTTON(back), GTK_RELIEF_NONE);
    wcss(back,
        "button { background: transparent; color: " C_MUTED ";"
        "         border: none; border-radius: 6px; padding: 7px 8px; font-size: 11px;"
        "         font-family: 'JetBrains Mono', monospace; }"
        "button:hover { color: " C_TEXT2 "; background: " C_SURF2 "; }");
    g_signal_connect(back, "clicked", G_CALLBACK(on_back_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(g_sidebar), back, FALSE, FALSE, 0);

    /* Module header */
    GtkWidget *mod_hdr = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    wcss(mod_hdr, "box { padding: 8px 2px 4px; }");

    char icon_buf[8];
    snprintf(icon_buf, sizeof(icon_buf), "%s", cidx >= 0 && cidx < 4 ? MODULE_ICONS[cidx] : "●");
    GtkWidget *m_icon = gtk_label_new(icon_buf);
    char m_icon_css[150];
    snprintf(m_icon_css, sizeof(m_icon_css),
        "label { font-size: 20px; color: %s; }", col);
    wcss(m_icon, m_icon_css);
    gtk_box_pack_start(GTK_BOX(mod_hdr), m_icon, FALSE, FALSE, 0);

    GtkWidget *m_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
    char title_css[256];
    snprintf(title_css, sizeof(title_css),
        "label { font-size: 13px; font-weight: bold; color: %s;"
        "        font-family: 'JetBrains Mono', monospace; }", col);
    gtk_box_pack_start(GTK_BOX(m_vbox), make_lbl(title, title_css), FALSE, FALSE, 0);
    char tag_line[32];
    snprintf(tag_line, sizeof(tag_line), "MODULE · %s",
             cidx >= 0 && cidx < 4 ? MODULE_TAGS[cidx] : "?");
    gtk_box_pack_start(GTK_BOX(m_vbox),
        make_lbl(tag_line,
            "label { font-size: 9px; color: " C_MUTED ";"
            "        letter-spacing: 1.5px; }"),
        FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(mod_hdr), m_vbox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(g_sidebar), mod_hdr, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(g_sidebar), make_sep(), FALSE, FALSE, 0);

    /* Actions scroll */
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
        GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    wcss(scroll, "scrolledwindow { background: transparent; }");

    GtkWidget *inner = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_set_border_width(GTK_CONTAINER(inner), 2);

    const Action *actions = NULL; ActionData *ad = NULL; int n = 0;
    switch (module) {
        case 1: actions=LL_ACTIONS;  n=LL_N;
                build_ad(g_ad_ll,  actions,n,module,cidx); ad=g_ad_ll;  break;
        case 2: actions=STK_ACTIONS; n=STK_N;
                build_ad(g_ad_stk, actions,n,module,cidx); ad=g_ad_stk; break;
        case 3: actions=BST_ACTIONS; n=BST_N;
                build_ad(g_ad_bst, actions,n,module,cidx); ad=g_ad_bst; break;
        case 4: actions=REC_ACTIONS; n=REC_N;
                build_ad(g_ad_rec, actions,n,module,cidx); ad=g_ad_rec; break;
        default: return;
    }
    for (int i = 0; i < n; i++) {
        GtkWidget *btn = make_action_btn(actions[i].label, i+1, cidx);
        g_signal_connect(btn, "clicked", G_CALLBACK(on_action_clicked), &ad[i]);
        gtk_box_pack_start(GTK_BOX(inner), btn, FALSE, FALSE, 0);
    }
    gtk_container_add(GTK_CONTAINER(scroll), inner);
    gtk_box_pack_start(GTK_BOX(g_sidebar), scroll, TRUE, TRUE, 0);

    gtk_widget_show_all(g_sidebar);
    output_set("");
    char smsg[128];
    snprintf(smsg, sizeof(smsg), "%s — select an action", title);
    status_set(smsg, col, "◈");
}

/* ── Window builder ── */
static void build_window(void)
{
    g_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(g_window), "ALSDD — Algeria History Database");
    gtk_window_set_default_size(GTK_WINDOW(g_window), WIN_W, WIN_H);
    gtk_window_set_resizable(GTK_WINDOW(g_window), TRUE);
    wcss(g_window, "window { background: " C_BG "; }");
    g_signal_connect(g_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(g_window), root);

    /* ── Top bar ── */
    GtkWidget *topbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    wcss(topbar,
        "box { background: " C_HEADER ";"
        "      border-bottom: 1px solid " C_BORDER ";"
        "      padding: 0 20px; min-height: 46px; }");

    /* Left: brand */
    GtkWidget *tb_brand = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *tb_icon  = gtk_label_new("⬡");
    wcss(tb_icon, "label { font-size: 18px; color: " C_ACCENT "; }");
    GtkWidget *tb_title = gtk_label_new("ALSDD");
    wcss(tb_title,
        "label { font-size: 14px; font-weight: bold; color: " C_TEXT ";"
        "        font-family: 'JetBrains Mono', monospace; }");
    GtkWidget *tb_sep2 = gtk_label_new("  /  ");
    wcss(tb_sep2, "label { font-size: 12px; color: " C_BORDER2 "; }");
    GtkWidget *tb_sub = gtk_label_new("Algeria History Database Manager");
    wcss(tb_sub, "label { font-size: 12px; color: " C_MUTED "; }");
    gtk_box_pack_start(GTK_BOX(tb_brand), tb_icon,  FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(tb_brand), tb_title, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(tb_brand), tb_sep2,  FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(tb_brand), tb_sub,   FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(topbar), tb_brand, FALSE, FALSE, 0);

    /* Right: traffic lights */
    GtkWidget *tlights = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_set_halign(tlights, GTK_ALIGN_END);
    gtk_widget_set_hexpand(tlights, TRUE);
    const char *dot_colors[] = { C_ERROR, C_WARN, C_SUCCESS };
    for (int i = 0; i < 3; i++) {
        GtkWidget *dot = gtk_label_new("●");
        char dc[128];
        snprintf(dc, sizeof(dc), "label { font-size: 12px; color: %s; opacity: 0.5; }", dot_colors[i]);
        wcss(dot, dc);
        gtk_box_pack_start(GTK_BOX(tlights), dot, FALSE, FALSE, 0);
    }
    gtk_box_pack_end(GTK_BOX(topbar), tlights, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(root), topbar, FALSE, FALSE, 0);

    /* ── Body ── */
    GtkWidget *body = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(root), body, TRUE, TRUE, 0);

    /* Sidebar */
    g_sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(g_sidebar, SIDEBAR_W, -1);
    gtk_container_set_border_width(GTK_CONTAINER(g_sidebar), 12);
    wcss(g_sidebar,
        "box { background: " C_SURF ";"
        "      border-right: 1px solid " C_BORDER "; }");
    gtk_box_pack_start(GTK_BOX(body), g_sidebar, FALSE, FALSE, 0);

    /* Output panel */
    GtkWidget *out_wrap = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(body), out_wrap, TRUE, TRUE, 0);

    /* Output sub-header */
    GtkWidget *out_hdr = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    wcss(out_hdr,
        "box { background: " C_SURF2 ";"
        "      border-bottom: 1px solid " C_BORDER ";"
        "      padding: 7px 18px; }");
    GtkWidget *out_lbl = gtk_label_new("OUTPUT");
    wcss(out_lbl,
        "label { font-size: 9px; color: " C_MUTED ";"
        "        font-family: 'JetBrains Mono', monospace;"
        "        letter-spacing: 2px; }");
    gtk_box_pack_start(GTK_BOX(out_hdr), out_lbl, FALSE, FALSE, 0);

    /* Clear button */
    GtkWidget *clr_btn = gtk_button_new_with_label("Clear");
    gtk_button_set_relief(GTK_BUTTON(clr_btn), GTK_RELIEF_NONE);
    wcss(clr_btn,
        "button { background: transparent; color: " C_MUTED ";"
        "         border: 1px solid " C_BORDER "; border-radius: 5px;"
        "         padding: 2px 10px; font-size: 10px;"
        "         font-family: 'JetBrains Mono', monospace; }"
        "button:hover { color: " C_TEXT2 "; border-color: " C_BORDER2 "; }");
    gtk_widget_set_halign(clr_btn, GTK_ALIGN_END);
    gtk_widget_set_hexpand(clr_btn, TRUE);
    g_signal_connect_swapped(clr_btn, "clicked",
        G_CALLBACK(output_set), (gpointer)"");
    gtk_box_pack_end(GTK_BOX(out_hdr), clr_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(out_wrap), out_hdr, FALSE, FALSE, 0);

    /* Text view */
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(out_wrap), scroll, TRUE, TRUE, 0);

    g_output = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(g_output), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(g_output), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(g_output), GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(g_output), 24);
    gtk_text_view_set_top_margin(GTK_TEXT_VIEW(g_output), 18);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(g_output), 24);
    gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(g_output), 18);
    wcss(g_output,
        "textview {"
        "  font-family: 'JetBrains Mono', 'Cascadia Code', 'Fira Code', Consolas, monospace;"
        "  font-size: 13px; line-height: 1.7;"
        "  background: " C_BG ";"
        "  color: " C_TEXT ";"
        "}"
        "textview text {"
        "  background: " C_BG ";"
        "  color: " C_TEXT ";"
        "}");
    gtk_container_add(GTK_CONTAINER(scroll), g_output);

    /* ── Status bar ── */
    GtkWidget *sbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    wcss(sbar,
        "box { background: " C_HEADER ";"
        "      border-top: 1px solid " C_BORDER ";"
        "      padding: 5px 18px; min-height: 28px; }");

    g_status_icon = gtk_label_new("●");
    wcss(g_status_icon,
        "label { font-size: 10px; color: " C_MUTED ";"
        "        font-family: monospace; }");
    gtk_box_pack_start(GTK_BOX(sbar), g_status_icon, FALSE, FALSE, 0);

    g_status = gtk_label_new("Ready");
    gtk_label_set_xalign(GTK_LABEL(g_status), 0.0f);
    wcss(g_status,
        "label { font-size: 11px; color: " C_MUTED ";"
        "        font-family: 'JetBrains Mono', monospace; }");
    gtk_box_pack_start(GTK_BOX(sbar), g_status, TRUE, TRUE, 0);

    /* Right side of status bar */
    GtkWidget *ver = gtk_label_new("Algeria · ALSDD  v1.0.0");
    wcss(ver,
        "label { font-size: 10px; color: " C_BORDER2 ";"
        "        font-family: 'JetBrains Mono', monospace; }");
    gtk_box_pack_end(GTK_BOX(sbar), ver, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(root), sbar, FALSE, FALSE, 0);
}

static void resolve_exe_dir(const char *argv0)
{
#ifdef _WIN32
    char path[4096] = {0};
    GetModuleFileNameA(NULL, path, sizeof(path)-1);
    char *sep = strrchr(path, '\\');
    if (sep) *sep = '\0';
    strncpy(g_exe_dir, path, sizeof(g_exe_dir)-1);
#else
    char resolved[4096] = {0};
    ssize_t len = readlink("/proc/self/exe", resolved, sizeof(resolved)-1);
    if (len > 0) {
        resolved[len] = '\0';
        char *dir = dirname(resolved);
        strncpy(g_exe_dir, dir, sizeof(g_exe_dir)-1);
        return;
    }
    if (argv0 && strchr(argv0, '/')) {
        strncpy(resolved, argv0, sizeof(resolved)-1);
        char *dir = dirname(resolved);
        strncpy(g_exe_dir, dir, sizeof(g_exe_dir)-1);
    } else {
        if (getcwd(g_exe_dir, sizeof(g_exe_dir)-1) == NULL)
            g_exe_dir[0] = '\0';
    }
#endif
}

int main(int argc, char *argv[])
{
    resolve_exe_dir(argv[0]);
    gtk_init(&argc, &argv);
    apply_global_css();
    build_window();
    show_main_menu();
    gtk_widget_show_all(g_window);
    gtk_main();
    return 0;
}
