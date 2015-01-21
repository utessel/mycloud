export ARCH=armhf
export CROSS_COMPILE=arm-linux-gnueabihf-
if [ ! -e busybox-1.23.0.tar.bz2 ]
then
  wget http://www.busybox.net/downloads/busybox-1.23.0.tar.bz2
else 
  echo "Busybox already loaded"
fi
if [ ! -e busybox-1.23.0 ]
then
  tar fax busybox-1.23.0.tar.bz2
else
  echo "Busybox already untared"
fi

cd busybox-1.23.0

if [ ! -e .config ]
then
  cp ../busybox-config .config
make oldconfig
fi

make -j8 
make -j8 install

if [ ! -e ../wd/packages/kernel_3.2/_bin/lib ]
then
  # compile kernel at least once
  cd ../wd/packages/kernel_3.2
  make
  cd ../../../../busybox-1.23.0
fi

cd _install

mkdir {bin,dev,sbin,etc,proc,sys}
cp -R ../../wd/packages/kernel_3.2/_bin/lib .
mkdir dev/pts
mkdir -p usr/share/udhcpc

echo "#!/bin/sh" >init
echo "mount -t proc proc /proc" >>init
echo "mount -t sysfs sysfs /sys" >>init
echo "mount -t devpts none /dev/pts" >>init
echo "echo /sbin/mdev >/proc/sys/kernel/hotplug" >>init
echo "mdev -s" >>init
echo "insmod /lib/modules/3.2.26/pfe.ko lro_mode=1 tx_qos=1 alloc_on_init=1" >>init
echo "ifconfig eth0 up" >>init
echo "udhcpc -b" >>init
echo "telnetd -l/bin/sh" >>init
echo "exec /sbin/init" >>init
chmod +x init

echo "T0:2345:respawn:/sbin/getty -L ttyS0 115200 vt100" >etc/inittab
echo "ttyS0::askfirst:-/bin/sh" >etc/inittab

echo "#!/bin/sh" >usr/share/udhcpc/default.script
echo "case \"\$1\" in" >>usr/share/udhcpc/default.script
echo "   renew|bound)" >>usr/share/udhcpc/default.script
echo "     /sbin/ifconfig \$interface \$ip \$BROADCAST \$NETMASK" >>usr/share/udhcpc/default.script
# note: route to set gw is missing
echo "     ;;" >>usr/share/udhcpc/default.script
echo "esac" >>usr/share/udhcpc/default.script
echo "exit 0" >>usr/share/udhcpc/default.script
chmod +x usr/share/udhcpc/default.script

echo "creating device files using sudo mknod"

sudo mknod dev/null c 1 3
sudo mknod dev/tty c 5 0
sudo mknod dev/console c 5 1

find . | cpio -H newc -o > ../../initramfs.cpio
cd ../..
cat initramfs.cpio | gzip > initramfs.cpio.gz

