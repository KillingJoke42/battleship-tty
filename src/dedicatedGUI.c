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
GtkWidget *fire_phase_window = NULL;
GtkWidget *select_oppn_window = NULL;

static server_t *server;
char *name;
GtkWidget **playerNameDialog;
GtkWidget **prepPhaseButtonGrid;

GtkWidget *prep_phase_curr_ship_label;
GtkWidget *prep_phase_label;
GtkWidget *turn_prompt;

ship_loc_t *ship_loc;
static int orientation = 0;
static int player_idx = 0;
static int placed_ships = 0;
uint8_t placing_ship = 0;

static void preparation_phase(void);
static void place_ship_on_grid(GtkWidget *widget, gpointer data);
static void refresh_button_grid(uint8_t all);
static void end_round(GtkWidget *widget, gpointer data);

void destroy (GtkWidget* widget, gpointer data)
{
  printf("Exiting...\n");
  g_application_quit(G_APPLICATION(app));
}

void close_window(GtkWidget *widget, gpointer data)
{
  gtk_window_close(GTK_WINDOW(data));
}

static void gtk_entry_blank_error_dialog(GtkWindow* parent, const char *message, const char *title)
{
  GtkWidget *dialog, *label, *content_area;
  GtkDialogFlags flags;

  flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
  dialog = gtk_dialog_new_with_buttons ((title == NULL) ? "ERROR" : title,
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

static void fire_phase_attack(GtkWidget *widget, gpointer data)
{
  ship_loc_t *but_pos = (ship_loc_t *)data;
  uint8_t hitormiss = fire_after_calc(server->player_list[but_pos->orientation], 
                                      server->player_list[player_idx], 
                                      but_pos->origin_row, 
                                      but_pos->origin_col);

  if (hitormiss)
    gtk_entry_blank_error_dialog(NULL, "HIT!", "Status");
  else
    gtk_entry_blank_error_dialog(NULL, "MISS!", "Status");
  gtk_widget_destroy(fire_phase_window);
  uint8_t update = 1;
  end_round(NULL, GINT_TO_POINTER(update));
}

static void fire_phase(GtkWidget *widget, gpointer data)
{
  gint index = gtk_list_box_row_get_index(gtk_list_box_get_selected_row(GTK_LIST_BOX(data)));

  if (index == -1)
  {
    gtk_entry_blank_error_dialog(NULL, "Please make a selection!!", NULL);
    return;
  }

  fire_phase_window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(fire_phase_window), "Fire phase!");
  gtk_window_set_default_size(GTK_WINDOW(fire_phase_window), 320, 240);
  gtk_window_set_position(GTK_WINDOW(fire_phase_window), GTK_WIN_POS_CENTER_ALWAYS);

  GtkWidget *fire_phase_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
  gtk_widget_set_valign(fire_phase_vbox, GTK_ALIGN_CENTER);
  gtk_widget_set_halign(fire_phase_vbox, GTK_ALIGN_CENTER);
  gtk_widget_set_vexpand(fire_phase_vbox, FALSE);
  gtk_widget_set_hexpand(fire_phase_vbox, FALSE);
  gtk_container_add(GTK_CONTAINER(fire_phase_window), fire_phase_vbox);

  char *lbl_txt = "Firing on ";
  uint8_t buffsz = strlen(lbl_txt)+strlen(server->player_list[index]->playerName)+1;
  char buffer[buffsz];
  snprintf(buffer, buffsz, "%s%s", lbl_txt, server->player_list[index]->playerName);
  GtkWidget *fire_phase_label = gtk_label_new(buffer);
  gtk_box_pack_start(GTK_BOX(fire_phase_vbox), fire_phase_label, FALSE, FALSE, 0);

  // GdkScreen *screen = gdk_screen_get_default();
  // GtkCssProvider *css = gtk_css_provider_new();
  // GtkStyleContext *ctx = gtk_style_context_new();
  // gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  // gtk_css_provider_load_from_data(GTK_CSS_PROVIDER(css), "#small {padding: 0;}", -1, NULL);

  GtkWidget *fire_phase_grid = gtk_grid_new();
  gtk_grid_set_column_homogeneous(GTK_GRID(fire_phase_grid), TRUE);
  gtk_grid_set_row_homogeneous(GTK_GRID(fire_phase_grid), TRUE);
  gtk_box_pack_start(GTK_BOX(fire_phase_vbox), fire_phase_grid, FALSE, FALSE, 0);

  for (int i = 0; i < NUM_ROWS; i++)
  {
    for (int j = 0; j < NUM_COLS; j++)
    {
      ship_loc_t *but_pos = (ship_loc_t *)malloc(sizeof(ship_loc_t));
      but_pos->origin_row = i;
      but_pos->origin_col = j;
      but_pos->orientation = index;
      char mark[2];
      mark[0] = server->player_list[player_idx]->oppn_info[index][(i*NUM_ROWS)+j];
      mark[1] = '\0';
      GtkWidget *button = gtk_button_new_with_label(" ");

      char *mrkp_txt;
      if(mark[0] == 'X')
      {
        mrkp_txt = "<span foreground='#ff0000'>X</span>";
        gtk_widget_set_sensitive(button, FALSE);
      }
      else if (mark[0] == '-')
      {
        mrkp_txt = "<span foreground='#808080'>-</span>";
        gtk_widget_set_sensitive(button, FALSE);
      }
      else
        mrkp_txt = "<span foreground='#00ff00'>O</span>";

      gtk_label_set_markup(GTK_LABEL(gtk_bin_get_child(GTK_BIN(button))), mrkp_txt);
      gtk_grid_attach(GTK_GRID(fire_phase_grid), button, j, i, 1, 1);
      g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(fire_phase_attack), but_pos);
    }
  }

  gtk_window_close(GTK_WINDOW(select_oppn_window));
  gtk_widget_show_all(fire_phase_window);
}

