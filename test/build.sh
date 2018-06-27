######################## Check if required things exist ########################
if [ ! -e libraw_r.a ]; then
	echo "libraw_r.a is required, add it to this folder."
	exit;
fi

######################## Variables and flags and stuff #########################
info=build_info.h
appname=ProcessingGraphApp
compiler=gcc
cppcompiler=g++
flags="-std=c99 -O3 -Wall"
if [[ "$OSTYPE" == "linux-gnu" ]]; then
	linkflags="-lstdc++ -lm -fopenmp"
elif [[ "$OSTYPE" == "darwin"* ]]; then
	linkflags="-lstdc++ -lm"
else
	echo Unknown os
fi



######################## Create build info header file #########################
echo Creating $info...
touch $info
echo "#ifndef _build_info_h_" > $info
echo "#define _build_info_h_" >> $info
echo >> $info
echo "#define BuildCCompiler" \"$($compiler --version | head -n 1)\" >> $info
echo "#define BuildCPPCompiler" \"$($cppcompiler --version | head -n 1)\">>$info
echo "#define BuildDate" \"$(date)\" >> $info
if [[ "$OSTYPE" == "linux-gnu" ]]; then
	. /etc/os-release
	echo "#define BuildSystem" \"$NAME" "$VERSION\" >> $info
elif [[ "$OSTYPE" == "darwin"* ]]; then
	echo "#define BuildSystem" \"$(sw_vers -productName)" "$(sw_vers \
	-productVersion)" ("$(sw_vers -buildVersion)")"\" >> $info
else
	echo Unknown OS
fi
echo >> $info
echo "#endif" >> $info
echo " "



############################# Find all source files ############################
cd ../
if [[ "$OSTYPE" == "linux-gnu" ]]; then
	src=$(find -name '*.c')
elif [[ "$OSTYPE" == "darwin"* ]]; then
	src=$(find . -name "*.c")
else
	echo Unknown os
fi
cd - > /dev/null

########################### Compile all source files ###########################
for file in $src
do
	echo Compiling $file
	$compiler -c $flags ../$file
done
echo " "

##################################### Link #####################################
echo Linking...
$compiler *.o libraw_r.a $linkflags -o $appname
echo " "
echo Done!

rm *.o
