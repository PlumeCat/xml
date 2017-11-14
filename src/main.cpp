#include "xml/xml.h"

void pprint(const xml::Element& elem, int tab=1)
{
    string ident(tab*4 - 1, ' ');
    cout << ident << "element: ", elem.tag_name << endl;
    
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


t

int main()
{
    srand(time(nullptr));

    try
    {
        xml::Element root;
        // xml::read("src/xml/test/books.xml", root);
        xml::read("resource/assets.dae", root);
        pprint(root);
    }
    catch (const exception& e)
    {
        print("caught exception:", e.what());
    }

}
