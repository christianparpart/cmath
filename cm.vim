" Vim syntax file
" Language: CMath (cmath programming language)
" Maintainer: Christian Parpart
" Latest Revision: 12 Jan 2018

if exists("b:current_syntax")
	finish
endif

" ---------------------------------------------------------------------------------
"
" comments
syn keyword cmTodo contained TODO FIXME XXX NOTE BUG
syn match cmComment "#.*$" contains=cmTodo
syn region cmComment start=/(\*/ end=/\*)/ contains=cmTodo

" blocks
syn region cmBlock start="{" end="}" transparent fold

" keywords
syn keyword cmKeywords when else for in mod
syn keyword cmCoreFunctions print
syn keyword cmCoreFunctions exp sin cos tan
syn keyword cmCoreFunctions sqrt pow
syn keyword cmCoreFunctions sum product over for to
syn keyword cmCoreFunctions solve expand
syn keyword cmCoreVar e i pi infinity

" symbols
syn match cmOperator '{\|}'
syn match cmOperator '\[\|\]\|(\|)'
syn match cmOperator '[^=<>~\$\^\/\*]\zs\(->\|=>\|:\|:=\|=\^\|=\$\|=\~\|==\|!=\|<=\|>=\|<\|>\|!\|=\|+\|-\|\*\*\|\*\|^\)\ze[^=<>~\$\^\/\*]'

" types
syn keyword cmType N Z Q R C
syn keyword cmSpecial _
syn match cmType '\.\.\.'

" numbers
syn match cmNumber '\d\+'
syn match cmNumber '\d\+.\d'
syn keyword cmNumber true false

" identifiers
syn match cmIdent /[a-zA-Z_][a-zA-Z0-9_.]*\([a-zA-Z0-9_.]*:\)\@!/

" ---------------------------------------------------------------------------------

let b:current_syntax = "cm"

hi def link cmIdent         Identifier
hi def link cmTodo          Todo
hi def link cmComment       Comment
hi def link cmNumber        Constant
hi def link cmType          Type
hi def link cmBlock         Statement
hi def link cmKeywords      Keyword
hi def link cmOperator      Operator
hi def link cmCoreFunctions Statement
hi def link cmCoreVar       Constant
hi def link cmSpecial       Special

" possible link targets Todo, Comment, Constant, Type, Keyword, Statement, Special, PreProc, Identifier
