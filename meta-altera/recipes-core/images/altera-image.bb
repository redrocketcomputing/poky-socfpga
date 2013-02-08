
DESCRIPTION = "The set of packages for development and testing provided by Altera"

LICENSE = "MIT"

ALTERA_IMAGE_INSTALL ?= "initramfs-altera kernel-modules netbase busybox base-passwd base-files tinylogin sysvinit initscripts e2fsprogs mtd-utils gdb gdbserver bash strace openssh openssl elfutils sysfsutils usbutils dtc gawk ethtool grep lighttpd iputils make pciutils portmap sed setserial wget autoconf diffutils perl minicom valgrind i2c-tools lttng-modules lttng-tools iptables oprofile net-tools gator openssh-sftp-server"
IMAGE_INSTALL ?= "${ALTERA_IMAGE_INSTALL}"

inherit core-image

# altera-image.inc must be included after inherit core-image to override functionality

include altera-image.inc

