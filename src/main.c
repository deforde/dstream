#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "dstream_packet.h"
#include "dstream_sock.h"
// #include "gui.h"

#include <sys/types.h>
#include <unistd.h>

int main(void) {
    int s = dstreamAcceptClientConnection();
    if (s == -1) {
        exit(1);
    }

    for(;;) {
        ssize_t rsz = 0;
        dstream_packet_header_t hdr = {0};

        rsz = dstreamSockRecv(s, &hdr, sizeof(hdr));
        if (rsz == 0) {
            puts("remote endpoint disconnected");
            break;
        }
        if (rsz != sizeof(hdr)) {
            puts("failed to read packet header");
            exit(1);
        }

        dstream_packet_t *p = dstreamPacketAlloc(hdr);
        rsz = dstreamSockRecv(s, p->data, p->hdr.sz);
        if (rsz != (ssize_t)p->hdr.sz) {
            puts("failed to read packet data");
            exit(1);
        }

        int d_ty;
        const char *nm;
        void *data;
        size_t sz;
        dstreamPacketUnpack(p, &d_ty, &nm, &data, &sz);
        assert(d_ty == I32);
        const size_t len = dstreamPacketGetDataLen(d_ty, sz);

        printf("Recieved packet:\n");
        printf("\tsz:        %zu\n", p->hdr.sz);
        printf("\tdata_type: %i\n", d_ty);
        printf("\tnm:        %s\n", nm);
        printf("\tdata:      [ ");
        for (size_t i = 0; i < len; i++) {
            printf("%i%s", *(int32_t*)dstreamPacketGetDataElem(data, d_ty, sz, i), (i < len - 1) ? ", " : "");
        }
        printf(" ]\n");

        free(p);
    }

    close(s);

    // runDemo();
    return 0;
}
