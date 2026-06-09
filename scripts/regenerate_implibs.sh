#!/bin/bash

set -ex

root=`cd -P -- "\`dirname -- "$0"\`/.." && printf '%s\n' "\`pwd -P\`"`

echo "root=${root}"


cd "${root}/external/iforce2/lib/x86"
i686-w64-mingw32-dlltool \
  --dllname "iforce2.dll" \
  --input-def "iforce2.def" \
  --output-lib "iforce2.lib"  \
  --add-stdcall-underscore \
  --deterministic-libraries

cd "${root}/external/smackw32/lib/x86"
i686-w64-mingw32-dlltool \
  --dllname "smackw32.dll" \
  --input-def "smackw32.def" \
  --output-lib "smackw32.lib" \
  --add-stdcall-underscore \
  --deterministic-libraries

cd "${root}/src/brender/dll/x86"
#i686-w64-mingw32-dlltool \
#  --dllname "brcore1.dll" \
#  --input-def "../brcore1.def" \
#  --output-lib "brcore1.lib" \
#  --deterministic-libraries
rm -f *.dll *.exp *.obj
cl -NOLOGO -c brcore1.c -Fobrcore1.obj
link \
  -NOLOGO \
  -MACHINE:IX86 \
  brcore1.obj \
  -OUT:brcore1.dll \
  -IMPLIB:brcore1.lib \
  -DEF:../brcore1.def
rm -f *.dll *.exp *.obj

cd "${root}/src/brender/dll/x86"
#i686-w64-mingw32-dlltool \
#  --dllname "brhost1.dll" \
#  --input-def "../brhost1.def" \
#  --output-lib "brhost1.lib" \
#  --deterministic-libraries
rm -f *.dll *.exp *.obj
cl -NOLOGO -c brhost1.c -Fobrhost1.obj
link \
  -NOLOGO \
  -MACHINE:IX86 \
  brhost1.obj \
  -OUT:brhost1.dll \
  -IMPLIB:brhost1.lib \
  -DEF:../brhost1.def
rm -f *.dll *.exp *.obj

cd "${root}/src/brender/dll/x86"
#i686-w64-mingw32-dlltool \
#  --dllname "brpmap1.dll" \
#  --input-def "../brpmap1.def" \
#  --output-lib "brpmap1.lib" \
#  --deterministic-libraries
rm -f *.dll *.exp *.obj
cl -NOLOGO -c brpmap1.c -Fobrpmap1.obj
link \
  -NOLOGO \
  -MACHINE:IX86 \
  brpmap1.obj \
  -OUT:brpmap1.dll \
  -IMPLIB:brpmap1.lib \
  -DEF:../brpmap1.def
rm -f *.dll *.exp *.obj

cd "${root}/src/brender/dll/x64"
x86_64-w64-mingw32-dlltool \
  --dllname "brcore1.dll" \
  --input-def "../brcore1.def" \
  --output-lib "brcore1.lib" \
  --deterministic-libraries

cd "${root}/src/brender/dll/x64"
x86_64-w64-mingw32-dlltool \
  --dllname "brhost1.dll" \
  --input-def "../brhost1.def" \
  --output-lib "brhost1.lib"  \
  --deterministic-libraries

cd "${root}/src/brender/dll/x64"
x86_64-w64-mingw32-dlltool \
  --dllname "brpmap1.dll" \
  --input-def "../brpmap1.def" \
  --output-lib "brpmap1.lib"  \
  --deterministic-libraries