static void view_your_notes(GtkWidget *widget, gpointer data)
{
  gint index = gtk_list_box_row_get_index(gtk_list_box_get_selected_row(GTK_LIST_BOX(data)));
  
  if (index == -1)
  {
    gtk_entry_blank_error_dialog(NULL, "Please make a selection!!", NULL);
    return;
  }

  GtkWidget *view_notes_window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(view_notes_window), "Notes");
  gtk_window_set_default_size(GTK_WINDOW(view_notes_window), 320, 240);
  gtk_window_set_position(GTK_WINDOW(view_notes_window), GTK_WIN_POS_CENTER_ALWAYS);

  GtkWidget *view_notes_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
  gtk_widget_set_valign(view_notes_vbox, GTK_ALIGN_CENTER);
  gtk_widget_set_halign(view_notes_vbox, GTK_ALIGN_CENTER);
  gtk_widget_set_vexpand(view_notes_vbox, FALSE);
  gtk_widget_set_hexpand(view_notes_vbox, FALSE);
  gtk_container_add(GTK_CONTAINER(view_notes_window), view_notes_vbox);

  char *lbl_txt = "Viewing your notes on ";
  uint8_t bufflen = strlen(lbl_txt)+strlen(server->player_list[index]->playerName)+1;
  char buffer[bufflen];
  snprintf(buffer, bufflen, "%s%s", lbl_txt, server->player_list[index]->playerName);
  GtkWidget *view_notes_label = gtk_label_new(buffer);
  gtk_box_pack_start(GTK_BOX(view_notes_vbox), view_notes_label, FALSE, FALSE, 0);

  GtkWidget *view_notes_grid = gtk_grid_new();
  gtk_grid_set_row_homogeneous(GTK_GRID(view_notes_grid), TRUE);
  gtk_grid_set_column_homogeneous(GTK_GRID(view_notes_grid), TRUE);
  gtk_grid_set_row_spacing(GTK_GRID(view_notes_grid), 10);
  gtk_grid_set_column_spacing(GTK_GRID(view_notes_grid), 10);
  gtk_box_pack_start(GTK_BOX(view_notes_vbox), view_notes_grid, FALSE, FALSE, 0);

  for (int i = 0; i < NUM_ROWS; i++)
  {
    for (int j = 0; j < NUM_COLS; j++)
    {
      char mark[2];
      mark[0] = server->player_list[player_idx]->oppn_info[index][(i*NUM_ROWS)+j];
      mark[1] = '\0';
      char *tag_open;
      if (mark[0] == 'X')
        tag_open = "<b><span background='#ff0000' foreground='#ffffff' font='16'>";
      else if (mark[0] == '-')
        tag_open = "<b><span background='#808080' foreground='#ffffff' font='16'>";
      else
        tag_open = "<b><span background='#00ff00' foreground='#ffffff' font='16'>";
      char *tag_close = "</span></b>";
      uint8_t mrkp_txt_sz = strlen(tag_open)+1+strlen(tag_close)+1;
      char mrkp_txt[mrkp_txt_sz];
      snprintf(mrkp_txt, mrkp_txt_sz, "%s%s%s", tag_open, mark, tag_close);
      GtkWidget *entry_elem = gtk_label_new(mrkp_txt);
      gtk_label_set_use_markup(GTK_LABEL(entry_elem), TRUE);
      gtk_grid_attach(GTK_GRID(view_notes_grid), entry_elem, j, i, 1, 1);
    }
  }

  GtkWidget *view_notes_close_button = gtk_button_new_with_label("Close");
  gtk_box_pack_start(GTK_BOX(view_notes_vbox), view_notes_close_button, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(view_notes_close_button), "clicked", G_CALLBACK(close_window), view_notes_window);

  gtk_window_close(GTK_WINDOW(select_oppn_window));
  gtk_widget_show_all(view_notes_window);
}

