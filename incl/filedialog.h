#pragma once
#include <gtk/gtk.h>
#include <string>

static void WaitForCleanup(void) {
    while (gtk_events_pending())
        gtk_main_iteration();
}

bool SaveFileDialog(std::string& filePath) {
    GtkWidget*           dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
    gint                 res;

    dialog = gtk_file_chooser_dialog_new("Save File",
                                         NULL,
                                         action,
                                         ("_Cancel"),
                                         GTK_RESPONSE_CANCEL,
                                         ("_Save"),
                                         GTK_RESPONSE_ACCEPT,
                                         NULL);

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char*           filename;
        GtkFileChooser* chooser = GTK_FILE_CHOOSER(dialog);
        filename                = gtk_file_chooser_get_filename(chooser);
        filePath                = filename;
        g_free(filename);
        WaitForCleanup();
        gtk_widget_destroy(dialog);
        WaitForCleanup();
        return true;
    } else {
        WaitForCleanup();
        gtk_widget_destroy(dialog);
        WaitForCleanup();
        return false;
    }
}

bool LoadFileDialog(std::string& filePath) {
    if (!gtk_init_check(NULL, NULL)) {
        // GTK initialization failed
        g_printerr("Failed to initialize GTK\n");
        return false;
    }

    GtkWidget*           dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint                 res;

    dialog = gtk_file_chooser_dialog_new("Open File",
                                         NULL,
                                         action,
                                         "_Cancel",
                                         GTK_RESPONSE_CANCEL,
                                         "_Open",
                                         GTK_RESPONSE_ACCEPT,
                                         NULL);

    if (dialog == NULL) {
        // Failed to create dialog
        g_printerr("Failed to create file chooser dialog\n");
        return false;
    }

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char*           filename;
        GtkFileChooser* chooser = GTK_FILE_CHOOSER(dialog);
        filename                = gtk_file_chooser_get_filename(chooser);
        filePath                = filename;
        g_free(filename);
        WaitForCleanup();
        gtk_widget_destroy(dialog);    // Destroy the dialog after selecting a file
        WaitForCleanup();
        return true;
    } else {
        WaitForCleanup();
        gtk_widget_destroy(dialog);
        WaitForCleanup();
        return false;
    }
}
