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
    struct Element
    {
        string tag;
        vector<string> text;
        map<string, string> attributes;
        vector<Element> children;
    };

    void pprint(const Element& elem, int tab=1)
    {
        string ident(tab*4, ' ');
        cout << ident << "element: " << elem.tag << endl;

        for (auto& i: elem.attributes)
        {
            cout << ident << "  - attr: " << i.first << " = " << i.second << endl;
        }

        for (auto& t : elem.text)
        {
            cout << ident << "  - text: " << t << endl;
        }

        for (auto& c: elem.children)
        {
            pprint(c, tab+1);
        }
    }

    void pprint_s(const Element& elem, int tab=1)
    {
        string ident(tab*4, ' ');
        cout << ident << "element: " << elem.tag << endl;

        for (auto& i : elem.children)
        {
            pprint(i, tab+1);
        }
    }

    namespace _ {
        typedef string::iterator stringit;

        bool is_whitespace(const stringit& s)
        {
            // return string(" \r\n\t").find(*s) != string::npos;
            auto ds = *s;
            return ds == ' ' || ds == '\r' || ds == '\n' || ds == '\t';
        }
        stringit read_whitespace(const stringit& s, const stringit& end)
        {
            stringit c = s;
            while (is_whitespace(c) && c < end) c++; // JINX!
            return c;
        }
        stringit read_until_whitespace(const stringit& s, const stringit& end)
        {
            stringit c = s;
            while (!is_whitespace(c) && c < end) c++;
            return c;
        }
        stringit read_until(const stringit& start, const stringit& end, char c)
        {
            return find(start, end, c);
        }
        stringit read_until(const stringit& start, const stringit& end, const string& str)
        {
            return search(start, end, str.begin(), str.end());
        }

        stringit read_attributes(const stringit& start, const stringit& end, Element& elem)
        {
            auto pos = start;
            while (true)
            {
                // scoot to the start of the next attribute
                pos = read_whitespace(pos, end);
                if (pos >= end)
                    break;

                // read an attribute key
                auto key_start = pos;
                auto key_end = read_until(key_start, end, '=');
                if (key_end == end)
                    throw runtime_error("malformed attribute: " + string(pos, pos+20));

                // read an attribute value
                auto val_start = key_end + 1;
                char quotechar = *val_start;
                if (quotechar != '"' && quotechar != '\'')
                    throw runtime_error("malformed attribute: " + string(pos, pos+20));
                auto val_end = read_until(val_start+1, end, quotechar);
                if (val_end == end)
                    throw runtime_error("malformed attribute: " + string(pos, pos+20));

                // add to the element's attribute list
                elem.attributes[string(key_start, key_end)] = string(val_start+1, val_end);

                pos = val_end + 1;
            }
        }

        stringit read_element(const stringit& start, const stringit& end, Element& elem)
        {
            // find the end of the opening tag
            auto start_tag_start = start;
            auto start_tag_end = read_until(start_tag_start+1, end, '>');
            if (start_tag_end == end)
                throw runtime_error("ill formed start tag: " + string(start_tag_start, start_tag_start+20));
            bool selfclosed = (*(start_tag_end-1) == '/');
            start_tag_end -= selfclosed ? 1 : 0;


            // extract the tag name
            auto namestart = start_tag_start + 1;
            auto nameend = read_until_whitespace(namestart, start_tag_end);
            elem.tag = string(namestart, nameend);


            // parse attributes
            auto attrs_start = nameend;
            auto attrs_end = start_tag_end;
            read_attributes(attrs_start, start_tag_end, elem);

            // if it's a self-closed element, we're done
            if (selfclosed)
                return start_tag_end+2;


            // start reading content
            stringit pos = start_tag_end + 1;
            while (pos < end)
            {
                // find the next element or text or etc
                pos = read_whitespace(pos, end);
                if (*pos == '<' && *(pos+1) == '/')
                {
                    // close tag
                    auto close_tag_start = pos;
                    auto close_tag_end = read_until(pos, end, '>');
                    if (close_tag_end == end)
                        throw runtime_error("malformed close tag" + string(close_tag_start, close_tag_start+20));
                    // break;
                    return close_tag_end+1;
                }
                else if (*pos == '<')
                {
                    // child element
                    Element child;
                    pos = read_element(pos, end, child);
                    elem.children.push_back(child);
                }
                else
                {
                    // read text
                    // FUCK COMMENTS >:)
                    auto text_end = read_until(pos, end, '<');
                    if (text_end == end)
                        throw runtime_error("could not find end of text content ('<' for end-tag or a child element start-tag)");

                    elem.text.push_back(string(pos, text_end));
                    pos = text_end;
                }
            }
            throw runtime_error("could not find close tag" + string(start_tag_start, start_tag_end+1));
        }
    };

    void load(const string& fname, Element& document)
    {
        using namespace xml::_;

        string source = util::read_text_file(fname);
        if (source.length() < 1)
        {
            throw runtime_error("could not open file " + fname);
        }

        stringit it = source.begin();

        // check for XML-declaration
        if (source[0] == '<' && source[1] == '?')
        {
            it += 5; // skip "<?xml"
            stringit declend = read_until(it, source.end(), "?>");
            if (declend == source.end())
            {
                throw runtime_error("broken xml declaration (found '<?' but not '?>'");
            }

            it = declend+2;
        }

        // skip whitespace until the root element
        it = read_whitespace(it, source.end());
        if (*it != '<')
        {
            throw runtime_error("could not find root element");
        }

        // read the root element
        Element root;
        read_element(it, source.end(), root);
        document.children.push_back(root);
        // ignore any trailing comments/processing instructions
    }
};


namespace xml2
{
    struct Element
    {
        string tag;
        vector<string> text;
        vector<string> attributes;
    };
    struct Document
    {

    };
};

#endif