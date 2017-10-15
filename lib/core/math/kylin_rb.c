#include <kylin/include/kylin.h>
#include <kylin/include/utils/kylin_limit.h>
#include <kylin/include/math/kylin_rb.h>

#include <kylin/lib/core/math/kylin_math.h>

struct kylin_rb_node
{
    krb_node_t *parent;
    krb_node_t *left;
    krb_node_t *right;
    kbool_t     color;
    kmath_val_t val;
};

struct kylin_rb
{
    krb_opts_t  opts;
    size_t      count;
    krb_node_t *min;
    krb_node_t *max;
    krb_node_t *root;
};

#define RB_RED                (KYLIN_TRUE)
#define RB_BLACK              (KYLIN_FALSE)

#define RB_LEFT(node)         (node) ? (node)->left   : NULL
#define RB_RIGHT(node)        (node) ? (node)->right  : NULL
#define RB_PARENT(node)       (node) ? (node)->parent : NULL
#define RB_COLOR(node)        (node) ? (node)->color  : NULL
#define RB_VAL(node)          (node) ? (node)->val    : NULL
#define RB_MIN(guard)         (guard) ? (guard)->min   : NULL
#define RB_MAX(guard)         (guard) ? (guard)->max   : NULL
#define RB_CNT(guard)         (guard) ? (guard)->count : NULL
#define RB_ROOT(guard)        (guard) ? (guard)->root  : NULL
#define RB_EMPTY(guard)       (RB_ROOT(guard) == NULL)
#define RB_COMPARE(guard)     ((guard)->opts.compare)

#define RB_VAL_MALLOC(opts)   ((opts)->allocator.val_ctor)
#define RB_VAL_FREE(opts)     ((opts)->allocator.val_dtor   ? (opts)->allocator.val_dtor   : free)
#define RB_NODE_MALLOC(opts)  ((opts)->allocator.node_ctor  ? (opts)->allocator.node_ctor  : malloc)
#define RB_NODE_FREE(opts)    ((opts)->allocator.node_dtor  ? (opts)->allocator.node_dtor  : free)
#define RB_GUARD_MALLOC(opts) ((opts)->allocator.guard_ctor ? (opts)->allocator.guard_ctor : malloc)
#define RB_GUARD_FREE(opts)   ((opts)->allocator.guard_dtor ? (opts)->allocator.guard_dtor : free)

static __kylin_inline void _rotate_left(krb_t *guard, krb_node_t *node, krb_node_t *tmp)
{
    tmp = RB_RIGHT(node);

    if((RB_RIGHT(node) = RB_LEFT(tmp)) != NULL)
        RB_PARENT(RB_LEFT(tmp)) = node;

    if((RB_PARENT(tmp) = RB_PARENT(node)) != NULL) {
        if(node == RB_LEFT(RB_PARENT(node))) {
            RB_LEFT(RB_PARENT(node)) = tmp;
        } else {
            RB_RIGHT(RB_PARENT(node)) = tmp;
        }
    } else
        RB_ROOT(guard) = tmp;

    RB_LEFT(tmp) = node;
    RB_PARENT(node) = tmp;
}

static __kylin_inline void _rotate_right(krb_t *guard, krb_node_t *node, krb_node_t *tmp)
{
    tmp = RB_LEFT(node);

    if((RB_LEFT(node) = RB_RIGHT(tmp)) != NULL)
        RB_PARENT(RB_RIGHT(tmp)) = node;

    if((RB_PARENT(tmp) = RB_PARENT(node)) != NULL) {
        if(node == RB_LEFT(RB_PARENT(node))) {
            RB_LEFT(RB_PARENT(node)) = tmp;
        } else {
            RB_RIGHT(RB_PARENT(node)) = tmp;
        }
    } else
        RB_ROOT(guard) = tmp;

    RB_RIGHT(tmp) = node;
    RB_PARENT(node) = tmp;
}

