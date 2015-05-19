  ;; Defines the greater-than operator
(define (> a b) (if (< a b) nil T))
(define (abs x) (if (< x 0) (- 0 x) x))
(define (min x y) (if (> x y) y x))
(define (max x y) (min y x))
(define (null? x) (eq? x nil))
(define (not a) (if a nil T))

;;; List stuff
(define (first sx) (car sx))
(define (rest sx) (cdr sx))

(define (length list)
  (if (null list)
    0
    (+ 1 (length (rest list)))))

(define (list . items)
  (foldr cons nil items))

(define (reverse list)
  (foldl (lambda (a x) (cons x a)) nil list))

;;; Functional things
(define (foldl proc init list)
  (if list
      (foldl proc
             (proc init (car list))
             (cdr list))
      init))

(define (foldr proc init list)
  (if list
      (proc (car list)
            (foldr proc init (cdr list)))
      init))

(define (unary-map proc list)
  (foldr (lambda (x rest) (cons (proc x) rest))
         nil
         list))

(define (map proc . arg-lists)
  (if (car arg-lists)
      (cons (apply proc (unary-map car arg-lists))
            (apply map (cons proc
                             (unary-map cdr arg-lists))))
      nil))

(define (append a b) (foldr cons b a))
(define (nth pos list)
  (if (= 0 pos)
    (car list)
    (nth (- pos 1) (cdr list))))

(define (caar x)  (car (car x)))
(define (caaar x) (car (car (car x))))
(define (cadr x)  (car (cdr x)))
(define (caadr x) (car (car (cdr x))))
(define (cdar x)  (cdr (car x)))
(define (cddar x) (cdr (cdr (car x))))
(define (cddr x)  (cdr (cdr x)))
(define (cdddr x) (cdr (cdr (cdr x))))

(define (macro-expand expr)
  (if (not (pair? expr))
      expr
      (let ((keyword (car expr)))
        (cond ((equal? keyword 'QUOTE) expr)
        ((equal? keyword 'LAMBDA)
         (list 'LAMBDA
               (cadr expr)
         (macro-expand (caddr expr))))
        (else
         (let ((expander (get-macro-expander keyword)))
     (if expander
         (macro-expand (expander expr))
         (map macro-expand expr))))))))

;;; QUASIQUOTE
(defmacro (quasiquote x)
  (if (pair? x)
      (if (eq? (car x) 'unquote)
          (cadr x)
          (if (eq? (if (pair? (car x)) (caar x) nil) 'unquote-splicing)
              (list 'append
                    (cadr (car x))
                    (list 'quasiquote (cdr x)))
              (list 'cons
                    (list 'quasiquote (car x))
                    (list 'quasiquote (cdr x)))))
      (list 'quote x)))

(defmacro (let defs . body)
  `((lambda ,(map car defs) ,@body)
    ,@(map cadr defs)))

(defmacro (ignore x)
  `(quote ,x))

;; Usage: (when (< x 2) (set! x 2)) ??
(defmacro (when condition . body)
    `(if ,condition (progn ,@body) nil))

(defmacro (unless condition . body)
    `(if ,condition nil (progn ,@body)))

(defmacro (cond condition . body)
    `(if ,condition nil (progn ,@body)))

(defmacro (and-list body)
  `(if ,body
     (if (null? (car ,body))
       nil
       (and-list (cdr ,body)))
     T))

(define (and . body)
  (and-list body))

(defmacro (or-list body)
  `(if ,body
     (if (null? (car ,body))
       (and-list (cdr ,body))
       T)
     nil))

(define (or . body)
  (or-list body))

;;; String operations
(define (string-not-lessp a b)
  (string-lessp b a))

(define (string-greaterp a b) 
  (string-lessp b a))

;; Needs to return index of mismatch
(define (string-not-equal a b)
  (if (string-equal a b)
    nil
    (if (string-lessp a b)
      (string-lessp a b)
      (string-greaterp a b))))

(define string= string-equal)
(define string/= string-not-equal)
(define string< string-lessp)
(define string> string-greaterp)

;;; list based arithmetic
(define +
  (let ((old+ +))
    (lambda xs (foldl old+ 0 xs))))
(define *
  (let ((old* *))
    (lambda xs (foldl old* 1 xs))))

;;; Basic tests
(define (fact n)
   (if (< n 2)
       1
     (* n (fact (- n 1)))))

(define (fib n)
  (if (> 1 n)
    1
    (+ (fib (- n 1)) (fib (- n 2))))) 
