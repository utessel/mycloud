export ARCH=armhf
export CROSS_COMPILE=arm-linux-gnueabihf-

current=`pwd`
kernel="$current/wd/packages/kernel_3.2"
busybox="busybox-1.23.0"

if [ ! -e $kernel ]
then
  echo "please download the GPL file from WD"
  echo "and unzip them to ./wd/"
  exit 1
fi

if [ ! -e $busybox.tar.bz2 ]
then
  wget http://www.busybox.net/downloads/$busybox.tar.bz2
fi
if [ ! -e $busybox ]
then
  tar fax $busybox.tar.bz2
fi

cd $busybox

if [ ! -e .config ]
then
  cp $current/busybox-config .config
  make oldconfig
fi

make -j8 

if [ "$?" != "0" ]
then
  echo "generation of busybox failed?!"
  exit 1
fi

if [ ! -e $kernel/_bin/lib ]
then
  # compile kernel at least once
  cd $kernel
  make
fi

cd $current
fakeroot ./create_install.sh $busybox $kernel

cat initramfs.cpio | gzip > initramfs.cpio.gz

