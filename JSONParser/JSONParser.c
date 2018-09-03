#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <alloca.h>
// #include <stdint.h>

#include "JSONParser.h"

/* FNV-1a Hash function
 * https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function */
// static uint64_t hash(uint8_t * data, size_t len)
// {
//     uint64_t hash = 0xcbf29ce484222325;
//     uint8_t * end = data + len;
//     while (data < end)
//     {
//         hash = hash ^ *data;
//         hash = hash * 0x100000001b3;
//         ++data;
//     }
//     return hash;
// }

int is_start_of_json_token(char c)
{
    return ( c == '{' || c == '}' || c == '[' || c == ']' ||
             c == ':' || c == 'n' || c == 't' || c == 'f' ||
             c == '"' || c == ',' || c == '.' || isdigit(c) );
}

char * find_end_of_json_token(char * Start)
{
    switch (*Start)
    {
    case '{':
    case '}':
    case '[':
    case ']':
    case ':':
    case ',':
        ++Start;
        break;
    case 'n':
        if (strncmp("null", Start, 4))
            return NULL;
        Start += 4;
        break;
    case 't':
        if (strncmp("true", Start, 4))
            return NULL;
        Start += 4;
        break;
    case 'f':
        if (strncmp("false", Start, 5))
            return NULL;
        Start += 5;
        break;
    case '"':
        while (1)
        {
            ++Start;
            if (*Start == '\\') ++Start;
            else if (*Start == '"') break;
        }
        ++Start;
        break;
    default: /* Is a number */
        while (*Start == '.' || isdigit(*Start)) ++Start;
        break;
    }

    return Start;
}

char * find_start_of_json_token(char * Text)
{
    while (*Text && !is_start_of_json_token(*Text)) ++Text;
    return Text;
}

int json_count_tokens(char * Text)
{
    int num_tokens = 0;

    char * t = Text;
    char * end = Text + strlen(Text);

    while (t < end)
    {
        t = find_start_of_json_token(t);
        if (*t == 0) break;
        t = find_end_of_json_token(t);
        if (t == NULL) return 0;
        ++num_tokens;
    }

    return num_tokens;
}

/* Returns how many tokens ahead next block is */
int find_end_of_json_block(char ** Tokens, int * TokenLengths)
{
    int offset = 0;

    if (**Tokens == '[' || **Tokens == '{')
    {
        int open_brackets = 0;
        int close_brackets = 0;
        do {
            if (*Tokens[offset] == '{' || *Tokens[offset] == '[') ++open_brackets;
            if (*Tokens[offset] == '}' || *Tokens[offset] == ']') ++close_brackets;
            ++offset;
        } while (open_brackets != close_brackets);
    }
    else
    {
        offset = 1;
    }

    return offset;
}

/* Only things backslashes are usful for are newline and quotes */
void json_remove_backslashes(char * Text, int ApplyEscapeCodes)
{
    while (*Text)
    {
        if (*Text == '\\')
        {
            if (*(Text+1) == 'n' && ApplyEscapeCodes) *(Text+1) = '\n';
            char * back = Text;
            while (*back)
            {
                *back = *(back+1);
                ++back;
            }
        }
        ++Text;
    }
}

char * json_read_quotes(MemoryBank_t * MB, char * Text, char ** Pointer)
{
    char * start = Text;
    while (*(start++) != '"');
    char * end = find_end_of_json_token(start-1)-1;
    int len = end - start;
    char * string = MBMalloc(MB, len+1);
    memcpy(string, start, len);
    string[len] = 0;
    /* Remove backslashes to get real data */
    json_remove_backslashes(string, 0);
    /* Now apply escape codes */
    json_remove_backslashes(string, 1);
    if (Pointer) *Pointer = ++end;
    return string;
}

