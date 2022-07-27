# 现象

机型：TM2113-Redmi_Book_Pro_15_2022

环境：Ubuntu 22.04 LTS 内核 5.17.0-1013-oem

现象：不插电的情况下修休眠后无法唤醒。每次休眠或者冷启动 SSD 字段 `Unsafe Shutdowns` 无故增加。

# 解决方法

在Linux启动参数添加`acpi_osi=! acpi_osi=Linux`

添加启动参数后虽然解决了问题但是也会导致隨眠后键盘灯常亮，同时也会废掉LPS0。

因此另外写了个驱动[redmibook_ec](https://github.com/vrolife/modern_laptop/blob/main/TM2113-Redmi_Book_Pro_15_2022/redmibook_ec/README.md)来导出EC的隐藏功能，用于实现隨眠时自动关闭键盘灯，并在唤醒后恢复原来状态。

# 风险声明

redmibook_ec使用了EC未公开的功能，有破坏你的计算机的可能性，因此风险自负。

redmibook_ec驱动基于TM2113-Redmi_Book_Pro_15_2022开发。若不是此机型，请不要安装此驱动。

# 安装

基于上述风险，不做全自动安装。愿意尝试的，请手动安装。

1. 将redmibook_ec.conf放置到/etc/modprobe.d/下。此文件启动redmibook_ec驱动的自动键盘灯功能。
2. 编辑/etc/modules，写入一行内容`redmibook_ec`。此文件使系统开机加载redmibook_ec
3. 运行`dkms add /path/to/modern_laptop/TM2113-Redmi_Book_Pro_15_2022/redmibook_ec`
4. 运行`dkms install redmibook_ec/1.0.0`
