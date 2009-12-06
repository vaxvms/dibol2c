" Vim syntax file
" Language:	DIBOL
" Maintainer:	Nicolas DUPEUX <nicolas@dupeux.net>
" Last Change:	2001 Apr 28

" Remove any old syntax stuff hanging around
syn clear

" ignore case
syn case ignore

" preprocessor directive
syn match include ".INCLUDE"

syn match precondit ".IFDEF"
syn match precondit ".IFNDEF"
syn match precondit ".ENDC"

" identifier
syn match identifier "\<[a-zA-Z][a-zA-Z0-9$_]*\>"

" string
syn region string start=+"+ end=+"+
syn region string start=+'+ end=+'+

" number
syn match number "[0-9]*"

" conditionnal
syn keyword conditional if then else
syn keyword conditional using select endusing

" repeat
syn keyword repeat for from thru

" statements
syn keyword statement proc open display begin
syn keyword statement end call goto close
syn keyword statement stop xcall return 

" operators
syn keyword operator + - * /

" record
syn keyword structure record common

" comment
syn match comment ";.*$"
