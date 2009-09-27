// Copyright (c) 2005-2009 Jay Berkenbilt
//
// This file is part of qpdf.  This software may be distributed under
// the terms of version 2 of the Artistic License which may be found
// in the source distribution.  It is provided "as is" without express
// or implied warranty.

#ifndef __QPDFTOKENIZER_HH__
#define __QPDFTOKENIZER_HH__

#include <qpdf/DLL.hh>

#include <string>
#include <stdio.h>

class QPDFTokenizer
{
  public:
    enum token_type_e
    {
	tt_bad,
	tt_array_close,
	tt_array_open,
	tt_brace_close,
	tt_brace_open,
	tt_dict_close,
	tt_dict_open,
	tt_integer,
	tt_name,
	tt_real,
	tt_string,
	tt_null,
	tt_bool,
	tt_word,
    };

    class Token
    {
      public:
	Token() : type(tt_bad) {}

	Token(token_type_e type, std::string const& value) :
	    type(type),
	    value(value)
	{
	}

	Token(token_type_e type, std::string const& value,
	      std::string raw_value, std::string error_message) :
	    type(type),
	    value(value),
	    raw_value(raw_value),
	    error_message(error_message)
	{
	}
	token_type_e getType() const
	{
	    return this->type;
	}
	std::string const& getValue() const
	{
	    return this->value;
	}
	std::string const& getRawValue() const
	{
	    return this->raw_value;
	}
	std::string const& getErrorMessage() const
	{
	    return this->error_message;
	}
	bool operator==(Token const& rhs)
	{
	    // Ignore fields other than type and value
	    return ((this->type != tt_bad) &&
		    (this->type == rhs.type) &&
		    (this->value == rhs.value));
	}

      private:
	token_type_e type;
	std::string value;
	std::string raw_value;
	std::string error_message;
    };

    DLL_EXPORT
    QPDFTokenizer();

    // PDF files with version < 1.2 allowed the pound character
    // anywhere in a name.  Starting with version 1.2, the pound
    // character was allowed only when followed by two hexadecimal
    // digits.  This method should be called when parsing a PDF file
    // whose version is older than 1.2.
    DLL_EXPORT
    void allowPoundAnywhereInName();

    // Mode of operation:

    // Keep presenting characters and calling getToken() until
    // getToken() returns true.  When it does, be sure to check
    // unread_ch and to unread ch if it is true.

    // It these are called when a token is available, an exception
    // will be thrown.
    DLL_EXPORT
    void presentCharacter(char ch);
    DLL_EXPORT
    void presentEOF();

    // If a token is available, return true and initialize token with
    // the token, unread_char with whether or not we have to unread
    // the last character, and if unread_char, ch with the character
    // to unread.
    DLL_EXPORT
    bool getToken(Token& token, bool& unread_char, char& ch);

    // This function returns true of the current character is between
    // tokens (i.e., white space that is not part of a string) or is
    // part of a comment.  A tokenizing filter can call this to
    // determine whether to output the character.
    DLL_EXPORT
    bool betweenTokens();

  private:
    void reset();

    // Lexer state
    enum { st_top, st_in_comment, st_in_string, st_lt, st_gt,
	   st_literal, st_in_hexstring, st_token_ready } state;

    bool pound_special_in_name;

    // Current token accumulation
    token_type_e type;
    std::string val;
    std::string raw_val;
    std::string error_message;
    bool unread_char;
    char char_to_unread;

    // State for strings
    int string_depth;
    bool string_ignoring_newline;
    char bs_num_register[4];
    bool last_char_was_bs;
};

#endif // __QPDFTOKENIZER_HH__
