#ifndef LSYSTEM_H
#define LSYSTEM_H

#include "../../engine/common.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include <stack>
#include <typeindex>
#include <algorithm>
#include "random.h"

// Each system is going to be defined by the data each evaluation carries -- called context here.
// For example, the data that a river generation system needs may not be the same as a cave generation system
template <class T>
class LSystem;

// The container of global data for the evaluation of an L-system
// Random number generation is also persisted here.
// This is NOT context in the formal grammar sense... for now
template <class T>
class LContext
{
public:
    T data;
    Random * random;
};

// Instructions do actual work. They can be stochastic in their evaluation and placement,
// but each rule is unique.
template <class T>
class LInstruction
{
protected:
    // Instructions are unique and thus cannot be created arbitrarily
    // NOTE: DO NOT IMPLEMENT CONSTRUCTORS DIRECTLY,
    // OR THE FACTORY PATTERN WONT WORK.
    friend class LSystem<T>;
    LInstruction(){}

public:
    virtual ~LInstruction() {}

    // So that each instruction can be human readable and parsable. It also works as hash
    virtual char GetSymbol() const = 0;
    virtual LContext<T> Evaluate(const LContext<T>& context, std::stack<LContext<T>> & contextStack) = 0;
};

// A necessary instruction for branching
template <class T>
class PushInstruction : public LInstruction<T>
{
public:
    char GetSymbol() const
    {
        return '[';
    }

    virtual LContext<T> Evaluate(const LContext<T>& context, std::stack<LContext<T>> & contextStack)
    {
        contextStack.push(context);
        return context;
    }
};

// A necessary instruction for branching
template <class T>
class PopInstruction : public LInstruction<T>
{
public:
    char GetSymbol() const
    {
        return ']';
    }

    virtual LContext<T> Evaluate(const LContext<T>& context, std::stack<LContext<T>> & contextStack)
    {
        LContext<T> c = contextStack.top();
        contextStack.pop();
        return c;
    }
};

template <class T>
class LInstructionChain
{
private:
    std::string axiom;
    std::vector<LInstruction<T>*> instructions;
    int depth;

public:
    LInstructionChain(std::string axiom, const std::vector<LInstruction<T>*>& instructions, int depth) :
        axiom(axiom), instructions(instructions), depth(depth) {}

    void Evaluate(T initialData, int seed)
    {
        // Now evaluate the chain!
        LContext<T> context;
        context.data = initialData;
        context.random = new MersenneRandom(seed);

        // For any instruction that needs backtracking...
        std::stack<LContext<T>> contextStack;

        for(unsigned int i = 0; i < instructions.size(); i++)
            context = instructions[i]->Evaluate(context, contextStack);
    }

    // For humans only
    std::string ToString()
    {
        std::string out = axiom + " : ";

        for(unsigned int i = 0; i < instructions.size(); i++)
            out += instructions[i]->GetSymbol();

        if(depth > 1)
            out += " (evaluated " + std::to_string(depth) + " times)";
        else
            out += " (evaluated 1 time)";

        return out;
    }
};

// Rules define the way the symbols are processed and replaced.
// They can be constant or not (e.g. in the case of a parametric grammar)
// Note: a rule can be constant, but applied with a probability, so both properties are orthogonal
template <class T>
class LRule
{
private:
    std::vector<LInstruction<T>*> instructions;
    std::vector<LInstruction<T>*> pattern;
    bool constant;

protected:
    virtual std::vector<LInstruction<T>*> InternalPattern(LSystem<T>& context) const = 0;
    virtual std::vector<LInstruction<T>*> InternalEvaluate(LSystem<T>& context) const = 0;

public:
    LRule() : LRule(true) {}
    LRule(bool constant) : instructions(), pattern(), constant(constant) {}
    virtual ~LRule() {}

    virtual float GetProbability()
    {
        return 1.f;
    }

    void Cache(LSystem<T>& context)
    {
        if(constant)
        {
            this->instructions = InternalEvaluate(context);
            this->pattern = InternalPattern(context);
        }
    }

