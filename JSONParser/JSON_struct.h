#ifndef _JSON_struct_h_
#define _JSON_struct_h_

typedef struct JSONBlock {
    MemoryBank_t * memory_bank;
    JSONObjectType_t type;
    union {
        struct {
            int num_elements;
            char ** element_names; /* not used if it's an array */
            struct JSONBlock ** elements;
        } object_or_array;
        double numerical_value;
        char * text;
        int boolean;
    } value;
} JSONBlock_t;

#endif