static void select_an_oppn(GtkWidget *widget, gpointer data)
{
  select_oppn_window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(select_oppn_window), "Select a player");
  gtk_window_set_default_size(GTK_WINDOW(select_oppn_window), 320, 240);
  gtk_window_set_position(GTK_WINDOW(select_oppn_window), GTK_WIN_POS_CENTER_ALWAYS);

  GtkWidget *select_oppn_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
  gtk_widget_set_valign(select_oppn_vbox, GTK_ALIGN_CENTER);
  gtk_widget_set_halign(select_oppn_vbox, GTK_ALIGN_CENTER);
  gtk_widget_set_vexpand(select_oppn_vbox, FALSE);
  gtk_widget_set_hexpand(select_oppn_vbox, FALSE);
  gtk_container_add(GTK_CONTAINER(select_oppn_window), select_oppn_vbox);

  GtkWidget *select_oppn_label = gtk_label_new("Select a player to perform operation on");
  gtk_box_pack_start(GTK_BOX(select_oppn_vbox), select_oppn_label, FALSE, FALSE, 0);
  
  GtkWidget *select_oppn_list = gtk_list_box_new();
  gtk_list_box_set_selection_mode(GTK_LIST_BOX(select_oppn_list), GTK_SELECTION_BROWSE);
  gtk_box_pack_start(GTK_BOX(select_oppn_vbox), select_oppn_list, FALSE, FALSE, 0);

  GtkWidget *players_lbl[server->playercnt];
  for (int i = 0; i < server->playercnt; i++)
  {
    players_lbl[i] = gtk_label_new(server->player_list[i]->playerName);
    gtk_list_box_insert(GTK_LIST_BOX(select_oppn_list), players_lbl[i], -1);
    if (i == player_idx)
      gtk_list_box_row_set_selectable(gtk_list_box_get_row_at_index(GTK_LIST_BOX(select_oppn_list), i), FALSE);
  }

  GtkWidget *select_oppn_button = gtk_button_new_with_label("Select");
  gtk_box_pack_start(GTK_BOX(select_oppn_vbox), select_oppn_button, FALSE, FALSE, 0);

  g_signal_connect(G_OBJECT(select_oppn_button), "clicked", 
  (GPOINTER_TO_UINT(data)) ? G_CALLBACK(view_your_notes) : G_CALLBACK(fire_phase), select_oppn_list);

  GtkWidget *select_oppn_back_button = gtk_button_new_with_label("Back");
  gtk_box_pack_start(GTK_BOX(select_oppn_vbox), select_oppn_back_button, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(select_oppn_back_button), "clicked", G_CALLBACK(close_window), select_oppn_window);

  gtk_widget_show_all(select_oppn_window);
}

