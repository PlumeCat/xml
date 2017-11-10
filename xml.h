#ifndef _XML_H
#define _XML_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <stack>

using namespace std;


namespace util
{
    // split a string into a series of tokens
    void split(const string& str, char c, vector<string>& tokens)
    {
        int s = 0;
        int e = str.find(c, 0);
        while (true)
        {
            if (e - s > 0)
            {
                tokens.push_back(str.substr(s, e-s));
            }

            s = e+1;
            e = str.find(c, s);

            if (e == string::npos)
            {
                tokens.push_back(str.substr(s, str.size()-s));
                break;
            }
        }
    }

    void split(const string& str, const string& c, vector<string>& tokens)
    {
        // get iterators for c
        auto c1 = c.begin();
        auto c2 = c.end();

        auto s = str.begin();
        while (s != str.end())
        {

        }
    }

    // split a string into a series of tokens
    void split(const string::iterator& begin, const string::iterator& end, vector<pair<string::iterator, string::iterator>>& delimiters)
    {
        auto current_start = begin;
        auto current_pos = current_start;

        while (current_pos != end)
        {
            if (*current_pos == ' ')
            {
                delimiters.push_back(make_pair(current_start, current_pos));
                current_pos++;
                current_start = current_pos;
            }
            else
            {
                current_pos++;
            }
        }

        delimiters.push_back(make_pair(current_start, current_pos));
    }

    // text file utility
    void read_text_file(const string& fname, vector<string>& lines)
    {
        ifstream file(fname);
        string line;
        while (getline(file, line))
        {
            lines.push_back(line);
        }
    }
    string read_text_file(const string& fname)
    {
        vector<string> lines;
        read_text_file(fname, lines);
        string source;
        for (auto& line : lines)
        {
            source += line + "\n";
        }
        return source;
    }

};

namespace xml
{
    struct Element
    {
        string tag_name = "";
        int generation = 0;
        vector<string> text = "";
        vector<string> attributes;
        vector<Element> children;
    };

    /*
        the following characters must be escaped
            "   &quot;
            '   &apos;
            <   &lt;
            >   &gt;
            &   &amp;
    */

    bool is_whitespace(const string::iterator& it)
    {
        const string whitespace = " \r\n\t";
        return find(whitespace.begin(), whitespace.end(), *it) != whitespace.end();
    }
    string::iterator read_whitespace(const string::iterator& begin, const string::iterator& end)
    {
        for (auto it = begin; is_whitespace(it) && it < end; it++);
    }

    void read_element(const string& src, Element& root)
    {
        if (src[0] != '<')
        {
            throw runtime_error("read_element() could not find STag" + src.substr(0, 10));
        }

        auto end_stag = src.find('>');
        if (end_stag == string::npos)
        {
            throw runtime_error("read_element() could not find complete STag" + src.substr(0, 10));
        }

        if (src[end_stag-1] == '/')
        {
            // is self closed
            vector<string> tokens;
            util::split(src.substr(1, src.size()-3));
        }
        else
        {
            vector<string> tokens;
            util::split(src.substr(1, src.size()-2));
        }
    }

    // basic xml parser
    // only supports utf8 encoding and won't handle multi-byte characters
    void read(const string& fname, Element& root)
    {
        string source = util::read_text_file(fname);
        auto current_pos = source.begin();

        // check for xml declaration
        if (source.substr(0, 2) == "<?")
        {
            auto close_decl = source.find("?>", 2);
            if (close_decl == string::npos)
            {
                throw runtime_error("declaration was not closed?");
            }

            string decl = source.substr(2, close_decl-2);

            // vector<string> decl_tokens;
            // split(decl, ' ', decl_tokens);
            current_pos += close_decl + 2;
        }

        if (*current_pos != '<')
        {
            throw runtime_error("could not find root element opening tag");
        }

        Element root;
        root.tag_name = "xml";
        read_element(string(current_pos, source.end()), root);
    }
};



