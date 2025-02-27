#!/bin/bash

root=`cd -P -- "\`dirname -- "$0"\`/.." && printf '%s\n' "\`pwd -P\`"`

echo "root=${root}"

cd "${root}/external/iforce2/lib/x86"
i686-w64-mingw32-dlltool \
  --input-def "iforce2.def" \
  --output-lib "iforce2.lib"  \
  --add-stdcall-underscore \
  --deterministic-libraries

cd "${root}/external/smackw32/lib/x86"
i686-w64-mingw32-dlltool \
  --input-def "smackw32.def" \
  --output-lib "smackw32.lib"  \
  --add-stdcall-underscore \
  --deterministic-libraries