/* Will return end of block to *TokenOffset */
JSONBlock_t * parse_json(JSONBlock_t * Parent, char ** Tokens, int * TokenLengths, int * TokenOffset)
{
    JSONBlock_t * json = NULL;

    int parse_offset = 0;

    switch(**Tokens) /* Is start of a block */
    {
    case '{':
    {
        int num_attributes = 0;

        /* Count number of elements in array */
        int offset = 1;
        do {
            offset += find_end_of_json_block(Tokens+offset, TokenLengths+offset); /* The name */
            ++offset; /* The colon */
            offset += find_end_of_json_block(Tokens+offset, TokenLengths+offset); /* The value */
            ++num_attributes;
        } while (*Tokens[offset++] != '}');

        json = new_EmptyJSONObject(Parent, num_attributes);

        offset = 1;
        for (int i = 0; i < num_attributes; ++i)
        {
            JSONObjectSetAttributeName(json, i, json_read_quotes(json->memory_bank, Tokens[offset], NULL));
            ++offset; /* Pass the name */
            ++offset; /* Pass the colon */
            int extra_offset;  /* How many tokens the block is */
            JSONObjectSetAttributeByIndex(json, i, parse_json(json, Tokens+offset, TokenLengths+offset, &extra_offset));
            offset += extra_offset; /* Pass the object */
            ++offset; /* To pass the comma */
        }

        parse_offset = offset;
    }
    break;

    case '[':
    {
        int num_elements = 0;

        /* Count number of elements in array */
        int offset = 1;
        do {
            offset += find_end_of_json_block(Tokens+offset, TokenLengths+offset);
            ++num_elements;
        } while (*Tokens[offset++] != ']');

        json = new_EmptyJSONArray(Parent, num_elements);

        offset = 1;
        for (int i = 0; i < num_elements; ++i)
        {
            int extra_offset; /* How many tokens the block is */
            JSONArraySetElement(json, i, parse_json(json,Tokens+offset,TokenLengths+offset,&extra_offset));
            offset += extra_offset; /* Pass the object */
            ++offset; /* To pass the comma */
        }

        parse_offset = offset;
    }
    break;

    case '"':
        json = new_JSONString(Parent, NULL);
        json->value.text = json_read_quotes(json->memory_bank, *Tokens, NULL);
        parse_offset = 1;
        break;

    case 't':
        json = new_JSONBoolean(Parent, 1);
        parse_offset = 1;
        break;
    case 'f':
        json = new_JSONBoolean(Parent, 1);
        parse_offset = 1;
        break;

    case 'n':
        json = new_JSONNull(Parent);
        parse_offset = 1;
        break;

    default:
        json = new_JSONNumber(Parent, 0.0);
        sscanf(*Tokens, "%lf", &json->value.numerical_value);
        parse_offset = 1;
        break;
    }

    if (TokenOffset) *TokenOffset = parse_offset;
    return json;
}

void json_clear_comments(char * Text)
{
    char previous = 0;

    while (*Text != 0)
    {
        if (*Text == '"') /* Skip text */
        {
            Text = find_end_of_json_token(Text);
        }
        if (*Text == '*' && previous == '/')
        {
            char * end = Text;
            *(Text-1) = ' ';
            *Text = ' ';
            while (*(end-1) != '*' && *end != '/') ++end;
            while (Text <= end)
            {
                *Text = ' ';
                ++Text;
            }
        }
        if (*Text == '/' && previous == '/')
        {
            char * end = Text;
            *(Text-1) = ' ';
            *Text = ' ';
            while (*end != '\n' && *end != 0) ++end;
            while (Text != end)
            {
                *Text = ' ';
                ++Text;
            }
        }

        previous = *Text;
        ++Text;
    }
}

