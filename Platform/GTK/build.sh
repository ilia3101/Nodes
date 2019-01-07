# Rule: static linking as much as possible

if [[ "$OSTYPE" == "linux-gnu" ]]; then buildstart=$(date +%s.%N); fi

# You won't get this joke
successemojay=🐸
failemojay=❌

function endbuild {
	echo " "
 	rm *.o 2> /dev/null
	rm build_info.h 2> /dev/null
	rm .output 2> /dev/null
	exit
}
function errormessage {
	if [[ "$OSTYPE" == "darwin"* ]]; then printf "$failemojay   $1\n"
	else printf "$failemojay  $1\n"; fi
}
function death {
	errormessage "$1"
	rm .output
	endbuild
}
function successmessage {
	if [[ "$OSTYPE" == "darwin"* ]]; then printf "$successemojay  $1\n"
	else printf "$successemojay  $1\n"; fi
}

######################## Variables and flags and stuff #########################
appname=Nodes
compiler=gcc
cppcompiler=g++
compilerflags="-Wall -Wextra -std=c99 -O3 -Ofast -m64 -DNDEBUG"
#system specific
if [[ "$OSTYPE" == "linux-gnu" ]]; then #linux
	linkflags="-lstdc++ -lm -fopenmp -lOpenCL -lOpenGL -lGLX -ldl -lGLEW -lGLU `pkg-config --libs gtk+-3.0` -rdynamic"
elif [[ "$OSTYPE" == "darwin"* ]]; then #mac
	linkflags="-lstdc++ -lm -framework OpenGL -framework OpenGL -ldl -rdynamic"
else
	echo "Unknown OS"
fi


######################## Check if required things exist ########################
echo -e "\nChecking a few things..."
if [ ! -e libraw_r.a ]; then
	if [[ "$OSTYPE" == "linux-gnu" ]]; then
		errormessage "libraw_r.a is not present, add libraw_r.a to this folder"
		echo "Compile instructions: https://www.libraw.org/docs/Install-LibRaw.html"
		exit
	elif [[ "$OSTYPE" == "darwin"* ]]; then
		errormessage "libraw_r.a is not present"
		read -p "Download libraw_r.a? [y/n] " yn
		case $yn in
			[Yy]* )
				echo "Downloading libraw_r.a ..."
				wget -O ./libraw.zip https://www.libraw.org/data/LibRaw-0.19.0-MacOSX.zip &> /dev/null
				unzip libraw.zip &> /dev/null
				librawfolder=LibRaw-0.19.0
				cp ./$librawfolder/lib/libraw_r.a ./
				rm libraw.zip &> /dev/null
				rm -rf $librawfolder &> /dev/null
				echo "Downloaded"
				break;;
			[Nn]* ) echo "Put libraw_r.a in this folder thanks bye."; exit;;
			* ) exit;;
		esac
	else
		death "Unknown OS"
		exit;
	fi
else
	successmessage "libraw_r.a exists"
fi
#check if compiler exists
if hash $compiler 2>/dev/null; then
	successmessage "C compiler \"$compiler\" exists"
else
	death "C compiler \"$compiler\" does not exist"
fi
echo " "


######################## Create build info header file #########################
echo Creating build_info.h...
touch build_info.h
echo "#ifndef _build_info_h_" > build_info.h
echo "#define _build_info_h_" >> build_info.h
echo >> build_info.h
echo "#define BuildCCompiler \"$($compiler --version | head -n 1)\"" >> build_info.h
echo "#define BuildCPPCompiler \"$($cppcompiler --version| head -n 1)\"" >> build_info.h
echo "#define BuildDate" \"$(date)\" >> build_info.h
if [[ "$OSTYPE" == "linux-gnu" ]]; then
	. /etc/os-release
	echo "#define BuildSystem" \"$NAME" "$VERSION\" >> build_info.h
elif [[ "$OSTYPE" == "darwin"* ]]; then
	echo "#define BuildSystem" \"$(sw_vers -productName)" "$(sw_vers \
	-productVersion)" ("$(sw_vers -buildVersion)")"\" >> build_info.h
