#!/bin/sh

# Written by Simon Josefsson.

# Start sshd, invoke parameters, saving exit code, kill sshd, and
# return exit code.

srcdir=${srcdir:-$PWD}
srcdir=`cd $srcdir; pwd`
cmd="./ssh2"
SSHD=${SSHD:-/usr/sbin/sshd}

PRIVKEY=$srcdir/etc/user
export PRIVKEY
PUBKEY=$srcdir/etc/user.pub
export PUBKEY

chmod go-r $srcdir/etc/host*
$SSHD -f /dev/null -h $srcdir/etc/host \
    -o 'Port 4711' -o 'Protocol 2' \
    -o "AuthorizedKeysFile $srcdir/etc/user.pub" -D &
sshdpid=$!

trap "kill ${sshdpid}; echo signal killing sshd; exit 1;" EXIT

: "started sshd (${sshdpid})"

sleep 1

: Invoking $cmd...
eval $cmd
ec=$?
: Self-test exit code $ec

: "killing sshd (${sshdpid})"
kill "${sshdpid}" > /dev/null 2>&1
trap "" EXIT
exit $ec