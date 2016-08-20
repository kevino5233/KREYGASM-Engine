#include <iostream>
#include <string>
#include <vector>
#include <map>

bool StringSplit(std::string& tok, int& i, const std::string& query)
{
    if (i >= query.size())
    {
        return false;
    }

    while (i < query.size() && query[i] == ' ')
    {
        i++;
    }

    const int start = i;

    while (i < query.size() && query[i] != ' ')
    {
        i++;
    }

    if (i == start)
    {
        return false;
    }

    tok = query.substr(start, i - start);

    return true;
}

bool StringCompare(const std::string& a, const std::string b)
{
    if (a.size() != b.size())
    {
        return false;
    }

    for (int i = 0; i < a.size(); i++)
    {
        if (a[i] != b[i])
        {
            // determine if a[i] is alphabetical
            if (a[i] >= 65 && a[i] < 91)
            {
                if (a[i] + 32 != b[i])
                {
                    return false;
                }
            }
            else if (a[i] >= 97 && a[i] < 122)
            {
                if (a[i] - 32 != b[i])
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
    }
    return true;
}

// TODO: Refactor code, multiple file building and shit and namespace and some
// unit tests and other stuff idk

typedef unsigned char uint8;

// Temp solution
typedef std::vector< std::vector<bool> > AdjacencyMap;

typedef std::vector< std::string > KreyClass;

typedef std::map< std::string, KreyClass > KreyClassMap;

KreyClassMap GlobalMap;

std::string DigitClass = "digits";

std::string AlphabetClass = "alphabet";

std::string ArticleClass = "article";

bool MatchesClass(std::string ClassName, std::string Word)
{
    if (StringCompare(ClassName, DigitClass))
    {
        for (int i = 0; i < Word.size(); i++)
        {
            char num = Word[i] - 48;
            if (num < 0 || num >= 10)
            {
                return false;
            }
        }
        return true;
    }
    else if (StringCompare(ClassName, ArticleClass))
    {
        return StringCompare(Word, "The")
            || StringCompare(Word, "A")
            || StringCompare(Word, "An");
    }
    else if (GlobalMap.count(ClassName) > 0)
    {
        for (std::string ClassMember : GlobalMap[ClassName])
        {
            if (StringCompare(Word, ClassMember))
            {
                return true;
            }
        }
    }
    return false;
}

AdjacencyMap GetEmptyAdjacencyMap(const int& N)
{
    return std::vector< std::vector<bool> >(N, std::vector<bool>(N, false));
}

struct KreygasmToken;

class Kreygasm
{
    private:
        // Do some crazy shit where you store the adjacency
        // map with integers
        AdjacencyMap NFA;

    public:
        std::vector<KreygasmToken> Tokens;

        void Add(KreygasmToken Tok);

        // Generates NFA
        void Compile();
        
        bool Match(const std::string& InputStr);
};

enum class KreygasmTokenType : uint8
{
    Literal,
    Class,

    MAX
};

enum class KreygasmTokenQuantity : uint8
{
    Single,
    Optional,
    Multiple,
    Variable,

    MAX
};

struct KreygasmToken
{
    // Token type
    KreygasmTokenType Type;
    
    // Quantity Type
    KreygasmTokenQuantity Quant;

    // Token content or class name. Should not contain regex tokens
    std::string Text;

    // Any nested tokens. Will be empty if this is a literal
    // Unnecessary for now
    // Kreygasm ChildTokens;
};

template <typename T>
std::ostream& operator << (std::ostream& o, const std::vector<T>& Vec)
{
    for (T item : Vec)
    {
        o << item << ' ';
    }
    o << std::endl;

    return o;
}

std::ostream& operator << (std::ostream& o, const KreygasmToken& Tok)
{
    o << "{ ";
    if (Tok.Type == KreygasmTokenType::Literal)
    {
        o << "Type: Literal, ";
    }
    else if (Tok.Type == KreygasmTokenType::Class)
    {
        o << "Type: Class, ";
    }
    else
    {
        o << "Type: Invalid, ";
    }

    if (Tok.Quant == KreygasmTokenQuantity::Single)
    {
        o<< "Quant: Single, ";
    }
    else if (Tok.Quant == KreygasmTokenQuantity::Optional)
    {
        o<< "Quant: Optional, ";
    }
    else if (Tok.Quant == KreygasmTokenQuantity::Multiple)
    {
        o<< "Quant: Multiple, ";
    }
    else if (Tok.Quant == KreygasmTokenQuantity::Variable)
    {
        o<< "Quant: Variable, ";
    }
    else
    {
        o<< "Quant: Invalid, ";
    }

    o << "Text: \"" << Tok.Text << "\"}";

    return o;
}

void Kreygasm::Add(KreygasmToken Tok)
{
    Tokens.push_back(Tok);
}

// Generates NFA
void Kreygasm::Compile()
{
    // TODO: Run NFA construction
    const int N = Tokens.size();
    NFA = GetEmptyAdjacencyMap(N + 1);

    // TODO: Implement more rules and shit
    for (int i = 0; i < N; i++)
    {
        KreygasmToken Tok = Tokens[i];
        // std::cout << Tok << std::endl;
        // If we are Multiple or Variable then we can loop back to ourselves
        if (Tok.Quant == KreygasmTokenQuantity::Multiple
            || Tok.Quant == KreygasmTokenQuantity::Variable)
        {
            NFA[i][i] = true;
        }

        // If we are variable or optional, we can automatically move to the
        // next state. Try and keep doing so.
        // TODO: Optimize. This operation gets done multiple times
        // TODO: Check if current optimization actually works
        if (Tok.Quant == KreygasmTokenQuantity::Variable
            || Tok.Quant == KreygasmTokenQuantity::Optional)
        {
            NFA[i][i + 1] = true;
            int Next = i + 1;
            KreygasmToken NextTok = Tokens[Next];
            while (Next < N
                && (NextTok.Quant == KreygasmTokenQuantity::Variable
                    || NextTok.Quant == KreygasmTokenQuantity::Optional))
            {
                Next++;

                NFA[i][Next] = true;
                NextTok = Tokens[Next];
            }
        }
    }
    // std::cout << NFA;
}

bool Kreygasm::Match(const std::string& query)
{
    // Get Epsilon state
    const int N = Tokens.size();
    AdjacencyMap EpsilonState = GetEmptyAdjacencyMap(N + 1);

    for (int row = 0; row < N + 1; row++)
    {
        for (int col = 0; col < N + 1; col++)
        {
            EpsilonState[row][col] = NFA[row][col];
        }
    }

    std::vector<int> StateQueue;
    StateQueue.push_back(0);

    bool StateValid[N + 1];

    // Invalidate all states
    for (int i = 0; i < N + 1; i++)
    {
        StateValid[i] = false;
    }
    while (!StateQueue.empty())
    {
        const int CurrState = StateQueue.back();
        StateQueue.pop_back();
        if (StateValid[CurrState])
        {
            continue;
        }
        StateValid[CurrState] = true;
        for (int iState = 0; iState < N + 1; iState++)
        {
            if (NFA[CurrState][iState] && !StateValid[iState])
            {
                StateQueue.push_back(iState);
            }
        }
    }

    std::string tok;
    int StringIndex = 0;

    while (StringSplit(tok, StringIndex, query))
    {
        // std::cout << tok << ' ' << std::endl;
        for (int iState = 0; iState < N; iState++)
        {
            if (StateValid[iState])
            {
                KreygasmToken RegexTok = Tokens[iState];
                if (RegexTok.Type == KreygasmTokenType::Literal)
                {
                    // do lexical match w/ text
                    if (StringCompare(tok, RegexTok.Text))
                    {
                        if (RegexTok.Quant == KreygasmTokenQuantity::Multiple
                            || RegexTok.Quant == KreygasmTokenQuantity::Variable)
                        {
                            StateQueue.push_back(iState);
                        }
                        StateQueue.push_back(iState + 1);
                    }
                }
                else if (RegexTok.Type == KreygasmTokenType::Class)
                {
                    // TODO: Serialize class data including default classes
                    if (MatchesClass(RegexTok.Text, tok))
                    {
                        if (RegexTok.Quant == KreygasmTokenQuantity::Multiple
                            || RegexTok.Quant == KreygasmTokenQuantity::Variable)
                        {
                            StateQueue.push_back(iState);
                        }
                        StateQueue.push_back(iState + 1);
                    }
                }
                else
                {
                    //error
                }
            }
        }

        // Invalidate all states
        for (int i = 0; i < N + 1; i++)
        {
            StateValid[i] = false;
        }
        while (!StateQueue.empty())
        {
            const int CurrState = StateQueue.back();
            StateQueue.pop_back();
            if (StateValid[CurrState])
            {
                continue;
            }
            StateValid[CurrState] = true;
            for (int iState = 0; iState < N + 1; iState++)
            {
                if (NFA[CurrState][iState] && !StateValid[iState])
                {
                    StateQueue.push_back(iState);
                }
            }
        }
    }

    return StateValid[N] == true;
}

// TODO: Should the return value represent a token added or an error found?
// Should we make a seperate enum for the return type? Seems like overkill
// Something like:
// enum ParseResult
// {
//  Valid,
//  Empty,
//  BadToken,
//  Brackets
// }
// ?????
bool ParseRegexToken(const std::string& regex, Kreygasm& TokenArr, int& i)
{
    if (i >= regex.size())
    {
        return false;
    }

    while (i < regex.size() && regex[i] == ' ')
    {
        i++;
    }


    KreygasmToken Tok;
    Tok.Type = KreygasmTokenType::Literal;
    Tok.Quant= KreygasmTokenQuantity::Single;

    const int start = i;
    int bracks = 0;

    char regex_i = regex[i];
    // iterate through regex string until we hit a space (end of token)
    while (i < regex.size() && regex_i != ' ')
    {
        regex_i = regex[i];
        // validates brackets
        switch(regex_i)
        {
        case '<':
            Tok.Type = KreygasmTokenType::Class;
            bracks++;
            break;
        case '>':
            if (bracks == 0)
            {
                return false;
            }
            bracks--;
            break;
        case '?':
            // Check invalid token placement
            if (i + 1 < regex.size() && regex[i + 1] != ' ')
            {
                return false;
            }
            // Set Type
            Tok.Quant= KreygasmTokenQuantity::Optional;
            break;
        case '+':
            // Check invalid token placement
            if (i + 1 < regex.size() && regex[i + 1] != ' ')
            {
                return false;
            }
            // Set Type
            Tok.Quant = KreygasmTokenQuantity::Multiple;
            break;
        case '*':
            // Check invalid token placement
            if (i + 1 < regex.size() && regex[i + 1] != ' ')
            {
                return false;
            }
            // Set Type
            Tok.Quant = KreygasmTokenQuantity::Variable;
            break;
        }
        i++;
    }

    // empty string
    if (i == start)
    {
        i++;
        return false;
    }

    // Get correct Text
    if (Tok.Type == KreygasmTokenType::Literal)
    {
        if (Tok.Quant == KreygasmTokenQuantity::Single)
        {
            Tok.Text =regex.substr(
                start,
                i - (i >= regex.size() ? 0 : 1) - start
            );
        }
        else
        {
            Tok.Text = regex.substr(
                start,
                i - (i >= regex.size() ? 1 : 2)  - start
            );
        }
    }
    else if (Tok.Type == KreygasmTokenType::Class)
    {
        if (Tok.Quant== KreygasmTokenQuantity::Single)
        {
            Tok.Text = regex.substr(
                start + 1,
                i - (i >= regex.size() ? 2 : 3)  - start
            );
        }
        else
        {
            Tok.Text = regex.substr(
                start + 1,
                i - (i >= regex.size() ? 3 : 4)  - start
            );
        }
    }
    else
    {
        // error
    }
    if (bracks == 0)
    {
        TokenArr.Add(Tok);
        return true;
    }
    return false;
}

// int main()
// {
//     // input
//     std::string pattern_str;
//     std::string match_str;
//     std::getline(std::cin, pattern_str);
//     std::getline(std::cin, match_str);
//     
//     // Kreygasm variables
//     Kreygasm TokenArr;
//     int i = 0;
// 
//     // TODO: come up with a better way to do this
//     ParseRegexToken(pattern_str, TokenArr, i);
//     do {
//     } while (ParseRegexToken(pattern_str, TokenArr, i));
// 
//     TokenArr.Compile();
// 
//     std::cout << TokenArr.Match(match_str) << std::endl;
// }
