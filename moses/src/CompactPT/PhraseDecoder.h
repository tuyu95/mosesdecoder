#ifndef moses_PhraseDecoder_h
#define moses_PhraseDecoder_h

#include <sstream>
#include <vector>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <fstream>
#include <string>
#include <iterator>
#include <algorithm>
#include <sys/stat.h>

#include "TypeDef.h"
#include "FactorCollection.h"
#include "Word.h"
#include "Util.h"
#include "InputFileStream.h"
#include "StaticData.h"
#include "WordsRange.h"
#include "UserMessage.h"

#include "PhraseDictionaryCompact.h"
#include "StringVector.h"
#include "CanonicalHuffman.h"
#include "ConsistantPhrases.h"
#include "TargetPhraseCollectionCache.h"

namespace Moses
{

class PhraseDictionaryCompact;

class PhraseDecoder
{
  protected:
    
    friend class PhraseDictionaryCompact;
    
    typedef std::pair<unsigned char, unsigned char> AlignPoint;
    typedef std::pair<unsigned, unsigned> SrcTrg;
        
    enum Coding { None, REnc, PREnc } m_coding;
    
    size_t m_numScoreComponent;
    bool m_containsAlignmentInfo;
    size_t m_maxRank;
    size_t m_maxPhraseLength;
    
    boost::unordered_map<std::string, unsigned> m_sourceSymbolsMap;
    StringVector<unsigned char, unsigned, std::allocator> m_sourceSymbols;
    StringVector<unsigned char, unsigned, std::allocator> m_targetSymbols;
    
    std::vector<size_t> m_lexicalTableIndex;
    std::vector<SrcTrg> m_lexicalTable;
    
    CanonicalHuffman<unsigned>* m_symbolTree;
    
    bool m_multipleScoreTrees;
    std::vector<CanonicalHuffman<float>*> m_scoreTrees;
    
    CanonicalHuffman<AlignPoint>* m_alignTree;
    
    TargetPhraseCollectionCache m_decodingCache;
    
    PhraseDictionaryCompact& m_phraseDictionary;   
    
    // ***********************************************
    
    const std::vector<FactorType>* m_input;
    const std::vector<FactorType>* m_output;
    const PhraseDictionaryFeature* m_feature;
    const std::vector<float>* m_weight;
    float m_weightWP;
    const LMList* m_languageModels;
    
    std::string m_separator;
  
    // ***********************************************
    
    unsigned GetSourceSymbolId(std::string& s);
    std::string GetTargetSymbol(unsigned id) const;
    
    size_t GetREncType(unsigned encodedSymbol);
    size_t GetPREncType(unsigned encodedSymbol);
    
    unsigned GetTranslation(unsigned srcIdx, size_t rank);
    
    size_t GetMaxSourcePhraseLength();
    
    unsigned DecodeREncSymbol1(unsigned encodedSymbol);
    unsigned DecodeREncSymbol2Rank(unsigned encodedSymbol);
    unsigned DecodeREncSymbol2Position(unsigned encodedSymbol);
    unsigned DecodeREncSymbol3(unsigned encodedSymbol);
    
    unsigned DecodePREncSymbol1(unsigned encodedSymbol);
    int DecodePREncSymbol2Left(unsigned encodedSymbol);
    int DecodePREncSymbol2Right(unsigned encodedSymbol);
    unsigned DecodePREncSymbol2Rank(unsigned encodedSymbol);
    
    std::string MakeSourceKey(std::string &);
    
  public:
    
    PhraseDecoder(
      PhraseDictionaryCompact &phraseDictionary,
      const std::vector<FactorType>* &input,
      const std::vector<FactorType>* &output,
      const PhraseDictionaryFeature* feature,
      size_t numScoreComponent,
      const std::vector<float>* weight,
      float weightWP,
      const LMList* languageModels
    );
    
    ~PhraseDecoder();
     
    size_t Load(std::FILE* in);
    
    TargetPhraseVectorPtr CreateTargetPhraseCollection(const Phrase &sourcePhrase,
                                                       bool topLevel = false);
    
    TargetPhraseVectorPtr DecodeCollection(TargetPhraseVectorPtr tpv,
                                           BitStream<> &encodedBitStream,
                                           const Phrase &sourcePhrase,
                                           bool topLevel);
    
    void PruneCache();
};

}

#endif