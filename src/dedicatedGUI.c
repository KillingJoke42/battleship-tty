#include <dedicatedGUI.h>
#include <playertools.h>
#include <gtk/gtk.h>

static GtkApplication *app;

static void hide_entry(GtkWidget* widget, gpointer data)
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)) == TRUE)
    gtk_widget_set_sensitive(GTK_WIDGET(data), TRUE);
  else
    gtk_widget_set_sensitive(GTK_WIDGET(data), FALSE);
}

static void back_to_main_action(GtkWidget* widget, gpointer data)
{
  gtk_window_close(GTK_WINDOW(gtk_application_get_active_window(app)));
  gtk_widget_show(GTK_WIDGET(data));
}

void destroy (GtkWidget* widget, gpointer data)
{
  g_application_quit(G_APPLICATION(data));
}

static void start_couch_multiplayer(GtkWidget* widget, gpointer data)
{
  gtk_widget_hide(GTK_WIDGET(data));
  
  GtkWidget *create_server_window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(create_server_window), "Create Server");
  gtk_window_set_default_size(GTK_WINDOW(create_server_window), 352, 240);
  gtk_window_set_position(GTK_WINDOW(create_server_window), GTK_WIN_POS_CENTER_ALWAYS);
  g_signal_connect(G_OBJECT(create_server_window), "destroy", G_CALLBACK(destroy), app);

  // GtkWidget *create_server_grid = gtk_grid_new();

  GtkWidget *server_data_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
  gtk_container_add(GTK_CONTAINER(create_server_window), server_data_vbox);
  gtk_widget_set_valign(server_data_vbox, GTK_ALIGN_START);
  gtk_widget_set_halign(server_data_vbox, GTK_ALIGN_START);
  gtk_widget_set_vexpand(server_data_vbox, FALSE);
  gtk_widget_set_hexpand(server_data_vbox, FALSE);

  GtkWidget *create_server_label = gtk_label_new("Create Server");
  gtk_box_pack_start(GTK_BOX(server_data_vbox), create_server_label, FALSE, FALSE, 0);

  GtkWidget *server_data_grid = gtk_grid_new();
  gtk_grid_set_column_spacing(GTK_GRID(server_data_grid), 50);
  gtk_grid_set_row_spacing(GTK_GRID(server_data_grid), 20);
  gtk_box_pack_start(GTK_BOX(server_data_vbox), server_data_grid, FALSE, FALSE, 0);

  GtkWidget *server_name_label = gtk_label_new("Server Name");
  gtk_grid_attach(GTK_GRID(server_data_grid), server_name_label, 0, 0, 1, 1);
  gtk_widget_set_halign(server_name_label, GTK_ALIGN_START);
  GtkWidget *server_name_entry = gtk_entry_new();
  gtk_grid_attach(GTK_GRID(server_data_grid), server_name_entry, 1, 0, 1, 1);
  gtk_widget_set_halign(server_name_entry, GTK_ALIGN_END);

  GtkWidget *player_count_label = gtk_label_new("Number of players");
  gtk_grid_attach(GTK_GRID(server_data_grid), player_count_label, 0, 1, 1, 1);
  gtk_widget_set_halign(player_count_label, GTK_ALIGN_START);
  GtkWidget *player_count_entry = gtk_entry_new();
  gtk_grid_attach(GTK_GRID(server_data_grid), player_count_entry, 1, 1, 1, 1);
  gtk_widget_set_halign(player_count_entry, GTK_ALIGN_END);

  GtkWidget *protect_server_checkbutton = gtk_check_button_new_with_label("Protect Server");
  gtk_grid_attach(GTK_GRID(server_data_grid), protect_server_checkbutton, 0, 2, 1, 1);

  GtkWidget *password_label = gtk_label_new("Server Password");
  gtk_grid_attach(GTK_GRID(server_data_grid), password_label, 0, 3, 1, 1);
  gtk_widget_set_halign(password_label, GTK_ALIGN_START);
  GtkWidget *password_entry = gtk_entry_new();
  gtk_grid_attach(GTK_GRID(server_data_grid), password_entry, 1, 3, 1, 1);
  gtk_widget_set_halign(password_entry, GTK_ALIGN_END);
  gtk_widget_set_sensitive(password_entry, FALSE);

  g_signal_connect(G_OBJECT(protect_server_checkbutton), "clicked", G_CALLBACK(hide_entry), password_entry);

  GtkWidget *confirm_button = gtk_button_new_with_label("Start Server!");
  gtk_grid_attach(GTK_GRID(server_data_grid), confirm_button, 0, 4, 1, 1);
  GtkWidget *back_to_main_button = gtk_button_new_with_label("Back to main menu");
  gtk_grid_attach(GTK_GRID(server_data_grid), back_to_main_button, 1, 4, 1, 1);
  
  g_signal_connect(G_OBJECT(back_to_main_button), "clicked", G_CALLBACK(back_to_main_action), data);

  gtk_widget_show_all(create_server_window);

  // gtk_widget_show(GTK_WIDGET(data));
}

static void activate (GtkApplication* app, gpointer user_data)
{
  GtkWidget *window;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Battleship");
  gtk_window_set_default_size (GTK_WINDOW (window), 640, 480);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER_ALWAYS);
  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(destroy), app);

  GtkWidget* main_menu = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
  gtk_container_add(GTK_CONTAINER(window), main_menu);
  gtk_widget_set_valign(main_menu, GTK_ALIGN_CENTER);
  gtk_widget_set_halign(main_menu, GTK_ALIGN_CENTER);
  gtk_widget_set_vexpand(main_menu, FALSE);
  gtk_widget_set_hexpand(main_menu, FALSE);
  // gtk_widget_set_size_request(main_menu, 1, 1);

  GtkWidget* title = gtk_label_new("BATTLESHIP");
  // gtk_container_add(GTK_CONTAINER(main_menu), title);
  gtk_box_pack_start(GTK_BOX(main_menu), title, FALSE, FALSE, 0);

  GtkWidget* couch_button = gtk_button_new_with_label("Start Couch Multiplayer Game");
  // gtk_container_add(GTK_CONTAINER(main_menu), couch_button);
  gtk_box_pack_start(GTK_BOX(main_menu), couch_button, FALSE, FALSE, 0);
  gtk_widget_set_size_request(couch_button, 200, 30);
  g_signal_connect(G_OBJECT(couch_button), "clicked", G_CALLBACK(start_couch_multiplayer), window);

  GtkWidget* lan_button = gtk_button_new_with_label("Start Local Game Server");
  // gtk_container_add(GTK_CONTAINER(main_menu), lan_button);
  gtk_box_pack_start(GTK_BOX(main_menu), lan_button, FALSE, FALSE, 0);
  gtk_widget_set_size_request(lan_button, 200, 30);

  GtkWidget* exit_button = gtk_button_new_with_label("Exit");
  // gtk_container_add(GTK_CONTAINER(main_menu), exit_button);
  gtk_box_pack_start(GTK_BOX(main_menu), exit_button, FALSE, FALSE, 0);
  gtk_widget_set_size_request(exit_button, 200, 30);
  g_signal_connect(G_OBJECT(exit_button), "clicked", G_CALLBACK(destroy), app);

  gtk_widget_show_all (window);
}

int gtk_app_init(int argc, char **argv)
{
    int status;

    app = gtk_application_new ("com.github.KillingJoke42.battleship-tty", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}