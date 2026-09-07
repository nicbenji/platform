#include "base/base_inc.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/mman.h>

#include "function_loader/wl_client_loader.h"
#include "function_loader/wl_client_loader.c"

int main(int argc, char **argv) {

    const char *curr_window_manager = getenv("XDG_SESSION_TYPE");

    if (strcmp(curr_window_manager, "wayland") == 0) {
        void *wl_client_lib = dlopen("libwayland-client.so.0", RTLD_NOW);
        if (!wl_client_lib) {
            // TODO: diagnostic
            printf("Failed to load wl client\n");
            return -1;
        }

        Wl_Funcs wl = {0};
        if (!wl_load_funcs(wl_client_lib, &wl)) {
            // TODO: diagnostic
            printf("Failed to load wl client functions\n");
            return -1;
        }

    } else if (strcmp(curr_window_manager, "x11") == 0) {


    } else {
        // TODO: diagnostic
        return 69;
    }

    return 0;
}
