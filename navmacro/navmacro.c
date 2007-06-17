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

int main( int argc, char **argv ) {
  GtkWidget *window;
  
  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  
  g_signal_connect(G_OBJECT (window), "delete_event",
      G_CALLBACK (delete_event), NULL);
  g_signal_connect (G_OBJECT (window), "destroy",
        G_CALLBACK (destroy), NULL);
  
  gtk_window_set_type_hint(GTK_WINDOW(window), GDK_WINDOW_TYPE_HINT_UTILITY);
  gtk_widget_show(window);

  gtk_main ();
  return 0;
}
