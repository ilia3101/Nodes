compiler=$1
compilerflags=$2

mkdir buildoutput


############################ make ProcessingGraph.a ############################
#without .c extension
compilefiles=(ProcessingGraph PGGraph PGImage PGNode)

#compile each file
for file in ${compilefiles[@]}
do
    #compile file
    $compiler $compilerflags -c $file.c -o buildoutput/$file.o
    #add it to library archive ting
    ar rvs buildoutput/ProcessingGraph.a buildoutput/$file.o
    if [ ! $? -eq 0 ]; then
        rm -rf buildoutput
        exit 1
    fi
done


#don't need these anymore
rm buildoutput/*.o


############################# make the node folder #############################
mkdir buildoutput/DefaultNodes
buildnodes=(PGExposureNode PGImageInputNode PGGraphOutputNode LibRawNode Tonemap)
for node in ${buildnodes[@]}
do
    $compiler $compilerflags -shared -fPIC DefaultNodes/$node.c  -o buildoutput/DefaultNodes/$node.so
    if [ ! $? -eq 0 ]; then
        rm -rf buildoutput
        exit 1
    fi
done



exit 0