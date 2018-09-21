compiler=$1
compilerflags=$2

mkdir buildoutput
mkdir buildoutput/objects
mkdir buildoutput/extras


############################## Compile all objects #############################
#without .c extension
compilefiles=(ProcessingGraph PGGraph PGImage PGNode)

#compile each file
for file in ${compilefiles[@]}
do
    #compile file
    $compiler $compilerflags -c $file.c -o buildoutput/objects/$file.o
done


############################# make the node folder #############################
mkdir buildoutput/extras/DefaultNodes
buildnodes=(PGExposureNode PGImageInputNode PGGraphOutputNode LibRawNode Tonemap)
for node in ${buildnodes[@]}
do
    $compiler $compilerflags -shared -fPIC DefaultNodes/$node.c  -o buildoutput/extras/DefaultNodes/$node.so
    if [ ! $? -eq 0 ]; then
        rm -rf buildoutput
        exit 1
    fi
done


exit 0