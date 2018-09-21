compiler=$1
compilerflags=$2

mkdir buildoutput
mkdir buildoutput/objects

############################## Compile all objects #############################
#without .c extension
compilefiles=(JSONParser)

#compile each file
for file in ${compilefiles[@]}
do
    #compile file
    $compiler $compilerflags -c $file.c -o buildoutput/objects/$file.o
done


exit 0