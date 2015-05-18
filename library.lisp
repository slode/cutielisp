(define (> a b)
  ;; Defines the greater-than operator
  (if (< a b) nil T))
(define (abs x) (if (< x 0) (- x) x))
(define (min x y) (if (> x y) y x))
(define (max x y) (min y x))

;; This is probably not correct
(defmacro (set! a b)
  (define a b))

(define (not a)
  (if a nil T))

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

(define (list . items)
  (foldr cons nil items))

(define (reverse list)
  (foldl (lambda (a x) (cons x a)) nil list))

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

(define (caar x) (car (car x)))
(define (cadr x) (car (cdr x)))
(define (cdar x) (cdr (car x)))
(define (cddr x) (cdr (cdr x)))

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

(define (cond . list)
  (if (pair? (car list))
    (if (eq? nil (caar list))
      (cdar list)
      (cond (cdr list)))
    nil))

(define (cond2 . list)
  (car list))

(define (cond3 . a)
  (if (car a)
    (if (pair? (car a))
      (if (eq? nil (caar a))
        (car (cdar a))
        (cond3 (cdr a)))
      nil)
    nil))

(defmacro (let defs . body)
  `((lambda ,(map car defs) ,@body)
    ,@(map cadr defs)))

(defmacro (ignore x)
  `(quote ,x))

(defmacro (when condition body)
    `(if ,condition (progn ,@body)))

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

(define +
  (let ((old+ +))
    (lambda xs (foldl old+ 0 xs))))
(define *
  (let ((old* *))
    (lambda xs (foldl old* 1 xs))))

(define fact
 (lambda (n)
   (if (< n 2)
       1
     (* n (fact (- n 1))))))
