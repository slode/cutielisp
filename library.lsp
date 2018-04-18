;;; CUTIELISP STANDARD LIBRARY

;;; CORE MACRO FEATURES
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

(defmacro (unwrap x)
  `(,@x))

;;; CORE FUNCTIONAL FEATURES
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

; y-combinator
(define Y
  (lambda (h)
    ((lambda (x) (x x))
     (lambda (g)
       (h (lambda args (apply (g g) args)))))))
 

;; FLOW CONTROL
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

(defmacro (cond . clauses)
  (if (pair? clauses)
    (let 
      ((first (car clauses))
       (rest (cdr clauses)))
      `(if ,(car first)
         (progn ,@(cdr first))
            (cond ,@rest)))
    nil))

;;; List stuff
(define (first sx) (car sx))
(define (rest sx) (cdr sx))
(define (caar x)  (car (car x)))
(define (caaar x) (car (car (car x))))
(define (cadr x)  (car (cdr x)))
(define (caadr x) (car (car (cdr x))))
(define (cdar x)  (cdr (car x)))
(define (cddar x) (cdr (cdr (car x))))
(define (cddr x)  (cdr (cdr x)))
(define (cdddr x) (cdr (cdr (cdr x))))

(define (length list)
  (if (eq? nil list)
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

(define (list-eq? a b) 
  (if (= (car a) (car b))
    (list-eq? (cdr a) (cdr b))
    nil))

(define (append a b) (foldr cons b a))

(define (nth pos list)
  (if (= 0 pos)
    (car list)
    (nth (- pos 1) (cdr list))))

  ;; Math & logic
(define e 2.718281828459045)
(define pi 3.14159)

(define eq =) ; this might be wrong.
(define (> a b) (if (< a b) nil T))
(define (abs x) (if (< x 0) (- 0 x) x))
(define (min x y) (if (> x y) y x))
(define (max x y) (if (< x y) y x))
(define (null? x) (eq? x nil))
(define (not a) (if a nil T))
(define (to-float a) (* a 1.0))
(define (rem a b)
  (- a (* b (/ a b))))

(define +
  (let ((old+ +))
    (lambda xs (foldl old+ 0 xs))))
(define *
  (let ((old* *))
    (lambda xs (foldl old* 1 xs))))


(define (exp a n)
  (if (> n 0)
    (exp-pos a n)
    (/ 1 (exp-pos a (- 0 n)))))

(define (exp-pos a n)
  (if (= 0 n)
    1
    (* a (exp a (- n 1)))))


;;; STRING OPERATIONS
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

(define string-concat
  (let ((old-sc string-concat))
    (lambda xs (foldl old-sc "" xs))))

;;; Basic tests
(define (fact n)
   (if (< n 2)
       1
     (* n (fact (- n 1)))))

(define (fib n)
  (if (> 1 n)
    1
    (+ (fib (- n 1)) (fib (- n 2))))) 

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

;; Merge-sort
(define (merge-sort list)
  (if (> list 2)
    list ;; return list
    (merge-lists
    (merge-sort (left-half list))
    (merge-sort (right-half list)))))

(define (right-half list)
  (last list (ceiling (/ (length list) 2))))

(define (left-half list)
  (ldiff list (right-half list)))

(define (sort-two a b)
  (and
    (print "a is " a " and b is " b)
    ((if (< a b)
      (cons a (cons b nil))
      (cons b (cons a nil))))))

(define (merge ls1 ls2)
  (cond 
    ((null? ls1) ls2)
    ((null? ls2) ls1)
    ((< (car ls1) (car ls2)) (cons (car ls1) (merge (cdr ls1) ls2)))
    (T (cons (car ls2) (merge ls1 (cdr ls2))))))

;;;
(define author "Stian Lode")
(define email "stian.lode@gmail.com")
