#include <stdio.h>

#include "hessian/hessian.h"
#include "util/buffer.h"

int main(void) {
    BUFFER * buffer;
    double din1, din2, dout1, dout2;
    int64_t lin1, lin2, lout1, lout2;
    hessian_object_t * h_din1, * h_din2, * h_dout1, * h_dout2, * h_lin1, * h_lin2, * h_lout1, * h_lout2;
    din1= -0.678687687;
    din2= 123423.3423;
    lin1= 979798797798;
    lin2= -65422454;
    printf("din1: %f\n",din1);
    printf("din2: %f\n",din2);
    h_din1= hessian_create(HESSIAN_DOUBLE, din1);
    h_din2= hessian_create(HESSIAN_DOUBLE, din2);
    printf("lin1: %ld\n",lin1);
    printf("lin2: %ld\n",lin2);
    h_lin1= hessian_create(HESSIAN_LONG, lin1);
    h_lin2= hessian_create(HESSIAN_LONG, lin2);
    
    buffer= buffer_create(1024);
    
    printf("serialize...\n");
    hessian_serialize(h_din1,buffer);
    hessian_serialize(h_din2,buffer);
    hessian_serialize(h_lin1,buffer);
    hessian_serialize(h_lin2,buffer);
    
    printf("deserialize...\n");
    h_dout1= hessian_deserialize(buffer);
    h_dout2= hessian_deserialize(buffer);
    h_lout1= hessian_deserialize(buffer);
    h_lout2= hessian_deserialize(buffer);

    dout1= hessian_double_getvalue(h_dout1);
    dout2= hessian_double_getvalue(h_dout2);
    lout1= hessian_long_getvalue(h_lout1);
    lout2= hessian_long_getvalue(h_lout2);
    printf("dout1: %f\n",dout1);
    printf("dout2: %f\n",dout2);
    printf("lout1: %ld\n",lout1);
    printf("lout2: %ld\n",lout2);
    
    printf("tests...\n");
    if (din1 != dout1) {
        printf("din1 (%f) != dout1 (%f)\n",din1,dout1);
        return 1;
    }
    if (din2 != dout2) {
        printf("din2 (%f) != dout2 (%f)\n",din2,dout2);
        return 1;
    }
    if (lin1 != lout1) {
        printf("lin1 (%ld) != lout1 (%ld)\n",lin1,lout1);
        return 2;
    }
    if (lin2 != lout2) {
        printf("lin2 (%ld) != lout2 (%ld)\n",lin2,lout2);
        return 2;
    }

    return 0;
}
