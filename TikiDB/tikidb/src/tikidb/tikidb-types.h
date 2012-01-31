#ifndef QUERY_TYPES_H
#define QUERY_TYPES_H

#include "contiki.h"
#include "net/rime.h"
#include "lib/list.h"

#include "scopes.h"

/**
 * \brief		Value for a lifetime of "infinity" for a periodic statement
 **/
#define INFINITY_LIFETIME		-1UL

typedef uint8_t tikidb_msg_type_t;
/**
 * \brief		Enumeration list for TikiDB message types.
 **/
typedef enum {
	STATEMENT = 0, //
	RESULT = 1
} tikidb_msg_type;

typedef uint8_t statement_type_t;
/**
 *  \brief		Enumeration list for the statement type.
 **/
typedef enum {
	OPEN_SCOPE_STATEMENT = 1, //
	CLOSE_SCOPE_STATEMENT = 2, //
	SELECT_STATEMENT = 3, //
	STOP_STATEMENT = 4, //
	LIST_QUERIES_STATEMENT = 5
} statement_type;

/**
 * \brief		General keywords
 **/
typedef enum {
	WHERE = 6, //
	GROUP_BY = 7, //
	SAMPLE_PERIOD = 8, //
	LIFETIME = 9
} tikidb_keywords;

/**
 * \brief		Enumeration list for the sensors and attributes.
 **/
typedef enum {
	NODE_ID_ATTR = 40,
	AMBIENT_LIGHT = 41,
	PAR_LIGHT = 42,
	HUMIDITY = 43,
	TEMPERATURE = 44,
	INTERNAL_TEMPERATURE = 45,
	ACCELERATION_X = 46,
	ACCELERATION_Y = 47,
	ACCELERATION_Z = 48,
	INTERNAL_VOLTAGE = 49,
	X_COORDINATE = 50,
	Y_COORDINATE = 51,
	Z_COORDINATE = 52,
	NODE_RSSI = 53,
	NODE_PARENT = 54,
	USER_DEFINED_1 = 55,
	USER_DEFINED_2 = 56,
	USER_DEFINED_3 = 57,
	STATEMENT_ID = 61,
	EPOCH = 62
} attribute_type;

/**
 * \brief		Aggregation operators
 **/
typedef enum {
	NONE = 11, //
	MAX = 12, //
	MIN = 13, //
	AVG = 14, //
	COUNT = 15, //
	SUM = 16
} aggregation_type;

/**
 * \brief		Select column type
 **/
struct select_col_t {
	struct select_col_t *next;
	aggregation_type aggregation;
	attribute_type value;
};

/**
 * \brief		From type
 **/
struct from_scope_t {
	struct from_scope_t *next;
	scope_id_t scope_id;
};

/**
 * \brief		Comparison operators
 **/
typedef enum {
	SMALLER = 21,
	SMALLER_EQUAL = 22,
	GREATER = 23,
	GREATER_EQUAL = 24,
	EQUAL = 25,
	NOT_EQUAL = 26
} comparison_operator;

/**
 * \brief		Conditions
 **/
typedef enum {
	AND = 31, OR = 32
} logic_conjunction;

struct condition_t {
	struct condition_t *next;
	attribute_type val1;
	comparison_operator op;
	uint16_t val2;
	logic_conjunction conjunction;
};

typedef uint8_t table_id_t;

/**
 * \brief 		statement id
 **/
typedef uint8_t statement_id_t;

typedef struct {
	/** \brief id of table */
	table_id_t id;
	/** \brief id of statement */
	statement_id_t statement_id;
	/** \brief size of scheme */
	data_len_t scheme_size;
	/** \brief scheme */
	uint8_t *scheme;
	/** \brief number of attributes in primary key */
	data_len_t key_size;
	/** \brief pointer to field of indexes which are key, each index points to an attribute in scheme */
	uint8_t *key_index;
	/** \brief num of entries */
	uint8_t entries;
	/** \brief array of pointer to rows */
	int16_t *rows[MAX_ROWS];
} table_t;

#define GENERIC_STATEMENT_FIELDS \
	statement_id_t statement_id; \
	statement_type_t type;

/**
 * \brief		Generic data structure for statements
 **/
typedef struct __attribute__((__packed__)) {
	GENERIC_STATEMENT_FIELDS
} statement_t;

/**
 * \brief		Data structure for select statements
 **/
typedef struct __attribute__((__packed__)) {

	GENERIC_STATEMENT_FIELDS

	/** \brief Pointer to the scope specification  */
	scope_id_t super_scope_id;
	scope_id_t scope_id;
	scope_ttl_t scope_ttl;
	data_len_t scope_spec_len;
	uint8_t *scope_spec;

} open_scope_statement_t;

/**
 * \brief		Data structure for select statements
 **/
typedef struct __attribute__((__packed__)) {
	GENERIC_STATEMENT_FIELDS

	/** \brief Pointer to the scope specification  */
	scope_id_t scope_id;

} close_scope_statement_t;

/**
 * \brief		Data structure for select statements
 **/
typedef struct __attribute__((__packed__)) {
	GENERIC_STATEMENT_FIELDS

	/** \brief List of attributes on which this select statement projects */
	LIST_STRUCT(selects);

	/** \brief List of scopes from which this select statement should retrieve data */
	LIST_STRUCT(scopes);

	/** \brief List of conditions to filter */
	LIST_STRUCT(conditions);

	/** \brief The id of the scope through which this statement arrived to this node */
	scope_id_t scope_id;

	/** \brief Cols to group the result. */
	uint8_t group_by;

	/** \brief Value for the period repeat. */
	clock_time_t sample_period;

	/** \brief The length of a slot within a sample period (is calculated from the sample period divided by the tree height) */
	clock_time_t slot_length;

	/** \brief ID of the temporary table, in which the data are collected until the time out.  */
	table_id_t temporary_table_id;

	/** \brief Value of the lifetime for this statement */
	clock_time_t lifetime;

	/** \brief The tree height used for this statement */
	uint8_t tree_height;

	/** \brief Indicates whether this node is who disseminated the statement */
	bool disseminator;

	/** \brief Timer */
	struct ctimer in_slot_timer;

	/** \brief The Timer for the periodic executions of a statement */
	struct ctimer periodic_timer;

	/** \brief Stores how long has elapsed since the statement started executing, in clock_time_t units  */
	clock_time_t elapsed_lifetime;

} select_statement_t;

/**
 * \brief		Data structure for select statements
 **/
typedef struct __attribute__((__packed__)) {

	GENERIC_STATEMENT_FIELDS

	/** \brief Id of the statement which is requested to be stop. */
	statement_id_t statement_to_stop;

} stop_statement_t;

/**
 * \brief		Data structure for select statements
 **/
typedef struct __attribute__((__packed__)) {

	GENERIC_STATEMENT_FIELDS

	/** \brief The id of the scope through which this statement arrived to this node */
	scope_id_t scope_id;

} list_statement_t;

#endif // QUERY_TYPES_H
