compiler=$1
compilerflags=$2

mkdir buildoutput


############################ make ProcessingGraph.a ############################
#without .c extension
compilefiles=(JSONParser)

#compile each file
for file in ${compilefiles[@]}
do
    #compile file
    $compiler $compilerflags -c $file.c -o buildoutput/$file.o
    #add it to library archive ting
    ar rvs buildoutput/JSONParser.a buildoutput/$file.o
    if [ ! $? -eq 0 ]; then
        rm -rf buildoutput
        exit 1
    fi
done


#don't need these anymore
rm buildoutput/*.o


exit 0