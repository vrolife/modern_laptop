# Smooth brightness for KDE powerdevil

```
sudo apt build-dep powerdevil
sudo apt source powerdevil
cd powerdevil-*
patch -p1 < smooth-brithness.patch
dpkg-buildpackage -uc -us -b -tc -j14
sudo dpkg -i ../*.deb
```
