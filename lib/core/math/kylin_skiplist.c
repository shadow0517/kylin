#include <kylin/include/kylin_base.h>
#include <kylin/include/math/kylin_skiplist.h>

struct kylin_skiplist_node {
    void *val;
    double score;
    kslist_node_t *backward;
    struct kylin_skiplist_level {
        kslist_node_t *forward;
        uint32_t span;
    } level[];
};

struct kylin_skiplist {
    kslist_node_t *header;
    kslist_node_t *tail;
    size_t         size;
    uint32_t       level;
};

