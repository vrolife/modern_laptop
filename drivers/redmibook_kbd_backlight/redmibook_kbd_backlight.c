// SPDX-License-Identifier: GPL-2.0
/* EC driver for Redmi Laptops */

#include <linux/acpi.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/suspend.h>
#include <linux/dmi.h>
#include <linux/leds.h>
#include <linux/version.h>

static acpi_handle handle_EC0;
static acpi_handle handle_ECRD;
static acpi_handle handle_ECWT;
static acpi_handle handle_KBBL;

static acpi_status ecram_read(acpi_handle handle, u64 *pval)
{
    struct acpi_object_list params;
    union acpi_object args[1];
    union acpi_object result;
    struct acpi_buffer result_buffer;
    acpi_status status;
    
    params.count = 1;
    params.pointer = &args[0];
    
    args[0].type = ACPI_TYPE_LOCAL_REFERENCE;
    args[0].reference.actual_type = ACPI_TYPE_INTEGER;
    args[0].reference.handle = handle;

    result_buffer.length = sizeof(result);
    result_buffer.pointer = &result;

    status = acpi_evaluate_object(handle_ECRD, NULL, &params, &result_buffer);
    if (!ACPI_SUCCESS(status)) {
        pr_err("evaluate ECRD failed. %s", acpi_format_exception(status));
        return status;
    }

    if (result.type != ACPI_TYPE_INTEGER) {
        pr_err("unexpected result type");
        return AE_TYPE;
    }

    *pval = result.integer.value;

    return status;
}

static acpi_status ecram_write(acpi_handle handle, u64 val)
{
    struct acpi_object_list params;
    union acpi_object args[2];
    acpi_status status;
    
    params.count = 2;
    params.pointer = &args[0];

    args[0].type = ACPI_TYPE_INTEGER;
    args[0].integer.value = val;
    
    args[1].type = ACPI_TYPE_LOCAL_REFERENCE;
    args[1].reference.actual_type = ACPI_TYPE_INTEGER;
    args[1].reference.handle = handle;

    status = acpi_evaluate_object(handle_ECWT, NULL, &params, NULL);
    if (!ACPI_SUCCESS(status)) {
        pr_err("evaluate ECWT failed. %s", acpi_format_exception(status));
    }
    return status;
}

// Module

static enum led_brightness kbbl_to_brightness(u64 level)
{
    switch(level) {
        case 0x08:
            return (enum led_brightness)0;
        case 0x04:
            return (enum led_brightness)3;
        case 0x02:
            return (enum led_brightness)1;
        case 0x01:
            return (enum led_brightness)2;
    }

    return LED_OFF;
}

static unsigned int brightness_to_kbbl(enum led_brightness brightness)
{
    switch((unsigned int)brightness) {
        case 0:
            return 0x08;
        case 3:
            return 0x04;
        case 1:
            return 0x02;
        case 2:
            return 0x01;
    }

    return 0x08;
}

static void kbbl_brightness_set(struct led_classdev* led, enum led_brightness brightness)
{
    ecram_write(handle_KBBL, brightness_to_kbbl(brightness));
}

enum led_brightness kbbl_brightness_get(struct led_classdev *led_cdev)
{
    u64 val;
    acpi_status status;
    status = ecram_read(handle_KBBL, &val);
    if (!ACPI_SUCCESS(status)) {
        return LED_OFF;
    }
    return (enum led_brightness)kbbl_to_brightness(val);
}

static struct led_classdev redmibook_kbd_backlight = {
    .name = "redmibook::kbd_backlight",
    .brightness = 0,
    .max_brightness = 3,
    .brightness_set = kbbl_brightness_set,
    .brightness_get = kbbl_brightness_get
};

static u64 saved_KBBL;

static int pm_notifier_call(struct notifier_block* bk, unsigned long action, void* data)
{
    if (action == PM_SUSPEND_PREPARE) {
        ecram_read(handle_KBBL, &saved_KBBL);
        ecram_write(handle_KBBL, 0x08);

    } else if (action == PM_POST_SUSPEND) {
        ecram_write(handle_KBBL, saved_KBBL);
    }

    return 0;
}

struct notifier_block pm_notifier_block = {
    pm_notifier_call, NULL, 0
};

static acpi_status __init acpi_get_devices_callback(acpi_handle handle, u32 level, void* context, void** return_value)
{
    *(acpi_handle*)return_value = handle;
    return AE_CTRL_TERMINATE;
}

static int __init ec_init(void)
{
    int err = 0;
    acpi_status status;
    struct acpi_device *device;

    status = acpi_get_devices("PNP0C09", acpi_get_devices_callback, NULL, &handle_EC0);
    if (!ACPI_SUCCESS(status)) {
        pr_err("EC device not found");
        return -ENODEV;
    }

    status = acpi_get_handle(handle_EC0, "ECRD", &handle_ECRD);
    if (!ACPI_SUCCESS(status)) {
        pr_err("method ECRD not found");
        return -ENODEV;
    }

    status = acpi_get_handle(handle_EC0, "ECWT", &handle_ECWT);
    if (!ACPI_SUCCESS(status)) {
        pr_err("method ECWT not found");
        return -ENODEV;
    }

    status = acpi_get_handle(handle_EC0, "KBBL", &handle_KBBL);
    if (!ACPI_SUCCESS(status)) {
        pr_err("field KBBL not found");
        return -ENODEV;
    }

    redmibook_kbd_backlight.brightness = (unsigned int)kbbl_brightness_get(&redmibook_kbd_backlight);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 18, 0))
    device = acpi_fetch_acpi_dev(handle_EC0);
#else
    acpi_bus_get_device(handle_EC0, &device);
#endif

    led_classdev_register(&device->dev, &redmibook_kbd_backlight);

    register_pm_notifier(&pm_notifier_block);

    return err;
}

static void __exit ec_exit(void)
{
    unregister_pm_notifier(&pm_notifier_block);
    led_classdev_unregister(&redmibook_kbd_backlight);
}

module_init(ec_init);
module_exit(ec_exit);

MODULE_ALIAS("dmi:bvnTIMI:bvr*:bd*:br*:efr*:svnTIMI:pn*:pvr:rvnTIMI:rnTM2113:rvr*:cvnTIMI:ct*:cvr:sku*");
MODULE_ALIAS("dmi:bvnTIMI:bvr*:bd*:br*:efr*:svnTIMI:pn*:pvr:rvnTIMI:rnTM2107:rvr*:cvnTIMI:ct*:cvr:sku*");

MODULE_AUTHOR("Pom");
MODULE_DESCRIPTION("Redmibook Keyboard Backlight");
MODULE_LICENSE("GPL v2");
