#ifndef _KYLIN_MATH_JSON_H_
#define _KYLIN_MATH_JSON_H_

struct kylin_json;
typedef struct kylin_json kylin_json_t;


extern kylin_json_t kylin_json_load(kylin_file_t *);
extern kylin_json_t kylin_json_loads(const char *);

extern const char *kylin_json_dumps(kylin_json_t);


extern kylin_json_t kylin_json_add_new(kylin_json_t , const char *name, json_type_t);


extern void kylin_json_destroy(kylin_json_t);

#endif /*_KYLIN_MATH_JSON_H_*/
