function endbuild {
	echo " "
 	rm *.o > /dev/null
	rm build_info.h > /dev/null
	exit
}
function errormessage {
	echo ❌ $1
	endbuild
}
function successmessage {
	echo 😊 $1
}

######################## Variables and flags and stuff #########################
info=build_info.h
appname=ProcessingGraphApp
compiler=gcc
cppcompiler=g++
flags="-std=c99 -O3 -Wall"
#system specific
if [[ "$OSTYPE" == "linux-gnu" ]]; then
	linkflags="-lstdc++ -lm -fopenmp"
elif [[ "$OSTYPE" == "darwin"* ]]; then
	linkflags="-lstdc++ -lm"
	librawurl=https://www.libraw.org/data/LibRaw-0.19.0-Beta6-MacOSX.zip
	librawfolder=LibRaw-0.19.0-Beta6
else
	echo Unknown os
fi


######################## Check if required things exist ########################
if [ ! -e libraw_r.a ]; then
	errormessage "libraw_r.a is not present"
	if [[ "$OSTYPE" == "linux-gnu" ]]; then
		echo "Add libraw_r.a to this folder."
		exit;
	elif [[ "$OSTYPE" == "darwin"* ]]; then
		read -p "Download libraw_r.a? [y/n] " yn
		case $yn in
			[Yy]* ) echo "Downloading libraw_r.a ..."
					wget -O ./libraw.zip $librawurl > /dev/null
					unzip libraw.zip > /dev/null
					cp ./$librawfolder/lib/libraw_r.a ./
					rm libraw.zip > /dev/null
					rm -rf $librawfolder > /dev/null
					echo "Downloaded"
					break;;
			[Nn]* ) echo "Put libraw_r.a in this folder thanks bye."; exit;;
			* ) exit;;
		esac
	else
		echo "Unknown os"
		exit;
	fi
else
	successmessage "libraw_r.a exists"
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
echo "Compiling source files..."
for file in $src
do
	$compiler -c $flags ../$file
	if [ $? -eq 0 ]; then
		successmessage "compiled $file"
	else
		errormessage "$file did not compile"
	fi
done
echo " "


##################################### Link #####################################
echo Linking...
$compiler *.o libraw_r.a $linkflags -o $appname
if [ $? -eq 0 ]; then
	successmessage "done!"
else
	errormessage "failed to link"
fi

endbuild
