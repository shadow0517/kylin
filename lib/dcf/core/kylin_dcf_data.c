#include <kylin/include/kylin_base.h>
#include <kylin/include/kylin_dcf.h>
#include <kylin/lib/dcf/core/kylin_dcf_data.h>

typedef struct kylin_dcf_node{
    kylin_dcf_t obj;
    krb_t       entry;
} kylin_dcf_node_t;

static krb_root *dcf_tree;

static int __rb_compare(krb_t *r1, krb_t *r2)
{
    kylin_dcf_node_t *node1 = kylin_container_of(r1, kylin_dcf_node_t, entry);
    kylin_dcf_node_t *node2 = kylin_container_of(r2, kylin_dcf_node_t, entry);
    return strncmp(node1->obj.name, node2->obj.name, KYLIN_NAME_LENGTH);
}

kerr_t kylin_dcf_data_init(void)
{
    kerr_t ret = KYLIN_ERROR_OK;

    dcf_tree = kylin_rb_create("dcf node tree", __rb_compare);
    if(dcf_tree == NULL) {
        ret = KYLIN_ERROR_NOMEM;
        goto exit;
    }

exit:
    return ret;
}

void kylin_dcf_data_fini(void)
{
    kylin_rb_destroy(dcf_tree);
}
