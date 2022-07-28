TOP=$(pwd)

cd "$TOP/libgcc-7-dev"

dpkg-buildpackage -uc -us -tc

cd "$TOP/libstdc++-7-dev"

dpkg-buildpackage -uc -us -tc

cd "$TOP/python"

dpkg-buildpackage -uc -us -tc

cd "$TOP"

dpkg -i *.deb
