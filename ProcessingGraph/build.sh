compiler=$1

mkdir buildoutput


########################## make libProcessingGraph.so ##########################
compilefiles=(PGGraph.c PGImage.c PGNode.c)

#compile each file
for file in ${compilefiles[@]}
do
    $compiler -c -fPIC $file -o buildoutput/$file.o
    if [ ! $? -eq 0 ]; then
        rm -rf buildoutput
        exit 1
    fi
done

#create the library ting
$compiler buildoutput/*.o -shared -o buildoutput/libProcessingGraph.so
if [ ! $? -eq 0 ]; then
    rm -rf buildoutput
    exit 1
fi

#don't need these anymore
rm buildoutput/*.o



############################# make the node folder #############################
mkdir buildoutput/DefaultNodes
buildnodes=(PGExposureNode PGImageInputNode)
for node in ${buildnodes[@]}
do
    $compiler -shared -o buildoutput/DefaultNodes/$node.so -fPIC DefaultNodes/$node.c
    if [ ! $? -eq 0 ]; then
        rm -rf buildoutput
        exit 1
    fi
done



exit 0