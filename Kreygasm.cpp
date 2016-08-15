#include <iostream>
#include <string>
#include <vector>

// DEFAULT CLASSES
// FIND A WAY TO SERIALIZE DIS SHIT
// number
// alphabet

typedef unsigned char uint8;

typedef std::vector< std::vector<bool> > AdjacencyMap;

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

    o << Tok.Text << "\"}";

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
                && !NFA[Next][Next + 1] // If this is true than the operation
                                        // has been done previously
                && (NextTok.Quant == KreygasmTokenQuantity::Variable
                    || NextTok.Quant == KreygasmTokenQuantity::Optional))
            {
                NFA[Next][Next + 1] = true;
            }
        }
    }
}

bool Kreygasm::Match(const std::string& query)
{

    // Get Epsilon state
    // TODO: Fill this with default reachable states from q0
    const int N = Tokens.size();
    AdjacencyMap EpsilonState = GetEmptyAdjacencyMap(N);

    int iRegex = 0; 
    const int RegexMax = N;

    int iQuery = 0;
    const int QueryMax = query.size();

    while (iRegex < RegexMax && iQuery < QueryMax)
    {
    }

    // WROOOONG
    return iRegex == RegexMax;
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
        case '[':
            Tok.Type = KreygasmTokenType::Class;
            bracks++;
            break;
        case ']':
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
            if (Tok.Type == KreygasmTokenType::Literal)
            {
                Tok.Quant= KreygasmTokenQuantity::Optional;
            }
            else
            {
                Tok.Quant= KreygasmTokenQuantity::Optional;
            }
            break;
        case '+':
            // Check invalid token placement
            if (i + 1 < regex.size() && regex[i + 1] != ' ')
            {
                return false;
            }
            // Set Type
            if (Tok.Type == KreygasmTokenType::Literal)
            {
                Tok.Quant = KreygasmTokenQuantity::Multiple;
            }
            else
            {
                Tok.Quant = KreygasmTokenQuantity::Multiple;
            }
            break;
        case '*':
            // Check invalid token placement
            if (i + 1 < regex.size() && regex[i + 1] != ' ')
            {
                return false;
            }
            // Set Type
            if (Tok.Type == KreygasmTokenType::Literal)
            {
                Tok.Quant = KreygasmTokenQuantity::Variable;
            }
            else
            {
                Tok.Quant = KreygasmTokenQuantity::Variable;
            }
            break;
        }
        i++;
    }

    // empty string
    if (i == start)
    {
        i++;
        return true;
    }

    // Get correct Text
    if (Tok.Type < KreygasmTokenType::Class)
    {
        if (Tok.Quant == KreygasmTokenQuantity::Single)
        {
            Tok.Text = regex.substr(start, i - 1 - start);
        }
        else
        {
            Tok.Text = regex.substr(start, i - 2 - start);
        }
    }
    else
    {
        if (Tok.Quant== KreygasmTokenQuantity::Single)
        {
            Tok.Text = regex.substr(start + 1, i - 2 - start);
        }
        else
        {
            Tok.Text = regex.substr(start + 1, i - 3 - start);
        }
    }
    if (bracks == 0)
    {
        TokenArr.Add(Tok);
        return true;
    }
    return false;
}

int main()
{
    // input
    std::string pattern_str;
    std::string match_str;
    std::getline(std::cin, pattern_str);
    std::getline(std::cin, match_str);
    
    // Kreygasm variables
    Kreygasm TokenArr;
    int i = 0;

    ParseRegexToken(pattern_str, TokenArr, i);
    do {
        std::cout << TokenArr.Tokens.back() << std::endl;
    } while (ParseRegexToken(pattern_str, TokenArr, i));
}
