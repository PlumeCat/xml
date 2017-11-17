#include "xml.h"

using namespace std;


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

// unit testing for the util::split() function
void test_split()
{
    auto test_split_char = [](const string& in, const vector<string>& expect) {
        return bind([](const string& in, const vector<string>& expect) {
            vector<string> output;
            util::split(in, ' ', output);
            assert_equal(output, expect);
        }, in, expect);
    };
    auto test_split_str = [](const string& in, const vector<string>& expect) {
        return bind([](const string& in, const vector<string>& expect) {        
            vector<string> output;
            util::split(in, " \r\t\n", output);
            assert_equal(output, expect);
        }, in, expect);
    };

    // simple case with a string containing whitespace
    unittest("split(): simple case: char", test_split_char(
        "the  quick brown   fox",
        {"the", "quick", "brown", "fox"}
        ));
    unittest("split(): simple case: str ", test_split_str(
        "the \r\rquick\n\tbrown   \r\r\n\n\n\tfox",
        {"the", "quick", "brown", "fox"}
        ));
    
    
    // general case (leading, containing, and trailing whitespace)
    unittest("split(): general case: char", test_split_char(
        "   the quick  brown   fox ",
        {"the", "quick", "brown", "fox"}
        ));
    unittest("split(): general case: str ", test_split_str(
        " \t\rthe quick   \r\r\t\n\r \n brown \r\n fox   \r\t\n",
        {"the", "quick", "brown", "fox"}
        ));

    
    // leading whitespace only
    unittest("split(): leading char: ", test_split_char(
        "    thequick",
        {"thequick"}
        ));
    unittest("split(): leading str:  ", test_split_str(
        "  \r\t\nthequick",
        {"thequick"}
        ));

    // trailing whitespace only
    unittest("split(): trailing char: ", test_split_char(
        "thequick   ",
        {"thequick"}
    ));
    unittest("split(): trailing str:  ", test_split_str(
        "thequick  \r\n\t  ",
        {"thequick"}
    ));

    // "none" case
    unittest("split(): none - char", test_split_char(
        "thequickbrownfox",
        {"thequickbrownfox"}
    ));
    unittest("split(): none - str  ", test_split_str(
        "thequickbrownfox",
        {"thequickbrownfox"}
    ));
}

int main(int argc, char* argv[])
{
    for (int i = 0; i < argc; i++)
    {
        cout << "argument: " << argv[i] << endl;
    }

    // test_split();

    xml::Element document;
    try
    {
        xml::load("test/note.xml", document);
        xml::pprint(document);
    }
    catch (const exception& e)
    {
        cout << "ERROR: " << e.what() << endl;
    }

    // parse::parse_pascalish();
}
