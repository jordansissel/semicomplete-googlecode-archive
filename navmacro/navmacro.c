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

static struct xdpyinfo {
  Display *display;
  int width;
} xwin;

static GdkPixmap *pixmap_top;
static GdkPixmap *pixmap_bottom;

static gboolean
input_changed_event(GtkWidget *widget, gpointer userdata) {
  g_print("input\n");
  return TRUE;
}

static gboolean
input_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data) {
  g_print("key %d\n", event->hardware_keycode);
  // keycode 9 == Escape
}

static gboolean
configure_event_border_top( GtkWidget *widget, GdkEventConfigure *event ) {
  if (pixmap_top)
    g_object_unref(pixmap_top);

  pixmap_top = gdk_pixmap_new(widget->window, widget->allocation.width,
                          BORDER_SIZE, -1);
  gdk_draw_rectangle(pixmap_top,
          widget->style->black_gc,
          TRUE,
          0, 0,
          widget->allocation.width,
          BORDER_SIZE);

  gdk_draw_line(pixmap_top,
          widget->style->white_gc,
          0, 0,
          widget->allocation.width,
          0);

  return TRUE;
}

static gboolean
configure_event_border_bottom( GtkWidget *widget, GdkEventConfigure *event ) {
  if (pixmap_bottom)
    g_object_unref(pixmap_bottom);

  pixmap_bottom = gdk_pixmap_new(widget->window, widget->allocation.width,
                          BORDER_SIZE, -1);
  gdk_draw_rectangle(pixmap_bottom,
          widget->style->black_gc,
          TRUE,
          0, 0,
          widget->allocation.width,
          BORDER_SIZE);

  gdk_draw_line(pixmap_bottom,
          widget->style->white_gc,
          0, BORDER_SIZE - 1,
          widget->allocation.width, BORDER_SIZE - 1);


  return TRUE;
}

static gboolean
expose_event_border_top( GtkWidget *widget, GdkEventExpose *event )
{
  gdk_draw_drawable(widget->window,
        widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
        pixmap_top,
        event->area.x, event->area.y,
        event->area.x, event->area.y,
        event->area.width, event->area.height);

  return FALSE;
}

static gboolean
expose_event_border_bottom( GtkWidget *widget, GdkEventExpose *event )
{
  gdk_draw_drawable(widget->window,
        widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
        pixmap_bottom,
        event->area.x, event->area.y,
        event->area.x, event->area.y,
        event->area.width, event->area.height);

  return FALSE;
}

static gboolean delete_event( GtkWidget *widget, GdkEvent *event, gpointer data ) {
    return FALSE; /* Call destroy on window closure (deletion) */
}

static void activate( GtkWidget *widget, gpointer   data ) {
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
  GtkWidget *window;
  GtkWidget *input;
  GtkWidget *label;
  GtkWidget *draw_top;
  GtkWidget *draw_bottom;
  GtkWidget *vbox;
  
  gtk_init (&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER_ALWAYS);
  g_signal_connect(G_OBJECT (window), "delete_event",
                   G_CALLBACK (delete_event), NULL);
  g_signal_connect(G_OBJECT (window), "destroy",
                   G_CALLBACK (destroy), NULL);
  g_signal_connect(G_OBJECT(window), "realize",
                   G_CALLBACK(activate), NULL);
  g_signal_connect(G_OBJECT(window), "configure_event",
                   G_CALLBACK(configure_event_mainwindow), NULL);

  vbox = gtk_vbox_new(FALSE, 0);
  
  input = gtk_entry_new();
  g_signal_connect(G_OBJECT(input), "changed", G_CALLBACK (input_changed_event), NULL);
  g_signal_connect_after(G_OBJECT(input), "key-press-event", G_CALLBACK (input_key_press), NULL);
  gtk_widget_set_size_request(input, -1, 1);
  
  label = gtk_label_new("");
  gtk_label_set_markup(GTK_LABEL(label), "foo bar <b>baz</b>");

  draw_top = gtk_drawing_area_new();
  draw_bottom = gtk_drawing_area_new();
  gtk_widget_set_size_request(draw_top, -1, BORDER_SIZE);
  gtk_widget_set_size_request(draw_bottom, -1, BORDER_SIZE);

  g_signal_connect(G_OBJECT(draw_top), "configure_event",  
                   G_CALLBACK(configure_event_border_top), NULL);
  g_signal_connect(G_OBJECT(draw_bottom), "configure_event",  
                   G_CALLBACK(configure_event_border_bottom), NULL);
  g_signal_connect(G_OBJECT(draw_top), "expose_event",  
                   G_CALLBACK(expose_event_border_top), NULL);
  g_signal_connect(G_OBJECT(draw_bottom), "expose_event",  
                   G_CALLBACK(expose_event_border_bottom), NULL);

  gtk_container_add(GTK_CONTAINER(vbox), draw_top);
  gtk_container_add(GTK_CONTAINER(vbox), input);
  gtk_container_add(GTK_CONTAINER(vbox), label);
  gtk_container_add(GTK_CONTAINER(vbox), draw_bottom);

  gtk_container_add(GTK_CONTAINER(window), vbox);

  gtk_widget_show_all(window);
  gtk_widget_grab_focus(GTK_WIDGET(input));

  gtk_main();
  return 0;
}