// namespace parse
// {
//     struct rule
//     {
//         rule operator + (const rule& r) { return rule(); }
//         rule operator | (const rule& r) { return rule(); }
//         rule operator - (const rule& r) { return rule(); }
//     };
//     struct sequence : rule
//     {
//     };
//     struct required : rule
//     {
//     };
//     struct optional : rule
//     {
//     };
//     struct repeat : rule
//     {
//     };
//     struct repeat1 : rule
//     {
//     };
//     struct literal : rule
//     {
//     };
//     struct char_from : rule
//     {
//     };
//     typedef literal l;

//     template<typename R>
//     void parse(const string& fname)
//     {
//         string source = read_text_file(fname);
//         auto begin = source.begin();
//         auto end = source.end();
//     }


//     void parse_postcode()
//     {
//         auto postcode = start_section + end_section;
//     }


//     void parse_xml()
//     {
//         // auto ch = char_from("\t\r\n"
//         //                     " !\"#$%&'()*+,-./" // [x20-x2F]
//         //                     "0123456789:;<=>?"  // [x30-x3F]
//         //                     "@ABCDEFGHIJKLMNO"  // [x40-x4F]
//         //                     "PQRSTUVWXYZ[\\]^_" // [x50-x5F]
//         //                     "`abcdefghijklmno"  // [x60-x6F]
//         //                     "pqrstuvwxyz{|}~␡" // [x70-x7F]
//         //                     "" // [x80-x8F]
//         //                     "" // [x90-x9F]
//         //                     " ¡¢£¤¥¦§¨©ª«¬ ®¯"  // [xA0-xAF]
//         //                     "°±²³´µ¶·¸¹º»¼½¾¿"  // [xB0-xBF]
//         //                     "ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏ"  // [xC0-xCF]
//         //                     "ÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞß"  // [xD0-xDF]
//         //                     "àáâãäåæçèéêëìíîï"  // [xE0-xEF]
//         //                     "ðñòóôõö÷øùúûüýþÿ");// [xF0-xFF]
//         //                     // fuck the rest of unicode tbh
//         //                     // TODO: make a "unicode range" rule as well as a "xml" rule
//         //                     // TODO: handle unicode tbh
//         // auto lowercase = char_from("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
//         // auto uppercase = char_from("abcdefghijklmnopqrstuvwxyz");
//         // auto whitespace = repeat1(char_from(" \n\r\t"));
//         // auto digit = char_from("0123456789");



//         // auto cd_end;
//         // auto char_data;
//         // auto cd_start;
//         // auto cd_sect;
//         // auto attribute_value = (l("\"") + repeat() + l("\"")) | (l("'") + repeat() + l("'");
//         // auto attribute = name + eq + attribute_value;
//         // auto content = optional(char_data) + repeat((element | reference | cd_sect | processor_instruction | comment) + optional(char_data));
//         // auto start_tag = l("<") + name + repeat(whitespace + attribute) + optional(whitespace) + l(">");
//         // auto end_tag = l("</") + name + optional(whitespace) + l(">");
//         // auto empty_tag = l("<") + name + repeat(whitespace + attribute) + optional(whitespace) + l("/>");
//         // auto element = empty_tag | (start_tag + content + end_tag);

//         // auto comment = l("<!--") + repeat(ch) - l("--") + l("-->");
//         // auto processor_instruction = l("<?") + processor_instruction_target + optional(whitespace + optional(repeat(ch) - l("?>"))) + literal("?>");

//         // auto name_token = repeat1(name_char);
//         // auto name_start_char = l(":") | uppercase | lowercase | "_" | char_from("ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõöøùúûüýþÿ");
//         // auto name_char = name_start_char | l("-") | l(".") | digit | l("·");
//         // auto name = name_start_char + repeat(name_char);

//         // auto markupdecl = elementdecl | att_list_decl | entity_decl | notation_decl | processor_instruction | comment;
//         // auto pe_reference = l("%") + name + ";";
//         // auto decl_sep = pe_reference | whitespace;
//         // auto int_subset = repeat(markupdecl | decl_sep);

