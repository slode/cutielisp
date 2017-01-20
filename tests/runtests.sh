echo "Running unit tests:"
echo $(date)

for i in tests/*_tests
do
    if test -f $i
    then
        if $VALGRIND ./$i 2>&1 >> tests/tests.log
        then
            echo $i PASS
        else
            echo "ERROR in test $i: here's tests/tests.log"
            echo "------"
            tail tests/tests.log
        fi
    fi
done

echo ""
