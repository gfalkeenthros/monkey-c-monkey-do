#include "code/code.h"
#include "test_helpers.h"
#include <string.h> 

#define ARRAY_SIZE(v) sizeof v / sizeof v[0]

void test_make_instruction() {
    struct {
        enum opcode opcode;
        int operands[MAX_OP_SIZE];
        unsigned char expected[MAX_OP_SIZE];
        size_t expected_size;
    } tests[] = {
        {
            .opcode = OP_CONST, 
            .operands = {65534}, 
            .expected = {OP_CONST, 255, 254},
            .expected_size = 3
        },
        {
            .opcode = OP_CONST, 
            .operands = {65535}, 
            .expected = {OP_CONST, 255, 255},
            .expected_size = 3
        },
        {
            .opcode = OP_CONST, 
            .operands = {1}, 
            .expected = {OP_CONST, 0, 1},
            .expected_size = 3
        },
        {
            .opcode = OP_CONST, 
            .operands = {2}, 
            .expected = {OP_CONST, 0, 2},
            .expected_size = 3
        }
    };

    for (int i=0; i < ARRAY_SIZE(tests); i++) {
        struct instruction *ins = make_instruction(tests[i].opcode, tests[i].operands);
        
        assertf(ins->size == tests[i].expected_size, "wrong length: expected %d, got %d", tests[i].expected_size, ins->size);
        for (int j=0; j < tests[i].expected_size; j++) {
            assertf(ins->bytes[j] == tests[i].expected[j], "[%d] invalid byte value at index %d: expected %d, got %d", i, j, tests[i].expected[j], ins->bytes[j]);
        }

        free(ins);
    }
}

void test_instruction_string() {
    struct instruction *instructions[] = {
        make_instruction(OP_CONST, (int[]) {1}),
        make_instruction(OP_CONST, (int[]) {2}),
        make_instruction(OP_CONST, (int[]) {65535})
    };

    char *expected_str = "0000 OpConstant 1\n0003 OpConstant 2\n0006 OpConstant 65535";
    struct instruction *ins = flatten_instructions_array(instructions, 3);

    char *str = instruction_to_str(ins);
    assertf(strcmp(expected_str, str) == 0, "wrong instruction string: expected \"%s\", got \"%s\"", expected_str, str);
    free(str);
}

void test_read_operands() {
    struct {
        enum opcode opcode;
        int operands[8];
        size_t bytes_read;
    } tests[] = {
        {OP_CONST, {65535}, 2},
        {OP_CONST, {1}, 2},
    };

    for (int t = 0; t < ARRAY_SIZE(tests); t++) {
        struct instruction *ins = make_instruction(tests[t].opcode, tests[t].operands);
        struct definition def = lookup(tests[t].opcode);
        int operands[3] = {0};
        size_t bytes_read = read_operands(operands, def, ins, 0);
        assertf(bytes_read == tests[t].bytes_read, "wrong number of bytes read: expected %d, got %d", tests[t].bytes_read, bytes_read);
        for (int i=0; i < def.operands; i++) {
            assertf(tests[t].operands[i] == operands[i], "wrong operand: expected %d, got %d", tests[t].operands[i], operands[i]);
        }
    }
}

int main() {
    test_make_instruction();
    test_read_operands();
    test_instruction_string();
    

    printf("\x1b[32mAll tests passed!\033[0m\n");
}