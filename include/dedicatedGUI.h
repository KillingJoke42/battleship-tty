#ifndef __dedicatedGUI_H__
#define __dedicatedGUI_H__

#include <gtk/gtk.h>

typedef struct {
    gpointer data1;
    gpointer data2;
} gpointer_double_t;

// void activate (GtkApplication* app, gpointer user_data);
void create_label_box_pair(GtkWidget *box, GtkWidget *label, GtkWidget *entry, const char *label_text);
int gtk_app_init(int argc, char **argv);
#endif