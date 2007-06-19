/*
 * Pointer navigation macros 
 *
 * Compile with:
 * gcc `pkg-config --cflags --libs x11 gtk+-2.0` navmacro.c
 *
 */

#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/XTest.h>

#include <gtk/gtk.h>
#include <gdk/gdkx.h>

#define BORDER_SIZE (3)

typedef struct ui {
  GtkWidget *window;
  GtkWidget *status_label;
  GtkWidget *vbox;

  /* Hidden pieces of the UI */
  GtkWidget *user_input;
  GtkWidget *fixed_container;
} ui_t;

ui_t ui;

static struct xdpyinfo {
  Display *display;
  int width;
} xwin;

static gboolean
input_changed_event(GtkWidget *widget, gpointer userdata) {
  const gchar *field_value;

  field_value = gtk_entry_get_text(GTK_ENTRY(widget));
  gtk_label_set_markup(GTK_LABEL(ui.status_label), field_value);
  return TRUE;
}

static gboolean
input_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data) {

  if (event->hardware_keycode == 9) // Escape
    gtk_main_quit ();

  //g_print("key %d\n", event->hardware_keycode);
}


static gboolean delete_event( GtkWidget *widget, GdkEvent *event, gpointer data ) {
    return FALSE; /* Call destroy on window closure (deletion) */
}

static void realize_mainwindow( GtkWidget *widget, gpointer data) {
  int window_id;
  XSetWindowAttributes winattr;
  GdkScreen *gdk_screen;
  int grabstatus;

  xwin.display = GDK_WINDOW_XDISPLAY(widget->window);
  window_id = GDK_WINDOW_XID(widget->window);

  /* Tell window managers to not manage us */
  winattr.override_redirect = 1;
  XChangeWindowAttributes(xwin.display, window_id, CWOverrideRedirect, &winattr);
  /* Set width to 100% of the screen */
  gdk_screen = gtk_widget_get_screen(widget);
  xwin.width = gdk_screen_get_width(gdk_screen);
  gtk_window_set_default_size(GTK_WINDOW(widget), xwin.width / 2, -1);

}

static void configure_event_mainwindow( GtkWidget *widget, gpointer data ) {
  int grabstatus;
  int window_id;

  window_id = GDK_WINDOW_XID(widget->window);
  grabstatus = XGrabKeyboard(xwin.display, window_id, FALSE, GrabModeSync, GrabModeAsync, CurrentTime);
  if (grabstatus != GrabSuccess)
    printf("Failed to grab the display: %d\n", grabstatus);
}

static void destroy( GtkWidget *widget, gpointer   data ) {
  gtk_main_quit ();
}

int main( int argc, char **argv ) {
  gtk_init (&argc, &argv);

  ui.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(ui.window), GTK_WIN_POS_CENTER_ALWAYS);
  g_signal_connect(G_OBJECT (ui.window), "delete_event",
                   G_CALLBACK (delete_event), NULL);
  g_signal_connect(G_OBJECT (ui.window), "destroy",
                   G_CALLBACK (destroy), NULL);
  g_signal_connect(G_OBJECT(ui.window), "realize",
                   G_CALLBACK(realize_mainwindow), NULL);
  g_signal_connect(G_OBJECT(ui.window), "configure_event",
                   G_CALLBACK(configure_event_mainwindow), NULL);

  ui.vbox = gtk_vbox_new(FALSE, 0);
  
  ui.user_input = gtk_entry_new();
  g_signal_connect(G_OBJECT(ui.user_input), "changed", G_CALLBACK (input_changed_event), NULL);
  g_signal_connect_after(G_OBJECT(ui.user_input), "key-press-event", G_CALLBACK (input_key_press), NULL);
  
  ui.status_label = gtk_label_new("");
  static GdkColor label_bgcolor;
  gdk_color_parse("#DFDF60", &label_bgcolor);
  //GdkColormap *cmap = gdk_colormap_get_system();
  //gdk_colormap_alloc_color(cmap, &label_bgcolor, TRUE, TRUE);
  gtk_widget_modify_base(ui.user_input, GTK_STATE_NORMAL, &label_bgcolor);

  PangoFontDescription *pfd;
  pfd = pango_font_description_new();
  pango_font_description_set_size(pfd, 14 * PANGO_SCALE);
  gtk_widget_modify_font(ui.user_input, pfd);

  gtk_container_set_border_width(GTK_CONTAINER(ui.window), 5);

  /* Hide the input field */
  //gtk_widget_set_size_request(ui.user_input, -1, 1);
  //ui.fixed_container = gtk_fixed_new();
  //gtk_fixed_put(GTK_FIXED(ui.fixed_container), ui.user_input, -50, -50);
  //gtk_container_add(GTK_CONTAINER(ui.vbox), ui.fixed_container);
  //gtk_container_add(GTK_CONTAINER(ui.vbox), ui.status_label);

  gtk_container_add(GTK_CONTAINER(ui.vbox), ui.user_input);
  gtk_container_add(GTK_CONTAINER(ui.window), ui.vbox);

  gtk_widget_show_all(ui.window);
  gtk_widget_grab_focus(GTK_WIDGET(ui.user_input));

  gtk_main();
  return 0;
}
