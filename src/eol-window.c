/*
    eol-window.c
    Description: This file contains the implementation of the main window of the image viewer.
   
    In this window, images are uploaded and displayed, and user interactions are managed.
    Navigation controls, zoom and other essential functionalities are also defined.
    Take advantage of GTK+ 4 features to provide a fluid and intuitive user experience.

    Contributions and improvements are welcome. Feel free to explore the code and participate in the development.
*/


#include <gtk/gtk.h>

GdkPixbuf *global_pixbuf = NULL;

// Function declarations
static void on_open_button_clicked(GtkButton *button, gpointer user_data);
static gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data);

int main(int argc, char *argv[]) {
    // Initialize GTK+
    gtk_init(&argc, &argv);

    // Create the main window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Image Viewer");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a vertical box
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), box);

    // "Open Image" button
    GtkWidget *open_button = gtk_button_new_with_label("Open Image");
    g_signal_connect(G_OBJECT(open_button), "clicked", G_CALLBACK(on_open_button_clicked), box);
    gtk_box_pack_start(GTK_BOX(box), open_button, FALSE, FALSE, 0);

    // Custom drawing area with Cairo
    GtkWidget *drawing_area = gtk_drawing_area_new();
    g_signal_connect(G_OBJECT(drawing_area), "draw", G_CALLBACK(on_draw), NULL);
    gtk_widget_set_size_request(drawing_area, 800, 500);
    gtk_box_pack_start(GTK_BOX(box), drawing_area, TRUE, TRUE, 0);

    // Show everything and run the GTK+ main loop
    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

// Callback for "Open Image" button
static void on_open_button_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *box = GTK_WIDGET(user_data);

    // File selection dialog
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Open Image",
                                                     GTK_WINDOW(gtk_widget_get_toplevel(box)),
                                                     GTK_FILE_CHOOSER_ACTION_OPEN,
                                                     "_Cancel", GTK_RESPONSE_CANCEL,
                                                     "_Open", GTK_RESPONSE_ACCEPT,
                                                     NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        // Get selected file path
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        // Load image with GdkPixbuf
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(filename, NULL);

        if (pixbuf != NULL) {
            // Release previous GdkPixbuf if exists
            if (global_pixbuf != NULL) {
                g_object_unref(global_pixbuf);
            }
            
            // Assign the new GdkPixbuf to the global variable
            global_pixbuf = pixbuf;

            // Redraw the drawing area
            GtkWidget *drawing_area = gtk_box_get_children(GTK_BOX(box))->data;
            gtk_widget_queue_draw(drawing_area);
        } else {
            g_print("Error loading the image.\n");
        }

        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

// Callback for drawing on the drawing area
static gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    // Get the size of the drawing area
    gint width, height;
    gtk_widget_get_size_request(widget, &width, &height);

    // Draw the loaded image (this is just a basic example)
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0); // Set background color
    cairo_paint(cr);

    if (global_pixbuf != NULL) {
        // Scale the pixbuf to fit the drawing area
        GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(global_pixbuf, width, height, GDK_INTERP_BILINEAR);

        // Draw the scaled pixbuf on the drawing area using Cairo
        gdk_cairo_set_source_pixbuf(cr, scaled_pixbuf, 0, 0);
        cairo_paint(cr);

        g_object_unref(scaled_pixbuf); // Release the scaled pixbuf
    }

    return FALSE;
}