JSONBlock_t * ParseJSON(char * Text)
{
    json_clear_comments(Text);
    int num_blocks = 0;
    int num_tokens = json_count_tokens(Text);
    if (num_tokens == 0) return NULL;
    // fix mallocc!!!!
    char ** tokens = malloc(num_tokens * sizeof(char *));
    int * token_lengths = malloc(num_tokens * sizeof(int));

    char * t = Text;
    for (int i = 0; i < num_tokens; ++i)
    {
        char * start = find_start_of_json_token(t);
        char * end = find_end_of_json_token(start);

        tokens[i] = start;
        token_lengths[i] = end-start;

        t = end;
    }

    for (int i = 1; i < (num_tokens-1); ++i)
    {
        if (*tokens[i] != ':' && *tokens[i] != ',' && (*tokens[i-1] == ':' || *tokens[i+1] != ':'))
            num_blocks++;
    }

    MemoryBank_t * mb = new_MemoryBank(1);
    JSONBlock_t * all_blocks = MBMalloc(mb, num_blocks * sizeof(JSONBlock_t));

    /* for (int i = 0; i < num_tokens; ++i) {
        char old = tokens[i][token_lengths[i]];
        tokens[i][token_lengths[i]] = 0;
        printf("token %i: %s\n", i, tokens[i]);
        tokens[i][token_lengths[i]] = old;
    } */

    /* Check if JSON is valid (very basic, not too good) */
    int close_brackets = 0, open_brackets = 0;
    for (int i = 0; i < num_tokens; ++i)
    {
        if (*tokens[i] == '{' || *tokens[i] == '[') ++open_brackets;
        else if (*tokens[i] == '}' || *tokens[i] == ']') ++close_brackets;
    }
    if (close_brackets != open_brackets) return NULL;

    /* Just a fake parent block to feed to the parse_json ting */
    JSONBlock_t * parent_block = alloca(sizeof(JSONBlock_t));
    parent_block->memory_bank = mb;
    JSONBlock_t * json = parse_json(NULL, tokens, token_lengths, NULL);
    return json;
}

void json_print_tabs(int Levels, FILE * Out)
{
    for (int i = 0; i < Levels; ++i) fprintf(Out, "    ");
}

int json_is_array_or_object(JSONBlock_t * JSON)
{
    JSONObjectType_t type = JSONBlockGetType(JSON);
    return (type == JSONArray || type == JSONObject);
}

/* Check is an object or awway should be printed on one line */
int json_is_one_liner(JSONBlock_t * JSON)
{
    int is_one_liner = 1;
    switch (JSONBlockGetType(JSON))
    {
        case JSONArray: {
            int has_arrays_or_objects = 0;
            int len = JSONArrayGetLength(JSON);
            for (int i = 0; i < len; ++i) {
                if (json_is_array_or_object(JSONArrayGetElement(JSON,i))) {
                    has_arrays_or_objects = 1;
                    break;
                }
            }
            if (has_arrays_or_objects) is_one_liner = 0;
        } break;
        case JSONObject: {
            // if ( JSONObjectGetNumAttributes(JSON) > 1 ||
            //      json_is_array_or_object(JSONObjectGetAttributeByIndex(JSON, 1)))
            //     is_one_liner = 0;
            is_one_liner = 0;
        } break;
    }
    return is_one_liner;
}

