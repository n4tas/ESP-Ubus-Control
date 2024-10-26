#include "ubus.h"

static const struct blobmsg_policy esp_system_policy[] = {
	[PORT] = { .name = "port", .type = BLOBMSG_TYPE_STRING },
    [PIN] = { .name = "pin", .type = BLOBMSG_TYPE_INT32 }
};

static const struct blobmsg_policy esp_get_policy[] = {
    [PORT_GET] = { .name = "port", .type = BLOBMSG_TYPE_STRING },
    [PIN_GET] = { .name = "pin", .type = BLOBMSG_TYPE_INT32 },
    [SENSOR] = { .name = "sensor", .type = BLOBMSG_TYPE_STRING },
    [MODEL] = { .name = "model", .type = BLOBMSG_TYPE_STRING },
};

static int esp_devices(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg);

static int esp_on(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg);

static int esp_off(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg);

static int esp_get(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg);
 
static const struct ubus_method esp_methods[] = {
	UBUS_METHOD_NOARG("devices", esp_devices),
    UBUS_METHOD("on", esp_on, esp_system_policy),
    UBUS_METHOD("off", esp_off, esp_system_policy),
    UBUS_METHOD("get", esp_get, esp_get_policy)
};

static struct ubus_object_type esp_object_type = 
    UBUS_OBJECT_TYPE("esp", esp_methods);

static struct ubus_object esp_object = {
	.name = "esp",
	.type = &esp_object_type,
	.methods = esp_methods,
	.n_methods = ARRAY_SIZE(esp_methods),
};                  

struct ubus_context* ubus_ctx()
{   
    struct ubus_context* ctx = ubus_connect(NULL);
	if (!ctx) {
		syslog(LOG_ERR, "Ubus failed to connect\n");
        return NULL;
	}
    return ctx;
}

