如果遇到PSR导致的内核崩溃，可通过禁用PSR规避。

可通过放置文件/etc/modprobe.d/disable-psr.conf实现禁用amdgpu的PSR支持。

此文件需要运行`update-initramfs`并重启生效。

禁用PSR后，功耗可能会增加。
