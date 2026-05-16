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

#define SIDEBAR_W  230
#define WIN_W      860
#define WIN_H      540


#define C_BG      "#ffffff"
#define C_SURF    "#f8f8f8"
#define C_BORDER  "#e8e8e8"
#define C_TEXT    "#1a1a1a"
#define C_MUTED   "#909090"
#define C_HOVER   "#f0f0f0"
#define C_ACTIVE  "#e8e8e8"

static char g_exe_dir[4096] = {0};


static void wcss(GtkWidget *w, const char *css)
{
    GtkCssProvider *p = gtk_css_provider_new();
    gtk_css_provider_load_from_data(p, css, -1, NULL);
    gtk_style_context_add_provider(gtk_widget_get_style_context(w),
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

static void output_set(const char *text)
{
    GtkTextBuffer *buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(g_output));
    gtk_text_buffer_set_text(buf, text ? text : "", -1);
    GtkTextIter it;
    gtk_text_buffer_get_start_iter(buf, &it);
    gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(g_output), &it, 0.0, FALSE, 0, 0);
}

static void status_set(const char *msg)
{
    gtk_label_set_text(GTK_LABEL(g_status), msg);
}



static void do_run(const char *stdin_str)
{
    
    if (g_exe_dir[0])
        chdir(g_exe_dir);

    GString *result = g_string_new(NULL);

#ifdef _WIN32
    FILE *fp = _popen(EXE_NAME, "r+");
    if (!fp) {
        output_set("Error: could not launch " EXE_NAME ".\n"
                   "Make sure alsdd.exe is in the same folder as gui.exe.");
        g_string_free(result, TRUE);
        status_set("Launch error.");
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
        output_set("Error: could not create temp file.");
        g_string_free(result, TRUE);
        status_set("Error.");
        return;
    }
    write(fd, stdin_str, strlen(stdin_str));
    close(fd);

    char cmd[1024];
    snprintf(cmd, sizeof(cmd), EXE_NAME " < %s 2>&1", tmppath);
    FILE *fp = popen(cmd, "r");
    if (!fp) {
        unlink(tmppath);
        output_set("Error: could not launch " EXE_NAME ".\n"
                   "Make sure the 'alsdd' binary is in the same folder as 'gui'\n"
                   "and has execute permission  (chmod +x alsdd).");
        g_string_free(result, TRUE);
        status_set("Launch error.");
        return;
    }
    char buf[512];
    while (fgets(buf, sizeof(buf), fp))
        g_string_append(result, buf);
    pclose(fp);
    unlink(tmppath);
#endif

    output_set(result->str);
    g_string_free(result, TRUE);
    status_set("Done.");
}


static gboolean input_dialog(const char *title,
                              const char **labels, int n, char **answers)
{
    GtkWidget *dlg = gtk_dialog_new_with_buttons(
        title, GTK_WINDOW(g_window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Run",    GTK_RESPONSE_OK, NULL);

    gtk_dialog_set_default_response(GTK_DIALOG(dlg), GTK_RESPONSE_OK);
    gtk_window_set_default_size(GTK_WINDOW(dlg), 360, -1);

    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dlg));
    gtk_container_set_border_width(GTK_CONTAINER(content), 18);
    gtk_box_set_spacing(GTK_BOX(content), 5);

    GtkWidget *entries[6] = {0};
    for (int i = 0; i < n; i++) {
        GtkWidget *lbl = gtk_label_new(labels[i]);
        gtk_widget_set_halign(lbl, GTK_ALIGN_START);
        wcss(lbl, "label { font-size: 12px; color: " C_MUTED "; margin-top: 4px; }");
        gtk_box_pack_start(GTK_BOX(content), lbl, FALSE, FALSE, 0);

        entries[i] = gtk_entry_new();
        gtk_entry_set_activates_default(GTK_ENTRY(entries[i]), TRUE);
        wcss(entries[i],
             "entry { font-size: 13px;"
             "        border: 1px solid " C_BORDER ";"
             "        border-radius: 5px;"
             "        padding: 7px 9px;"
             "        background: " C_BG "; color: " C_TEXT "; }");
        gtk_box_pack_start(GTK_BOX(content), entries[i], FALSE, FALSE, 0);
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

    status_set("Running...");
    do_run(s->str);
    g_string_free(s, TRUE);
}

static GtkWidget *make_btn(const char *text, const char *extra_css)
{
    GtkWidget *btn = gtk_button_new_with_label(text);
    gtk_widget_set_hexpand(btn, TRUE);
    gtk_button_set_relief(GTK_BUTTON(btn), GTK_RELIEF_NONE);
    GtkWidget *lbl = gtk_bin_get_child(GTK_BIN(btn));
    if (GTK_IS_LABEL(lbl)) gtk_label_set_xalign(GTK_LABEL(lbl), 0.0f);
    
    char full[512];
    snprintf(full, sizeof(full),
        "button { background: transparent; color: " C_TEXT ";"
        "         border: none; border-radius: 5px;"
        "         padding: 8px 10px; font-size: 12px; }"
        "button:hover { background: " C_HOVER "; }"
        "button:active { background: " C_ACTIVE "; }"
        "%s", extra_css ? extra_css : "");
    wcss(btn, full);
    return btn;
}

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
    wcss(s, "separator { min-height: 1px; background: " C_BORDER "; margin: 6px 0; }");
    return s;
}

