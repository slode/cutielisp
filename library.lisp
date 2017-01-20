  ;; Defines the greater-than operator
(define (> a b) (if (< a b) nil T))
(define (abs x) (if (< x 0) (- 0 x) x))
(define (min x y) (if (> x y) y x))
(define (max x y) (min y x))
(define (null? x) (eq? x nil))
(define (not a) (if a nil T))

(defmacro (eval-cond c . cx)
  `(if ,c
    (progn ,@cx)
    nil))

(defmacro (unwrap x)
  `(,@x))

(defmacro (cond-list cases)
  `(if ,cases
      (if (caar ,cases)
           (progn ,@cases)
           (cond-list (cdr ,cases)))
      nil))

(define (cond . b)
  (cond-list b))

;;; List stuff
(define (first sx) (car sx))
(define (rest sx) (cdr sx))

(define (length list)
  (if (null list)
    0
    (+ 1 (length (rest list)))))

(define (last list)
  (if (cdr list)
    (last (cdr list))
    (car list)))

(define (list . items)
  (foldr cons nil items))

(define (reverse list)
  (foldl (lambda (a x) (cons x a)) nil list))

(defmacro (append . ls)
  `(foldr cons () ,@ls))

(define (circular lst)
  (set! (cdr (last lst)) lst))

(define (merge proc ls1 ls2 f)
  (foldr 
    (lambda(a b) 
      (if (f a b)
        (cons a b)
        (cons b a)))
    ls1 ls2))

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
    `(if ,condition
       (progn ,@body)
       nil))

(defmacro (unless condition . body)
    `(if ,condition 
       nil 
       (progn ,@body)))

(defmacro (for-each fn seq)
  `(progn (map nil ,fn ,seq) t))

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
       (or-list (cdr ,body))
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

(define Y
  (lambda (h)
    ((lambda (x) (x x))
     (lambda (g)
       (h (lambda args (apply (g g) args)))))))
 
(define facy
  (Y
    (lambda (f)
      (lambda (x)
        (if (< x 2)
            1
            (* x (f (- x 1))))))))
 
(define fiby
  (Y
    (lambda (f)
      (lambda (x)
        (if (< x 2)
            x
            (+ (f (- x 1)) (f (- x 2))))))))

(define (ceiling x) x)

(define (merge-sort list)
  (if (small list) list
    (merge-lists
    (merge-sort (left-half list))
    (merge-sort (right-half list)))))

(define (small list)
  (or (eq (length list) 0) (eq (length list) 1)))

(define (right-half list)
  (last list (ceiling (/ (length list) 2))))

(define (left-half list)
  (ldiff list (right-half list)))

(define (merge-lists list1 list2)
  (merge 'list list1 list2 '<))
