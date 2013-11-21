DESCRIPTION = "Altera MCAPI build"
PACKAGES = "${PN} ${PN}-dev ${PN}-staticdev ${PN}-dbg"
PROVIDES = "mcapi-staticdev mcapi-dbg"
RPROVIDES = "mcapi"

DEPENDS += "virtual/kernel"
RDEPENDS += "kernel-image update-modules"

inherit module

SECTION = "gsrd"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/BSD;md5=3775480a712fc46a69647678acb234cb"
PR = "r0"
FILES_${PN} = "/usr/bin/* /lib/modules/*"
FILES_${PN}-staticdev = "/usr/include/* /usr/lib"
FILES_${PN}-dbg = "/usr /usr/lib /usr/src/debug /usr/bin/.debug"

SRCREV = "${AUTOREV}"

SRC_URI = "git://gitolite@pg-git/openmcapi.git;protocol=ssh;branch=socfpga_devbranch"

S = "${WORKDIR}"

do_configure() {
	cd git
	autoreconf -i
	rm -rf build
	mkdir build
	cd build
	../configure --host=arm-linux-gnueabihf KERNELDIR=${STAGING_KERNEL_DIR} --prefix=/usr
}

do_compile() {
	cd git/build
	oe_runmake all
}

do_install() {
	cd git/build
	update-modules || true
	oe_runmake install DESTDIR=${D} INSTALL_MOD_PATH="${D}" KERNELDIR=${STAGING_KERNEL_DIR}
}

