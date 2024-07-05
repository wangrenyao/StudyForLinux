#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/acpi.h>

int platf_probe(struct platform_device * device)
{
    dump_stack();
    dev_info(&device->dev, "device probe");
    return 0;
}

int platf_remove(struct platform_device * device)
{
    dev_info(&device->dev, "device remove");
    return 0;
}

struct acpi_device_id g_acpi_device[] = {
    { "platf", 0, },
    {}
};

struct platform_driver g_plat_driver = {
    .probe = platf_probe,
    .remove = platf_remove,
    .driver = {
        .name = "platfd",
        .acpi_match_table = g_acpi_device,
    },
};
struct platform_device *g_platf_dev = NULL; 
int __init platf_driver_init(void)
{
    pr_info("driver init...\n");
    platform_driver_register(&g_plat_driver);

    struct resource platf_res[] = {
        [0] = DEFINE_RES_NAMED(0x000, 0x4,  "conf", IORESOURCE_IO),
        [1] = DEFINE_RES_NAMED(0x100, 0x8,  "data", IORESOURCE_IO),
    };

    g_platf_dev = platform_device_register_simple("platf", 1, platf_res, ARRAY_SIZE(platf_res));
    return 0;
}

void __exit platf_driver_exit(void)
{
    pr_info("driver exit...\n");

    platform_device_unregister(g_platf_dev);
    return;
}


module_init(platf_driver_init);
module_exit(platf_driver_exit);
MODULE_LICENSE("GPL v2");