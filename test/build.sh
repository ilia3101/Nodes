# Build for linux - add libraw_a.a to this directory when building

appname=ProcessingGraphApp
flags="-std=c99 -O3 -Wall"
compiler=gcc
cppcompiler=g++

# find all C files
cd ../
src=$(find -name '*.c')
cd - > /dev/null

echo Compiler: $($compiler --version | head -n 1)
echo Flags: $flags

#create info file
# rm build_info.h
touch build_info.h
echo "#ifndef _build_info_h_" > build_info.h
echo "#define _build_info_h_" >> build_info.h
echo >> build_info.h
echo "#define BuildCCompiler" \"$($compiler --version | head -n 1)\" >> build_info.h
echo "#define BuildCPPCompiler" \"$($cppcompiler --version | head -n 1)\" >> build_info.h
echo "#define BuildDate" \"$(date)\" >> build_info.h
. /etc/os-release
echo "#define BuildSystem" \"$NAME" "$VERSION\" >> build_info.h
echo >> build_info.h
echo "#endif" >> build_info.h

echo " "

for file in $src
do
	echo Compiling $file
	$compiler -c $flags ../$file
done

echo " "

echo Linking...
$compiler *.o libraw_r.a -lstdc++ -lm -fopenmp -o $appname
echo " "
echo Done!

rm *.o
