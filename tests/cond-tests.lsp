(load "library.lsp")
(load "tests/test-lib.lsp")

(test-false (cond))

(test-false (cond
  (nil (print "output NIL 1"))
  (nil (print "output NIL 2"))
  (nil (print "output NIL 3"))
  ))

(test-false (cond
  (nil (print "output NIL 1"))
  ))

(test-true (cond
  (nil (print "output NIL 1"))
  (T (print "output T 1"))
  ))

(test-true (cond
  (T (print "output T 1"))
  (nil (print "output NIL 1"))
  (T (print "output T 2"))
  ))

(test-true (cond
  (nil (print "output NIL 1"))
  (T (print "output T 1"))
  (nil (print "output NIL 2"))
  ))

(print 
  (cond
    (nil 2) (T 3)))
;(merge-sorted-lists (list 3 4) (list 2))
;(merge-sorted-lists (3 4) (2))

