#!/bin/bash

root=`cd -P -- "\`dirname -- "$0"\`/.." && printf '%s\n' "\`pwd -P\`"`

echo "root=${root}"

i686-w64-mingw32-dlltool \
  --input-def "${root}/external/iforce2/lib/x86/iforce2.def" \
  --output-lib "${root}/external/iforce2/lib/x86/iforce2.lib"  \
  --add-stdcall-underscore \
  --deterministic-libraries
