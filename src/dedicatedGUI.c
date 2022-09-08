#include <dedicatedGUI.h>
#include <battleship.h>
#include <playertools.h>
#include <utils.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>

static GtkApplication *app;
GtkWidget *create_server_window = NULL;

static server_t *server;
char *name;
GtkWidget **playerNameDialog, **prepPhaseButtonGrid;

GtkWidget *activeButton;
GtkCssProvider *css = NULL;

gint orientation = 0;

void destroy (GtkWidget* widget, gpointer data)
{
  printf("Exiting...\n");
  g_application_quit(G_APPLICATION(app));
}

static void gtk_entry_blank_error_dialog(GtkWindow* parent, const char* message)
{
  GtkWidget *dialog, *label, *content_area;
  GtkDialogFlags flags;

  flags = GTK_DIALOG_DESTROY_WITH_PARENT;
  dialog = gtk_dialog_new_with_buttons ("ERROR",
                                        parent,
                                        flags,
                                        ("OK"),
                                        GTK_RESPONSE_NONE,
                                        NULL);
  content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
  label = gtk_label_new (message);

  g_signal_connect_swapped (dialog,
                            "response",
                            G_CALLBACK (gtk_widget_destroy),
                            dialog);

  gtk_container_add (GTK_CONTAINER (content_area), label);
  gtk_widget_show_all (dialog);
}

static void place_ship_on_grid(GtkWidget* widget, gpointer data)
{
  ship_loc_t *ship_origin = (ship_loc_t *)data;
  int ship_origin_row = ship_origin->origin_row;
  int ship_origin_col = ship_origin->origin_col;
  GtkStyleContext *context[SHIP_CARRIER_SZ];
  for (int i = 0; i < SHIP_CARRIER_SZ; i++)
    context[i] = gtk_widget_get_style_context(prepPhaseButtonGrid[(ship_origin_row*NUM_ROWS)+ship_origin_col+i]);
  if (css == NULL)
  {
    css = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css, "* { background-image:none; background-color:green; }", -1, NULL);
    for (int i = 0; i < SHIP_CARRIER_SZ; i++)
      gtk_style_context_add_provider(context[i], GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_USER);

    g_object_unref(css);
  }
  else
  {
    for (int i = 0; i < SHIP_CARRIER_SZ; i++)
      gtk_style_context_remove_provider(context[i], GTK_STYLE_PROVIDER(css));
    css = NULL;
  }
}

static void preparation_phase(void)
{
  GtkWidget *prep_phase_window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(prep_phase_window), "Preparation Phase");
  gtk_window_set_default_size(GTK_WINDOW(prep_phase_window), 640, 480);
  gtk_window_set_position(GTK_WINDOW(prep_phase_window), GTK_WIN_POS_CENTER_ALWAYS);
  g_signal_connect(G_OBJECT(prep_phase_window), "destroy", G_CALLBACK(destroy), app);

  GtkWidget *prep_phase_gridbox = gtk_grid_new();
  gtk_widget_set_halign(prep_phase_gridbox, GTK_ALIGN_CENTER);
  gtk_widget_set_valign(prep_phase_gridbox, GTK_ALIGN_CENTER);
  gtk_grid_set_row_spacing(GTK_GRID(prep_phase_gridbox), 100);
  gtk_grid_set_column_spacing(GTK_GRID(prep_phase_gridbox), 100);
  gtk_container_add(GTK_CONTAINER(prep_phase_window), prep_phase_gridbox);
  
  char *label_info = "Preparation Phase for ";
  char buffer[strlen(label_info)+strlen(server->player_list[0]->playerName)+1];
  snprintf(buffer, sizeof(buffer), "%s%s", label_info, server->player_list[0]->playerName);
  GtkWidget *prep_phase_label = gtk_label_new(buffer);
  gtk_widget_set_halign(prep_phase_label, GTK_ALIGN_CENTER);
  gtk_grid_attach(GTK_GRID(prep_phase_gridbox), prep_phase_label, 0, 0, 2, 1);

  GtkWidget *prep_phase_button_gridbox = gtk_grid_new();
  gtk_grid_attach(GTK_GRID(prep_phase_gridbox), prep_phase_button_gridbox, 0, 1, 1, 1);
  gtk_grid_set_row_homogeneous(GTK_GRID(prep_phase_button_gridbox), TRUE);
  gtk_grid_set_column_homogeneous(GTK_GRID(prep_phase_button_gridbox), TRUE);
  prepPhaseButtonGrid = (GtkWidget **)malloc(sizeof(GtkWidget *) * NUM_ROWS * NUM_COLS);
  for (int i = 0; i < NUM_ROWS; i++)
  {
    for (int j = 0; j < NUM_COLS; j++)
    {
      ship_loc_t *ship_loc = (ship_loc_t *)malloc(sizeof(ship_loc_t));
      ship_loc->origin_row = i;
      ship_loc->origin_col = j;
      prepPhaseButtonGrid[(i*NUM_ROWS)+j] = gtk_button_new();
      gtk_grid_attach(GTK_GRID(prep_phase_button_gridbox), prepPhaseButtonGrid[(i*NUM_ROWS)+j], j, i, 1, 1);
      g_signal_connect(G_OBJECT(prepPhaseButtonGrid[(i*NUM_ROWS)+j]), "clicked", G_CALLBACK(place_ship_on_grid), ship_loc);
    }
  }

  GtkWidget *buttons_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 50);
  gtk_widget_set_valign(buttons_vbox, GTK_ALIGN_CENTER);
  gtk_widget_set_halign(buttons_vbox, GTK_ALIGN_CENTER);
  gtk_widget_set_vexpand(buttons_vbox, FALSE);
  gtk_widget_set_hexpand(buttons_vbox, FALSE);
  gtk_grid_attach(GTK_GRID(prep_phase_gridbox), buttons_vbox, 1, 1, 1, 1);

  GtkWidget *prep_phase_rotate = gtk_button_new_with_label("Rotate");
  GtkWidget *prep_phase_set = gtk_button_new_with_label("Set ship");
  GtkWidget *prep_phase_finish = gtk_button_new_with_label("Finish");
  gtk_box_pack_start(GTK_BOX(buttons_vbox), prep_phase_rotate, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(buttons_vbox), prep_phase_set, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(buttons_vbox), prep_phase_finish, FALSE, FALSE, 0);

  gtk_widget_show_all(prep_phase_window);
}

