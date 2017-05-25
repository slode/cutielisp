(load "library.lsp")
(load "tests/test-lib.lsp")

(test-equal (length (list 1 2)) 2)
(test-equal (length (list 1 2 3 4 5)) 5)

(define (ss x) (* x x))
(test-equal (map ss (list 1 2)) (2 4))