static void game_over(void)
{
  GtkWidget *end_game_window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(end_game_window), "GAME OVER");
  gtk_window_set_default_size(GTK_WINDOW(end_game_window), 320, 240);
  gtk_window_set_position(GTK_WINDOW(end_game_window), GTK_WIN_POS_CENTER_ALWAYS);
  g_signal_connect(G_OBJECT(end_game_window), "destroy", G_CALLBACK(destroy), NULL);

  GtkWidget *end_game_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 50);
  gtk_widget_set_valign(end_game_vbox, GTK_ALIGN_CENTER);
  gtk_widget_set_halign(end_game_vbox, GTK_ALIGN_CENTER);
  gtk_widget_set_vexpand(end_game_vbox, FALSE);
  gtk_widget_set_hexpand(end_game_vbox, FALSE);
  gtk_container_add(GTK_CONTAINER(end_game_window), end_game_vbox);

  GtkWidget *end_game_label = gtk_label_new("");
  gtk_label_set_markup(GTK_LABEL(end_game_label), "<b><span font='36'>GAME OVER</span></b>");
  gtk_box_pack_start(GTK_BOX(end_game_vbox), end_game_label, FALSE, FALSE, 0);

  GtkWidget *end_game_button = gtk_button_new_with_label("EXIT");
  gtk_box_pack_start(GTK_BOX(end_game_vbox), end_game_button, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(end_game_button), "clicked", G_CALLBACK(destroy), NULL);

  gtk_widget_show_all(end_game_window);
}

static void end_round(GtkWidget *widget, gpointer data)
{
  if (game_over_base_case(server))
  {
    if (ship_loc)
      free(ship_loc);

    exit_free(server);
    game_over();
    return;
  }
  if (player_idx == server->playercnt - 1)
  {
    player_idx = 0;
    goto skip_increment;
  }

  player_idx++;

skip_increment:
  // char *next_turn_pref = "Round Ended. ";
  // char *next_turn_suff = "'s turn.";
  // uint8_t buffsz = strlen(next_turn_pref)+strlen(server->player_list[player_idx]->playerName)+strlen(next_turn_suff)+1;
  // char buffer[buffsz];
  // snprintf(buffer, buffsz, "%s%s%s", next_turn_pref, server->player_list[player_idx]->playerName, next_turn_suff);
  // gtk_entry_blank_error_dialog(NULL, buffer, "End of Round");
  if (GPOINTER_TO_UINT(data))
  {
    char *lbl_txt = "'s trun";
    char *turn_name = server->player_list[player_idx]->playerName;
    uint8_t bufsz = strlen(lbl_txt)+strlen(turn_name)+1;
    char buffer[bufsz];
    snprintf(buffer, bufsz, "%s%s", turn_name, lbl_txt);
    gtk_label_set_text(GTK_LABEL(turn_prompt), buffer);
  }
}

static void combat_menu(void)
{
  GtkWidget *combat_menu_window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(combat_menu_window), "Begin Game");
  gtk_window_set_default_size(GTK_WINDOW(combat_menu_window), 320, 240);
  gtk_window_set_position(GTK_WINDOW(combat_menu_window), GTK_WIN_POS_CENTER_ALWAYS);
  g_signal_connect(G_OBJECT(combat_menu_window), "destroy", G_CALLBACK(destroy), NULL);

  GtkWidget *combat_menu_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
  gtk_widget_set_valign(combat_menu_vbox, GTK_ALIGN_CENTER);
  gtk_widget_set_halign(combat_menu_vbox, GTK_ALIGN_CENTER);
  gtk_widget_set_vexpand(combat_menu_vbox, FALSE);
  gtk_widget_set_hexpand(combat_menu_vbox, FALSE);
  gtk_container_add(GTK_CONTAINER(combat_menu_window), combat_menu_vbox);

  char *lbl_txt = "'s trun";
  char *turn_name = server->player_list[player_idx]->playerName;
  uint8_t bufsz = strlen(lbl_txt)+strlen(turn_name)+1;
  char buffer[bufsz];
  snprintf(buffer, bufsz, "%s%s", turn_name, lbl_txt);

  turn_prompt = gtk_label_new(buffer);
  gtk_box_pack_start(GTK_BOX(combat_menu_vbox), turn_prompt, FALSE, FALSE, 0);

  uint8_t choose_view_notes = 0;

  GtkWidget *fire_phase_button = gtk_button_new_with_label("Fire on selected player");
  gtk_box_pack_start(GTK_BOX(combat_menu_vbox), fire_phase_button, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(fire_phase_button), "clicked", G_CALLBACK(select_an_oppn), GINT_TO_POINTER(choose_view_notes));

  choose_view_notes = 1;

  GtkWidget *view_notes_button = gtk_button_new_with_label("View notes of selected player");
  gtk_box_pack_start(GTK_BOX(combat_menu_vbox), view_notes_button, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(view_notes_button), "clicked", G_CALLBACK(select_an_oppn), GINT_TO_POINTER(choose_view_notes));

  uint8_t upd = 1;
  GtkWidget *end_round_button = gtk_button_new_with_label("End round");
  gtk_box_pack_start(GTK_BOX(combat_menu_vbox), end_round_button, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(end_round_button), "clicked", G_CALLBACK(end_round), GINT_TO_POINTER(upd));

  gtk_widget_show_all(combat_menu_window);
}

