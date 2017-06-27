
enum Tag{
	// special
	err,
	end,
	// identifier
	id,
	// const
	const_num,
	const_str,
	const_ch,
	// keyword
	kw_int,
	kw_char,
	kw_void,
	kw_extern,
	kw_if,
	kw_else,
	kw_switch,
	kw_case,
	kw_default,
	kw_while,
	kw_do,
	kw_for,
	kw_break,
	kw_continue,
	kw_return,
	// operator&semicolon
	add,// +
	sub,// -
	mul,// *
	div,// /
	mod,// %
	inc,// ++
	dec,// --
	bool_not,// !
	bool_and,// &&
	bool_or,// ||
	neg,// ~
	bit_and,// &
	bit_or,// |
	assign,// =
	gt,// >
	ge,// >=
	lt,// <
	le,// <=
	equ,// ==
	nequ,// !=
	comma,// ,
	colon,// :
	semicolon,// ;
	lparen,// (
	rparen,// )
	lbrace,// {
	rbrace,// }
	lbrack,// [
	rbrack,// ]
}
