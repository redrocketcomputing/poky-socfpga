DESCRIPTION = "Altera MCAPI build"
DEPENDS = "mcapi-staticdev"
SECTION = "gsrd"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/BSD;md5=3775480a712fc46a69647678acb234cb"
PR = "r0"
FILES_${PN} = "/usr/local/bin/*"

SRCREV = "${AUTOREV}"

SRC_URI = "file://app_mcapi_demo.c \
	   file://linux_init.c"

S = "${WORKDIR}"

do_configure() {
}

do_compile() {
	${CC} app_mcapi_demo.c linux_init.c ${LDFLAGS} -L{STAGING_LIBDIR} -I{STAGING_INCDIR} -lopenmcapi -o mcapi_test
}

do_install() {
	install -d ${D}/usr/local/bin
	install -m 0755 mcapi_test ${D}/usr/local/bin/mcapi_test
}

