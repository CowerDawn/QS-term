#include <gtk/gtk.h>
#include <vte/vte.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <glib.h>
#include <glib/gstdio.h>

#define THEME_GRAY "gray"
#define THEME_DARK "dark"
#define THEME_DARK2 "dark2"
#define THEME_PINK "pink"
#define THEME_HOME "home"
#define THEME_LIGHT "light"

#define CONFIG_DIR g_get_user_config_dir()
#define CONFIG_FILE g_build_filename(CONFIG_DIR, "qsterm", "qsset.conf", NULL)

static const char *current_theme = THEME_DARK;
static char *current_font = "Courier 10";
static char *current_icon = "icon.png";
static char *current_title = "QS-term";

static void set_theme(VteTerminal *terminal, const char *theme) {
    if (strcmp(theme, THEME_GRAY) == 0) {
        GdkRGBA fg = {0.0, 0.0, 0.0, 1.0};
        GdkRGBA bg = {0.8, 0.8, 0.8, 1.0};
        vte_terminal_set_colors(terminal, &fg, &bg, NULL, 0);
    } else if (strcmp(theme, THEME_DARK) == 0) {
        GdkRGBA fg = {1.0, 1.0, 1.0, 1.0};
        GdkRGBA bg = {0.0, 0.0, 0.0, 1.0};
        vte_terminal_set_colors(terminal, &fg, &bg, NULL, 0);
    } else if (strcmp(theme, THEME_DARK2) == 0) {
        GdkRGBA fg = {1.0, 1.0, 1.0, 1.0};
        GdkRGBA bg = {0.15, 0.15, 0.15, 1.0};
        vte_terminal_set_colors(terminal, &fg, &bg, NULL, 0);
    } else if (strcmp(theme, THEME_PINK) == 0) {
        GdkRGBA fg = {0.0, 0.0, 0.0, 1.0};
        GdkRGBA bg = {0.6, 0.45, 0.5, 1.0};
        vte_terminal_set_colors(terminal, &fg, &bg, NULL, 0);
    } else if (strcmp(theme, THEME_HOME) == 0) {
        GdkRGBA fg = {0.8, 0.8, 0.8, 1.0};
        GdkRGBA bg = {0.2, 0.2, 0.2, 1.0};
        vte_terminal_set_colors(terminal, &fg, &bg, NULL, 0);
    } else if (strcmp(theme, THEME_LIGHT) == 0) {
        GdkRGBA fg = {0.0, 0.0, 0.0, 1.0};
        GdkRGBA bg = {1.0, 1.0, 1.0, 1.0};
        vte_terminal_set_colors(terminal, &fg, &bg, NULL, 0);
    }
}

static void create_config() {
    g_mkdir_with_parents(g_build_filename(CONFIG_DIR, "qsterm", NULL), 0755);

    GKeyFile *keyfile = g_key_file_new();
    g_key_file_set_string(keyfile, "Settings", "font", "Courier 10");
    g_key_file_set_string(keyfile, "Settings", "color", "dark");
    g_key_file_set_string(keyfile, "Settings", "icon", "icon.png");
    g_key_file_set_string(keyfile, "Settings", "title", "QS-term");
    g_key_file_set_string(keyfile, "Settings", "default_terminal", "no");
    g_key_file_set_string(keyfile, "Settings", "shortcut_ctrl_s", "yes");

    gsize length;
    gchar *data = g_key_file_to_data(keyfile, &length, NULL);
    g_file_set_contents(CONFIG_FILE, data, length, NULL);

    g_key_file_free(keyfile);
    g_free(data);
}

static void load_config() {
    GKeyFile *keyfile = g_key_file_new();
    if (!g_key_file_load_from_file(keyfile, CONFIG_FILE, G_KEY_FILE_NONE, NULL)) {
        create_config();
    } else {
        current_font = g_key_file_get_string(keyfile, "Settings", "font", NULL);
        current_theme = g_key_file_get_string(keyfile, "Settings", "color", NULL);
        current_icon = g_key_file_get_string(keyfile, "Settings", "icon", NULL);
        current_title = g_key_file_get_string(keyfile, "Settings", "title", NULL);
    }
    g_key_file_free(keyfile);
}

static void on_copy_clipboard(GtkWidget *widget, gpointer data) {
    VteTerminal *terminal = VTE_TERMINAL(data);
    vte_terminal_copy_clipboard_format(terminal, VTE_FORMAT_TEXT);
}

