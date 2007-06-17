/*
 * Pointer navigation macros 
 */

#include <gtk/gtk.h>
#include <gdk/gdkx.h>

static gboolean delete_event( GtkWidget *widget,
                              GdkEvent  *event,
                              gpointer   data ) {
    return FALSE; /* Call destroy on window closure (deletion) */
}

static void destroy( GtkWidget *widget, gpointer   data ) {
  gtk_main_quit ();
}

static void hello( GtkWidget *widget, gpointer   data ) {
  //g_print("foo: %d\n", GDK_IS_WINDOW(GTK_WINDOW(GTK_WIDGET(widget)->parent)->frame));
  g_print("foo: %d\n", GDK_WINDOW_XID(widget->window));
}

int main( int argc, char **argv ) {
  GtkWidget *window;
  GtkWidget *button;
  
  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  
  g_signal_connect(G_OBJECT (window), "delete_event",
      G_CALLBACK (delete_event), NULL);
  g_signal_connect (G_OBJECT (window), "destroy",
        G_CALLBACK (destroy), NULL);
  
  gtk_container_set_border_width (GTK_CONTAINER (window), 5);
  //gtk_window_set_type_hint(GTK_WINDOW(window), GDK_WINDOW_TYPE_HINT_UTILITY);

  button = gtk_button_new_with_label("foo bar");
  g_signal_connect(G_OBJECT (button), "clicked", G_CALLBACK (hello), NULL);
  gtk_container_add (GTK_CONTAINER (window), button);
  gtk_widget_show(button);
  gtk_widget_show(window);

  gtk_main();
  return 0;
}
