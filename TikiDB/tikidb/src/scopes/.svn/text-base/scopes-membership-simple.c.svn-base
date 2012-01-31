#include "contiki.h"

#include "scopes-types.h"
#include "scopes-membership-simple.h"
#include "scopes-repository.h"

static int pos;
static int length;
static uint8_t *bytecode;
static int invalid_input;

/* function prototypes */
static int parse_operator_and(void);
static int parse_operator_or(void);
static int parse_operator_not(void);
static int parse_predicate_eq(void);
static int parse_predicate_neq(void);
static int parse_predicate_lt(void);
static int parse_predicate_gt(void);
static int parse_predicate_let(void);
static int parse_predicate_get(void);
static int parse_uint8_value(void);
static int parse_uint16_value(void);
static int parse_int8_value(void);
static int parse_int16_value(void);
static int parse_repository_value(void);
static int parse_operator_add(void);
static int parse_operator_sub(void);
static int parse_operator_div(void);
static int parse_operator_mult(void);
static int parse_operator_mod(void);

/* array of parser functions */
static parse_func parser[] = { parse_operator_and,
                               parse_operator_or,
                               parse_operator_not,
                               parse_predicate_eq,
                               parse_predicate_neq,
                               parse_predicate_lt,
                               parse_predicate_gt,
                               parse_predicate_let,
                               parse_predicate_get,
                               parse_uint8_value,
                               parse_uint16_value,
                               parse_int8_value,
                               parse_int16_value,
                               parse_repository_value,
							   parse_operator_add,
							   parse_operator_sub,
							   parse_operator_div,
							   parse_operator_mult,
							   parse_operator_mod};

/* read single byte from bytecode */
static int
read_byte(void)
{
  /* check if there is input left */
  if (pos < length) {
    uint8_t value = bytecode[pos];
    pos++;
    return value;
  }

  /* no more input */
  invalid_input = 1;
  return MEMBERSHIP_NO_MORE_INPUT;
}

/* parse operand of unary operator or predicate */
static void
parse_unary(int *operand)
{
  int code;

  /* parse operand */
  code = read_byte();

  if (IS_VALID(code)) {
    *operand = parser[code]();
  }
  else {
    invalid_input = 1;
  }
}

/* parse operands of binary operator or predicate */
static void
parse_binary(int *operand1, int *operand2)
{
  int code;

  /* parse first operand */
  code = read_byte();

  if (IS_VALID(code)) {
    *operand1 = parser[code]();
  }
  else {
    invalid_input = 1;
  }

  PRINTF(3,",");

  /* parse second operand */
  code = read_byte();

  if (IS_VALID(code)) {
    *operand2 = parser[code]();
  }
  else {
    invalid_input = 1;
  }
}

/* operators */
static int
parse_operator_and(void)
{
  PRINTF(3,"AND(");

  /* parse operands */
  int op1, op2;
  parse_binary(&op1, &op2);

  /* calculate result */
  int result = (op1 && op2);

  PRINTF(3,")");

  return result;
}

static int
parse_operator_or(void)
{
  PRINTF(3,"OR(");

  /* parse operands */
  int op1, op2;
  parse_binary(&op1, &op2);

  /* calculate result */
  int result = (op1 || op2);

  PRINTF(3,")");

  return result;
}

static int
parse_operator_add(void)
{
	PRINTF(3,"ADD(");
	
	/* parse operands */
	int op1, op2;
	parse_binary(&op1, &op2);
	
	/* calculate result */
	int result = (op1 + op2);
	
	PRINTF(3,")");
	
	return result;
}

static int
parse_operator_sub(void)
{
	PRINTF(3,"SUB(");
	
	/* parse operands */
	int op1, op2;
	parse_binary(&op1, &op2);
	
	/* calculate result */
	int result = (op1 - op2);
	
	PRINTF(3,")");
	
	return result;
}

