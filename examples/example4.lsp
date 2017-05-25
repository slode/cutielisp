(load "library.lsp")

(define (merge-sorted-lists ls1 ls2)
  (let
    ((val1 (car ls1))
     (val2 (car ls2))
     (rst1 (cdr ls1))
     (rst2 (cdr ls2)))
  (cond
    ((null? ls1) ls2)
    ((null? ls2) ls1)
    ((< val1 val2) (cons val1 (merge-sorted-lists rst1 ls2)))
    (T             (cons val2 (merge-sorted-lists ls1 rst2))))))

(print (merge-sorted-lists (list 1 4 5 9) (list 2 6))) 

(define (split-list l)
  (cond
    ((null? l) '(() ()))
    ((null? (cdr l)) (list l '()))
    (T 
      (let ((split-rest (split-list (cddr l))))
        (list (cons (car l) (car split-rest))
              (cons (cadr l) (cadr split-rest)))))))

(print (split-list (list 1 5 3 2 5 3 7)))