typedef struct { int module; const char *title; } SubMenuData;
static SubMenuData g_sub_data[4];

static void on_module_clicked(GtkButton *b, gpointer d)
{
    (void)b;
    SubMenuData *sd = (SubMenuData *)d;
    show_sub_menu(sd->module, sd->title);
}

static void show_main_menu(void)
{
    GList *ch = gtk_container_get_children(GTK_CONTAINER(g_sidebar));
    for (GList *l = ch; l; l = l->next) gtk_widget_destroy(GTK_WIDGET(l->data));
    g_list_free(ch);

    gtk_box_pack_start(GTK_BOX(g_sidebar),
        make_lbl("MODULES",
            "label { font-size: 10px; color: " C_MUTED ";"
            "        padding: 2px 2px 6px; letter-spacing: 1px; }"),
        FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(g_sidebar), make_sep(), FALSE, FALSE, 0);

    static const struct { int n; const char *t; } M[] = {
        {1,"Linked Lists & Queues"}, {2,"Stacks"},
        {3,"Binary Search Tree"},   {4,"Recursion"},
    };
    for (int i = 0; i < 4; i++) {
        char buf[64]; snprintf(buf, sizeof(buf), "%d.  %s", M[i].n, M[i].t);
        GtkWidget *btn = make_btn(buf,
            "button { font-size: 13px; padding: 10px 10px; }");
        g_sub_data[i].module = M[i].n;
        g_sub_data[i].title  = M[i].t;
        g_signal_connect(btn, "clicked", G_CALLBACK(on_module_clicked), &g_sub_data[i]);
        gtk_box_pack_start(GTK_BOX(g_sidebar), btn, FALSE, FALSE, 1);
    }

    gtk_box_pack_start(GTK_BOX(g_sidebar),
        gtk_box_new(GTK_ORIENTATION_VERTICAL, 0), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(g_sidebar), make_sep(), FALSE, FALSE, 0);

    GtkWidget *ex = make_btn("Exit",
        "button { color: " C_MUTED "; }"
        "button:hover { color: #c0392b; background: #fff5f5; }");
    g_signal_connect(ex, "clicked", G_CALLBACK(gtk_main_quit), NULL);
    gtk_box_pack_start(GTK_BOX(g_sidebar), ex, FALSE, FALSE, 0);

    gtk_widget_show_all(g_sidebar);
    output_set("");
    status_set("Select a module.");
}


typedef struct { int module; int sub; const char **fields; int nf; } ActionData;
static ActionData g_ad_ll[LL_N], g_ad_stk[STK_N], g_ad_bst[BST_N], g_ad_rec[REC_N];

static int nfields(const char * const *f) { int n=0; while(f[n]) n++; return n; }

static void build_ad(ActionData *arr, const Action *a, int n, int mod) {
    for (int i=0;i<n;i++){
        arr[i].module=mod; arr[i].sub=i+1;
        arr[i].fields=(const char**)a[i].fields;
        arr[i].nf=nfields(a[i].fields);
    }
}

static void on_action_clicked(GtkButton *b, gpointer d)
{
    (void)b;
    ActionData *ad = (ActionData *)d;
    run_action(ad->module, ad->sub, ad->fields, ad->nf);
}

static void on_back_clicked(GtkButton *b, gpointer d) { (void)b;(void)d; show_main_menu(); }

