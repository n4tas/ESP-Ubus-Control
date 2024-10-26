#pragma once
#include <blob.h>
#include <libubox/blobmsg.h>
#include <libubox/blobmsg_json.h>
#include <libubus.h>
#include <syslog.h>
#include <libserialport.h>
#define TIMEOUT 3000
#define ESP_VID 0x10C4
#define ESP_PID 0xEA60
#define MAX_PORTS 20
#define STRING_SIZE 256
#include "ubus.h"

enum system {
    PORT,
    PIN,
    __DEVICE_MAX
};
enum get {
    PORT_GET,
    PIN_GET,
    SENSOR,
    MODEL,
    __GET_MAX
};
/**
 * Get ports' list.
 * @param port_list port list.
 */
int ports_list(struct sp_port ***list);

/**
 * Wrapper function for esp_devices(). Checks if a USB device.
 * is an ESP8286 device.
 * @param b blob_buf buffer.
 */
int get_devices(char (*port_array)[20]);

/**
 * Based on the action_string, sends it to the ESP8286 device.
 * @param port A pointer to a struct sp_port, which will refer to the port found.
 * @param pin Pin for digital data.
 * @param action_string A string which is sent to ESP8286 device.
 */
int command_write(struct sp_port *port, char *action_string);

/**
 * Reads received data from ESP8286 device, stores in read_string.
 * @param port A pointer to a struct sp_port, which will refer to the port found.
 * @param pin Pin for digital data.
 * @param action_string A string which is sent to ESP8286 device.
 */
int command_read(struct sp_port *port, char *read_string, int size);

/**
 * Opens a port.
 * @param port_name port name
 * @param variable in which we store the port, received by sp_get_port_by_name().
 */
int open_port(struct ubus_context* ctx, char *port_name, struct sp_port **port, struct ubus_request_data *req, struct blob_buf b);

/**
 * Closes and frees the given port.
 * @param port given port.
 */
void close_port(struct sp_port *port);

/**
 * Returns true if a given port is an ESP device.
 * @param port given port
 */
bool is_tty(struct sp_port *port);

/**
 * Initializes UBUS context, adds an object, and starts the event loop; returns -1 on failure.
 */
int run();