static __kylin_inline void _insert_color(krb_t *guard, krb_node_t *node)
{
    krb_node_t *parent = NULL, *gparent = NULL, *tmp = NULL;

    while((parent = RB_PARENT(node)) != NULL &&
            RB_COLOR(parent) == RB_RED) {
        gparent = RB_PARENT(parent);

        if(parent == RB_LEFT(gparent)) {
            tmp = RB_RIGHT(gparent);

            if(tmp && RB_COLOR(tmp) == RB_RED) {
                RB_COLOR(tmp) = RB_BLACK;
                RB_COLOR(parent) = RB_BLACK;
                RB_COLOR(gparent) = RB_RED;
                node = gparent;
                continue;
            }

            if(RB_RIGHT(parent) == node) {
                _rotate_left(guard, parent, tmp);
                KYLIN_SWAP(node, parent);
            }

            RB_COLOR(parent) = RB_BLACK;
            RB_COLOR(gparent) = RB_RED;

            _rotate_right(guard, gparent, tmp);
        } else {
            tmp = RB_LEFT(gparent);

            if(tmp && RB_COLOR(tmp) == RB_RED) {
                RB_COLOR(tmp) = RB_BLACK;
                RB_COLOR(parent) = RB_BLACK;
                RB_COLOR(gparent) = RB_RED;
                node = gparent;
                continue;
            }

            if(RB_LEFT(parent) == node) {
                _rotate_right(guard, parent, tmp);
                KYLIN_SWAP(node, parent);
            }

            RB_COLOR(parent) = RB_BLACK;
            RB_COLOR(gparent) = RB_RED;

            _rotate_left(guard, gparent, tmp);
        }
    }

    RB_COLOR(RB_ROOT(guard)) = RB_BLACK;
}

static __kylin_inline void _remove_color(krb_t *guard, krb_node_t *parent, krb_node_t *node)
{
	krb_node_t *tmp = NULL;

	while ((node == NULL || RB_COLOR(node) == RB_BLACK) &&
	    node != RB_ROOT(guard)) {

		if (RB_LEFT(parent) == node) {
			tmp = RB_RIGHT(parent);

			if (RB_COLOR(tmp) == RB_RED) {
                RB_COLOR(tmp)    = RB_BLACK;
                RB_COLOR(parent) = RB_RED;
				_rotate_left(guard, parent, tmp);
				tmp = RB_RIGHT(parent);
			}

			if ((RB_LEFT(tmp) == NULL ||
			    RB_COLOR(RB_LEFT(tmp)) == RB_BLACK) &&
			    (RB_RIGHT(tmp) == NULL ||
			    RB_COLOR(RB_RIGHT(tmp)) == RB_BLACK)) {
				RB_COLOR(tmp) = RB_RED;
				node = parent;
				parent = RB_PARENT(node);
			}
            else {
				if (RB_RIGHT(tmp) == NULL ||
				    RB_COLOR(RB_RIGHT(tmp)) == RB_BLACK) {
					krb_node_t *oleft;

					if ((oleft = RB_LEFT(tmp)) != NULL)
						RB_COLOR(oleft) = RB_BLACK;

					RB_COLOR(tmp) = RB_RED;
					_rotate_right(guard, tmp, oleft);

					tmp = RB_RIGHT(parent);
				}

				RB_COLOR(tmp) = RB_COLOR(parent);
				RB_COLOR(parent) = RB_BLACK;

				if (RB_RIGHT(tmp))
					RB_COLOR(RB_RIGHT(tmp)) = RB_BLACK;

				_rotate_left(guard, parent, tmp);
				node = RB_ROOT(guard);

				break;
			}
		}
        else {
			tmp = RB_LEFT(parent);

			if (RB_COLOR(tmp) == RB_RED) {
                RB_COLOR(tmp) = RB_BLACK;
                RB_COLOR(parent) = RB_RED;
				_rotate_right(guard, parent, tmp);
				tmp = RB_LEFT(parent);
			}

			if ((RB_LEFT(tmp) == NULL ||
			    RB_COLOR(RB_LEFT(tmp)) == RB_BLACK) &&
			    (RB_RIGHT(tmp) == NULL ||
			    RB_COLOR(RB_RIGHT(tmp)) == RB_BLACK)) {
				RB_COLOR(tmp) = RB_RED;
				node = parent;
				parent = RB_PARENT(node);
			}
            else {
				if (RB_LEFT(tmp) == NULL ||
				    RB_COLOR(RB_LEFT(tmp)) == RB_BLACK) {
					krb_node_t *oright;

					if ((oright = RB_RIGHT(tmp)) != NULL)
						RB_COLOR(oright) = RB_BLACK;

					RB_COLOR(tmp) = RB_RED;
					_rotate_left(guard, tmp, oright);

					tmp = RB_LEFT(parent);
				}

				RB_COLOR(tmp) = RB_COLOR(parent);
				RB_COLOR(parent) = RB_BLACK;

				if (RB_LEFT(tmp))
					RB_COLOR(RB_LEFT(tmp)) = RB_BLACK;

				_rotate_right(guard, parent, tmp);
				node = RB_ROOT(guard);

				break;
			}
		}
	}

	if (node)
		RB_COLOR(node) = RB_BLACK;
}

