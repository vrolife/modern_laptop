// SPDX-License-Identifier: GPL-2.0
/* WMI driver for Redmi Laptops */

#include <linux/acpi.h>
#include <linux/input.h>
#include <linux/module.h>
#include <linux/wmi.h>

#include <uapi/linux/input-event-codes.h>

struct redmibook_wmi_data {
    struct input_dev *input_dev;
};

static bool dump_event_data;
module_param(dump_event_data, bool, 0644);
MODULE_PARM_DESC(dump_event_data, "Dump event data");

static int redmibook_wmi_probe(struct wmi_device *wdev, const void *context)
{
    struct redmibook_wmi_data *data = NULL;
    if (wdev == NULL)
        return -EINVAL;

    data = devm_kzalloc(&wdev->dev, sizeof(struct redmibook_wmi_data), GFP_KERNEL);
    if (data == NULL)
        return -ENOMEM;

    dev_set_drvdata(&wdev->dev, data);

    data->input_dev = devm_input_allocate_device(&wdev->dev);
    data->input_dev->name = "Redmi WMI keys";
    data->input_dev->phys = "wmi/input0";
    set_bit(EV_KEY, data->input_dev->evbit);
    set_bit(EV_MSC, data->input_dev->evbit);
    set_bit(MSC_RAW, data->input_dev->mscbit);
    set_bit(KEY_FN_ESC, data->input_dev->keybit);
    set_bit(KEY_PROG1, data->input_dev->keybit);
    set_bit(KEY_PROG2, data->input_dev->keybit);
    set_bit(KEY_PROG3, data->input_dev->keybit);
    set_bit(KEY_PROG4, data->input_dev->keybit);
    set_bit(KEY_FN_F10, data->input_dev->keybit);
    set_bit(KEY_MEDIA, data->input_dev->keybit);
    return input_register_device(data->input_dev);
}

static void redmibook_wmi_notify(struct wmi_device *wdev, union acpi_object *obj)
{
    struct redmibook_wmi_data *data = NULL;
    int i = 0;
    u8 magic = 0;
    u8 code = 0;
    u8 value = 0;

    if (wdev == NULL) 
        return;

    data = dev_get_drvdata(&wdev->dev);
    if (data == NULL)
        return;
    
    if (dump_event_data) {
        pr_info("DEBUG: event data: ");
        switch (obj->type) {
        case ACPI_TYPE_BUFFER:
            pr_cont("buffer: ");
            for (i = 0; i < obj->buffer.length; ++i) {
                pr_cont("%02X ", obj->buffer.pointer[i]);
            }
            break;
        default:
            pr_cont("type: %d", obj->type);
            break;
        }
    }

    if (obj->type != ACPI_TYPE_BUFFER) {
        pr_warn("got unexpected object type");
        return;
    }

    // Name (EVBU, Buffer (0x20)
    if (obj->buffer.length != 0x20) {
        pr_warn("object buffer size mismatch");
        return;
    }

    magic = obj->buffer.pointer[0];
    code = obj->buffer.pointer[1];
    value = obj->buffer.pointer[2];

    if (magic != 0x01) {
        pr_warn("invalid magic");
        return;
    }

    switch (code) {
    case 0x07: // Fn + ESC
        input_event(data->input_dev, EV_MSC, MSC_RAW, value);
        input_report_key(data->input_dev, KEY_FN_ESC, 1);
        input_sync(data->input_dev);
        input_event(data->input_dev, EV_MSC, MSC_RAW, value);
        input_report_key(data->input_dev, KEY_FN_ESC, 0);
        input_sync(data->input_dev);
        break;
    case 0x01: // Fn + F6
        input_report_key(data->input_dev, KEY_PROG1, 1);
        input_sync(data->input_dev);
        input_report_key(data->input_dev, KEY_PROG1, 0);
        input_sync(data->input_dev);
        break;
    case 0x02: // Fn + F7
        input_report_key(data->input_dev, KEY_PROG2, 1);
        input_sync(data->input_dev);
        input_report_key(data->input_dev, KEY_PROG2, 0);
        input_sync(data->input_dev);
        break;
    case 0x03: // Fn + F8
        input_report_key(data->input_dev, KEY_PROG3, 1);
        input_sync(data->input_dev);
        input_report_key(data->input_dev, KEY_PROG3, 0);
        input_sync(data->input_dev);
        break;
    case 0x1B: // Fn + F9
        input_report_key(data->input_dev, KEY_PROG4, 1);
        input_sync(data->input_dev);
        input_report_key(data->input_dev, KEY_PROG4, 0);
        input_sync(data->input_dev);
        break;
    case 0x05: // Fn + F10 Keyboard Backlight
        input_event(data->input_dev, EV_MSC, MSC_RAW, value);
        input_report_key(data->input_dev, KEY_FN_F10, 1);
        input_sync(data->input_dev);
        input_event(data->input_dev, EV_MSC, MSC_RAW, value);
        input_report_key(data->input_dev, KEY_FN_F10, 0);
        input_sync(data->input_dev);
        break;    
    case 0x18: // <Xiao Ai> Down
        input_report_key(data->input_dev, KEY_MEDIA, 1);
        input_sync(data->input_dev);
        break;
    case 0x19: // <Xiao Ai> Up
        input_report_key(data->input_dev, KEY_MEDIA, 0);
        input_sync(data->input_dev);
        break;
    }
}

static void redmibook_wmi_remove(struct wmi_device *wdev)
{
    struct redmibook_wmi_data *data = NULL;

    if (wdev == NULL) 
        return;

    data = dev_get_drvdata(&wdev->dev);
    if (data == NULL)
        return;
    
    if (data->input_dev != NULL) {
        input_unregister_device(data->input_dev);
        data->input_dev = NULL;
    }

    dev_set_drvdata(&wdev->dev, NULL);
}

static const struct wmi_device_id redmibook_wmi_id_table[] = {
    { "46C93E13-EE9B-4262-8488-563BCA757FEF", NULL },
    { }
};

static struct wmi_driver redmibook_wmi_driver = {
    .driver = {
        .name = "redmibook-wmi"
    },
    .id_table = redmibook_wmi_id_table,
    .probe = redmibook_wmi_probe,
    .notify = redmibook_wmi_notify,
    .remove = redmibook_wmi_remove
};
module_wmi_driver(redmibook_wmi_driver);

MODULE_DEVICE_TABLE(wmi, redmibook_wmi_id_table);
MODULE_AUTHOR("Pom");
MODULE_DESCRIPTION("Redmibook WMI driver");
MODULE_LICENSE("GPL v2");
