/*
 * Phoenix-RTOS
 *
 * i.MX 6ULL SDMA lib
 *
 * Copyright 2018 Phoenix Systems
 * Author: Krystian Wasik
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/msg.h>
#include <sys/mman.h>

#include "sdma-lib.h"

static int sdma_dev_ctl(sdma_t *s,
                        const sdma_dev_ctl_t *dev_ctl,
                        void *data,
                        size_t size)
{
    int res;
    msg_t msg;

    msg.type = mtDevCtl;

    msg.i.data = NULL;
    msg.i.size = 0;
    msg.o.data = data;
    msg.o.size = size;
    memcpy(msg.o.raw, dev_ctl, sizeof(sdma_dev_ctl_t));

    if ((res = msgSend(s->oid.port, &msg)) < 0) {
        fprintf(stderr, "msgSend failed (%d)\n\r", res);
        return -1;
    } else if (msg.o.io.err != EOK) {
        fprintf(stderr, "devctl failed (%d)\n\r", msg.o.io.err);
        return -2;
    }

    return 0;
}

int sdma_open(sdma_t *s, const char *dev_name)
{
    int fd, res;
    if (s == NULL)
        return -1;

    if ((fd = open(dev_name, O_RDWR)) < 0)
        return -2;

    if ((res = lookup(dev_name, &s->oid)) < 0)
        return -3;

    return 0;
}

int sdma_close(sdma_t *s)
{
    /* TODO: Implement sdma_channel_close */

    return 0;
}

int sdma_channel_configure(sdma_t *s, sdma_channel_config_t *cfg)
{
    sdma_dev_ctl_t dev_ctl;

    dev_ctl.oid = s->oid;
    dev_ctl.type = sdma_dev_ctl__channel_cfg;
    dev_ctl.cfg = *cfg;

    return sdma_dev_ctl(s, &dev_ctl, NULL, 0);
}

int sdma_data_mem_write(sdma_t *s, void *data, size_t size, addr_t addr)
{
    sdma_dev_ctl_t dev_ctl;

    dev_ctl.oid = s->oid;
    dev_ctl.type = sdma_dev_ctl__data_mem_write;
    dev_ctl.mem.addr = addr;
    dev_ctl.mem.len = size;

    return sdma_dev_ctl(s, &dev_ctl, data, size);
}

int sdma_data_mem_read(sdma_t *s, void *data, size_t size, addr_t addr)
{
    sdma_dev_ctl_t dev_ctl;

    dev_ctl.oid = s->oid;
    dev_ctl.type = sdma_dev_ctl__data_mem_read;
    dev_ctl.mem.addr = addr;
    dev_ctl.mem.len = size;

    return sdma_dev_ctl(s, &dev_ctl, data, size);
}

int sdma_context_dump(sdma_t *s, sdma_context_t *ctx)
{
    sdma_dev_ctl_t dev_ctl;

    dev_ctl.oid = s->oid;
    dev_ctl.type = sdma_dev_ctl__context_dump;

    return sdma_dev_ctl(s, &dev_ctl, (void*)ctx, sizeof(sdma_context_t));
}

int sdma_context_set(sdma_t *s, const sdma_context_t *ctx)
{
    sdma_dev_ctl_t dev_ctl;

    dev_ctl.oid = s->oid;
    dev_ctl.type = sdma_dev_ctl__context_set;

    return sdma_dev_ctl(s, &dev_ctl, (void*)ctx, sizeof(sdma_context_t));
}

int sdma_enable(sdma_t *s)
{
    sdma_dev_ctl_t dev_ctl;

    dev_ctl.oid = s->oid;
    dev_ctl.type = sdma_dev_ctl__enable;

    return sdma_dev_ctl(s, &dev_ctl, NULL, 0);
}

int sdma_trigger(sdma_t *s)
{
    sdma_dev_ctl_t dev_ctl;

    dev_ctl.oid = s->oid;
    dev_ctl.type = sdma_dev_ctl__trigger;

    return sdma_dev_ctl(s, &dev_ctl, NULL, 0);
}

void *sdma_alloc_uncached(size_t size, addr_t *paddr)
{
    uint32_t n = (size + SIZE_PAGE - 1)/SIZE_PAGE;

    void *vaddr = mmap(NULL, n*SIZE_PAGE, PROT_READ | PROT_WRITE, MAP_UNCACHED, OID_NULL, 0);
    if (vaddr == MAP_FAILED)
        return NULL;

    if (paddr) {
        addr_t page_addr = va2pa(vaddr - (addr_t)vaddr % SIZE_PAGE);
        *paddr = page_addr + (addr_t)vaddr % SIZE_PAGE;
    }

    return vaddr;
}

int sdma_free_uncached(void *vaddr, size_t size)
{
    uint32_t n = (size + SIZE_PAGE - 1)/SIZE_PAGE;

    return munmap(vaddr, n*SIZE_PAGE);
}