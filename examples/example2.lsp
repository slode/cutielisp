(define counter 10)
(while (> counter 0) 
       (progn 
         (print "counter " counter) 
         (set! counter (- counter 1))))
