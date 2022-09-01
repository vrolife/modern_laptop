## 键盘不工作

影响的机型

1. TM2113

2. TM2107

如果内核版本小于Linux 6.0，安装此ACPI补丁能修复键盘不工作的问题

## 不插电的情况下修休眠后无法唤醒 和 SSD Unsafe Shutdowns

不插电的情况下修休眠后无法唤醒。每次休眠或者冷启动 SSD 字段 `Unsafe Shutdowns` 无故增加。

影响的机型

1. TM2113

在Linux启动参数添加`acpi_osi=! acpi_osi=Linux`可解决这两个问题。但是也会导致隨眠后键盘灯常亮，同时也会废掉LPS0。

键盘背光控制功能由驱动`redmibook_kbd_backlight`实现

## 麦克风

影响的机型

1. TM2113

ACPI补丁通过插入`AcpDmicConnected`字段。使`Linux 5.19`和 Ubuntu 的`linux-oem-5.17 5.17.0-1014`自动识别麦克风，无需安装`redmibook_dmic`。