void write_json(JSONBlock_t * JSON, FILE * Out, int LevelsIn, int NewLineBracket, int OneLiner)
{
    switch (JSONBlockGetType(JSON))
    {
    case JSONObject:
        if (!OneLiner)
        {
            fprintf(Out, "{\n");
            for (int e = 0; e < JSONObjectGetNumAttributes(JSON); ++e)
            {
                int is_one_liner = json_is_one_liner(JSONObjectGetAttributeByIndex(JSON, e));
                json_print_tabs(LevelsIn+1, Out);
                fprintf(Out, "\"%s\": ", JSONObjectGetAttributeName(JSON, e));
                /* Go to next line if it is an array */
                if (!is_one_liner && NewLineBracket)
                {
                    fprintf(Out, "\n");
                    json_print_tabs(LevelsIn+1, Out);
                }
                write_json(JSONObjectGetAttributeByIndex(JSON, e), Out, LevelsIn+1, NewLineBracket, is_one_liner);
                if (e != JSONObjectGetNumAttributes(JSON)-1) fprintf(Out, ",");
                fprintf(Out, "\n");
            }
            json_print_tabs(LevelsIn, Out);
            fprintf(Out, "}");
        }
        else
        {
            fprintf(Out, "{\"%s\": ", JSONObjectGetAttributeName(JSON, 0));
            write_json(JSONObjectGetAttributeByIndex(JSON, 0), Out, 0, NewLineBracket, OneLiner);
            fprintf(Out, "}");
        }
        break;

    case JSONArray:
        fprintf(Out, "[");
        for (int e = 0; e < JSONArrayGetLength(JSON); ++e)
        {  
            if (!OneLiner)
            {
                fprintf(Out, "\n");
                json_print_tabs(LevelsIn+1, Out);
            }
            write_json(JSONArrayGetElement(JSON, e), Out, LevelsIn+1, NewLineBracket, 0);
            if (e !=  JSONArrayGetLength(JSON)-1) fprintf(Out, ",");
            else if (!OneLiner) fprintf(Out, "\n");
        }
        if (!OneLiner) json_print_tabs(LevelsIn, Out);
        fprintf(Out, "]");
        break;

    case JSONNumber:
        fprintf(Out, "%.7lg", JSONNumberGetValue(JSON));
        break;

    case JSONString:
        fprintf(Out, "\"");
        char * t = JSONStringGetTextPointer(JSON);
        while (1)
        {
            char * u = t;
            while (*u && *u != '"' && *u != '\\' && *u != '\n') ++u;
            char c = *u;
            *u = 0;
            fprintf(Out, "%s", t);
            *u = c;
            if (c == '"') fprintf(Out, "\\\\\\\"");
            if (c == '\\') fprintf(Out, "\\\\\\\\");
            if (c == '\n') fprintf(Out, "\\\\\\n");
            else if (c == 0) break;
            t = u+1;
        }
        fprintf(Out, "\"");
        break;

    case JSONBoolean:
        if (JSONBooleanGetValue(JSON))
            fprintf(Out, "true");
        else
            fprintf(Out, "false");
        break;

    case JSONNull:
        fprintf(Out, "null");
        break;
    }
}

void WriteJSON(JSONBlock_t * JSON, int BracketOnNewLine, FILE * Out)
{
    write_json(JSON, Out, 0, BracketOnNewLine, 0);
}

void FreeJSON(JSONBlock_t * JSON)
{
    delete_MemoryBank(JSON->memory_bank);
}

void JSONArraySetLength(JSONBlock_t * Array, int Length)
{
    Array->value.object_or_array.num_elements = Length;
    Array->value.object_or_array.elements = MBRealloc( Array->memory_bank,
                                                       Array->value.object_or_array.elements,
                                                       sizeof(char *) * Length );
}

void JSONObjectSetNumAttributes(JSONBlock_t * Object, int NumAttributes)
{
    JSONArraySetLength(Object, NumAttributes); /* Does half the work */
    Object->value.object_or_array.element_names = MBRealloc( Object->memory_bank,
                                                             Object->value.object_or_array.element_names,
                                                             sizeof(char *) * NumAttributes );
}

void JSONArrayAppendElement(JSONBlock_t * Array, JSONBlock_t * Element)
{
    int length = JSONArrayGetLength(Array);
    JSONArraySetLength(Array, length+1);
    Array->value.object_or_array.elements[length] = Element;
}

void JSONObjectAppendAttribute(JSONBlock_t * Object, JSONBlock_t * Value, char * Name)
{
    int num_attributes = JSONObjectGetNumAttributes(Object);
    JSONObjectSetNumAttributes(Object, num_attributes+1);
    JSONObjectSetAttributeName(Object, num_attributes, Name);
    JSONObjectSetAttributeByIndex(Object, num_attributes, Value);
}