static void on_paste_clipboard(GtkWidget *widget, gpointer data) {
    VteTerminal *terminal = VTE_TERMINAL(data);
    vte_terminal_paste_clipboard(terminal);
}

static void on_set_title(GtkWidget *widget, gpointer data) {
    GtkWidget *window = GTK_WIDGET(data);
    const gchar *title = gtk_menu_item_get_label(GTK_MENU_ITEM(widget));
    gtk_window_set_title(GTK_WINDOW(window), title);
}

static gboolean on_key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer data);

static void on_app_activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *terminal;
    GtkWidget *menu;
    GtkWidget *copy_item;
    GtkWidget *paste_item;
    GtkWidget *title_item;

    load_config();

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), current_title);
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    GdkPixbuf *icon = gdk_pixbuf_new_from_file(current_icon, NULL);
    if (icon) {
        gtk_window_set_icon(GTK_WINDOW(window), icon);
        g_object_unref(icon);
    }

    terminal = vte_terminal_new();
    vte_terminal_spawn_async(VTE_TERMINAL(terminal),
                             VTE_PTY_DEFAULT,
                             NULL, 
                             (char *[]){"/bin/bash", NULL}, 
                             NULL, 
                             0, 
                             NULL, NULL, 
                             NULL, 
                             -1, 
                             NULL,
                             NULL, 
                             NULL); 

    set_theme(VTE_TERMINAL(terminal), current_theme);
    vte_terminal_set_font(VTE_TERMINAL(terminal), pango_font_description_from_string(current_font));

    gtk_container_add(GTK_CONTAINER(window), terminal);

    menu = gtk_menu_new();
    copy_item = gtk_menu_item_new_with_label("Copy");
    paste_item = gtk_menu_item_new_with_label("Paste");
    title_item = gtk_menu_item_new_with_label("Set Title");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), copy_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), paste_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), title_item);
    gtk_widget_show_all(menu);

    g_signal_connect(copy_item, "activate", G_CALLBACK(on_copy_clipboard), terminal);
    g_signal_connect(paste_item, "activate", G_CALLBACK(on_paste_clipboard), terminal);
    g_signal_connect(title_item, "activate", G_CALLBACK(on_set_title), window);

    g_signal_connect(terminal, "button-press-event", G_CALLBACK(gtk_menu_popup_at_widget), menu);

    g_signal_connect(terminal, "key-press-event", G_CALLBACK(on_key_press_event), terminal);

    gtk_widget_show_all(window);
}

static gboolean on_key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer data) {
    VteTerminal *terminal = VTE_TERMINAL(data);

    if (event->state & GDK_CONTROL_MASK && event->keyval == GDK_KEY_t) {
        if (strcmp(current_theme, THEME_GRAY) == 0) {
            current_theme = THEME_DARK;
        } else if (strcmp(current_theme, THEME_DARK) == 0) {
            current_theme = THEME_DARK2;
        } else if (strcmp(current_theme, THEME_DARK2) == 0) {
            current_theme = THEME_PINK;
        } else if (strcmp(current_theme, THEME_PINK) == 0) {
            current_theme = THEME_HOME;
        } else if (strcmp(current_theme, THEME_HOME) == 0) {
            current_theme = THEME_LIGHT;
        } else if (strcmp(current_theme, THEME_LIGHT) == 0) {
            current_theme = THEME_GRAY;
        }
        set_theme(terminal, current_theme);
        return TRUE;
    } else if (event->state & GDK_CONTROL_MASK && event->keyval == GDK_KEY_c) {
        vte_terminal_copy_clipboard_format(terminal, VTE_FORMAT_TEXT);
        return TRUE;
    } else if ((event->state & GDK_CONTROL_MASK && event->keyval == GDK_KEY_v) ||
               (event->state & GDK_SHIFT_MASK && event->keyval == GDK_KEY_Insert)) {
        vte_terminal_paste_clipboard(terminal);
        return TRUE;
    } else if (event->state & GDK_CONTROL_MASK && event->keyval == GDK_KEY_q) {
        vte_terminal_reset(terminal, TRUE, TRUE);
        return TRUE;
    } else if (event->state & GDK_SHIFT_MASK && event->keyval == GDK_KEY_Home) {
        vte_terminal_copy_clipboard_format(terminal, VTE_FORMAT_TEXT);
        return TRUE;
    } else if (event->state & GDK_CONTROL_MASK && event->keyval == GDK_KEY_s) {
        system("python3 ~/.QS/qsset.py &");
        return TRUE;
    }

    return FALSE;
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.gtk.terminal", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_app_activate), NULL);

    status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);

    return status;
}
