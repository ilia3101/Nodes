compiler=$1

mkdir buildoutput


############################ make libMemoryBank.so #############################
compilefiles=(MemoryBank.c)

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
$compiler buildoutput/*.o -shared -o buildoutput/libMemoryBank.so
if [ ! $? -eq 0 ]; then
    rm -rf buildoutput
    exit 1
fi

#don't need these anymore
rm buildoutput/*.o


exit 0