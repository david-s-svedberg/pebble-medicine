#include <pebble.h>
#include <math.h>

#include "app.h"

int main()
{
    init();
    app_event_loop();
    deinit();
}