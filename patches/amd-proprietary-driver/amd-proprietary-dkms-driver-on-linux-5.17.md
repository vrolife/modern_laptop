# amdgpu/5.16.9.22.20-1438747~22.04/amd/dkms/configure

remove two lines

```
printf "%s\n" "#define HAVE_ZONE_DEVICE_PUBLIC 1" >>confdefs.h
printf "%s\n" "#define HAVE_DEVM_MEMREMAP_PAGES_DEV_PAGEMAP 1" >>confdefs.h
```

# amdgpu/5.16.9.22.20-1438747~22.04/amd/amdkcl/kcl_drm_print.c

change

```
#include <stdarg.h>
```

to

```
#include <linux/stdarg.h>
```