/* Parent can be NULL */
JSONBlock_t * new_EmptyJSONObject(JSONBlock_t * Parent, int NumAttributes)
{
    MemoryBank_t * mb;
    if (Parent != NULL) mb = Parent->memory_bank;
    else mb = new_MemoryBank(1);
    JSONBlock_t * object = MBMalloc(mb, sizeof(JSONBlock_t));
    object->memory_bank = mb;

    object->type = JSONObject;

    object->value.object_or_array.num_elements = NumAttributes;
    object->value.object_or_array.element_names = MBZeroAlloc(mb, NumAttributes * sizeof(char *));
    object->value.object_or_array.elements = MBZeroAlloc(mb, NumAttributes * sizeof(JSONBlock_t *));

    return object;
}

/* Parent can be NULL */
JSONBlock_t * new_EmptyJSONArray(JSONBlock_t * Parent, int Length)
{
    MemoryBank_t * mb;
    if (Parent != NULL) mb = Parent->memory_bank;
    else mb = new_MemoryBank(1);
    JSONBlock_t * array = MBMalloc(mb, sizeof(JSONBlock_t));
    array->memory_bank = mb;

    array->type = JSONArray;

    array->value.object_or_array.num_elements = Length;
    array->value.object_or_array.elements = MBZeroAlloc(mb, Length * sizeof(JSONBlock_t *));

    return array;
}

void JSONArraySetElement(JSONBlock_t * Array, int Index, JSONBlock_t * Value)
{
    Array->value.object_or_array.elements[Index] = Value;
}

JSONBlock_t * new_JSONNumber(JSONBlock_t * Parent, double Value)
{
    JSONBlock_t * value = MBMalloc(Parent->memory_bank, sizeof(JSONBlock_t));
    value->memory_bank = Parent->memory_bank;

    value->type = JSONNumber;

    value->value.numerical_value = Value;

    return value;
}

JSONBlock_t * new_JSONString(JSONBlock_t * Parent, char * Text)
{
    JSONBlock_t * string = MBMalloc(Parent->memory_bank, sizeof(JSONBlock_t));
    string->memory_bank = Parent->memory_bank;

    string->type = JSONString;

    if (Text == NULL) Text = "";

    string->value.text = MBMalloc(Parent->memory_bank, sizeof(char) * (strlen(Text)+1));
    strcpy(string->value.text, Text);

    return string;
}

JSONBlock_t * new_JSONBoolean(JSONBlock_t * Parent, int Value)
{
    JSONBlock_t * boolean = MBMalloc(Parent->memory_bank, sizeof(JSONBlock_t));
    boolean->memory_bank = Parent->memory_bank;

    boolean->type = JSONBoolean;

    boolean->value.boolean = Value;

    return boolean;
}

JSONBlock_t * new_JSONNull(JSONBlock_t * Parent)
{
    JSONBlock_t * null = MBMalloc(Parent->memory_bank, sizeof(JSONBlock_t));
    null->memory_bank = Parent->memory_bank;

    null->type = JSONNull;

    return null;
}

void JSONObjectSetAttributeName(JSONBlock_t * Object, int Index, char * Name)
{
    Object->value.object_or_array.element_names[Index] = MBMalloc(Object->memory_bank, strlen(Name)+1);
    strcpy(Object->value.object_or_array.element_names[Index], Name);
}

void JSONObjectSetAttributeByIndex(JSONBlock_t * Object, int AttributeIndex, JSONBlock_t * Value)
{
    JSONArraySetElement(Object, AttributeIndex, Value);
}

void JSONObjectSetAttributeByName(JSONBlock_t * Object, char * AttributeName, JSONBlock_t * Value)
{
    int index = 0;
    while (index < Object->value.object_or_array.num_elements)
    {
        if (Object->value.object_or_array.element_names[index] != NULL)
            if (!strcmp(AttributeName, Object->value.object_or_array.element_names[index]))
                break;
        ++index;
    }
    JSONObjectSetAttributeByIndex(Object, index, Value);
}

