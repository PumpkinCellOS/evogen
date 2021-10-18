#!/bin/bash

walk_dir () {
    shopt -s nullglob dotglob

    for pathname in "$1"/*; do
        if [ -d "$pathname" ]; then
            walk_dir "$pathname"
        else
            printf -- '---- Running test: %s\n' "$pathname"
            ../../build-Debug/tests/evotest-script-TestRunner $pathname
        fi
    done
}

walk_dir lang
