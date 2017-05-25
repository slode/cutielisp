echo "Running unit tests:"
echo "Test log" > tests/tests.log
echo "CPP TESTS" >> tests/tests.log

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

echo "LISP TESTS" >> tests/tests.log

for i in tests/*-tests.lsp
do
    if test -f $i
    then
        echo $i >> tests/tests.log
        if ./bin/cutie ./$i 2>&1 | tee -a tests/tests.log | grep "test failed!"
        then
            echo $i FAILED
        else
            echo $i PASS
        fi
    fi
done
echo ""