    std::vector<LInstruction<T>*> GetPattern(LSystem<T>& context)
    {
        if(constant)
            return pattern;

        return InternalPattern(context);
    }

    std::vector<LInstruction<T>*> Evaluate(LSystem<T>& context)
    {
        // TODO: the original instructions that matched the pattern could be an interesting input...
        if(constant)
            return instructions;

        return InternalEvaluate(context);
    }

    std::string ToString()
    {
        std::string out = "";

        for(unsigned int i = 0; i < pattern.size(); i++)
            out += pattern[i]->GetSymbol();

        out += " -> ";

        for(unsigned int i = 0; i < instructions.size(); i++)
            out += instructions[i]->GetSymbol();

        return out;
    }
};

template <class T>
class LSystem
{
private:
    struct LPattern
    {
    public:
        std::vector<LInstruction<T>*> instructions;
        std::vector<LRule<T>*> rules;
    };

    typedef std::unordered_map<char, LInstruction<T>*> SymbolMap;
    typedef std::unordered_map<std::type_index, LInstruction<T>*> InstructionMap;
    typedef std::unordered_map<std::string, std::vector<LInstruction<T>*>> AxiomMap;

    std::vector<LPattern> patterns;
    std::vector<LRule<T>*> rules;

    // TODO: Currently, instructions themselves are being used for pattern matching, etc.
    // If we wanted to create a parametric grammar, we would also need inputs here...
    AxiomMap cachedAxioms;
    InstructionMap instructions;
    SymbolMap instructionSymbols;

    LPattern& FindPattern(const std::vector<LInstruction<T>*>& pattern)
    {
        for(unsigned int i = 0; i < patterns.size(); i++)
        {
            LPattern& p = patterns[i];

            bool equal = true;

            for(unsigned int j = 0; j < p.instructions.size(); j++)
                if(p.instructions[j] != pattern[j])
                    equal = false;

            if(equal)
                return patterns[i];
        }

        // If we found no pattern, create&save it and return it
        LPattern p;
        p.instructions = pattern;
        patterns.push_back(p);

        return patterns[patterns.size() - 1];
    }

    std::vector<LInstruction<T>*> ParseAxiom(std::string axiom)
    {
        typename AxiomMap::iterator foundAxiom = cachedAxioms.find(axiom);

        if(foundAxiom != cachedAxioms.end())
            return foundAxiom->second;

        // For now, we assume that symbols are JUST one letter...
        std::vector<LInstruction<T>*> axiomInstructions;

        for(unsigned int i = 0; i < axiom.size(); i++)
        {
            char c = axiom[i];
            typename SymbolMap::iterator it = instructionSymbols.find(c);

            // We failed to parse the string!
            if(it == instructionSymbols.end())
                return std::vector<LInstruction<T>*>();

            axiomInstructions.push_back(it->second);
        }

        cachedAxioms[axiom]  = axiomInstructions;
        return axiomInstructions;
    }

    std::vector<LInstruction<T>*> Expand(std::vector<LInstruction<T>*>& chain, Random * random)
    {
        std::vector<LInstruction<T>*> result;

        typename std::vector<LInstruction<T>*>::iterator chainIterator;
        typename std::vector<LPattern>::iterator patternIterator;

        for(chainIterator = chain.begin(); chainIterator != chain.end(); chainIterator++)
        {
            bool matched = false;

            for(patternIterator = patterns.begin(); patternIterator != patterns.end(); patternIterator++)
            {
                LPattern& pattern = *patternIterator;

                int patternSize = pattern.instructions.size();

                if(pattern.instructions.size() > 0)
                {
                    // And here we pray the implementation is boyer moore or similar
                    if(std::search(chainIterator, chain.end(), pattern.instructions.begin(), pattern.instructions.end()) == chainIterator)
                    {
                        // We found a pattern! (Note: if two patterns overlap, this method is NOT deterministic on its resolution...)
                        // Also note: we only want to match a pattern at this point of the string
                        float r = random->GetNormalizedValue();

                        LRule<T> * selectedRule = nullptr;
                        typename std::vector<LRule<T>*>::iterator ruleIterator;

                        for(ruleIterator = pattern.rules.begin(); ruleIterator != pattern.rules.end(); ruleIterator++)
                        {
                            float p =  (*ruleIterator)->GetProbability();

                            if(r < p || p >= 1.0f) // Exact can be a case
                            {
                                selectedRule = *ruleIterator;
                                break;
                            }
                            else
                                r -= p;
                        }

                        if(selectedRule != nullptr)
                        {
                            std::vector<LInstruction<T>*> ruleEvaluation = selectedRule->Evaluate(*this);
                            result.insert(result.end(), ruleEvaluation.begin(), ruleEvaluation.end());

                            // Jump all instructions from this pattern
                            chainIterator += patternSize - 1;
                            matched = true;
                            break; // Dont match another pattern
                        }
                    }
                }
            }

            // If we didn't match anything, just add the instruction unmodified
            if(!matched)
                result.push_back(*chainIterator);
        }

        return result;
    }

public:
    LSystem() : patterns(), instructions()
    {
    }

