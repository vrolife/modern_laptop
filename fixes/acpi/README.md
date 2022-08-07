# 现象

机型：TM2113-Redmi_Book_Pro_15_2022

环境：Ubuntu 22.04 LTS 内核 5.17.0-1013-oem

现象：不插电的情况下修休眠后无法唤醒。每次休眠或者冷启动 SSD 字段 `Unsafe Shutdowns` 无故增加。

# 解决方法

在Linux启动参数添加`acpi_osi=! acpi_osi=Linux`

添加启动参数后虽然解决了问题但是也会导致隨眠后键盘灯常亮，同时也会废掉LPS0。

键盘背光控制功能由驱动redmibook_kbd_backlight实现
