#ifndef UES_H
#define UES_H

#include "dibol.h"

void pASCII(variable* afield, variable* dfield);
void pDATE(variable* afield);
void pDECML(variable* afield,variable* dfield);
void pDELET(variable* ch, variable* filespec);
void pERROR(variable* errnum, variable* line);
void pFATAL(variable* action, variable* filespec);
void pFLAGS(variable* parameters, variable* action);
void pINSTR(variable* start, variable* string1, variable* string2, variable* position);
void pJBNO(variable* afield);
void pMONEY(variable* afield);
void pPAK(variable* record, variable* size, variable* dfield, ...);
void pRENAM(variable* ch, variable* newfile, variable* oldfile);
void pRSTAT(variable* size, variable* character);
void pRUNJB(variable* filespec, variable* terminal);
void pSIZE(variable* field, variable* size);
void pTEMPLATE(variable* afield);
void pTIME(variable* dfield);
void pTNMBR(variable* terminal);
void pTTSTS(variable* nfield,variable* ch);
void pUNPAK(variable* afield);
void pVERSN(variable* version);
void pWAIT(variable* seconds, variable* parameters, variable* event);
void pWKDAY(variable* afield);

#endif
