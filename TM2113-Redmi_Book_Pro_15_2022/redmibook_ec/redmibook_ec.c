// SPDX-License-Identifier: GPL-2.0
/* EC driver for Redmi Laptops */

#include <linux/acpi.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/suspend.h>
#include <linux/dmi.h>

#include "TIMI_TM2113.h"

static bool turn_keyboard_backlight_off_after_suspend = 0;
module_param(turn_keyboard_backlight_off_after_suspend, bool, 0644);
MODULE_PARM_DESC(turn_keyboard_backlight_off_after_suspend, "Turn keyboard backlight off after suspend");

static bool turn_screen_off_after_suspend = 0;
module_param(turn_screen_off_after_suspend, bool, 0644);
MODULE_PARM_DESC(turn_screen_off_after_suspend, "Turn screen off after suspend");

static struct kobject* kobj_ec;

static acpi_handle handle_ec;
static acpi_handle handle_ecrd;
static acpi_handle handle_ecwt;
static acpi_handle handle_ecmd;

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

    status = acpi_evaluate_object(handle_ecrd, NULL, &params, &result_buffer);
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

    status = acpi_evaluate_object(handle_ecwt, NULL, &params, NULL);
    if (!ACPI_SUCCESS(status)) {
        pr_err("evaluate ECWT failed. %s", acpi_format_exception(status));
    }
    return status;
}

static acpi_status ecmd(u64 val)
{
    struct acpi_object_list params;
    union acpi_object args[2];
    acpi_status status;
    
    params.count = 2;
    params.pointer = &args[0];

    args[0].type = ACPI_TYPE_INTEGER;
    args[0].integer.value = val;
    
    args[1].type = ACPI_TYPE_INTEGER;
    args[1].integer.value = 0;

    status = acpi_evaluate_object(handle_ecmd, NULL, &params, NULL);
    if (!ACPI_SUCCESS(status)) {
        pr_err("evaluate ECMD failed. %s", acpi_format_exception(status));
    }
    return status;
}

// Common

#define DEF_FILE(machine, name) \
acpi_handle handle_##machine##_##name; \
static ssize_t machine##_##name##_show(struct kobject* kobj, struct kobj_attribute* attr, char* buf) \
{ \
    u64 val; \
    acpi_status status; \
    status = ecram_read(handle_##machine##_##name, &val); \
    if (!ACPI_SUCCESS(status)) { \
        return -EIO; \
    } \
    return sprintf(buf, "%lld", val); \
} \
\
static ssize_t machine##_##name##_store(struct kobject* kobj, struct kobj_attribute* attr, const char* buf, size_t count) \
{ \
    acpi_status status; \
    unsigned int val = 0; \
\
    sscanf(buf, "%u", &val); \
\
    status = ecram_write(handle_##machine##_##name, val); \
    if (!ACPI_SUCCESS(status)) { \
        return -EIO; \
    } \
\
    return count; \
} \
\
static struct kobj_attribute file_##machine##_##name = __ATTR(name, 0660, machine##_##name##_show, machine##_##name##_store);

#define INIT_FILE(machine, name) \
status = acpi_get_handle(handle_ec, #name, &handle_##machine##_##name); \
if (!ACPI_SUCCESS(status)) { \
    pr_err("field " #name " not found"); \
    return -ENODEV; \
}

#define CREATE_FILE(machine, name) \
err = sysfs_create_file(kobj_ec, &file_##machine##_##name.attr); \
if (err) { \
    pr_err("failed to create " #name " file"); \
    return err; \
}

TIMI_TM2113_EC_FIELDS(DEF_FILE)

// PM

static u64 saved_KBBL;

static int pm_notifier_call(struct notifier_block* bk, unsigned long action, void* data)
{
    if (action == PM_SUSPEND_PREPARE) {
        if (turn_keyboard_backlight_off_after_suspend) {
            ecram_read(handle_TIMI_TM2113_KBBL, &saved_KBBL);
            ecram_write(handle_TIMI_TM2113_KBBL, 0x08);
        }

        if (turn_screen_off_after_suspend) {
            ecmd(0x5d);
        }

    } else if (action == PM_POST_SUSPEND) {
        if (turn_keyboard_backlight_off_after_suspend) {
            ecram_write(handle_TIMI_TM2113_KBBL, saved_KBBL);
        }
        if (turn_screen_off_after_suspend) {
            ecmd(0x5e);
        }
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

// Module

static int __init ec_init(void)
{
    int err = 0;
    acpi_status status;

    status = acpi_get_devices("PNP0C09", acpi_get_devices_callback, NULL, &handle_ec);
    if (!ACPI_SUCCESS(status)) {
        pr_err("EC device not found");
        return -ENODEV;
    }

    status = acpi_get_handle(handle_ec, "ECRD", &handle_ecrd);
    if (!ACPI_SUCCESS(status)) {
        pr_err("method ECRD not found");
        return -ENODEV;
    }

    status = acpi_get_handle(handle_ec, "ECWT", &handle_ecwt);
    if (!ACPI_SUCCESS(status)) {
        pr_err("method ECWT not found");
        return -ENODEV;
    }

    TIMI_TM2113_EC_FIELDS(INIT_FILE)

    kobj_ec = kobject_create_and_add("redmibook_ec", kernel_kobj);

    if (kobj_ec == NULL) {
        return -ENOMEM;
    }

    TIMI_TM2113_EC_FIELDS(CREATE_FILE)

    register_pm_notifier(&pm_notifier_block);

    return err;
}

static void __exit ec_exit(void)
{
    unregister_pm_notifier(&pm_notifier_block);
    kobject_put(kobj_ec);
}

module_init(ec_init);
module_exit(ec_exit);

MODULE_AUTHOR("Morfeo");
MODULE_DESCRIPTION("Redmibook EC driver");
MODULE_LICENSE("GPL v2");
