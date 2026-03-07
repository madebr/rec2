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

cd "${root}/src/brender/dll/x86"
i686-w64-mingw32-dlltool \
  --input-def "../brcore1.def" \
  --output-lib "brcore1.lib"  \
  --deterministic-libraries

cd "${root}/src/brender/dll/x86"
i686-w64-mingw32-dlltool \
  --input-def "../brhost1.def" \
  --output-lib "brhost1.lib"  \
  --deterministic-libraries

cd "${root}/src/brender/dll/x86"
i686-w64-mingw32-dlltool \
  --input-def "../brpmap1.def" \
  --output-lib "brpmap1.lib"  \
  --deterministic-libraries

cd "${root}/src/brender/dll/x64"
x86_64-w64-mingw32-dlltool \
  --input-def "../brcore1.def" \
  --output-lib "brcore1.lib"  \
  --deterministic-libraries

cd "${root}/src/brender/dll/x64"
x86_64-w64-mingw32-dlltool \
  --input-def "../brhost1.def" \
  --output-lib "brhost1.lib"  \
  --deterministic-libraries

cd "${root}/src/brender/dll/x64"
x86_64-w64-mingw32-dlltool \
  --input-def "../brpmap1.def" \
  --output-lib "brpmap1.lib"  \
  --deterministic-libraries


