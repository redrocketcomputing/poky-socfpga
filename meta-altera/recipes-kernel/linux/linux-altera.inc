PR = "r1"
SRC_URI = "git://git.rocketboards.org/linux-socfpga.git;protocol=http;branch=socfpga-${PV}-rel file://0001-kernel-config.patch"

FILES_${PN} += "0001-kernel-config.patch"

SRCREV_pn-${PN} = "${AUTOREV}"

inherit kernel
SECTION = "kernel"
DESCRIPTION = "Altera Linux kernel"
LICENSE = "GPLv2"

PROVIDES+=" linux virtual/kernel"
COMPATIBLE_MACHINE = "socfpga"

LIC_FILES_CHKSUM = "file://${S}/COPYING;md5=d7810fab7487fb0aad327b76f1be7cd7"

S = "${WORKDIR}/git"

SRCREV_pn-${PN} = "${AUTOREV}"

do_configure() {
	export INITRAMFS_IMAGE_FILE="${INITRAMFS_IMAGE}-${MACHINE}.cpio"

	unset CFLAGS CPPFLAGS CXXFLAGS LDFLAGS MACHINE
	export CROSS_COMPILE="${TARGET_PREFIX}"
	export ARCH=${ARCH}
	
	oe_runmake ${MACHINE_DEFCONFIG} CONFIG_BLK_DEV_INITRD=y
	
	cp .config .config.ori

	grep CONFIG_BLK_DEV_INITRD .config || echo "CONFIG_BLK_DEV_INITRD=y" >> .config
	grep CONFIG_INITRAMFS_SOURCE .config || echo "CONFIG_INITRAMFS_SOURCE=\"\"" >> .config
	grep CONFIG_INITRAMFS_ROOT_UID .config || echo "CONFIG_INITRAMFS_ROOT_UID=0" >> .config
	grep CONFIG_INITRAMFS_ROOT_GID .config || echo "CONFIG_INITRAMFS_ROOT_GID=0" >> .config
	grep CONFIG_INITRAMFS_COMPRESSION_NONE .config || echo "CONFIG_INITRAMFS_COMPRESSION_NONE=y" >> .config

	if [ ! -z ${INITRAMFS_IMAGE} ] && [ -e ${DEPLOY_DIR_IMAGE}/${INITRAMFS_IMAGE_FILE} ]; then
		cp ${DEPLOY_DIR_IMAGE}/${INITRAMFS_IMAGE_FILE} .
		echo "Found valid initramfs ${INITRAMFS_IMAGE_FILE}"
		sed -i "s|^.*CONFIG_BLK_DEV_INITRD[ =].*$|CONFIG_BLK_DEV_INITRD=y|g" .config
		sed -i "s|^.*CONFIG_INITRAMFS_SOURCE[ =].*$|CONFIG_INITRAMFS_SOURCE=\"${INITRAMFS_IMAGE_FILE}\"|g" .config
		sed -i "s|^.*CONFIG_INITRAMFS_ROOT_UID[ =].*$|CONFIG_INITRAMFS_ROOT_UID=0|g" .config
		sed -i "s|^.*CONFIG_INITRAMFS_ROOT_GID[ =].*$|CONFIG_INITRAMFS_ROOT_GID=0|g" .config
		sed -i "s|^.*CONFIG_INITRAMFS_COMPRESSION_NONE[ =].*$|CONFIG_INITRAMFS_COMPRESSION_NONE=y|g" .config
		oe_runmake oldconfig
	else
		rm -f *.cpio
		echo "Initramfs disabled or can't find valid initramfs ${INITRAMFS_IMAGE_FILE}"
		sed -i "s|^CONFIG_BLK_DEV_INITRD=.*$|# CONFIG_BLK_DEV_INITRD is not set|g" .config
		sed -i "s|^CONFIG_INITRAMFS_SOURCE=\".*\"$|# CONFIG_INITRAMFS_SOURCE is not set|g" .config
		sed -i "s|^CONFIG_INITRAMFS_ROOT_UID=.*$|# CONFIG_INITRAMFS_ROOT_UID is not set|g" .config
		sed -i "s|^CONFIG_INITRAMFS_ROOT_GID=.*$|# CONFIG_INITRAMFS_ROOT_GID is not set|g" .config
		sed -i "s|^CONFIG_INITRAMFS_COMPRESSION_NONE=.*$|# CONFIG_INITRAMFS_COMPRESSION_NONE is not set|g" .config
		oe_runmake oldconfig
	fi	
}

kernel_do_compile() {
        unset CFLAGS CPPFLAGS CXXFLAGS LDFLAGS MACHINE
        oe_runmake __headers CC="${KERNEL_CC}" LD="${KERNEL_LD}"
        oe_runmake ${KERNEL_IMAGETYPE_FOR_MAKE} ${KERNEL_ALT_IMAGETYPE} CC="${KERNEL_CC}" LD="${KERNEL_LD}" LOADADDR=0x8000
        if test "${KERNEL_IMAGETYPE_FOR_MAKE}.gz" = "${KERNEL_IMAGETYPE}"; then
                gzip -9c < "${KERNEL_IMAGETYPE_FOR_MAKE}" > "${KERNEL_OUTPUT}"
        fi
}

VMLINUX_NAME ?= "vmlinux-${PV}-${PR}-${MACHINE}-${DATETIME}"

kernel_do_deploy_append() {
	install -m 0644 ${S}/vmlinux ${DEPLOYDIR}/${VMLINUX_NAME}
	cd ${DEPLOYDIR} &&
		rm -f vmlinux &&
		ln -sf ${VMLINUX_NAME} vmlinux
}

