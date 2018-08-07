#ifndef _JSONParser_h_
#define _JSONParser_h_

#include <stdio.h>
#include "../MemoryBank/MemoryBank.h"
#include "JSON_struct.h"


/* JSONObjectType_t possible values: */
#define JSONObject 0
#define JSONArray 1
#define JSONNumber 2
#define JSONString 3
#define JSONBoolean 4
#define JSONNull 5


/* Constructors. For JSONObject and JSONArray Parent may be NULL. */
JSONBlock_t * new_EmptyJSONObject(JSONBlock_t * Parent, int NumAttributes); /* NumAttributes can be 0, then use append */
JSONBlock_t * new_EmptyJSONArray(JSONBlock_t * Parent, int Length); /* Length can be 0, then append */
JSONBlock_t * new_JSONNumber(JSONBlock_t * Parent, double Value);
JSONBlock_t * new_JSONString(JSONBlock_t * Parent, char * Text);
JSONBlock_t * new_JSONBoolean(JSONBlock_t * Parent, int Value);
JSONBlock_t * new_JSONNull(JSONBlock_t * Parent);

/* Creates a whole JSON structure from JSON text */
JSONBlock_t * ParseJSON(char * Text);


/* Destructor (Destroys all blocks linked to parent, so only call once) */
void FreeJSON(JSONBlock_t * JSON);


/* Writes JSON structure to JSON text to FILE * out */
void WriteJSON(JSONBlock_t * JSON, int BracketOnNewLine, FILE * Out);


/* Getters */
JSONObjectType_t JSONBlockGetType(JSONBlock_t * JSON);
int JSONArrayGetLength(JSONBlock_t * Array);
int JSONObjectGetNumAttributes(JSONBlock_t * Object);
JSONBlock_t * JSONObjectGetAttributeByName(JSONBlock_t * Object, char * Name);
JSONBlock_t * JSONObjectGetAttributeByIndex(JSONBlock_t * Object, int Index);
JSONBlock_t * JSONArrayGetElement(JSONBlock_t * Array, int Index);
char * JSONStringGetTextPointer(JSONBlock_t * String);
char * JSONObjectGetAttributeName(JSONBlock_t * Object, int AttributeIndex);
double JSONNumberGetValue(JSONBlock_t * Number);
int JSONBooleanGetValue(JSONBlock_t *_Boolean);


/* Setters */
void JSONArraySetElement(JSONBlock_t * Array, int Index, JSONBlock_t * Value);
void JSONObjectSetAttributeName(JSONBlock_t * Object, int Index, char * Name);
void JSONObjectSetAttributeByName(JSONBlock_t * Object, char * AttributeName, JSONBlock_t * Value);
void JSONObjectSetAttributeByIndex(JSONBlock_t * Object, int AttributeIndex, JSONBlock_t * Value);
void JSONBoolanSetValue(JSONBlock_t *_Boolean, int Value);
void JSONNumberSetValue(JSONBlock_t * Number, double Value);
void JSONArraySetLength(JSONBlock_t * Array, int Length);
void JSONObjectSetNumAttributes(JSONBlock_t * Object, int NumAttributes);


/* Other */
void JSONArrayAppendElement(JSONBlock_t * Array, JSONBlock_t * Element);
void JSONObjectAppendAttribute(JSONBlock_t * Object, JSONBlock_t * Value, char * Name);


#endif