krb_t *kylin_rb_create(const krb_opts_t *opts)
{
    krb_t *rb = NULL;

    rb = RB_GUARD_MALLOC(opts)(sizeof(krb_t));
    if(rb == NULL)
        return NULL;
    memset(rb, 0, sizeof(krb_t));

    memcpy(&rb->opts, opts, sizeof(krb_opts_t));

    return rb;
}

void kylin_rb_destroy(krb_t *guard)
{
    krb_node_t *node = NULL;

    do {
        node = kylin_rb_min(guard);
        kmath_val_dtor(&RB_VAL(node), guard->opts.val_type, RB_VAL_FREE(&guard->opts));
        kylin_rb_remove(guard, node);
    } while(node);
    RB_GUARD_FREE(&guard->opts)(guard);
}

void kylin_rb_clear(krb_t *guard)
{
    return;
}

void *kylin_rb_val(const krb_t *guard, krb_node_t *node)
{
    return kmath_val_get(&RB_VAL(node), guard->opts.val_type);
}

krb_node_t *kylin_rb_insert(krb_t *guard, void *elm)
{
    krb_node_t *tmp = NULL, *parent = NULL, *node = NULL;
    int result = 0;

    node = RB_NODE_MALLOC(&guard->opts)(sizeof(krb_node_t));
    if(!node)
        return NULL;
    memset(node, 0, sizeof(krb_node_t));

    kmath_val_ctor(&RB_VAL(node), elm, guard->opts.val_type, 
            guard->opts.val_size, RB_VAL_MALLOC(&guard->opts));

    tmp = RB_ROOT(guard);
    while(tmp) {
        parent = tmp;

        result = kmath_val_cmp(&RB_VAL(node), &RB_VAL(tmp), guard->opts.val_type, RB_COMPARE(guard));
        if(result < 0)
            tmp = RB_LEFT(tmp);
        else if(result > 0)
            tmp = RB_RIGHT(tmp);
        else {
            RB_NODE_FREE(&guard->opts)(node);
            return NULL; /*有相似的结点存在*/
        }
    }

    RB_PARENT(node) = parent;
    RB_LEFT(node) = RB_RIGHT(node) = NULL;
    RB_COLOR(node) = RB_RED;

    if(tmp != NULL) {
        if(result < 0)
            RB_LEFT(parent) = node;
        else
            RB_RIGHT(parent) = node;
    }
    else
        RB_ROOT(guard) = node;

    _insert_color(guard, node);
    return node;
}

krb_node_t *kylin_rb_insert_raw(krb_t *guard, krb_node_t *node)
{
    return NULL;
}

void kylin_rb_remove(krb_t *guard, void *cmp)
{
    return;
}

