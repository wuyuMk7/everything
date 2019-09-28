(define empty-str '())
(define one-element-str '(#\a))
(define odd-elements-str '(#\a #\b #\c #\d #\@ #\% #\&))
(define even-elements-str '(#\a #\b #\c #\d #\@ #\% #\& #\*))

(define (len mystr) 
    (if (null? mystr) 0 (+ 1 (len (cdr mystr)))))

;(display (len odd-elements-str))

(define (empty? mystr) (null? mystr))

;(display (empty? empty-str))

(define (compare mystr1 mystr2) 
    (cond
        ((and (null? mystr1) (null? mystr2)) 0)
        ((null? mystr1) -1)
        ((null? mystr2) 1)
        (else (cond ((char>? (car mystr1) (car mystr2)) 1)
                    ((char<? (car mystr1) (car mystr2)) -1)
                    (else (compare (cdr mystr1) (cdr mystr2)))
                    ))
        ))

;(display (compare odd-elements-str even-elements-str))

(define (find mystr ch)
    (define (find-index n mystr ch) 
        (cond ((null? mystr) -1) 
              ((char=? (car mystr) ch) n)
              (else (find-index (+ 1 n) (cdr mystr) ch))
              ))
    (find-index 0 mystr ch))

;(display (find even-elements-str #\@))

(define (take mystr n) 
    (cond ((null? mystr) '())
          ((<= n 0) '())
          (else (cons (car mystr) (take (cdr mystr) (- n 1))))
          ))

;(display (take odd-elements-str 5))

(define (drop mystr n) 
    (cond ((null? mystr) mystr)
          ((<= n 0) mystr)
          (else (drop (cdr mystr) (- n 1)))
          ))
      
;(display (drop odd-elements-str -6))

(define (slice mystr start end) 
    (if (< start 0) 
        (take mystr end) 
        (take (drop mystr start) (- end start))))

;(display (slice odd-elements-str 1 2))

(define (append mystr1 mystr2)
    (if (null? mystr1) 
        mystr2 
        (cons (car mystr1) (append (cdr mystr1) mystr2))
        ))

;(display (append odd-elements-str even-elements-str))

(define (odd mystr)
    (cond ((null? mystr) '())
          ((null? (cdr mystr)) mystr)
          (else (cons (car mystr) (odd (cdr (cdr mystr)))))
          ))
      
(display "- Test 1, input sequence is empty, output sequence is: ")
(display (odd empty-str))
(display "\n- Test 2, input sequence is a, output sequence is: ")
(display (odd one-element-str))
(display "\n- Test 3, input sequence is abcd@%&, output sequence is: ")
(display (odd odd-elements-str))
(display "\n- Test 4, input sequence is abcd@%&*, output sequence is: ")
(display (odd even-elements-str))
(display "\n")

