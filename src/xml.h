#ifndef _XML_H
#define _XML_H

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <stack>

// definitions will be removed at the end of this header
// just because std:: everywhere is damn unreadable >:(
#define string  std::string
#define vector  std::vector
#define cout    std::cout
#define endl    std::endl

// utility for streaming a vector
template<typename Type, typename Traits, typename Elem>
std::basic_ostream<Type, Traits>& operator << (std::basic_ostream<Type, Traits>& stream, const vector<Elem>& vec)
{
    stream << "[";
    for (const auto& elem : vec)
    {
        stream << "'" << elem << "', ";
    }
    stream << "]";
    return stream;
}

namespace util
{
    // split a string into a series of tokens separated by 'c'
    void split(const string& str, char c, vector<string>& tokens)
    {
        // clean tokens
        tokens.clear();

        // skip to the first not-'c' character
        int pos = str.find_first_not_of(string(1, c));
        int cpos = pos;
        
        while (true)
        {
            // find the first instance of 'c'
            cpos = str.find(c, pos);
            if (cpos == string::npos)
            {
                if (pos < str.length())
                {
                    tokens.push_back(str.substr(pos, str.length()));
                }
                break;
            }

            // push the token
            tokens.push_back(str.substr(pos, cpos-pos));

            pos = cpos;
            while (pos < str.length() && str[pos] == c)
            {
                pos++;
            }
        }
    }

    // split a string into a series of tokens separated by any character in "chars"
    void split(const string& str, const string& chars, vector<string>& tokens)
    {
        // clean tokens
        tokens.clear();

        // skip to the first not-in-"chars" character
        int pos = str.find_first_not_of(chars);
        int wpos = pos;

        while (true)
        {
            wpos = str.find_first_of(chars, pos);
            if (wpos == string::npos)
            {
                // push the rest of the string and abort
                if (pos < str.length())
                {
                    tokens.push_back(str.substr(pos, str.length()));
                }
                break;
            }
            
            // push the current token
            tokens.push_back(str.substr(pos, wpos-pos));

            // scan for the next non-whitespace character
            // don't need to check if pos < str.size() :)
            pos = wpos;
            while (pos < str.length() && chars.find(str[pos]) != string::npos)
            {
                pos++;
            }
        }
    }

    void read_text_file(const string& fname, vector<string>& lines)
    {
        std::ifstream file(fname);
        string line;
        while (std::getline(file, line))
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
    struct Document
    {
    };
    struct Prolog
    {

    };
    struct DocTypeDecl
    {

    };
    struct Element
    {
        string tag_name = "";
        int generation = 0;
        vector<string> text;
        vector<string> attributes;
        vector<Element> children;
    };

    void pprint(const Element& elem, int tab=1)
    {
        string ident(tab*4 - 1, ' ');
        cout << ident << "element: " << elem.tag_name << endl;
        
        for (auto& i: elem.attributes)
        {
            cout << ident << "    attr: " << i << endl;
        }

        for (auto& t : elem.text)
        {
            cout << ident << "    text: " << t << endl;
        }

        for (auto& c: elem.children)
        {
            pprint(c, tab+1);
        }
    }

    /*
        the following characters must be escaped
            "   &quot;
            '   &apos;
            <   &lt;
            >   &gt;
            &   &amp;
    */

    bool parse_document(const string& source, Document& document)
    {
    }

    bool read(const string& fname, Document& document)
    {
        string source = util::read_text_file(fname);
        // TODO: source = process(source)
        // where "process" will check for utf8 / etc and map anything above xFF to "unprintable"

        return parse_document(source, document);
    }


    bool read_postcode()
    {

    }

};

namespace parse
{
    struct AstNode
    {
        vector<AstNode> children;
    };

    namespace rule
    {
        // Char literal construct
        // Must match any one of the characters in the given string
        struct chr
        {
            chr(const string& str) {}
            bool parse(const string&) { return true; }
        };
        
        // String literal construct
        // Must match all of the input string
        struct str
        {
            str(const string& str) {}
            bool parse(const string&) { return true; }
        };

        // Not construct
        // Match Include but not Exclude
        template<typename Inc, typename Exc>
        struct rem
        {
            rem(const Inc& inc, const Exc& exc) { }
            bool parse(const string&) { /* return inc.parse() && !exc.parse(); */ return true; }
        };

        // Optional construct
        template<typename Parseable>
        struct opt
        {
            opt(const Parseable& parse) { }
            bool parse(const string&) { return true; }
        };

        // Repeated construct (one-or-more)
        // You can use Opt(Rpt(...)) to represent zero-or-more
        template<typename Parseable>
        struct rpt
        {
            rpt(const Parseable& parse) {}
            bool parse(const string&) { return true; }
        };

        // Sequence of constructs (a series of parseables; all must match)
        template<typename Parseable, typename ...Rest>
        struct seq
        {
            seq(const Parseable& parseable, const Rest& ...rest) {}
            bool parse(const string& str) { /* return first.parse() || Alt(rest).parse(); */ return true; }
        };
        template<typename Parseable>
        struct seq<Parseable>
        {
            seq(const Parseable& first) {}
            bool parse(const string& str) { /* return first.parse() && Seq(rest).parse(); */ return true; }
        };