static void initialize_players(GtkWidget *widget, gpointer data)
{
  uint8_t error_flag = 1;
  for (int i = 0; i < server->playercnt; i++)
  {
    if (!strlen(gtk_entry_get_text(GTK_ENTRY(playerNameDialog[i]))))
    {
      gtk_entry_blank_error_dialog(NULL, "ERROR: Names cannot be blank!");
      error_flag = 0;
      break;
    }
    for (int j = i+1; j < server->playercnt; j++)
    {
      if (!strcmp(gtk_entry_get_text(GTK_ENTRY(playerNameDialog[i])), gtk_entry_get_text(GTK_ENTRY(playerNameDialog[j]))))
      {
        gtk_entry_blank_error_dialog(NULL, "ERROR: Names cannot be same!");
        error_flag = 0;
        break;
      }
    }
  }

  if (!error_flag)
    return;

  server->player_list = (player_t **)malloc(sizeof(player_t *) * server->playercnt);
  uint8_t *rng_arr = alloc_rng_arr(server->playercnt);
  uint8_t server_sz = server->playercnt;
  for (int i = 0; i < server->playercnt; i++)
  {
    uint8_t rng_val = get_rng_val(rng_arr, &server_sz);
    server->player_list[rng_val] = (player_t *)malloc(sizeof(player_t));
    name = g_strdup(gtk_entry_get_text(GTK_ENTRY(playerNameDialog[i])));
    player_init(server->player_list[rng_val], name, server->playercnt, rng_val);
  }

  for (int i = 0; i < server->playercnt; i++)
  {
    printf("player %s\n", server->player_list[i]->playerName);
  }

  free(rng_arr);
  free(playerNameDialog);
  gtk_widget_hide(create_server_window);
  gtk_widget_destroy(GTK_WIDGET(data));
  preparation_phase();
}

static void get_player_names(void)
{
  uint8_t err_flag = 1;

  playerNameDialog = (GtkWidget **)malloc(sizeof(GtkWidget *) * server->playercnt);

  GtkWidget *get_player_names_window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(get_player_names_window), "Initialize Players");
  gtk_window_set_position(GTK_WINDOW(get_player_names_window), GTK_WIN_POS_CENTER_ALWAYS);
  gtk_widget_set_size_request(get_player_names_window, 640, 480);

  GtkWidget *get_player_names_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
  gtk_container_add(GTK_CONTAINER(get_player_names_window), get_player_names_vbox);
  gtk_widget_set_valign(get_player_names_vbox, GTK_ALIGN_CENTER);
  gtk_widget_set_halign(get_player_names_vbox, GTK_ALIGN_CENTER);

  GtkWidget *get_player_names_label = gtk_label_new("Enter Player Names");
  gtk_box_pack_start(GTK_BOX(get_player_names_vbox), get_player_names_label, FALSE, FALSE, 0);


  for (int i = 0; i < server->playercnt; i++)
  {
    GtkWidget *entry = gtk_entry_new();
    playerNameDialog[i] = entry;
    gtk_box_pack_start(GTK_BOX(get_player_names_vbox), entry, FALSE, FALSE, 0);
  }

  GtkWidget *get_player_names_button = gtk_button_new_with_label("OK");
  gtk_box_pack_start(GTK_BOX(get_player_names_vbox), get_player_names_button, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(get_player_names_button), "clicked", G_CALLBACK(initialize_players), get_player_names_window);

  gtk_widget_show_all(get_player_names_window);
}