else
	death "Unknown OS"
fi
echo >> build_info.h
echo "#endif" >> build_info.h
successmessage "created build_info.h"
echo " "



########################## Create buildoutput folder ###########################
rm -rf buildoutput &> /dev/null
echo Creating buildoutput folder...
#this just won't fail will it
mkdir buildoutput
mkdir buildoutput/result
mkdir buildoutput/objects
successmessage "created buildoutput folders"
echo " "


############################ Build all the libraries ###########################
libraries=(ProcessingGraph MemoryBank JSONParser GraphJSON UILibrary NodeEditorWidget)
# Every library must have build.sh script, which does:
# 1. create a buildoutput folder
# 2. place all .o objects to link in buildoutput/objects
# 3. (optional) create 'extras' folder, the contents of which will be placed in
#     	the same directory as the application executable itself
# 4. returns 0 on success: 'exit 0'
echo "Building libraries ..."
for library in ${libraries[@]}
do
	if [[ "$OSTYPE" == "linux-gnu" ]]; then startfile=$(date +%s.%N); fi
	cd ../../$library > /dev/null
	touch .output
	# Each library has its own build script whcih outputs to a buildoutput folder
	./build.sh $compiler $compilerflags #&> .output
	if [ $? -eq 0 ]; then
		if [[ "$OSTYPE" == "linux-gnu" ]]; then
			ftime=$(echo "scale=0; ($(date +%s.%N)-$startfile)*10000.0/10.0" |bc -l)
			successmessage "built $library in $ftime ms"
		else
			successmessage "built $library"
		fi
	else
		death "$library did not compile:\n\x1b[93;41m$(cat .output)\x1b[0m"
	fi
	rm .output
	#copy everything built to main build folder
	for f in buildoutput/*; do 
		cp -RT $f $OLDPWD/$f
	done
	rm -rf buildoutput &> /dev/null
	cd - > /dev/null
done
echo " "


######## Now compile C files that just need to be compiled on their own ########
# echo "Compiling C files ..."
# # without .c extension
# cfiles=(main bitmap)
# touch .output
# for file in ${cfiles[@]}
# do
# 	if [[ "$OSTYPE" == "linux-gnu" ]]; then startfile=$(date +%s.%N); fi
# 	$compiler $compilerflags -c $file.c -o buildoutput/objects/$file.o &> .output
# 	if [ $? -eq 0 ]; then
# 		if [[ "$OSTYPE" == "linux-gnu" ]]; then
# 			ftime=$(echo "scale=0; ($(date +%s.%N)-$startfile)*10000.0/10.0" |bc -l)
# 			successmessage "compiled $file in $ftime ms"
# 		else
# 			successmessage "compiled $file"
# 		fi
# 	else
# 		death "$file did not compile:\n\x1b[93;41m$(cat .output)\x1b[0m"
# 	fi
# done
# rm .output
# echo " "

$compiler -O3 -c main.c `pkg-config --cflags gtk+-3.0` -o buildoutput/objects/main.o



##################################### Link #####################################
echo "Linking..."
cd buildoutput &> /dev/null
# create link command
linkcommand="$compiler -o result/$appname"
# linkcommand+=" -lraw"
linkcommand+=" objects/*.o"
linkcommand+=" ../libraw_r.a"
linkcommand+=" "$linkflags

$linkcommand #&> /dev/null
if [ $? -eq 0 ]; then
	successmessage "done!"
else
	death "failed to link"
fi
cd - &> /dev/null



################## Move all things in extras folder to result ##################
echo "Copying extra files to application directory..."
cd buildoutput/extras &> /dev/null
for f in *; do
	echo $f;
	cp -RT $f ../result/$f
done
cd - &> /dev/null



if [[ "$OSTYPE" == "linux-gnu" ]]; then
buildtime=$(echo "scale=0; ($(date +%s.%N)-$buildstart)*10000.0/10.0" | bc -l)
echo -e "\nbuild completed in $buildtime""ms." #" See results in buildoutput/result folder."
fi

endbuild
