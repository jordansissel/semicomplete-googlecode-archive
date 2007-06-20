/*
 * Pointer navigation macros 
 *
 * Compile with:
 * gcc `pkg-config --cflags --libs x11 gtk+-2.0` navmacro.c
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/XTest.h>

#include <gtk/gtk.h>
#include <gdk/gdkx.h>

#include "xdo.h"

#define BORDER_SIZE (3)

#define UI_INACTIVE 0
#define UI_ACTIVE 1

extern char **environ;

typedef struct ui {
  GtkWidget *window;
  GtkWidget *status_label;
  GtkWidget *vbox;

  /* Hidden pieces of the UI */
  GtkWidget *user_input;
  GtkWidget *fixed_container;

  int state;
} ui_t;

ui_t ui;

static struct xdpyinfo {
  Display *gtk_display;
  Display *x_display;
  int width;
  int main_window;
} xwin;

static void main_loop();
static void ui_init();

static void x_bindkey();
static void x_unbindkey();

static void x_loop_iteration();
static void gtk_loop_iteration();

static void exec_macro(const gchar *macro) {
  if (fork() == 0) {
    const char *argv[3] = { "/bin/sh", "-c", macro };
    close(0);
    close(1);
    close(2);
    int devnull_r = open("/dev/null", "r");
    int devnull_w = open("/dev/null", "w");
    dup(devnull_r, 0);
    dup(devnull_w, 1);
    dup(devnull_w, 2);
    execve("/bin/sh", argv, environ); 
    exit(0);
  }
}

static void show_ui() {
  int grabstatus;
  x_unbindkey();

  ui_init();
  gtk_widget_show_all(ui.window);
  gtk_widget_grab_focus(GTK_WIDGET(ui.user_input));
  ui.state = UI_ACTIVE;

}

static void hide_ui() {
  XUngrabKeyboard(xwin.gtk_display, CurrentTime);
  XFlush(xwin.gtk_display);

  gtk_widget_destroy(ui.window);
  //gtk_widget_destroy(ui.vbox);
  gtk_widget_destroy(ui.user_input);
  //gtk_widget_destroy(ui.fixed_container);

  while (gtk_events_pending()) 
    gtk_main_iteration();

  ui.state = UI_INACTIVE;
  x_bindkey();
}

static gboolean
input_changed_event(GtkWidget *widget, gpointer userdata) {
  const gchar *field_value;

  field_value = gtk_entry_get_text(GTK_ENTRY(widget));
  //gtk_label_set_markup(GTK_LABEL(ui.status_label), field_value);
  return TRUE;
}

static gboolean
input_activate(GtkWidget *widget, gpointer userdata) {
  const gchar *field_value;

  field_value = gtk_entry_get_text(GTK_ENTRY(widget));
  exec_macro(field_value);

  hide_ui();
  return TRUE;
}

static gboolean
input_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data) {
  if (event->hardware_keycode == 9) // Escape
    hide_ui();
  return FALSE;
}


static gboolean delete_event( GtkWidget *widget, GdkEvent *event, gpointer data ) {
  hide_ui();
}

static void realize_mainwindow( GtkWidget *widget, gpointer data) {
  XSetWindowAttributes winattr;
  GdkScreen *gdk_screen;
  int grabstatus;

  xwin.gtk_display = GDK_WINDOW_XDISPLAY(widget->window);
  xwin.main_window = GDK_WINDOW_XID(widget->window);

  /* Tell window managers to not manage us */
  winattr.override_redirect = 1;
  XChangeWindowAttributes(xwin.gtk_display, xwin.main_window, CWOverrideRedirect, &winattr);
  /* Set width to 100% of the screen */
  gdk_screen = gtk_widget_get_screen(widget);
  xwin.width = gdk_screen_get_width(gdk_screen);
  gtk_window_set_default_size(GTK_WINDOW(widget), xwin.width / 2, -1);
}

static void configure_event_mainwindow( GtkWidget *widget, gpointer data ) {
  int grabstatus;
  grabstatus = XGrabKeyboard(xwin.gtk_display, xwin.main_window, FALSE, GrabModeAsync, GrabModeAsync, CurrentTime);
  if (grabstatus != GrabSuccess)
    printf("Failed to grab the display: %d\n", grabstatus);
}

static void destroy( GtkWidget *widget, gpointer   data ) {
  hide_ui();
}

int main( int argc, char **argv ) {
  char *display_name;

  if ((display_name = getenv("DISPLAY")) == (char *)NULL) {
    fprintf(stderr, "Error: DISPLAY variable not set\n");
    exit(1);
  }

  if ((xwin.x_display = XOpenDisplay(display_name)) == NULL) {
    fprintf(stderr, "Error: Can't open display: %s\n", display_name);
    exit(1);
  }

  gtk_init (&argc, &argv);
  main_loop();

  return 0;
}

static void ui_init() {
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
  g_signal_connect(G_OBJECT(ui.user_input), "activate", G_CALLBACK (input_activate), NULL);
  g_signal_connect_after(G_OBJECT(ui.user_input), "key-press-event", G_CALLBACK (input_key_press), NULL);
  
  ui.status_label = gtk_label_new("");
  static GdkColor label_bgcolor;
  gdk_color_parse("#DFDF60", &label_bgcolor);
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
}

void main_loop() {
  x_bindkey();
  ui.state = UI_INACTIVE;

  x_bindkey();

  while (1) {
    if (ui.state == UI_INACTIVE) {
      x_loop_iteration();
    } else if (ui.state == UI_ACTIVE) {
      gtk_loop_iteration();
    } else {
      fprintf(stderr, "Should not have gotten here. State == %d\n", ui.state);
    }
  }
}

static void x_loop_iteration() {
  XEvent e;

  XNextEvent(xwin.x_display, &e);
  switch (e.type) {
    case KeyPress:
      /* XGrabKey grabs the keyboard too, ungrab it */
      XUngrabKeyboard(xwin.x_display, CurrentTime);
      x_unbindkey();
      XFlush(xwin.x_display);
      ui.state = UI_ACTIVE;
      show_ui();
      break;
  }
}

static void gtk_loop_iteration() {
  gtk_main_iteration();
}

static void x_bindkey() {
  int root;
  int keycode;
  
  root = XDefaultRootWindow(xwin.x_display);
  keycode = XKeysymToKeycode(xwin.x_display, XStringToKeysym("apostrophe"));
  
  XGrabKey(xwin.x_display, keycode, ControlMask, root, False, GrabModeAsync, GrabModeAsync);
  XFlush(xwin.x_display);
}

static void x_unbindkey() {
  int root;
  int keycode;
  
  root = XDefaultRootWindow(xwin.x_display);
  keycode = XKeysymToKeycode(xwin.x_display, XStringToKeysym("apostrophe"));
  
  XUngrabKey(xwin.x_display, keycode, ControlMask, root);
  XFlush(xwin.x_display);
}