static int esp_devices(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{   
    struct blob_buf b = {};
    blob_buf_init(&b, 0);
    if (devices_JSON(b) != 0){
        ubus_send_reply(ctx, req, b.head);
        blob_buf_free(&b);
        return -1;
    }
    ubus_send_reply(ctx, req, b.head);
    blob_buf_free(&b);
    return 0;
}

void str_to_hex(int decimal, char *buffer)
{   
    sprintf(buffer, "0x%X", decimal);
}

int devices_JSON(struct blob_buf b)
{   
    char port_array[20][20] = {};
    if (get_devices(port_array) != 0)
        return -1;
    int vid = 0;
    int pid = 0;
    void *array = blobmsg_open_array(&b, "devices");
    for (int i = 0; port_array[i][0] != '\0'; i++){
        struct sp_port* port;
        char buffer[10] = "";
        sp_get_port_by_name(port_array[i], &port);
        sp_get_port_usb_vid_pid(port, &vid, &pid);
        void *table = blobmsg_open_table(&b, NULL);
        blobmsg_add_string(&b, "port_name", port_array[i]);
        str_to_hex(vid, buffer);
        blobmsg_add_string(&b, "vid", buffer);
        str_to_hex(pid, buffer);
        blobmsg_add_string(&b, "pid", buffer);
        blobmsg_close_table(&b, table);
        sp_free_port(port);
        vid = 0;
        pid = 0;
    }
    blobmsg_close_array(&b, array);
    return 0;
}

int read_write_ubus(struct ubus_context *ctx, struct ubus_request_data *req, struct sp_port *port, int pin, char *action_string, struct blob_buf b)
{   
    char buffer[512] = "";
    if (command_write(port, action_string) != 0)
        return UBUS_STATUS_INVALID_COMMAND;
    if (command_read(port, buffer, sizeof(buffer)) != 0){
        return UBUS_STATUS_UNKNOWN_ERROR;
    }
    blobmsg_add_string(&b, "response", buffer);
    ubus_send_reply(ctx, req, b.head);
    return 0;
}

static int esp_on(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
    struct sp_port *port = NULL;
    struct blob_buf b = {};
    blob_buf_init(&b, 0);
    char action_string[40];
    char port_name[50];
    int pin = 0;
    if (parse_system(ctx, msg, port_name, &pin) != 0)
        return UBUS_STATUS_INVALID_ARGUMENT;
    if (open_port(ctx, port_name, &port, req, b) != 0){
        blob_buf_free(&b);
        return -1;
    }
    snprintf(action_string, sizeof(action_string), "{\"action\": \"on\", \"pin\": %d}", pin);
    if (read_write_ubus(ctx, req, port, pin, action_string, b) != 0){
        blob_buf_free(&b);
        close_port(port);
        return -1;
    }
    blob_buf_free(&b);
    close_port(port);
    return 0;
}

static int esp_off(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{   
    struct sp_port *port = NULL;
    struct blob_buf b = {};
    blob_buf_init(&b, 0);
    char action_string[40];
    char port_name[50];
    int pin = 0;
    if (parse_system(ctx, msg, port_name, &pin) != 0)
        return UBUS_STATUS_INVALID_ARGUMENT;
    if (open_port(ctx, port_name, &port, req, b) != 0)
        return -1;
    snprintf(action_string, sizeof(action_string), "{\"action\": \"off\", \"pin\": %d}", pin);
    if (read_write_ubus(ctx, req, port, pin, action_string, b) != 0){
        blob_buf_free(&b);
        close_port(port);
        return -1;
    }
    blob_buf_free(&b);
    close_port(port);
    return 0;
}

static int esp_get(struct ubus_context *ctx, struct ubus_object *obj,
                   struct ubus_request_data *req, const char *method,
                   struct blob_attr *msg) {
    struct sp_port *port;
    char action_string[150];
    char port_name[20];
    int pin = 0;
    char model[15];
    char sensor[15];
    struct blob_buf b = {};
    blob_buf_init(&b, 0);
    if (parse_get(ctx, msg, port_name, &pin, sensor, model) != 0)
        return UBUS_STATUS_INVALID_ARGUMENT;
    if (open_port(ctx, port_name, &port, req, b) != 0)
        return -1;
    snprintf(action_string, sizeof(action_string), 
             "{\"action\": \"get\", \"port\": \"%s\", \"sensor\": \"%s\", \"pin\": %d, \"model\": \"%s\"}", port_name, sensor, pin, model);
    if (read_write_ubus(ctx, req, port, pin, action_string, b) != 0){
        blob_buf_free(&b);
        close_port(port);
        return -1;
    }
    blob_buf_free(&b);
    close_port(port);
    return 0;
}

int parse_system(struct ubus_context *ctx, struct blob_attr *msg, char *port_name, int *pin)
{
    struct blob_attr *tb[__DEVICE_MAX];
    blobmsg_parse(esp_system_policy, __DEVICE_MAX, tb, blob_data(msg), blob_len(msg));
    if (!tb[PORT] || !tb[PIN])
        return UBUS_STATUS_INVALID_ARGUMENT;
    strcpy(port_name, blobmsg_get_string(tb[PORT]));
    *pin = blobmsg_get_u32(tb[PIN]);
    return 0;
}

int parse_get(struct ubus_context *ctx, struct blob_attr *msg, char *port_name, int *pin, char *sensor, char *model)
{
    struct blob_attr *tb[__GET_MAX];
    blobmsg_parse(esp_get_policy, __GET_MAX, tb, blob_data(msg), blob_len(msg));
    if (!tb[PORT_GET] || !tb[PIN_GET] || !tb[SENSOR] || !tb[MODEL]) 
        return UBUS_STATUS_INVALID_ARGUMENT;
    strcpy(port_name, blobmsg_get_string(tb[PORT_GET]));
    *pin = blobmsg_get_u32(tb[PIN_GET]);
    strcpy(model, blobmsg_get_string(tb[MODEL]));
    strcpy(sensor, blobmsg_get_string(tb[SENSOR]));
    return 0;
}

int wrapper_esp_object(struct ubus_context* ctx)
{
    if (ubus_add_object(ctx, &esp_object) != 0)
        return UBUS_STATUS_CONNECTION_FAILED;
    return 0;
}

int run(struct ubus_context *ctx)
{
    ubus_add_uloop(ctx);
    if (wrapper_esp_object(ctx) != 0)
        return -1;
    uloop_run();
    return 0;
}
