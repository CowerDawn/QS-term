#include <gtk/gtk.h>
#include <vte/vte.h>
#include <string.h>
#include <stdlib.h>

#define THEME_LIGHT "light"
#define THEME_DARK "dark"
#define THEME_DARK2 "dark2"
#define THEME_PINK "pink"

static const char *current_theme = THEME_DARK;

static void set_theme(VteTerminal *terminal, const char *theme) {
    if (strcmp(theme, THEME_LIGHT) == 0) {
        vte_terminal_set_colors(terminal, NULL, NULL, NULL, 0);
    } else if (strcmp(theme, THEME_DARK) == 0) {
        GdkRGBA fg = {1.0, 1.0, 1.0, 1.0};
        GdkRGBA bg = {0.0, 0.0, 0.0, 1.0};
        vte_terminal_set_colors(terminal, &fg, &bg, NULL, 0);
    } else if (strcmp(theme, THEME_DARK2) == 0) {
        GdkRGBA fg = {1.0, 1.0, 1.0, 1.0};
        GdkRGBA bg = {0.15, 0.15, 0.15, 1.0};
        vte_terminal_set_colors(terminal, &fg, &bg, NULL, 0);
    } else if (strcmp(theme, THEME_PINK) == 0) {
        GdkRGBA fg = {1.0, 0.5, 0.75, 1.0};
        GdkRGBA bg = {1.0, 0.85, 0.9, 1.0};
        vte_terminal_set_colors(terminal, &fg, &bg, NULL, 0);
    }
}

static void on_copy_clipboard(GtkWidget *widget, gpointer data) {
    VteTerminal *terminal = VTE_TERMINAL(data);
    vte_terminal_copy_clipboard_format(terminal, VTE_FORMAT_TEXT);
}

static void on_paste_clipboard(GtkWidget *widget, gpointer data) {
    VteTerminal *terminal = VTE_TERMINAL(data);
    vte_terminal_paste_clipboard(terminal);
}

static gboolean on_key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer data);

static void on_app_activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *terminal;
    GtkWidget *menu;
    GtkWidget *copy_item;
    GtkWidget *paste_item;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "QS-term");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

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

    gtk_container_add(GTK_CONTAINER(window), terminal);

    menu = gtk_menu_new();
    copy_item = gtk_menu_item_new_with_label("Copy");
    paste_item = gtk_menu_item_new_with_label("Paste");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), copy_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), paste_item);
    gtk_widget_show_all(menu);

    g_signal_connect(copy_item, "activate", G_CALLBACK(on_copy_clipboard), terminal);
    g_signal_connect(paste_item, "activate", G_CALLBACK(on_paste_clipboard), terminal);

    g_signal_connect(terminal, "button-press-event", G_CALLBACK(gtk_menu_popup_at_widget), menu);

    g_signal_connect(terminal, "key-press-event", G_CALLBACK(on_key_press_event), terminal);

    gtk_widget_show_all(window);
}

static gboolean on_key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer data) {
    VteTerminal *terminal = VTE_TERMINAL(data);

    if (event->state & GDK_CONTROL_MASK && event->keyval == GDK_KEY_t) {
        if (strcmp(current_theme, THEME_LIGHT) == 0) {
            current_theme = THEME_DARK;
        } else if (strcmp(current_theme, THEME_DARK) == 0) {
            current_theme = THEME_DARK2;
        } else if (strcmp(current_theme, THEME_DARK2) == 0) {
            current_theme = THEME_PINK;
        } else if (strcmp(current_theme, THEME_PINK) == 0) {
            current_theme = THEME_LIGHT;
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