static void show_sub_menu(int module, const char *title)
{
    GList *ch = gtk_container_get_children(GTK_CONTAINER(g_sidebar));
    for (GList *l=ch; l; l=l->next) gtk_widget_destroy(GTK_WIDGET(l->data));
    g_list_free(ch);

    GtkWidget *back = make_btn("← Back",
        "button { font-size: 12px; color: " C_MUTED "; }"
        "button:hover { color: " C_TEXT "; }");
    g_signal_connect(back, "clicked", G_CALLBACK(on_back_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(g_sidebar), back, FALSE, FALSE, 0);


    gtk_box_pack_start(GTK_BOX(g_sidebar),
        make_lbl(title,
            "label { font-size: 13px; font-weight: bold; color: " C_TEXT ";"
            "        padding: 4px 2px; }"),
        FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(g_sidebar), make_sep(), FALSE, FALSE, 0);

    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
        GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);

    GtkWidget *inner = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    const Action *actions = NULL; ActionData *ad = NULL; int n = 0;
    switch (module) {
        case 1: actions=LL_ACTIONS;  n=LL_N;  build_ad(g_ad_ll,  actions,n,module); ad=g_ad_ll;  break;
        case 2: actions=STK_ACTIONS; n=STK_N; build_ad(g_ad_stk, actions,n,module); ad=g_ad_stk; break;
        case 3: actions=BST_ACTIONS; n=BST_N; build_ad(g_ad_bst, actions,n,module); ad=g_ad_bst; break;
        case 4: actions=REC_ACTIONS; n=REC_N; build_ad(g_ad_rec, actions,n,module); ad=g_ad_rec; break;
        default: return;
    }

    for (int i = 0; i < n; i++) {
        char buf[128]; snprintf(buf, sizeof(buf), "%2d.  %s", i+1, actions[i].label);
        GtkWidget *btn = make_btn(buf, NULL);
        g_signal_connect(btn, "clicked", G_CALLBACK(on_action_clicked), &ad[i]);
        gtk_box_pack_start(GTK_BOX(inner), btn, FALSE, FALSE, 0);
    }

    gtk_container_add(GTK_CONTAINER(scroll), inner);
    gtk_box_pack_start(GTK_BOX(g_sidebar), scroll, TRUE, TRUE, 0);

    gtk_widget_show_all(g_sidebar);
    output_set("");
    status_set("Select an action.");
}

static void build_window(void)
{
    g_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(g_window), "ALSDD — Algeria Database");
    gtk_window_set_default_size(GTK_WINDOW(g_window), WIN_W, WIN_H);
    gtk_window_set_resizable(GTK_WINDOW(g_window), TRUE);
    wcss(g_window, "window { background: " C_BG "; }");
    g_signal_connect(g_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(g_window), root);


    GtkWidget *topbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    wcss(topbar,
        "box { background: " C_BG ";"
        "      border-bottom: 1px solid " C_BORDER ";"
        "      padding: 9px 14px; }");
    GtkWidget *title_lbl = gtk_label_new("Algeria Database");
    wcss(title_lbl, "label { font-size: 13px; font-weight: bold; color: " C_TEXT "; }");
    gtk_box_pack_start(GTK_BOX(topbar), title_lbl, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(root), topbar, FALSE, FALSE, 0);


    GtkWidget *body = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(root), body, TRUE, TRUE, 0);

    g_sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(g_sidebar, SIDEBAR_W, -1);
    gtk_container_set_border_width(GTK_CONTAINER(g_sidebar), 8);
    wcss(g_sidebar,
        "box { background: " C_SURF ";"
        "      border-right: 1px solid " C_BORDER "; }");
    gtk_box_pack_start(GTK_BOX(body), g_sidebar, FALSE, FALSE, 0);

    GtkWidget *out_wrap = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(body), out_wrap, TRUE, TRUE, 0);

    GtkWidget *out_hdr = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    wcss(out_hdr,
        "box { background: " C_BG ";"
        "      border-bottom: 1px solid " C_BORDER ";"
        "      padding: 5px 12px; }");
    GtkWidget *out_lbl = gtk_label_new("Output");
    wcss(out_lbl, "label { font-size: 11px; color: " C_MUTED "; }");
    gtk_box_pack_start(GTK_BOX(out_hdr), out_lbl, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(out_wrap), out_hdr, FALSE, FALSE, 0);

    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(out_wrap), scroll, TRUE, TRUE, 0);

    g_output = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(g_output), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(g_output), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(g_output), GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(g_output), 14);
    gtk_text_view_set_top_margin(GTK_TEXT_VIEW(g_output), 10);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(g_output), 14);
    wcss(g_output,
        "textview {"
        "  font-family: 'Cascadia Code', Consolas, 'Courier New', monospace;"
        "  font-size: 12px;"
        "  background: " C_BG ";"
        "  color: " C_TEXT ";"
        "}");
    gtk_container_add(GTK_CONTAINER(scroll), g_output);

    GtkWidget *sbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    wcss(sbar,
        "box { background: " C_SURF ";"
        "      border-top: 1px solid " C_BORDER ";"
        "      padding: 3px 14px; }");
    g_status = gtk_label_new("Ready");
    gtk_label_set_xalign(GTK_LABEL(g_status), 0.0f);
    wcss(g_status, "label { font-size: 11px; color: " C_MUTED "; }");
    gtk_box_pack_start(GTK_BOX(sbar), g_status, TRUE, TRUE, 0);
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
    build_window();
    show_main_menu();
    gtk_widget_show_all(g_window);
    gtk_main();
    return 0;
}
