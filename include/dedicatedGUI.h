#ifndef __dedicatedGUI_H__
#define __dedicatedGUI_H__

#include <playertools.h>
#include <gtk/gtk.h>

typedef struct {
    server_t **server_ptr;
    gpointer data;
} server_ctx_t;

typedef struct {
    GtkWidget *server_name_widget;
    GtkWidget *server_playercnt_widget;
    GtkWidget *server_pass_enabled_widget;
    GtkWidget *server_password_widget;
} create_server_ctx_t;

static char *ship_css_map[55] = {
    "* { background-image:none; background-color:lime; }",
    "* { background-image:none; background-color:red; }",
    "* { background-image:none; background-color:blue; }",
    "* { background-image:none; background-color:olive; }",
    "* { background-image:none; background-color:fuchsia; }"
};

// void activate (GtkApplication* app, gpointer user_data);
void create_label_box_pair(GtkWidget *box, GtkWidget *label, GtkWidget *entry, const char *label_text);
int gtk_app_init(int argc, char **argv);
#endif