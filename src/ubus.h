#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <blob.h>
#include <libubox/blobmsg.h>
#include <libubox/blobmsg_json.h>
#include <libubus.h>
#include <syslog.h>
#include "serial.h"

/**
 * Establishes ubus context connection.
 */
struct ubus_context* ubus_ctx();

/**
 * Wrapper function for get_devices(). Opens a table of ``struct sp_port`` JSON.
 */
int devices_JSON(struct blob_buf b);

/**
 * Parses ESP System Policy (PORT, PIN). Afterwards, data is transmitted to esp_on() and esp_off().
 */
int parse_system(struct ubus_context *ctx, struct blob_attr *msg, char *port_name, int *pin);

/**
 * Parses ESP Get Policy (PORT, PIN, SENSOR, MODEL). Afterwards, data is transmitted to esp_get().
 */
int parse_get(struct ubus_context *ctx, struct blob_attr *msg, char *port_name, int *pin, char *sensor, char *model);

/**
 * Reads and writes from and to ESP device.
 */
int read_write_ubus(struct ubus_context *ctx, struct ubus_request_data *req, struct sp_port *port, int pin, char *action_string, struct blob_buf b);

/**
 * Wrapper function for run(). Adds esp_object.
 */
int wrapper_esp_object(struct ubus_context* ctx);

/**
 * Runs an uloop.
 */
int run(struct ubus_context* ctx);
/**
 * Converts an integer decimal to string hexadecimal (buffer).
 */
void str_to_hex(int decimal, char *buffer);
