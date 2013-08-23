DESCRIPTION = "Altera GSRD customization"
SECTION = "gsrd"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/BSD;md5=3775480a712fc46a69647678acb234cb"
PR = "r0"
FILES_${PN} = "/www/pages/* \
		/home/root/altera/* \
		/home/root/README"
FILES_${PN}-dbg = "/www/pages/cgi-bin/.debug/ /usr /home/root/altera/.debug"

SRC_URI = "file://blink/blink.c "
SRC_URI += "file://blink/Makefile "
SRC_URI += "file://toggle/toggle.c "
SRC_URI += "file://toggle/Makefile "
SRC_URI += "file://scroll_client/scroll_client.c "
SRC_URI += "file://scroll_client/Makefile "
SRC_URI += "file://server/scroll_server.c "
SRC_URI += "file://server/Makefile "
SRC_URI += "file://Makefile "
SRC_URI += "file://hello/hello.c "
SRC_URI += "file://hello/Makefile "
SRC_URI += "file://doc/README "

S = "${WORKDIR}"

do_compile() {
	oe_runmake all
}

do_install() {
	install -d ${D}/www/pages/cgi-bin
	install -d ${D}/home/root/altera
	install -m 0755 blink/blink ${D}/www/pages/cgi-bin/blink
	install -m 0755 server/scroll_server ${D}/www/pages/cgi-bin/scroll_server
	install -m 0755 scroll_client/scroll_client ${D}/www/pages/cgi-bin/scroll_client
	install -m 0755 toggle/toggle ${D}/www/pages/cgi-bin/toggle

	install -m 0755 blink/blink ${D}/home/root/altera/blink
	install -m 0755 scroll_client/scroll_client ${D}/home/root/altera/scroll_client
	install -m 0755 toggle/toggle ${D}/home/root/altera/toggle
	install -m 0755 hello/hello ${D}/home/root/altera/hello
	install -m 0755 doc/README ${D}/home/root/
}

