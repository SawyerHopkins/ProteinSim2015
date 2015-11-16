#!/bin/bash

function add {
    echo "$1" >> .automation/forceList
}
function remove {
    sed -i "/$1/d" .automation/forceList
}
function build {
    readarray -t forces < .automation/forceList
    #Write the core complier from template
    sed -n -e "/#CORE/,/#END/w buildAll.sh" .automation/buildAll.template
    #Write the force compilers from template
    for var in "${forces[@]}"
    do
	echo "Writing ${var}"
        echo "$(sed -n '/#FORCE/,/#END/p' .automation/buildAll.template)" >> buildAll.sh
        sed -i -e "s;%NAME%;${var};g" buildAll.sh
    done
    #Write the UI compile from template
    echo "$(sed -n '/#UI/,/#END/p' .automation/buildAll.template)" >> buildAll.sh
}
function display {
    echo
    cat < .automation/forceList
    echo
}

while getopts "a:r:bc" OPTION
do
    case $OPTION in
        a)
            add $OPTARG
            ;;
        r)
            remove $OPTARG
            ;;
        b)
            build
            ;;
        c)
            display
            ;;
    esac
done
