#include "numa_utils.h"

#ifdef HAVE_NUMA
#include <numa.h>
#include <assert.h>
#endif

void pb_bind_thread(int thread_id) {
#ifdef HAVE_NUMA
    static int nodes = -1;
    if (nodes < 0) {
        nodes = numa_available() >= 0 ? numa_num_configured_nodes() : 0;
    }
    if (nodes > 0) {
        int node = thread_id % nodes;
        int ret = numa_run_on_node(node);
        (void)ret;
        assert(ret == 0);
    }
#else
    (void)thread_id;
#endif
}