        // Series of alternates
        template<typename Parseable, typename ...Rest>
        struct alt 
        {
            alt(const Parseable& first, const Rest& ...rest) { }
            bool parse(const string& str) { /* return first.parse() || Alt(rest).parse(); */ return true; }
        };
        template<typename Parseable>
        struct alt<Parseable>
        {
            alt(const Parseable& parseable) {}
            bool parse(const string& str) { return true; }
        };
    }


    rule::chr chr(const string& str) { return rule::chr(str); }
    rule::str str(const string& str) { return rule::str(str); }
    template<typename T>
    rule::opt<T> opt(const T& t) { return rule::opt<T>(t); }
    template<typename Inc, typename Exc>
    rule::rem<Inc, Exc> rem(const Inc& inc, const Exc& exc) { return rule::rem<Inc, Exc>(inc, exc); }
    template<typename T>
    rule::rpt<T> rpt(const T& t) { return rule::rpt<T>(t); }
    template<typename T1, typename ...Tn>
    rule::seq<T1, Tn...> seq(const T1& t1, const Tn& ...tn) { return rule::seq<T1, Tn...>(t1, tn...); }
    template<typename T1, typename ...Tn>
    rule::alt<T1, Tn...> alt(const T1& t1, const Tn& ...tn) { return rule::alt<T1, Tn...>(t1, tn...); }

    const string alphabetUpper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const string alphabetLower = "abcdefghijklmnopqrstuvwxyz";
    const string alphabet = alphabetUpper + alphabetLower;
    const string digits = "0123456789";

    void parse_pascal()
    {
        /*
        
        a simple program syntax in EBNF − Wikipedia
            
            program = 'PROGRAM', white space, 
                        identifier, white space, 
                        'BEGIN', white space, 
                        { 
                            assignment, ";", white space
                        }, 
                        'END.' ;
            identifier = alphabetic character, { alphabetic character | digit } ;
            number = [ "-" ], digit, { digit } ;
            string = '"' , { all characters - '"' }, '"' ;
            assignment = identifier , ":=" , ( number | identifier | string ) ;
            alphabetic character = "A" | "B" | "C" | "D" | "E" | "F" | "G"
                                | "H" | "I" | "J" | "K" | "L" | "M" | "N"
                                | "O" | "P" | "Q" | "R" | "S" | "T" | "U"
                                | "V" | "W" | "X" | "Y" | "Z" ;
            digit = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" ;
            white space = ? white space characters ? ;
            all characters = ? all visible characters ? ;

        A syntactically correct program then would be:

            PROGRAM DEMO1
            BEGIN
                A:=3;
                B:=45;
                H:=-100023;
                C:=A;
                D123:=B34A;
                BABOON:=GIRAFFE;
                TEXT:="Hello world!";
            END

        */
        
        auto whitespace = rpt(chr(" \r\n\t"));
        auto identifier = seq(chr(alphabet), alt(chr(alphabet), chr(digits)));
        auto number = rpt(chr(digits));
        auto string_ = rpt(seq(chr("\""), rpt(chr(alphabet + digits)), chr("\"")));
        auto assignment = seq(identifier, str(":="), alt(identifier, string_, number));
        auto program_grammar = seq(
            str("PROGRAM"), whitespace,
            identifier, whitespace,
            str("BEGIN"), whitespace,
            rpt(
                seq(assignment, str(";"), whitespace)
            ),
            str("END")
        );

        auto example_program = 
            "PROGRAM DEMO1"
            "BEGIN"
                "A:=3;"
                "B:=45;"
                "H:=-100023;"
                "C:=A;"
                "D123:=B34A;"
                "BABOON:=GIRAFFE;"
                "TEXT:=\"Hello world!\";"
            "END";
    }
};

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
        [4]     NameStartChar       ::=      ":" | [A-Z] | "_" | [a-z] | [#xC0-#xD6] | [#xD8-#xF6] | [#xF8-#x2FF] | [#x370-#x37D] | [#x37F-#x1FFF] | [#x200C-#x200D] | [#x2070-#x218F] | [#x2C00-#x2FEF] | [#x3001-#xD7FF] | [#xF900-#xFDCF] | [#xFDF0-#xFFFD] | [#x10000-#xEFFFF]
        [4a]    NameChar            ::=      NameStartChar | "-" | "." | [0-9] | #xB7 | [#x0300-#x036F] | [#x203F-#x2040]
        [6]     ?? Names               ::=      Name (#x20 Name)*
    [7]     Nmtoken             ::=      (NameChar)+
        [8]     ?? Nmtokens            ::=      Nmtoken (#x20 Nmtoken)*

Comment, Processing instructions
    [15]    Comment             ::=      '<!--' (   (Char - '-') |  ('-' (Char - '-')   ))* '-->'
                                         '<!--' followed by multiple "Char" (with no instance of '--', also first char must not be '-'), then '-->'
    [16]    PI                  ::=      '<?' PITarget (  S (   Char* - (Char* '?>' Char*)   )  )? '?>'
                                         '<?' + PITarget +  + '?>'
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



#undef string
#undef vector
#undef cout
#undef endl