static int
parse_operator_div(void)
{
	PRINTF(3,"DIV(");
	
	/* parse operands */
	int op1, op2;
	parse_binary(&op1, &op2);
	
	/* calculate result */
	int result = (op1 / op2);
	
	PRINTF(3,")");
	
	return result;
}

static int
parse_operator_mult(void)
{
	PRINTF(3,"MULT(");
	
	/* parse operands */
	int op1, op2;
	parse_binary(&op1, &op2);
	
	/* calculate result */
	int result = (op1 * op2);
	
	PRINTF(3,")");
	
	return result;
}

static int
parse_operator_mod(void)
{
	PRINTF(3,"MOD(");
	
	/* parse operands */
	int op1, op2;
	parse_binary(&op1, &op2);
	
	/* calculate result */
	int result = (op1 % op2);
	
	PRINTF(3,")");
	
	return result;
}

static int
parse_operator_not(void)
{
  PRINTF(3,"NOT(");

  /* parse operand */
  int op = 0;
  parse_unary(&op);

  /* calculate result */
  int result = !op;

  PRINTF(3,")");

  return result;
}


/* predicates */
static int
parse_predicate_eq(void)
{
  PRINTF(3,"EQ(");

  /* parse operands */
  int op1, op2;
  parse_binary(&op1, &op2);

  /* calculate result */
  int result = (op1 == op2);

  PRINTF(3,")");

  return result;
}

static int
parse_predicate_neq(void)
{
  PRINTF(3,"!");

  int result = !parse_predicate_eq();

  return result;
}

static int
parse_predicate_lt(void)
{
  PRINTF(3,"LT(");

  /* parse operands */
  int op1, op2;
  parse_binary(&op1, &op2);

  /* calculate result */
  int result = (op1 < op2);

  PRINTF(3,")");

  return result;
}

static int
parse_predicate_gt(void)
{
  PRINTF(3,"GT(");

  /* parse operands */
  int op1, op2;
  parse_binary(&op1, &op2);

  /* calculate result */
  int result = (op1 > op2);

  PRINTF(3,")");

  return result;
}

static int
parse_predicate_let(void)
{
  PRINTF(3,"!");

  int result = !parse_predicate_gt();

  return result;
}

static int
parse_predicate_get(void)
{
  PRINTF(3,"!");

  int result = !parse_predicate_lt();

  return result;
}

/* values */
static int
parse_uint8_value(void)
{
  uint8_t value = read_byte();

  PRINTF(3,"%d", value);

  return value;
}

static int
parse_uint16_value(void)
{
  uint16_t value = read_byte();
  value <<= 8;
  value |= read_byte();

  PRINTF(3,"%d", value);

  return value;
}

static int
parse_int8_value(void)
{
  int8_t value = read_byte();

  PRINTF(3,"%d", value);

  return value;
}

static int
parse_int16_value(void)
{
  int16_t value = read_byte();
  value <<= 8;
  value |= read_byte();

  PRINTF(3,"%d", value);

  return value;
}

static int
parse_repository_value(void)
{
  uint8_t index = read_byte();

  /* read value from repository */
  int value = scopes_repository_value(index);
  PRINTF(3,"%d", value);

  return value;
}

static int
parse(uint8_t *b, data_len_t spec_len)
{
  /* initialize position, length, and invalid flag */
  pos = 0;
  length = spec_len;
  invalid_input = 0;

  /* set byte code */
  bytecode = b;

  /* get first code */
  int code = read_byte();

  if (IS_VALID(code)) {
    /* call parser */
    int value = parser[code]();
    PRINTF(3,"=%d\n", value);

    /* check if invalid input flag is set */
    if (!invalid_input) {
      PRINTF(3,"parsed input is valid\n");
      return value;
    }
  }

  /* return false if something went wrong during parsing */
  PRINTF(3,"parsed input is invalid\n");
  return 0;
}

static void
init(void)
{
  process_start(&repository_process, NULL);
}

static int
check(void *specs, data_len_t spec_len)
{
  return parse(specs, spec_len);
}

MEMBERSHIP(simple_membership, "simple membership", init, check);
