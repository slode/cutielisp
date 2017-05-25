; loading std library
(load "library.lsp")

; calculating exponents
(let 
  ((c 10))
  (while (> c -10)
         (progn
           (print "e^" c " = " (exp e c))
           (set! c (- c 1)))))
