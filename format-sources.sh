#!/usr/bin/env bash

for f in $@
do
    # apply clang-format file .clang-format in top dir
    clang-format -i -style=file -fallback-style=none $f

    # divider lines with 120 chars
    perl -pi -e 's#^//-+$#//----------------------------------------------------------------------------------------------------------------------#g' $f
done