#include "xml/xml.h"

void pprint(const xml::Element& elem, int tab=1)
{
    string ident(tab*4 - 1, ' ');
    print(ident, "element:", elem.tag_name);
    for (auto& i: elem.attributes)
    {
        print(ident, "    attr:", i);
    }

    print(ident, "    text:", "\"" + elem.text + "\"");
    for (auto& c: elem.children)
    {
        pprint(c, tab+1);
    }
}

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