    ~LSystem(){}

    void AddRule(LRule<T> * rule)
    {
        this->rules.push_back(rule);
    }

    void Initialize()
    {
        // Find all patterns and their rules
        for(typename std::vector<LRule<T>*>::iterator r = rules.begin(); r != rules.end(); r++)
        {
            LRule<T> * rule = *r;
            rule->Cache(*this);

            LPattern& pattern = FindPattern(rule->GetPattern(*this));
            pattern.rules.push_back(rule);
        }
    }

    LInstructionChain<T> ExpandAxiom(std::string axiom, int depth, int seed)
    {
        int originalDepth = depth;
        std::vector<LInstruction<T>*> chain = ParseAxiom(axiom);
        MersenneRandom random(seed);

        // First expand until desired depth
        while(depth > 0)
        {
            chain = Expand(chain, &random);
            depth--;
        }

        return LInstructionChain<T>(axiom, chain, originalDepth);
    }

    template <class E>
    void RegisterInstruction()
    {
        GetInstruction<E>();
    }

    // Instructions can be constructed only by the system
    template <class E>
    E * GetInstruction()
    {
        typename std::unordered_map<std::type_index, LInstruction<T>*>::iterator it = instructions.find(typeid(E));

        if(it == instructions.end())
        {
            E * t = new E();
            instructionSymbols[t->GetSymbol()] = t;
            instructions[typeid(E)] = t;
            return t;
        }

        // Already exists, return it
        return dynamic_cast<E*>(it->second);
    }
};


/*
 * TEST INSTRUCTIONS/RULES
 *
 */

class TestInstruction : public LInstruction<float>
{
public:
    char GetSymbol() const { return 'A'; }

    LContext<float> Evaluate(const LContext<float>& context, std::stack<LContext<float>>& contextStack)
    {
        return context;
    }
};

class TestInstructionB : public LInstruction<float>
{
public:
    char GetSymbol() const { return 'B'; }

    LContext<float> Evaluate(const LContext<float>& context, std::stack<LContext<float>>& contextStack)
    {
        return context;
    }
};

// AB -> AAAB
class TestRule: public LRule<float>
{
public:
    TestRule() : LRule<float>(true) {}

    std::vector<LInstruction<float>*> InternalPattern(LSystem<float>& context) const
    {
        std::vector<LInstruction<float>*> pattern;

        // AB
        pattern.push_back(context.GetInstruction<TestInstruction>());
        pattern.push_back(context.GetInstruction<TestInstructionB>());

        return pattern;
    }

    std::vector<LInstruction<float>*> InternalEvaluate(LSystem<float>& context) const
    {
        std::vector<LInstruction<float>*> evaluation;

        // AAAB
        evaluation.push_back(context.GetInstruction<TestInstruction>());
        evaluation.push_back(context.GetInstruction<TestInstruction>());
        evaluation.push_back(context.GetInstruction<TestInstruction>());
        evaluation.push_back(context.GetInstruction<TestInstructionB>());

        return evaluation;
    }
};

#endif // LSYSTEM_H
