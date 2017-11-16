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

(define (split-list l)
  (cond
    ((null? l) '(() ()))
    ((null? (cdr l)) (list l '()))
    (T
      (let ((split-rest (split-list (cddr l))))
        (list (cons (car l) (car split-rest))
              (cons (cadr l) (cadr split-rest)))))))

(define (merge-sort l)
  (cond
    ((null? l) '())
    ((null? (cdr l)) l)
    (T
      (let
        ((l-split (split-list l)))
        (merge-sorted-lists
          (merge-sort (car l-split))
          (merge-sort (cadr l-split)))
     ))))

(print
  (merge-sort
    (list 7 8 1 1 3 2 57 8 1 1 3 2 57 8 1 1 3 2 57 8 1 1 3 2 57 8 1 1 3 2 57 8
          1 1 3 2 57 8 1 1 3 2 57 8 1 1 3 2 57 8 1 1 3 2 57 8 1 1 3 2 57 8 1 1
          3 2 57 8 1 1 3 2 57 8 1 1 3 2 57 8 1 1 3 2 57 8 1 1 3 2 57 8 1 1 3 2
          57 8 1 1 3 2 57 8 1 1 3 2 57 8 1 1 3 2 57 8 1 1 3 2 57 8 1 1 3 2 57 8
          1 1 3 2 57 8 1 1 3 2 57 8 1 1 3 2 57 8 1 1 3 2 5)))