//         // auto misc = comment | processor_instruction | whitespace;
//         // auto version_num = l("1.") + digit;
//         // auto version_info = whitespace + l("version") + equal + ((l("'") + version_num + l("'")) | (l("\"") + version_num + l("\"")));
//         // auto doctypedecl = l("<!DOCTYPE") + whitespace + name + optional(whitespace + external_id) + optional(whitespace) + optional(l("[") + int_subset + l("]") + optional(whitespace)) + literal(">");
//         // auto equal = whitespace + l("=") + whitespace;
//         // auto sd_decl = whitespace + l("standalone") + equal + (
//         //     (l("'")  + (l("yes") | l("no")) + l("'")) |
//         //     (l("\"") + (l("yes") | l("no")) + l("\""))
//         // );
//         // auto xmldecl = l("<?xml?") + version_info + optional(encoding_decl) + optional(sd_decl) + optional(whitespace) + l("?>");
//         // auto prolog = optional(xmldecl) + repeated(misc) + optional(doctypedecl + repeated(misc));
//         // auto document = prolog + element + repeat(misc);

//     }

//     namespace xml
//     {
//     };
// };


#endif


/*
We don't currently support
    - entity declarations   "<!ENTITY ... >"
    - conditionals          "<![ ... ]]>"
*/

