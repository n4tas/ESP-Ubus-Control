#include "serial.h"
#include "ubus.h"


int main(int argc, char **argv) {
    uloop_init();
    struct ubus_context *ctx = NULL;
    ctx = ubus_ctx();
    if (ctx == NULL) {
        syslog(LOG_ERR, "Failed to connect to ubus [UBUS_CTX]\n\n");
        return -1;            
    }
    if (run(ctx) != 0)
        syslog(LOG_ERR, "Could not add ubus object\n\n");
    ubus_free(ctx);
    uloop_done();
    return 0;
}