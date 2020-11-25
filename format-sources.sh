#!/usr/bin/env bash

# Usage: find . -iname "*.h" -or -iname "*.cc" -exec ./format-sources.sh {} \;

for f in $@
do
    # apply clang-format file .clang-format in top dir
    clang-format -i -style=file -fallback-style=none $f

    # divider lines with 120 chars
    perl -pi -e 's#^//-+$#//----------------------------------------------------------------------------------------------------------------------#g' $f
    perl -pi -e 's#^//=+$#//----------------------------------------------------------------------------------------------------------------------#g' $f
done