/*
Xml Document

Document, Prolog and doctype
    [1]     document            ::=      prolog element Misc*
    [22]    prolog              ::=      XMLDecl? Misc* (doctypedecl Misc*)?
        [23]    XMLDecl             ::=      '<?xml' VersionInfo EncodingDecl? SDDecl? S? '?>'
        [32]    SDDecl              ::=      S 'standalone' Eq (("'" ('yes' | 'no') "'") | ('"' ('yes' | 'no') '"'))
        [28]    doctypedecl         ::=      '<!DOCTYPE' S Name (S ExternalID)? S? ('[' intSubset ']' S?)? '>'
    [24]    VersionInfo         ::=      S 'version' Eq ("'" VersionNum "'" | '"' VersionNum '"')
    [25]    Eq                  ::=      S? '=' S?
        [26]    VersionNum          ::=      '1.' [0-9]+
        [27]    Misc                ::=      Comment | PI | S
    [28b]   intSubset           ::=      (markupdecl | DeclSep)*
    [28a]   DeclSep             ::=      PEReference | S
        [69]    PEReference         ::=      '%' Name ';'
    [29]    markupdecl          ::=      elementdecl | AttlistDecl | EntityDecl | NotationDecl | PI | Comment


Chars and Whitespace
    [2]     Char                ::=      #x9 | #xA | #xD | [#x20-#xD7FF] | [#xE000-#xFFFD] | [#x10000-#x10FFFF]
                                            any unicode char except "surrogate blocks", FFFE, FFFF
    [3]     S                   ::=      (#x20 | #x9 | #xD | #xA)+
                                            whitespace: ' ', '\r', '\n', '\t'

Names and Tokens
    [5]     Name                ::=      NameStartChar (NameChar)*
        [4a]    NameChar            ::=      NameStartChar | "-" | "." | [0-9] | #xB7 | [#x0300-#x036F] | [#x203F-#x2040]
        [4]     NameStartChar       ::=      ":" | [A-Z] | "_" | [a-z] | [#xC0-#xD6] | [#xD8-#xF6] | [#xF8-#x2FF] | [#x370-#x37D] | [#x37F-#x1FFF] | [#x200C-#x200D] | [#x2070-#x218F] | [#x2C00-#x2FEF] | [#x3001-#xD7FF] | [#xF900-#xFDCF] | [#xFDF0-#xFFFD] | [#x10000-#xEFFFF]
        [6]     ?? Names               ::=      Name (#x20 Name)*
    [7]     Nmtoken             ::=      (NameChar)+
        [8]     ?? Nmtokens            ::=      Nmtoken (#x20 Nmtoken)*

Comment, Processing instructions
    [15]    Comment             ::=      '<!--' (   (Char - '-') |  ('-' (Char - '-')   ))* '-->'
    [16]    PI                  ::=      '<?' PITarget (  S (   Char* - (Char* '?>' Char*)   )  )? '?>'
        [17]    PITarget            ::=      Name - (('X' | 'x') ('M' | 'm') ('L' | 'l'))

Element
    [39]    element             ::=      EmptyElemTag | STag content ETag
    [43]    content             ::=      CharData? ((element | Reference | CDSect | PI | Comment) CharData?)*
        [40]    STag                ::=      '<' Name (S Attribute)* S? '>'
        [14]    CharData            ::=      [^<&]* - ([^<&]* ']]>' [^<&]*)
        [42]    ETag                ::=      '</' Name S? '>'
        [44]    EmptyElemTag        ::=      '<' Name (S Attribute)* S? '/>'
            [41]    Attribute           ::=      Name Eq AttValue
            [10]    AttValue            ::=      '"' ([^<&"] | Reference)* '"' |  "'" ([^<&'] | Reference)* "'"

        [18]    CDSect     ::=      CDStart CData CDEnd
            [19]    CDStart    ::=      '<![CDATA['
            [20]    CData      ::=      (Char* - (Char* ']]>' Char*))
            [21]    CDEnd      ::=      ']]>'


Element declarations
    [45]    elementdecl         ::=      '<!ELEMENT' S Name S contentspec S? '>'
        [46]    contentspec         ::=      'EMPTY' | 'ANY' | Mixed | children
        [47]    children            ::=      (choice | seq) ('?' | '*' | '+')?
        [49]    choice              ::=      '(' S? cp ( S? '|' S? cp )+ S? ')'  [VC: Proper Group/PE Nesting]
        [50]    seq                 ::=      '(' S? cp ( S? ',' S? cp )* S? ')'  [VC: Proper Group/PE Nesting]
        [48]    cp                  ::=      (Name | choice | seq) ('?' | '*' | '+')?
        [51]    Mixed               ::=      '(' S? '#PCDATA' (S? '|' S? Name)* S? ')*' | '(' S? '#PCDATA' S? ')'
    [52]    AttlistDecl         ::=      '<!ATTLIST' S Name AttDef* S? '>'
        [53]    AttDef              ::=      S Name S AttType S DefaultDecl
            [54]    AttType             ::=      StringType | TokenizedType | EnumeratedType
                [55]    StringType          ::=      'CDATA'
                [56]    TokenizedType       ::=      'ID' | 'IDREF' | 'IDREFS' | 'ENTITY' | 'ENTITIES' | 'NMTOKEN' | 'NMTOKENS'
                [57]    EnumeratedType      ::=      NotationType | Enumeration
                    [58]    NotationType        ::=      'NOTATION' S '(' S? Name (S? '|' S? Name)* S? ')'
                    [59]    Enumeration         ::=      '(' S? Nmtoken (S? '|' S? Nmtoken)* S? ')'
            [60]    DefaultDecl         ::=      '#REQUIRED' | '#IMPLIED' | (('#FIXED' S)?

Character and entity references

    [67]    Reference           ::=      EntityRef | CharRef
        [66]    CharRef             ::=      '&#' [0-9]+ ';' | '&#x' [0-9a-fA-F]+ ';'
        [68]    EntityRef           ::=      '&' Name ';'

    [75]    ExternalID          ::=      'SYSTEM' S SystemLiteral | 'PUBLIC' S PubidLiteral S SystemLiteral
    [80]    EncodingDecl        ::=      S 'encoding' Eq ('"' EncName '"' | "'" EncName "'" )
        [81]    EncName             ::=      [A-Za-z] ([A-Za-z0-9._] | '-')*
    [82]    NotationDecl        ::=      '<!NOTATION' S Name S (ExternalID | PublicID) S? '>'
        [83]    PublicID            ::=      'PUBLIC' S PubidLiteral
        [12]    PubidLiteral        ::=      '"' PubidChar* '"' | "'" (PubidChar - "'")* "'"
        [13]    PubidChar           ::=      #x20 | #xD | #xA | [a-zA-Z0-9] | [-'()+,./:=?;!*#@$_%]
        [11]    SystemLiteral       ::=      ('"' [^"]* '"') | ("'" [^']* "'")

*/