(define (test-true expr)
  (if (not expr)
    (print "test failed!")
    (print "test success!")))

(define (test-false expr)
  (test-true (not expr)))

(define (test-equal expr expected)
  (test-true (list-eq? expr expected)))
