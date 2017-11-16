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

using namespace std;

namespace xml
{
    struct Element
    {

    };
};

#ifdef 0

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
    namespace rule {};

    struct Context final
    {
    private:
        string sourcecode;
        uint pos;
        uint line;
        uint column;
        
        // advance "pos" by n
        // update "line" and "column" accordingly
        // returns the number of characters advanced
        uint advance(uint n)
        {
            for (uint i = 0; i < n; i++)
            {
                if (pos+1 >= sourcecode.length())
                {
                    return i;
                }

                pos++;
                column++;
                if (sourcecode[pos-1] == '\n')
                {
                    line++;
                    column = 0;
                }
            }

            return n;
        }
    
    public:
        Context(const string& source)
        {
            pos = 0;
            line = 0;
            column = 0;
            sourcecode = source;
        }
        Context(const Context&) = delete;
        Context(Context&&) = delete;
        Context& operator = (const Context&) = delete;
        Context& operator = (Context&&) = delete;
        ~Context() {}

        uint pos() { return pos; }
        uint lineno() { return line; }
        uint colno() { return column; }

        char peek() const
        {
            return sourcecode[pos];
        }
        string peek(uint n) const
        {
        }
        char next()
        {
            if (advance(1) == 0) { return 0; }
            return sourcecode[pos-1];
        }
        string next(uint n)
        {
        }
    };
    struct AstNode
    {
        string value;
        vector<AstNode> children;
    };

    namespace rule
    {
        // Char literal construct
        // Must match any one of the characters in the given string
        struct chr
        {
            string chars;
            chr(const string& str) : chars(str) {}
            AstNode* parse(Context& context)
            {
                if (chars.find(context.peek()) != string::npos)
                {
                    AstNode node;
                    node.value = context.next();
                    parent.children.push_back(node);
                    return true;
                }
                return false;
            }
        };
        
        // String literal construct
        // Must match all of the input string
        struct str
        {
            string literal;
            str(const string& str) : literal(str) {}
            AstNode* parse(Context& context)
            {
                if (literal == context.peek(literal.length()))
                {
                    auto node = new AstNode;
                    node->value = context.next(literal.length());
                    return node;
                }
                return nullptr;
            }
        };

        // Not construct
        // Match Include but not Exclude
        template<typename Inc, typename Exc>
        struct rem
        {
            rem(const Inc& inc, const Exc& exc) { }
            AstNode* parse(Context& context)
            {
                // rem is tricky because we have to parse then not-parse
                
                // just this once, we'll create a copy of Context
                Context copycontext(context.sourcecode); copycontext.advance(context.pos());

            }
        };

        // Optional construct
        template<typename Parseable>
        struct opt
        {
            Parseable type;
            opt(const Parseable& parse) { }
            AstNode* parse(Context& context) { return true; }
        };

        // Repeated construct (one-or-more)
        // You can use Opt(Rpt(...)) to represent zero-or-more
        template<typename Parseable>
        struct rpt
        {
            rpt(const Parseable& parse) {}
            AstNode* parse(Context& context) { return true; }
        };

        // Sequence of constructs (a series of parseables; all must match)
        template<typename Parseable, typename ...Rest>
        struct seq
        {
            seq(const Parseable& parseable, const Rest& ...rest) {}
            AstNode* parse(Context& context) { /* return first.parse() || Alt(rest).parse(); */ return true; }
        };
        template<typename Parseable>
        struct seq<Parseable>
        {
            seq(const Parseable& first) {}
            AstNode* parse(Context& context) { /* return first.parse() && Seq(rest).parse(); */ return true; }
        };



        // Series of alternates
        template<typename Parseable, typename ...Rest>
        struct alt 
        {
            alt(const Parseable& first, const Rest& ...rest) { }
            bool parse(const string& str, AstNode& parent) { /* return first.parse() || Alt(rest).parse(); */ return true; }
        };
        template<typename Parseable>
        struct alt<Parseable>
        {
            alt(const Parseable& parseable) {}
            bool parse(const string& str, AstNode& parent) { return true; }
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

    void parse_pascalish()
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

        after parsing, we get a basic AST that looks like this

            seq
                str "PROGRAM"
                whitespace
                identifier "DEMO1"
                whitespace
                str "BEGIN"
                whitespace
                rpt
                    seq
                        seq
                            identifier "A", str ":=", number 3
                        str ";"
                        whitespace
                    seq
                        seq
                            identifier "B", str ":=", number 45
                        str ";"
                        whitespace
                    seq
                        seq
                            identifier "H", str ":=", number -100023
                        str ";"
                        whitespace
                    seq
                        seq
                            identifier "C", str ":=", identifier "A"
                        str ";"
                        whitespace
                    seq
                        seq
                            identifier "D123", str ":=", identifier "B34A"
                        str ";"
                        whitespace
                    seq
                        seq
                            identifier "BABOON", str ":=", identifier "GIRAFFE"
                        str ";"
                        whitespace
                    seq
                        seq
                            identifier "TEXT", str ":=", string("Hello world!")
                        str ";"
                        whitespace
                str "END"

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

        string example_program = 
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

        AstNode program;
        Context context(example_program);
        if (program_grammar.parse(context, program))
        {

        }
    }
};

#endif

#endif