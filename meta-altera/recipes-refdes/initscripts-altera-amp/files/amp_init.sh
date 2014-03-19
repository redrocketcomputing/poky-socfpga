#!/bin/sh

# Copyright 2013 Altera Corporation. All Rights Reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.

# 3. The name of the author may not be used to endorse or promote products
# derived from this software without specific prior written permission.

# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER "AS IS" AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ARE DISCLAIMED. IN NO
# EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
# OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
# IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
# OF SUCH DAMAGE.

function get_ip()
{
	echo `ifconfig eth0 | head -n 2 | tail -n 1 | sed s/inet\ addr:// | sed s/\ Bcast.*// | sed s/\ *//g`
}

# checks if the ip is valid
function check_ip()
{
	echo `echo $1 | sed 's/\(\([0-9]\{1,3\}\)\.\)\{3\}\([0-9]\{1,3\}\)//g'`
}

# clears and prints to LCD
# first parameter prints to line one, second parameter prints to line two
function print_to_lcd()
{
	printf '\e[2J' > /dev/ttyLCD0
	printf "$1" > /dev/ttyLCD0
	printf "\n" > /dev/ttyLCD0
	printf "$2" > /dev/ttyLCD0
}

case "$1" in
  start)
	if [ "`cat /sys/class/fpga/fpga0/status`" == "user mode" ]; then
		for module in altera-hwmutex mailbox-altera mcomm_socfpga; do
			modprobe $module
			if [ $? -ne 0 ]; then
				echo "Error: Failed to load module $module"
			fi
		done
	else
		echo "Error: FPGA not in user mode. Unable to load FPGA drivers"
	fi

	if [ -e /dev/ttyLCD0 ]; then
		IP=$(get_ip)
		IP_CHECK=$(check_ip $IP)
		if [ -n "$IP_CHECK" ]; then
			IP="No IP obtained"
		fi

		print_to_lcd "$IP" "AMP Demo"

	else
		echo "LCD device not found, will not print anything to LCD"
	fi
	;;
  stop)
	for module in altera-hwmutex mailbox-altera mcomm_socfpga; do
		rmmod $module
		if [ $? -ne 0 ]; then
			echo "Error: Failed to load module $module"
		fi
	done
	;;
esac

