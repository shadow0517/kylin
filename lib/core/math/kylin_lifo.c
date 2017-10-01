#include <kylin/include/kylin.h>
#include <kylin/include/math/kylin_lifo.h>

struct kylin_lifo {
    size_t  cap;       /*总容量*/
    size_t  top;       /*栈顶位置*/
    kbyte_t buf[];
};

klifo_t *kylin_lifo_create(size_t cap)
{

}

void kylin_lifo_destroy(klifo_t *lifo)
{

}

int kylin_lifo_push(klifo_t *lifo, const kbyte_t *buf, size_t len)
{
    return KYLIN_ERROR_OK;

}

void kylin_lifo_pop(klifo_t *lifo, kbyte_t *buf, size_t len)
{

}

void kylin_lifo_top(klifo_t *lifo, kbyte_t *buf, size_t len)
{

}



