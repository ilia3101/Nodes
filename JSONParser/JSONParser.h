#ifndef _JSONParser_h_
#define _JSONParser_h_

#include "../MemoryBank/MemoryBank.h"
#include "JSON_struct.h"


/* JSONObjectType_t possible values: */
#define JSONObject 0
#define JSONArray 1
#define JSONNumber 2
#define JSONString 3
#define JSONBoolean 4
#define JSONNull 5


JSONBlock_t * ParseJSON(char * Text);
/* new_EmptyJSONObject and new_EmptyJSONArray do not require a Parent */
JSONBlock_t * new_EmptyJSONObject(JSONBlock_t * Parent, int NumAttributes);
JSONBlock_t * new_EmptyJSONArray(JSONBlock_t * Parent, int Length);
JSONBlock_t * new_JSONNumber(JSONBlock_t * Parent, double Value);
JSONBlock_t * new_JSONString(JSONBlock_t * Parent, char * Text);
JSONBlock_t * new_JSONBoolean(JSONBlock_t * Parent, int Value);
JSONBlock_t * new_JSONNull(JSONBlock_t * Parent);

void FreeJSON(JSONBlock_t * JSON);


JSONObjectType_t JSONBlockGetType(JSONBlock_t * JSON);


void WriteJSON(JSONBlock_t * JSON, int BracketOnNewLine, FILE * Out);


void JSONArraySetElement(JSONBlock_t * Array, int Index, JSONBlock_t * Value);
void JSONObjectSetAttributeName(JSONBlock_t * Object, int Index, char * Name);
void JSONObjectSetAttributeByName(JSONBlock_t * Object, char * AttributeName, JSONBlock_t * Value);
void JSONObjectSetAttributeByIndex(JSONBlock_t * Object, int AttributeIndex, JSONBlock_t * Value);
void JSONBoolanSetValue(JSONBlock_t *_Boolean, int Value);
void JSONNumberSetValue(JSONBlock_t * Number, double Value);


#endif