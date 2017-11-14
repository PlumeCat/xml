#include "xml.h"

void pprint(const xml::Element& elem, int tab=1)
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


template<typename Test>
void unittest(const string& label, Test test)
{
    cout << "RUNNING TEST: " << label << "...";
    try
    {
        test();
        cout << "succeeded!" << endl;
    }
    catch (const exception& e)
    {
        cout << " failed: " << e.what() << endl;
    }
}

template<typename Type, typename Traits, typename Elem>
basic_ostream<Type, Traits>& operator << (basic_ostream<Type, Traits>& stream, const vector<Elem>& vec)
{
    stream << "vector [";
    for (const auto& elem : vec)
    {
        stream << "'" << elem << "', ";
    }
    stream << "]";
    return stream;
}

template<typename Type>
void assert_equal(const Type& a, const Type& b)
{
    if (!(a == b))
    {
        stringstream err;
        err << "assert_equal failed: ";
        err << "    expected: " << a;
        err << "    actual:   " << b;
        throw runtime_error(err.str());
    }
}


template<typename Type>
void simpletest(const string& label, Type in, Type expect)
{
    unittest(label, [&](){
        // cout << "    in: " << in << endl;
        // cout << "    expect: " << expect << endl;
        assert_equal(expect, in);
    });
}


int main(int argc, char* argv[])
{
    for (int i = 0; i < argc; i++)
    {
        cout << "argument: " << argv[i] << endl;
    }

    auto puresplit = [](const string& in, char c) {
        vector<string> output;
        util::split(in, c, output);
        return output;
    };
    auto puresplit2 = [](const string& in, const string& c) {
        vector<string> output;
        util::split(in, c, output);
        return output;
    };
    
    simpletest("split char", puresplit("   the quick  brown   fox ", ' '),
        vector<string>{"the", "quick", "brown", "fox"});
    simpletest("split str", puresplit2(" \t\rthe quick   \r\r\t\n\r \n brown \r\n fox   \r\t\n", "\r\t\n "),
        vector<string>{"the", "quick", "brown", "fox"});
}
