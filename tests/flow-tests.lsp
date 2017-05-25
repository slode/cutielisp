(load "library.lsp")
(load "tests/test-lib.lsp")

(test-true (and))
(test-false (and T T T nil))
(test-false (and nil nil T nil))
(test-true (and T T T))
(test-true (and T T))
(test-true (and T))

(test-false (or nil nil nil))
(test-false (or nil nil))
(test-false (or nil))
(test-false (or))
(test-true (or T nil T))
(test-true (or nil nil T nil))
(test-true (and T T T))
(test-true (and T T))
(test-true (and T))


(test-false (when T nil))
