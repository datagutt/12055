#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "gbapi.h"


GBRETURN LoadLanguageDB(GBLPCVOID ldb);
GBRETURN Initialize();
GBRETURN Reset();
GBRETURN SetEngineOption(GBINT engine_id, GBU32 config, GBU32 option);

GBRETURN SetCurLanguage(GBINT lang, GBINT subLang);
GBRETURN SetCurInputMode(GBINT inputMode);


GBRETURN HandleKey(GBINT key);
GBPCWCHAR * GetCandidates(GBINT startIndex, GBINT wantCount, GBU16 *returnCount, GBU16 *start);
GBBOOL HaveMoreCandate();
GBRETURN SelectCandidate(GBLPCWCHAR cand);
GBRETURN SelectCandidateEx(GBLPCWCHAR cand, GBINT index);
GBINT GetInputMethodState();
GBPWCHAR GetComposing();
GBPCWCHAR GetOutputString();
GBWCHAR	* GetUpScreenStr();

GBPCWCHAR * GetSyllables(GBINT *returnCount);
GBRETURN SelectSyllable(GBINT nSyllableIndex);
GBPCWCHAR GetComponents();
GBRETURN SetAssocWord(GBLPCWCHAR pConfirmedWord, GBBOOL bReset);
GBPWCHAR Word2CodesByInputmod(int nInputmod, unsigned short isn, GBINT indexMulPron);


GBRETURN CnUDBAttach(GBINT lang, GBINT subLang, GBLPVOID pUDBBuffer, GBLPVOID pUpContentBuffer);
GBRETURN UDBAttach(GBINT lang, GBINT subLang, GBLPVOID pUDBBuffer);
GBRETURN UDBAddPhrase (GBLPCWCHAR pWord, GBLPCWCHAR pSyllable);
GBRETURN UDBDeletePhrase(GBLPCWCHAR phrase);
GBRETURN UDBStartSearchPhrase();
GBPWCHAR UDBGetNextPhrase();
GBRETURN ChnUDBDataReset();
