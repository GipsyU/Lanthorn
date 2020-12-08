#include <rbt.h>
#include <error.h>
#include <memory.h>
#include <util.h>
#include <log.h>

static inline struct vpage_t *rbt_search(struct rbt_t *rbt, addr_t addr)
{
	struct rbt_node_t *n = rbt->root;
	struct vpage_t *vpage;

	while (n)
	{
		vpage = container_of(n, struct vpage_t, rbt_node);

		if (addr < vpage->addr)
			n = n->l;
		else if (addr > vpage->addr)
			n = n->r;
		else{
			return vpage;
		}
	}

	return NULL;
}
struct vpage_t v1, v2, v3;

static inline int _rbt_insert_(struct rbt_t *rbt, struct vpage_t *vpage)
{
	struct rbt_node_t **p = &rbt->root;
	struct rbt_node_t *parent = NULL;
	struct vpage_t * vp;

	while (*p)
	{
		parent = *p;

        vp = container_of(parent, struct vpage_t, rbt_node);

		if (vpage->addr < vp->addr)
			p = &(*p)->l;
		else if (vpage->addr > vp->addr)
			p = &(*p)->r;
        else
            return E_FAULT;
	}

	rb_link_node(&vpage->rbt_node, parent, p);
	
	return E_OK;
}

static inline int rbt_insert(struct rbt_t *rbt, struct vpage_t *vpage)
{
	int err;
	err = _rbt_insert_(rbt, vpage);
	rbt_insert_color(rbt, &vpage->rbt_node);
    return E_OK;
}



struct rbt_t rbt;

int rbt_test(void)
{
    v1.addr = 0 * PAGE_SIZE;
    v2.addr = 1 * PAGE_SIZE;
    v3.addr = 2 * PAGE_SIZE;

    rbt.root = NULL;

    int err;

	err = rbt_insert(&rbt, &v1);
    
	err = rbt_insert(&rbt, &v2);

    err = rbt_insert(&rbt, &v3);

	struct vpage_t *v;

	v = rbt_search(&rbt, 0*PAGE_SIZE);
	
	debug("%p\n", v);

	v = rbt_search(&rbt, 1*PAGE_SIZE);
	
	debug("%p\n", v);
	
	v = rbt_search(&rbt, 2*PAGE_SIZE);

	debug("%p\n", v);

	v = rbt_search(&rbt, 3*PAGE_SIZE);

	debug("%p\n", v);

    return E_OK;
}