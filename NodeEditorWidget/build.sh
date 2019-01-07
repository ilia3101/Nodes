compiler=$1
compilerflags=$2

mkdir buildoutput
mkdir buildoutput/objects

############################## Compile all objects #############################
compilefiles=(NodeEditorWidget.c NENode.c)

#compile each file
cd buildoutput/objects
for file in ${compilefiles[@]}
do
    #compile file
    $compiler $compilerflags -c ../../$file
done
cd -

exit 0