/*
 *  stream.h
 *  
 */

#ifndef STREAM_H
#define STREAM_H

#include "entryTable.h"


/* stream */
 
void GetCurTrans(char* stream, STREAM* trans, int offset);

void ShowStream(STREAM stream);//for debagging
void FreeStream(STREAM *stream, int stream_len, int phase);
void PrintZakiStream(STREAM steam, PARAMETER* para, FILE* file);

#endif
