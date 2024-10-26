#include "serial.h"

int ports_list(struct sp_port ***list)
{
    int result = sp_list_ports(list);
    if (result != SP_OK || *list == NULL) {
        if (*list != NULL) {
            sp_free_port_list(*list);
        }
        return -1;
    }
    return 0;
}

bool is_tty(struct sp_port *port)
{   
    int vid, pid;
    sp_get_port_usb_vid_pid(port, &vid, &pid);
    enum sp_transport transport = sp_get_port_transport(port);
    sp_get_port_usb_vid_pid(port, &vid, &pid);
    if (transport == SP_TRANSPORT_USB && vid == ESP_VID && 
            pid == ESP_PID) {
                return true;
            }
    return false;
}

int get_devices(char (*port_array)[20])
{   
    struct sp_port **list = NULL;
    if (ports_list(&list) != 0)
        return -1;
    int j = 0;
    for (int i = 0; list[i] != NULL; i++) {
        struct sp_port *port = list[i];
        if (is_tty(port) == true) {
            strcpy(port_array[j], sp_get_port_name(port));
            j++;
        }
    }
    sp_free_port_list(list);
    return 0;
}

int open_port(struct ubus_context* ctx, char *port_name, struct sp_port **port, struct ubus_request_data *req, struct blob_buf buf)
{   
    if (sp_get_port_by_name(port_name, port) != 0){
        blobmsg_add_string(&buf, "response", "Port does not exist.");
        ubus_send_reply(ctx, req, buf.head);
        return UBUS_STATUS_INVALID_ARGUMENT;
    }
    if (is_tty(*port) == false){
        blobmsg_add_string(&buf, "response", "Not an ESP device.");
        ubus_send_reply(ctx, req, buf.head);
        sp_free_port(*port);
        return UBUS_STATUS_INVALID_ARGUMENT;
    }
    if (sp_open(*port, SP_MODE_READ_WRITE) != SP_OK) {
        blobmsg_add_string(&buf, "response", "Could not open port.");
        ubus_send_reply(ctx, req, buf.head);
        sp_free_port(*port);
        return UBUS_STATUS_UNKNOWN_ERROR;
    }
    return 0;
}

void close_port(struct sp_port *port)
{
    sp_close(port);
    sp_free_port(port);
}

int command_write(struct sp_port *port, char *action_string)
{      
    char filepath[STRING_SIZE];
    snprintf(filepath, sizeof(filepath), "%s", sp_get_port_name(port));
    if (sp_blocking_write(port, action_string, strlen(action_string)+1, 1000) == -1){
        close_port(port);                   
        return -1;
    }
    return 0;
}

int command_read(struct sp_port *port, char *read_string, int size)
{
    char filepath[STRING_SIZE];
    snprintf(filepath, sizeof(filepath), "%s", sp_get_port_name(port));
    if (sp_blocking_read(port, read_string, size-1, TIMEOUT) == -1){ 
        close_port(port);
        return -1;
    }
    read_string[strcspn(read_string, "\n")] = '\0';
    read_string[strcspn(read_string, "\r")] = '\0'; 
    return 0;
}
