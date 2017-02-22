#include "base.h"

static UINT MakeBelt(BYTE fmt_id,BYTE *belt);		//·µ»ØBELT³¤¶È
static BYTE *BeltSelect(BYTE *des_belt,BYTE *src_belt,BYTE **sfmt);
static BYTE *GetFmt(BYTE fmt_id,BYTE *fmt);
static BYTE *ReadCxRecBelt(BYTE *belt,BYTE **sfmt);
static BYTE *AddCxBelt(BYTE *belt,UINT uAddr,BYTE CxBeltLen,UINT recTop,UINT recBottom);
static void MakeFrmHead(BYTE op,UINT lenMsg,BYTE *msg);
static BYTE numMatched(BYTE *recDT);

