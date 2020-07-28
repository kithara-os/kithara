#include <amd64/tss.h>
#include <memory/bdalloc.h>
#include <smp/localstorage.h>
#include <utils.h>

void tss_init() {
	struct tss *tss = bdalloc_new(sizeof(struct tss));
	assert(tss != NULL, "[tss] Panic: Failed to allocate TSS\n");
	memset(tss, sizeof(struct tss), 0);
	tss->iopb = sizeof(struct tss);
	// hack to get stack used by cpu
	int val;
	tss->rsp[0] = ALIGN_UP((uint64_t)(&val), 0x10000);
	local_storage_get()->tss = tss;
	local_storage_get()->stack_top = tss->rsp[0];
}