void kylin_rb_remove_raw(krb_t *guard, krb_node_t *node)
{
    int color = RB_RED;
    krb_node_t *child = NULL, *parent = NULL, *old = node;

    if(NULL == RB_LEFT(node))
        child = RB_RIGHT(node);
    else if(NULL == RB_RIGHT(node))
        child = RB_LEFT(node);
    else {
        krb_node_t *left = NULL;
        node = RB_RIGHT(node);
        while((left = RB_LEFT(node)) != NULL)
            node = left;

        child = RB_RIGHT(node);
        parent = RB_PARENT(node);
        color = RB_COLOR(node);

        if(child)
            RB_PARENT(child) = parent;

        if(parent) {
            if(RB_LEFT(parent) == node)
                RB_LEFT(parent) = child;
            else
                RB_RIGHT(parent) = child;
        }
        else
            RB_ROOT(guard) = child;

        if(RB_PARENT(node) == old)
            parent = node;

        if(RB_PARENT(old)) {
            if(RB_LEFT(RB_PARENT(old)) == old)
                RB_LEFT(RB_PARENT(old)) = node;
            else
                RB_RIGHT(RB_PARENT(old)) = node;
        }
        else
            RB_ROOT(guard) = node;
        RB_PARENT(RB_LEFT(old)) = node;

        if(RB_RIGHT(old))
            RB_PARENT(RB_RIGHT(old)) = node;
        goto color;
    }

    parent = RB_PARENT(node);
    color = RB_COLOR(node);
    if(child)
        RB_PARENT(child) = parent;

    if(parent) {
        if(RB_LEFT(parent) == node)
            RB_LEFT(parent) = child;
        else
            RB_RIGHT(parent) = child;
    }
    else
        RB_ROOT(guard) = child;

color:
    if(color == RB_BLACK)
        _remove_color(guard, parent, child);

    RB_NODE_FREE(&guard->opts)(old);
    return ;
}

void *kylin_rb_unlink(krb_t *guard, void *cmp)
{
    return NULL;
}

krb_node_t *kylin_rb_unlink_raw(krb_t *guard, krb_node_t *node)
{
    return NULL;
}

krb_node_t *kylin_rb_min(krb_t *guard)
{
    return RB_MIN(guard);
}

krb_node_t *kylin_rb_max(krb_t *guard)
{
    return RB_MAX(guard);
}

size_t kylin_rb_count(krb_t *guard)
{
    return RB_CNT(guard);
}

krb_node_t *kylin_rb_next(krb_node_t *node)
{
    if(RB_RIGHT(node)) {
        node = RB_RIGHT(node);
        while(RB_LEFT(node))
            node = RB_LEFT(node);
    }
    else {
        if(RB_PARENT(node) && (node == RB_LEFT(RB_PARENT(node))))
            node = RB_PARENT(node);
        else {
            while(RB_PARENT(node) && (node == RB_RIGHT(RB_PARENT(node))))
                node = RB_PARENT(node);
            node = RB_PARENT(node);
        }
    }

    return node;
}

krb_node_t *kylin_rb_prev(krb_node_t *node)
{
    if(RB_LEFT(node)) {
        node = RB_LEFT(node);
        while(RB_RIGHT(node))
            node = RB_RIGHT(node);
    }
    else {
        if(RB_PARENT(node) && (node == RB_RIGHT(RB_PARENT(node))))
            node = RB_PARENT(node);
        else {
            while(RB_PARENT(node) && (node == RB_LEFT(RB_PARENT(node))))
                node = RB_PARENT(node);
            node = RB_PARENT(node);
        }
    }

    return node;
}

krb_node_t *kylin_rb_find(krb_t *guard, void *cmp)
{
    int result = 0;
    krb_node_t *tmp = RB_ROOT(guard);

    while(tmp) {
        result = kmath_val_cmp_raw(&RB_VAL(tmp), cmp, guard->opts.val_type, RB_COMPARE(guard));
        if(result < 0)
            tmp = RB_LEFT(tmp);
        else if(result > 0)
            tmp = RB_RIGHT(tmp);
        else
            return tmp;
    }

    return NULL;
}

krb_node_t *kylin_rb_find_or_next(krb_t *guard, void *cmp)
{
    int result = 0;
    krb_node_t *tmp = RB_ROOT(guard);
    krb_node_t *res = NULL;

    while(tmp) {
        result = kmath_val_cmp_raw(&RB_VAL(tmp), cmp, guard->opts.val_type, RB_COMPARE(guard));
        if(result < 0) {
            res = tmp;
            tmp = RB_LEFT(tmp);
        }
        else if(result > 0)
            tmp = RB_RIGHT(tmp);
        else
            return tmp;
    }

    return res;
}
