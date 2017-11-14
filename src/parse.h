#ifndef _PARSE_H
#define _PARSE_H

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>

using namespace std;


namespace parse
{
    enum RuleType
    {
        Rule_Production,

        Rule_Symbol,
        Rule_Range, // unicode character or range of characters
        Rule_Literal,
        Rule_Alternates,
    };

    const uint RULE_OPTIONAL = 1;
    const uint RULE_REPEAT = 2;
    const uint RULE_EXCLUDE = 3;

    struct Rule
    {
        RuleType type;
        uint flags;

        vector<Rule*> children; // for sequences and alternates

        string value; // for literals and symbols
        pair<uint, uint> range; // for unicode chars/ranges
        bool optional;
        bool repeat;

        Rule(const string& rule)
        {

        }

    };
    struct Grammar
    {
        map<string, Rule*> rules;

        void read_rule(const string& rule)
        {
        }

        Grammar(const vector<string>& grammar)
        {
            for (auto& rule : grammar)
            {
                read_rule(rule);
            }
        }
    };

    // some test cases

    // content             ::=    CharData? ((element | Reference | CDSect | PI | Comment) CharData?)*
    // doctypedecl         ::=   '<!DOCTYPE' S Name (S ExternalID)? S? ('[' intSubset ']' S?)? '>'
    // ignoreSect          ::=   '<![' S? 'IGNORE' S? '[' ignoreSectContents* ']]>'
};

#endif
