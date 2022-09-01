##  RMARB5B0P0B0B

DSDT没有变动，因此沿用上一个版本的补丁。对比ACPI，能看得懂的就是无线网卡部分有变化。

## ssdt21.diff

插入`AcpDmicConnected`字段使`>= Linux5.19`或者`>=linux-image-5.17.0-1015-oem`自动识别麦克风DMIC。应用此补丁后不需要再安装`redmibook_dmic`
