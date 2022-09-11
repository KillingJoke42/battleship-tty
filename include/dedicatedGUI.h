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

static char *default_css = "<span>  </span>";

static char *ship_css_map[55] = {
    "<span background='#00ff00'>  </span>",
    "<span background='#ff0000'>  </span>",
    "<span background='#0000ff'>  </span>",
    "<span background='#808000'>  </span>",
    "<span background='#ff00ff'>  </span>"
};

// void activate (GtkApplication* app, gpointer user_data);
char* evaluate_lbl_txt(void);
void create_label_box_pair(GtkWidget *box, GtkWidget *label, GtkWidget *entry, const char *label_text);
int gtk_app_init(int argc, char **argv);
#endif