static void next_player_placement(GtkWidget *widget, gpointer data)
{
  if (placed_ships != NUM_SHIPS)
  {
    gtk_entry_blank_error_dialog(NULL, "You haven't placed all the ships!", NULL);
    return;
  }

  player_idx++;

  if (player_idx == server->playercnt)
  {
    player_idx = 0;

    for (int i = 0; i < (NUM_ROWS * NUM_COLS); i++)
      gtk_widget_destroy(prepPhaseButtonGrid[i]);
    free(prepPhaseButtonGrid);

    gtk_widget_hide(GTK_WIDGET(data));
    combat_menu();
    return;
  }

  orientation = 0;
  placed_ships = 0;
  placing_ship = 0;
  ship_loc->origin_row = 0;
  ship_loc->origin_col = 0;
  
  char *label_info = "Preparation Phase for ";
  char buffer[strlen(label_info)+strlen(server->player_list[player_idx]->playerName)+1];
  snprintf(buffer, sizeof(buffer), "%s%s", label_info, server->player_list[player_idx]->playerName);
  gtk_label_set_text(GTK_LABEL(prep_phase_label), buffer);

  gtk_label_set_text(GTK_LABEL(prep_phase_curr_ship_label), "Placing carrier");

  refresh_button_grid(1);
  for (int i = 0; i < NUM_ROWS; i++)
  {
    for (int j = 0; j < NUM_COLS; j++)
    {
      gtk_widget_set_sensitive(prepPhaseButtonGrid[(i*NUM_ROWS)+j], TRUE);
    }
  }
}

static void change_orientation(GtkWidget *widget, gpointer data)
{
  if (placing_ship)
  {
    gtk_entry_blank_error_dialog(NULL, "Unable to change orientation; You are placing a ship!", NULL);
    return;
  }
  orientation = (orientation) ? 0 : 1;
}

static void fix_ship(GtkWidget *widget, gpointer data)
{
  if (!placing_ship)
    return;

  uint8_t origin_row = ship_loc->origin_row;
  uint8_t origin_col = ship_loc->origin_col;

  player_t *player = server->player_list[player_idx];

  player->player_ship_status.ship_locs[placed_ships].origin_row = origin_row;
  player->player_ship_status.ship_locs[placed_ships].origin_col = origin_col;
  player->player_ship_status.ship_locs[placed_ships].orientation = orientation;
  
  if (orientation == 0)
  {
      for (int j = 0; j < ship_sz_map[placed_ships]; j++)
      {
          arr_2d_set_char_val(player->playerPlacement, NUM_ROWS, origin_row, origin_col+j, placed_ships+1);
      }
  }
  else
  {
      for (int j = 0; j < ship_sz_map[placed_ships]; j++)
      {
          arr_2d_set_char_val(player->playerPlacement, NUM_ROWS, origin_row+j, origin_col, placed_ships+1);
      }
  }

  placing_ship = 0;
  placed_ships++;

  if (placed_ships == NUM_SHIPS)
  {
    for (int i = 0; i < NUM_ROWS; i++)
    {
      for (int j = 0; j < NUM_COLS; j++)
      {
        gtk_widget_set_sensitive(prepPhaseButtonGrid[(i*NUM_ROWS)+j], FALSE);
      }
    }
    gtk_label_set_text(GTK_LABEL(data), "All ships placed!");
    return;
  }

  char buffer[25];
  char *lbl_str = "Placing ";
  snprintf(buffer, 20, "%s%s", lbl_str, ship_name_map[placed_ships]);
  gtk_label_set_text(GTK_LABEL(data), buffer);
}

