// SPDX-License-Identifier: GPL-2.0
/* EC driver for Redmi Laptops */

#include <linux/acpi.h>
#include <linux/module.h>
#include <linux/init.h>

static int __init gnvs_init(void)
{
    acpi_handle handle = NULL;
    acpi_status status = 0;
    struct acpi_buffer result_buffer;
    union acpi_object result;

#define FILED "DAS3"

    status = acpi_get_handle(NULL, "\\" FILED, &handle);
    if (status) {
        pr_err("ACPI object not found. %u\n", status);
        return -ENODEV;
    }

    pr_info("handle: %p\n", handle);

    result_buffer.length = sizeof(result);
    result_buffer.pointer = &result;

    status = acpi_evaluate_object(handle, NULL, NULL, &result_buffer);
    if (status) {
        pr_err("Failed to get value. %u\n", status);
        return -ENODEV;
    }
    pr_info(FILED ": %llu\n", result.integer.value);
    return 0;
}

static void __exit gnvs_exit(void)
{

}

module_init(gnvs_init);
module_exit(gnvs_exit);

MODULE_AUTHOR("Pom");
MODULE_DESCRIPTION("Redmibook GNVS driver");
MODULE_LICENSE("GPL v2");
