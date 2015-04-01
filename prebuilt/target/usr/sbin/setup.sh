#!/bin/sh
cmd="$1";
[ -z "$cmd" ] && cmd="start"

dev_wan=eth0
dev_lan=br0

MAIN_PATH=/tmp/trend
idp_mod=$MAIN_PATH/IDP.ko
fw_mod=$MAIN_PATH/bw_forward.ko
qos_mod=$MAIN_PATH/tc_cmd.ko
rule=$MAIN_PATH/rule.trf
agent=bwdpi-rule-agent
qos_conf_path=$MAIN_PATH/qosd.conf
qos_dbg_level=1

dev=/dev/detector
dev_maj=190
dev_min=0

fwdev=/dev/idpfw
fwdev_maj=191
fwdev_min=0

SINK=/dev/null
IPT=iptables
iqos_cli=iqos_cli

case "$cmd" in
start)
	# create dev node
	echo "Creating device nodes..."
	[ ! -c "$dev" ] && mknod $dev c $dev_maj $dev_min
	[ ! -c "$fwdev" ] && mknod $fwdev c $fwdev_maj $fwdev_min
	[ -c $dev ] || echo "...Create $dev failed"
	[ -c $fwdev ] || echo "...Create $fwdev failed"

	echo "Insert IDP engine..."	
	insmod $idp_mod || exit -1
	echo "Insert forward module $fw_mod with param - dev_wan=$dev_wan..."
	insmod $fw_mod \
		dev_wan=$dev_wan \
		dbg_mod=1 \
		qos_conf_path=$qos_conf_path || exit -1
	echo "Insert tc module..."
	insmod $qos_mod || exit -1

	if [ ! -f "$rule" ]; then
		echo "Signature file $rule doesn't exist!"
		exit -1
	fi

	echo "Run rule agent to setup signature file $rule..."
	(cd /tmp/trend; ./$agent -g -R)

	#echo start > /proc/iqos_ctrl
	echo "Runnig iqos_cli background mode"
	($iqos_cli -b &)
	;;

stop)
	# Stop iqos_cli
	echo "kill iqos cli"
	(killall $iqos_cli)
	echo "Unload engine..."
	echo stop > /proc/iqos_ctrl
	rmmod $qos_mod > $SINK 2>&1
	rmmod $fw_mod > $SINK 2>&1
	rmmod $idp_mod > $SINK 2>&1

	echo "Remove device nodes..."
	[ -c "$dev" ] && rm -f $dev 
	[ ! -c "$dev" ] || echo "...Remove $dev failed"
	[ -c "$fwdev" ] && rm -f $fwdev
	[ ! -c "$fwdev" ] || echo "...Remove $fwdev failed"
	;;
	
restart)
	$0 stop
	sleep 2
	$0 start
	;;

iqos_cli_start)
	echo "Runnig iqos_cli background mode"
	($iqos_cli -b &)
	;;
iqos_cli_stop)
	echo "kill iqos cli"
	(killall $iqos_cli)
	;;
iqos_cli_restart)
	$0 iqos_cli_stop
	sleep 2
	$0 iqos_cli_start
	;;
esac
