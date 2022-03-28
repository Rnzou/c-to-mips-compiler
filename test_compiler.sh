bin/compiler compiler_tests/functions/call_constant_external.c \
    > working/output/compiled.txt


test="compiler_tests"
output="temp"

echo "----------------------------------------------------------------------------------"
echo "                            Cleaning previous result                              "
echo "----------------------------------------------------------------------------------"

make clean
rm -r ${output}

echo "----------------------------------------------------------------------------------"
echo "                             Compiling test compiler                              "
echo "----------------------------------------------------------------------------------"

make all
mkdir -p ${output}
# 弄一个sub directories https://www.javatpoint.com/linux-mkdir-p

COMPILER=bin/c_compiler

PASS_track=0
FAIL_track=0

for folder in ${test}/* ; do

echo "----------------------------------------------------------------------------------"
echo "Testing folder:" ${folder}
echo "----------------------------------------------------------------------------------"

    for i in ${folder}/*_driver.c ; do

        INDEX=$((INDEX+1));  
        TEST=$(basename $i _driver.c)
        TEST_FILE=${folder}/${TEST}.c

        ${COMPILER} -S ${TEST_FILE} -o ${output}/${TEST}.s

        mips-linux-gnu-gcc -mfp32 -o ${output}/${TEST}.o -c ${output}/${TEST}.s

        mips-linux-gnu-gcc -mfp32 -static -o ${output}/${TEST}_qemu ${output}/${TEST}.o ${folder}/${TEST}_driver.c

        qemu-mips ${output}/${TEST}_qemu


        RETURN=$?
        if [[ $RETURN -ne 0 ]]; then
            RESULT="FAIL"
            FAIL_track=$((FAIL_track+1))
            printf  "%s \n" $INDEX $TEST $RESULT     
        else
            RESULT="PASS"
            PASS_track=$((PASS_track+1))
            printf  "%s \n" $INDEX $TEST $RESULT     
        fi

    done

done

echo "----------------------------------------------------------------------------------"
echo "Testbench report:" ${folder}
echo "----------------------------------------------------------------------------------"
echo "Total tests:" ${INDEX}
echo "Passed tests:" ${PASS_track}
echo "Failed tests:" ${FAIL_track}