void JSONBoolanSetValue(JSONBlock_t *_Boolean, int Value)
{
    _Boolean->value.boolean = Value;
}

void JSONNumberSetValue(JSONBlock_t * Number, double Value)
{
    Number->value.numerical_value = Value;
}

JSONObjectType_t JSONBlockGetType(JSONBlock_t * JSON)
{
    return JSON->type;
}

int JSONArrayGetLength(JSONBlock_t * Array)
{
    return Array->value.object_or_array.num_elements;
}

int JSONObjectGetNumAttributes(JSONBlock_t * Object)
{
    return JSONArrayGetLength(Object); /* Interchangeable sometimes */
}

JSONBlock_t * JSONObjectGetAttributeByName(JSONBlock_t * Object, char * Name)
{
    for (int i = 0; i < Object->value.object_or_array.num_elements; ++i)
    {
        if (!strcmp(Object->value.object_or_array.element_names[i], Name))
            return JSONArrayGetElement(Object, i);
    }
    return NULL;
}

JSONBlock_t * JSONObjectGetAttributeByIndex(JSONBlock_t * Object, int Index)
{
    return JSONArrayGetElement(Object, Index);
}

JSONBlock_t * JSONArrayGetElement(JSONBlock_t * Array, int Index)
{
    return Array->value.object_or_array.elements[Index];
}

char * JSONStringGetTextPointer(JSONBlock_t * String)
{
    return String->value.text;
}

char * JSONObjectGetAttributeName(JSONBlock_t * Object, int AttributeIndex)
{
    return Object->value.object_or_array.element_names[AttributeIndex];
}

double JSONNumberGetValue(JSONBlock_t * Number)
{
    return Number->value.numerical_value;
}

int JSONBooleanGetValue(JSONBlock_t *_Boolean)
{
    return _Boolean->value.boolean;
}


#if 0
int main()
{
    FILE * text = fopen("JSONSamples/JSONSample8.json", "rb");
    fseek(text, 0, SEEK_END);
    int filesize = ftell(text);
    fseek(text, 0, SEEK_SET);
    char * JSONText = malloc(filesize+1);
    JSONText[filesize] = 0;
    fread(JSONText, filesize, 1, text);
    // puts(JSONText);

    // FILE * output = fopen("JSONOUTPUT.json", "wb");
    JSONBlock_t * JSON = ParseJSON(JSONText);
    if (!JSON) puts("Failed");
    else
    {
        WriteJSON(JSON, 1, stdout);
        puts("");
        FreeJSON(JSON);
    }


    /*
     * Test
     */
    // JSONBlock_t * test = new_EmptyJSONObject(NULL, 3);
    // JSONObjectSetAttributeName(test, 0, "thing1");
    // JSONObjectSetAttributeName(test, 1, "some_numbers");
    // JSONObjectSetAttributeName(test, 2, "other");
    // JSONObjectSetAttributeByName(test, "thing1", new_JSONNull(test));
    // JSONObjectSetAttributeByName(test, "other", new_JSONString(test, "asdfghjklqwertyuiopzx"));
    // JSONBlock_t * array = new_EmptyJSONArray(test, 4);
    // JSONArraySetElement(array, 0, new_JSONNumber(array, 4.354));
    // JSONArraySetElement(array, 1, new_JSONNumber(array, 9.34332));
    // JSONArraySetElement(array, 2, new_JSONNumber(array, 34564.2));
    // JSONArraySetElement(array, 3, new_JSONString(array,
    //   "hahaha this is a string in an array of numbers ahahaha json"));
    // JSONObjectSetAttributeByIndex(test, 1, array);
    // WriteJSON(test, 1, stdout);
    // puts("");
    // FreeJSON(test);

    return 0;
}

// rm *.o; clear; gcc -c JSONParser.c; gcc -c ../MemoryBank/MemoryBank.c; gcc *.o; ./a.out
#endif