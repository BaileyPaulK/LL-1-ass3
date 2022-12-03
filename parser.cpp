/*
Bailey Kleinhans
problume: given static Grammar design and implement LL(1) Table Parsing Algorithm
*/
#include <iostream>
#include <deque>
#include <string>
//#include <limits>

using namespace std;

int WIDTH = 20;
bool NoErr      = true;
struct rule
{
    string   symbol;
    string   to;
};

deque<rule> GRAMMER = {
    {.symbol="P",   .to="E"},
    {.symbol="E",   .to="TE!"},
    {.symbol="E!",  .to="+TE!"},
    {.symbol="E!",  .to="$"}, //representing empty as $ for ease of code && looks/ behaves better than "\0" or empty string
    {.symbol="T",   .to="FT!"},
    {.symbol="T!",  .to="*FT!"},
    {.symbol="T!",  .to="$"},
    {.symbol="F",   .to="(E)"},
    {.symbol="F",   .to="int"}
};

struct grammerTable
{
    deque<string> columnLookup;
    deque<string> rowLookup;
    deque < deque <int> > table;
};

grammerTable GrammarTABLE = {
    .columnLookup=  {"P", "E", "E!", "T", "T!", "F"},
    .rowLookup=     {"int", "+", "*", "(", ")", "$"},
    .table=         {
    //  int  +   *   (   )   $
        {1,  0,  0,  1,  0,  0}, //P
        {2,  0,  0,  2,  0,  0}, //E
        {0,  3,  0,  0,  4,  4}, //E!
        {5,  0,  0,  5,  0,  0}, //T
        {0,  7,  6,  0,  7,  7}, //T!
        {9,  0,  0,  8,  0,  0}  //F
    }
};

void printInWidth(string msg, bool leftAlin)
{
    if (leftAlin)
    {
        cout << msg;
    }
    for (size_t i = msg.length(); i < WIDTH; i++)
    {
        cout << ' ';
    }
    if (!leftAlin)
    {
        cout << msg;
    }
}   

void printInWidth(deque<string> manyMsgs, bool leftAlin)
{
    string msg;
    for (string amsg: manyMsgs)
    {
        msg = msg + ' ' + amsg;
    }
    printInWidth(msg, leftAlin);
}

deque<string> tokeniz(string convert)
{
    deque<string> tokens;
    string buildToken = "\0";
    for (char& i: convert)
    {
        //reserved chars
        switch (i)
        {
        case ' ':   //space deliminated (reached end of unreserved word(will likely error out unless special GRAMMER && TABLE rules))
            if (buildToken.size() != 0)
            {
                tokens.push_back(buildToken);
                buildToken = "\0";
            }
            continue;
            break;
        case '*':
            if (buildToken.size() != 0)
            {
                tokens.push_back(buildToken);
                buildToken = "\0";
            }
            tokens.push_back("*");
            continue;
            break;
        case '+':
            if (buildToken.size() != 0)
            {
                tokens.push_back(buildToken);
                buildToken = "\0";
            }
            tokens.push_back("+");
            continue;
            break;
        case '|':
            if (buildToken.size() != 0)
            {
                tokens.push_back(buildToken);
                buildToken = "\0";
            }
            tokens.push_back("|");
            continue;
            break;
        case '(':
            if (buildToken.size() != 0)
            {
                tokens.push_back(buildToken);
                buildToken = "\0";
            }
            tokens.push_back("(");
            continue;
            break;
        case ')':
            if (buildToken.size() != 0)
            {
                tokens.push_back(buildToken);
                buildToken = "\0";
            }
            tokens.push_back(")");
            continue;
            break;
        case 't':   //int && ident reserved words (optimized reservation)
            if (buildToken == "in") //int
            {
                tokens.push_back("int");
                buildToken = "\0";
            } else if (buildToken == "iden") //ident
            {
                tokens.push_back("ident");
                buildToken = "\0"; 
            }
            continue;
            break;    
        default:
            break;
        }
        //gross check for Rules (any capitol letter is reserved for rules, but also prime rule (capitol followed by !))
        if (buildToken.size() == 1)
        {
            if (isupper(buildToken[0]))
            {
                if (i == '!')
                {
                    tokens.push_back(buildToken + "!");
                    buildToken = "\0";
                    continue; 
                } else {
                    tokens.push_back(buildToken);
                    buildToken = "\0";  
                }
            }
        }
        buildToken = buildToken + i; 
    }
    if (buildToken.size() != 0) //make sure all tokens pushed in
    {
        tokens.push_back(buildToken);
    }
    return tokens;
}

int find(string token, deque<string> line)
{
    for (size_t index = 0; index < line.size(); index++)
    {
        if (token == line[index])
        {
            return(index);
        }
    }
    return -1;
}

deque<string> getDownStream(string element, string token)
{
    int row     = find(token, GrammarTABLE.rowLookup);
    int column  = find(element, GrammarTABLE.columnLookup);
    if (row < 0 || column < 0) //make sure rule exists downstream somewhere that will resolve to token
    {
        cout << "\n\nERROR: not found in grammar table\n" << "element = " << element << ", token = " << token << "\n";
        NoErr = false;
        return deque<string> {};
        EXIT_FAILURE;
    }
    int rule = GrammarTABLE.table[column][row];
    if (rule == 0)
    {
        cout << "\n\nERROR: no aplicable rule\n" << "element = " << element << ", token = " << token << "\n";
        NoErr = false;
        return deque<string> {};
        EXIT_FAILURE;
    }
    string elemRule = GRAMMER[rule - 1].to; //because adults index at 0
    printInWidth("apply " + to_string(rule) + ": " + element + " => " + elemRule, 1);
    return tokeniz(elemRule);
}

int main()
{
    deque<string> stack = {"P", "$"};
    cout << "please input string to be parsed (note use ! for prime)" << endl << ":";
    string in;
    getline(cin, in);
    //cin >> in;
    //cin.ignore(numeric_limits<streamsize>::max(),'\n'); //wipe out leftover stream buffer (probubly not needed in this use case, but i never trust users on inputs)
    deque<string> input = tokeniz(in);
    if (input.back() != "$") {input.push_back("$");}

    //pretty table header
        //check table width
        in = "\0";
        for (string amsg: input){in = in + ' ' + amsg;}
        if (WIDTH < in.size()){WIDTH = in.size();}    
    printInWidth("Stack", 0);
    cout << " | ";
    printInWidth("Input", 1);
    cout << " | ";
    printInWidth("Action", 1);
    cout << endl;

    for (size_t a = 0; a < 3; a++)
    {
        for (size_t i = 0; i < WIDTH; i++)
        {
            cout << "-";
        }
        if (a != 2)
        {
            cout << "-+-";
        }
    }
    cout << endl;

    string token    = input.front(); //current first token of input
    while (NoErr && stack.size() != 0)
    {
        printInWidth(stack, 0); //print stack in table
        cout << " | ";
        printInWidth(input, 1); //print input in table
        cout << " | ";
        string element  = stack.front(); //current top element of stack
        stack.pop_front();
        if (element == token)
        {
            printInWidth("match " + element, 1);
            cout << endl;
            input.pop_front();
            token    = input.front();
            continue;
        }
        deque<string> toadd = getDownStream(element, token); //get next downstream elements to add to stack based on rules
        cout << endl;
        while (toadd.size() != 0)   //add new elements to stack preserving order
        {
            if (toadd.back() != "$") //you don't add nothing to the stack
            {
                stack.push_front(toadd.back());
            }
            toadd.pop_back();
        }
    }
    
    return 0;
}