static void refresh_button_grid(uint8_t all)
{
  if (all)
  {
    for (int i = 0; i < NUM_ROWS; i++)
    {
      for (int j = 0; j < NUM_COLS; j++)
      {
        gtk_label_set_markup(
          GTK_LABEL(gtk_bin_get_child(GTK_BIN(prepPhaseButtonGrid[(i*NUM_ROWS)+j]))), 
          refresh_all_css);
      }
    }
  }
  else
  {
    for (int i = 0; i < ship_sz_map[placed_ships]; i++)
    {
      uint8_t button_ctx = (orientation) ? 
                            (((ship_loc->origin_row+i)*NUM_ROWS)+ship_loc->origin_col) :
                            ((ship_loc->origin_row*NUM_ROWS)+ship_loc->origin_col+i);
      gtk_label_set_markup(
          GTK_LABEL(gtk_bin_get_child(GTK_BIN(prepPhaseButtonGrid[button_ctx]))), 
          default_css);
    }
  }
}

static void place_ship_on_grid(GtkWidget* widget, gpointer data)
{
  if (placing_ship)
  {
    refresh_button_grid(0);
    placing_ship = 0;
    return;
  }
  ship_loc_t *ship_origin = (ship_loc_t *)data;
  int ship_origin_row = ship_origin->origin_row;
  int ship_origin_col = ship_origin->origin_col;
  // GtkStyleContext *context[SHIP_CARRIER_SZ];

  if (!check_availability(server->player_list[player_idx], 
                          (ship_loc_t){ship_origin_row, ship_origin_col, orientation}, 
                          ship_sz_map[placed_ships]))
  {
    gtk_entry_blank_error_dialog(NULL, "Cannot place ship at that location!", NULL);
    return;
  }

  // char inform[2];
  // inform[0] = ship_name_map[placed_ships][0];
  // inform[1] = '\0';

  for (int i = 0; i < ship_sz_map[placed_ships]; i++)
  {
    uint8_t button_ctx = (orientation) ? 
                        (((ship_origin_row+i)*NUM_ROWS)+ship_origin_col) :
                        ((ship_origin_row*NUM_ROWS)+ship_origin_col+i);
    gtk_label_set_markup(
          GTK_LABEL(gtk_bin_get_child(GTK_BIN(prepPhaseButtonGrid[button_ctx]))), 
          ship_css_map[placed_ships]);
  }
  ship_loc->origin_row = ship_origin_row;
  ship_loc->origin_col = ship_origin_col;
  placing_ship = 1;
}

