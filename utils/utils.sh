#!/bin/bash
do_get_deps() {
	src=$1
	[ ! -e "$src" ] && return
	(grep "^#include" $src | while read dep; do
		if [[ "$dep" =~ $regex ]]; then
			do_get_deps ${BASH_REMATCH[1]}
			echo "${BASH_REMATCH[1]}"
		fi
	done)
}

get_deps() {
	local regex="#include\s*\"(.*)\""
	do_get_deps $1 | sort | uniq | xargs echo -n
}

