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

int main(int argc, char* argv[])
{
    for (int i = 0; i < argc; i++)
    {
        cout << "argument: " << argv[i] << endl;
    }
}