static void set_server_details(GtkWidget *widget, gpointer data)
{
  create_server_ctx_t *gui_args = (create_server_ctx_t *)data;
  uint8_t err_flag = 1;
  server = (server_t *)malloc(sizeof(server_t));

  const char *text_from_name_entry = gtk_entry_get_text(GTK_ENTRY(gui_args->server_name_widget));
  const char *text_from_playercnt_entry = gtk_entry_get_text(GTK_ENTRY(gui_args->server_playercnt_widget));
  const char *text_from_pass_entry = gtk_entry_get_text(GTK_ENTRY(gui_args->server_password_widget));

  if (!strlen(text_from_name_entry))
  {
    gtk_entry_blank_error_dialog(NULL, "Server name field cannot be blank!");
    printf("ERROR: Field cannot be blank!\n");
    err_flag = 0;
  }

  if (!strlen(text_from_playercnt_entry))
  {
    gtk_entry_blank_error_dialog(NULL, "Number of players field cannot be blank!");
    printf("ERROR: Field cannot be blank!\n");
    err_flag = 0;
  }

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gui_args->server_pass_enabled_widget)) == TRUE &&
        !strlen(text_from_pass_entry))
  {
    gtk_entry_blank_error_dialog(NULL, "Server password field cannot be blank!");
    err_flag = 0;
    printf("ERROR: Field cannot be blank\n");
  }

  if (!err_flag)
  {
    return;
  }

  uint8_t playercnt = atoi(text_from_playercnt_entry);
  char *name = g_strdup(text_from_name_entry);
  char *pass = g_strdup(text_from_pass_entry);

  // server->player_list = (player_t **)malloc(sizeof(player_t *) * playercnt);
  server->server_name = name;
  server->playercnt = playercnt;
  
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gui_args->server_pass_enabled_widget)))
  {
    server->pass = pass;
  }
  else
  {
    server->pass = NULL;
  }

  free(gui_args);
  get_player_names();
}

static void hide_entry(GtkWidget* widget, gpointer data)
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)) == TRUE)
    gtk_widget_set_sensitive(GTK_WIDGET(data), TRUE);
  else
    gtk_widget_set_sensitive(GTK_WIDGET(data), FALSE);
}

static void back_to_main_action(GtkWidget* widget, gpointer data)
{
  gtk_widget_hide(GTK_WIDGET(gtk_application_get_active_window(app)));
  gtk_widget_show(GTK_WIDGET(data));
}

static void create_server_menu(GtkWidget* widget, gpointer data)
{
  gtk_widget_hide(GTK_WIDGET(data));
  
  if (create_server_window != NULL)
  {
    gtk_widget_show(create_server_window);
    return;
  }
  create_server_window = gtk_application_window_new(app);
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
  gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);

  g_signal_connect(G_OBJECT(protect_server_checkbutton), "clicked", G_CALLBACK(hide_entry), password_entry);

  GtkWidget *confirm_button = gtk_button_new_with_label("Start Server!");
  gtk_grid_attach(GTK_GRID(server_data_grid), confirm_button, 0, 4, 1, 1);
  GtkWidget *back_to_main_button = gtk_button_new_with_label("Back to main menu");
  gtk_grid_attach(GTK_GRID(server_data_grid), back_to_main_button, 1, 4, 1, 1);

  create_server_ctx_t *server_gui_ctx = (create_server_ctx_t *)malloc(sizeof(create_server_ctx_t));
  server_gui_ctx->server_name_widget = server_name_entry;
  server_gui_ctx->server_playercnt_widget = player_count_entry;
  server_gui_ctx->server_password_widget = password_entry;
  server_gui_ctx->server_pass_enabled_widget = protect_server_checkbutton;
  
  g_signal_connect(G_OBJECT(confirm_button), "clicked", G_CALLBACK(set_server_details), server_gui_ctx);
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
  g_signal_connect(G_OBJECT(couch_button), "clicked", G_CALLBACK(create_server_menu), window);

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