static void preparation_phase(void)
{
  ship_loc = (ship_loc_t *)malloc(sizeof(ship_loc_t));

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
  char buffer[strlen(label_info)+strlen(server->player_list[player_idx]->playerName)+1];
  snprintf(buffer, sizeof(buffer), "%s%s", label_info, server->player_list[player_idx]->playerName);
  prep_phase_label = gtk_label_new(buffer);
  gtk_widget_set_halign(prep_phase_label, GTK_ALIGN_CENTER);
  gtk_grid_attach(GTK_GRID(prep_phase_gridbox), prep_phase_label, 0, 0, 2, 1);

  GtkWidget *prep_phase_button_gridbox = gtk_grid_new();
  gtk_grid_attach(GTK_GRID(prep_phase_gridbox), prep_phase_button_gridbox, 0, 1, 1, 1);
  gtk_grid_set_row_homogeneous(GTK_GRID(prep_phase_button_gridbox), TRUE);
  gtk_grid_set_column_homogeneous(GTK_GRID(prep_phase_button_gridbox), TRUE);
  prepPhaseButtonGrid = (GtkWidget **)malloc(sizeof(GtkWidget *) * NUM_ROWS * NUM_COLS);

  GdkScreen *screen = gdk_screen_get_default();
  GtkCssProvider *css = gtk_css_provider_new();
  GtkStyleContext *ctx = gtk_style_context_new();
  gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  gtk_css_provider_load_from_data(GTK_CSS_PROVIDER(css), "* {padding: 1px;}", -1, NULL);

  for (int i = 0; i < NUM_ROWS; i++)
  {
    for (int j = 0; j < NUM_COLS; j++)
    {
      ship_loc_t *button_pos = (ship_loc_t *)malloc(sizeof(ship_loc_t));
      button_pos->origin_row = i;
      button_pos->origin_col = j;
      prepPhaseButtonGrid[(i*NUM_ROWS)+j] = gtk_button_new_with_label("      ");
      gtk_grid_attach(GTK_GRID(prep_phase_button_gridbox), prepPhaseButtonGrid[(i*NUM_ROWS)+j], j, i, 1, 1);
      g_signal_connect(G_OBJECT(prepPhaseButtonGrid[(i*NUM_ROWS)+j]), "clicked", G_CALLBACK(place_ship_on_grid), button_pos);
    }
  }

  GtkWidget *buttons_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 50);
  gtk_widget_set_valign(buttons_vbox, GTK_ALIGN_CENTER);
  gtk_widget_set_halign(buttons_vbox, GTK_ALIGN_CENTER);
  gtk_widget_set_vexpand(buttons_vbox, FALSE);
  gtk_widget_set_hexpand(buttons_vbox, FALSE);
  gtk_grid_attach(GTK_GRID(prep_phase_gridbox), buttons_vbox, 1, 1, 1, 1);

  prep_phase_curr_ship_label = gtk_label_new("Placing carrier");
  GtkWidget *prep_phase_rotate = gtk_button_new_with_label("Rotate");
  GtkWidget *prep_phase_set = gtk_button_new_with_label("Set ship");
  GtkWidget *prep_phase_finish = gtk_button_new_with_label("Finish");
  gtk_box_pack_start(GTK_BOX(buttons_vbox), prep_phase_curr_ship_label, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(buttons_vbox), prep_phase_rotate, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(buttons_vbox), prep_phase_set, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(buttons_vbox), prep_phase_finish, FALSE, FALSE, 0);

  g_signal_connect(G_OBJECT(prep_phase_rotate), "clicked", G_CALLBACK(change_orientation), NULL);
  g_signal_connect(G_OBJECT(prep_phase_set), "clicked", G_CALLBACK(fix_ship), prep_phase_curr_ship_label);
  g_signal_connect(G_OBJECT(prep_phase_finish), "clicked", G_CALLBACK(next_player_placement), prep_phase_window);

  gtk_widget_show_all(prep_phase_window);
}

static void initialize_players(GtkWidget *widget, gpointer data)
{
  uint8_t error_flag = 1;
  for (int i = 0; i < server->playercnt; i++)
  {
    if (!strlen(gtk_entry_get_text(GTK_ENTRY(playerNameDialog[i]))))
    {
      gtk_entry_blank_error_dialog(NULL, "ERROR: Names cannot be blank!", NULL);
      error_flag = 0;
      break;
    }
    for (int j = i+1; j < server->playercnt; j++)
    {
      if (!strcmp(gtk_entry_get_text(GTK_ENTRY(playerNameDialog[i])), gtk_entry_get_text(GTK_ENTRY(playerNameDialog[j]))))
      {
        gtk_entry_blank_error_dialog(NULL, "ERROR: Names cannot be same!", NULL);
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

  for (int i = 0; i < server->playercnt; i++)
    gtk_widget_destroy(playerNameDialog[i]);

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
    gtk_entry_blank_error_dialog(NULL, "Server name field cannot be blank!", NULL);
    printf("ERROR: Field cannot be blank!\n");
    err_flag = 0;
  }

  if (!strlen(text_from_playercnt_entry))
  {
    gtk_entry_blank_error_dialog(NULL, "Number of players field cannot be blank!", NULL);
    printf("ERROR: Field cannot be blank!\n");
    err_flag = 0;
  }

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gui_args->server_pass_enabled_widget)) == TRUE &&
        !strlen(text_from_pass_entry))
  {
    gtk_entry_blank_error_dialog(NULL, "Server password field cannot be blank!", NULL);
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