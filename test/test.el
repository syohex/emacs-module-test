;;; test.el --- dynamic module test

;; Copyright (C) 2015 by Syohei YOSHIDA

;; Author: Syohei YOSHIDA <syohex@gmail.com>

;; This program is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation, either version 3 of the License, or
;; (at your option) any later version.

;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.

;; You should have received a copy of the GNU General Public License
;; along with this program.  If not, see <http://www.gnu.org/licenses/>.

;;; Commentary:

;;; Code:

(require 'ert)
(require 'cl-lib)
(require 'module-test)

(ert-deftest call-closure ()
  "Call closure where is created in Emacs Lisp from C"
  (let ((n 0))
    (let ((fn (lambda ()
                (cl-incf n))))
      (should (= (module-test-call-closure fn) 1))
      (should (= (module-test-call-closure fn) 2))
      (should (= (module-test-call-closure fn) 3))
      (should (= (module-test-call-closure fn) 4))
      (should (= (module-test-call-closure fn) 5))

      (cl-loop repeat 100
               do
               (module-test-call-closure fn))
      (should (= n 105)))))

;;; test.el ends here
