# 声明

1. 指纹传感器(USB 0x10a5 0x9201)是图像型传感器，本驱动会对从传感器采集到的指纹图像进行预处理，因此本驱动不保证预处理后的指纹图像和从传感器中读取到的图像一致，不保证预处理后的指纹图像能唯一对应用户真实手指上的指纹。
2. 本驱动不能分辨假冒或模拟的指纹图像，不能分辨指纹传感器提供的指纹数据是否来自真实用户，不能保证驱动和指纹传感器之间的通讯不被侧信道探测不被篡改。
3. 预处理后的指纹在经过由指纹传感器提供的秘密（后文简称密码）用chacha20_poly1305加密算法加密后存储在用户计算机的文件系统中。本驱动不能保证密码不被盗取，不能保证此密码是解密指纹数据的唯一手段，不能保证存储在用户计算机文件系统中的指纹图像数据有不被盗取和解密。驱动进程启动后指纹数据会解密释放到内存中，如果用户的操作系统应当启动了coredump功能，指纹数据有随着coredump泄漏的风险，用户应当知悉此风险。指纹数据的周全由用户负责，指纹数据丢失、被盗用、泄漏的风险由用户承担。
4. 指纹识别原理为用SIFT算法分别对两组指纹进行提特征提取和比较，若这两组指纹具备一定相似度（特征点数量大于等于4）则进一步用MSSIM算法计算两组指纹的相似度。在MSSIM算法得出相似度分数大于0.5时得出正面结论，否则得出负面结论。本驱动仅能得出两组指纹在图形学上是否相似的结论。本驱动在面对伪造的指纹图像或者特殊构造的图像或随机数据时，有可能得出随机的结论。因此本驱动用作用户身份识别用途时具有风险性，此风险由用户承担。
5. 本驱动是第三方软件，有可能破坏用户计算机的保修条款，有可能减少用户计算机的寿命，有可能损坏用户计算机，如造成任何损失，用户需自行承担损失。
6. 本驱动仅供学习和研究用途，不可用于商业目的

# 要求

系统：Ubuntu 22.04 LTS (Jammy Jellyfish)

包：

- libopencv-dev (或逐一安装 opencv_core opencv_imgproc opencv_features2d opencv_imgcodecs opencv_calib3d opencv_flann)
- libusb-1.0-0
- libevent-core-2.1-7
- libssl3
- libdbus-1-3
- fprintd

# Archlinux

[传送](https://github.com/vrolife/modern_laptop/issues/5#issuecomment-1191811841)

# 安装方法

将文件fingerpp复制到/opt/fingerpp/bin/fingerpp

运行`sudo systemctl edit fprintd`并输入以下内容

```
[Service]
ExecStart=
ExecStart=/opt/fingerpp/bin/fingerpp --bus=system
```

接着运行`sudo pam-auth-update`启用"Fingerprint authentication"

# 使用方法

以 KDE 5.24.4 为例，进入System Settings，Users，点击用户，点击“Configure Fingerprint Authentication”进行指纹录入

本驱动以收集到的指纹数据有大于等于120000个像素点为录入成功条件，界面上没有进度提示，KDE中指纹录入界面提示的进度没有参考价值。

录入技巧是先重复几次录入主要区域，再移动手指录入周边区域。能否成功全看运气。另外目前的算法容错率较低，很可能今天录的明天就用不了，取决于用户指纹的稳定性。

# fingerpp2

导出两个参数。同时提供录入进度。

如果识别率较低可以使用以下参数`/opt/fingerpp/bin/fingerpp --bus=system --filter-before-ssim=true --min-score=0.3`

# 开放源代码

驱动用了内部异步框架开发的，而这个框架还没做好开源的准备。因此目前只放出二进制文件。若有财主任性，再另说了。

2022年7月22日星期五

整理了一些文档资料，放在docs/目录下。其中包括传感器的通信协议和通信流程。还有两份基于C和Python的通信程序不完整例子。基于这些资料，已经足够开发另一个驱动程序。

# 文件

- fingerpp SHA56:ed1dfe8c57ea9c54321a5288d05dca93869f6e67476034e8b752f768cd0ab8cc

- fingerpp2 SHA56:2557555afa7ef05c9f6deb9b5e17ff700739c840935381fdcfc6343f8157c02c

# 资料

[基于OpenCV的小尺寸指纹传感器图像处理思路](https://zhuanlan.zhihu.com/p/540376287)
