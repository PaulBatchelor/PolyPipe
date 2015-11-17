#include <stdio.h>
#include <stdint.h>
#include "poly.h"

void poly_demo_1() 
{
    uint32_t len = 15;
    uint32_t n, evt, p;
    poly_data cd;
    poly_event *my_evt;
    poly_init(&cd);

    poly_add(&cd, 0, 3);
        poly_pset(&cd, 0, 1.1);
        poly_pset(&cd, 1, 2.3);
        poly_pset(&cd, 2, 3.4);
    poly_add(&cd, 0, 3);
        poly_pset(&cd, 2, 0.5);
    poly_add(&cd, 10, 3);
        poly_pset(&cd, 0, 100);
        poly_pset(&cd, 1, 222);
        poly_pset(&cd, 2, 345);
    poly_end(&cd);
    for(n = 0; n < len; n++) {
        poly_compute(&cd);
        poly_itr_reset(&cd);
        for(evt = 0; evt < poly_nevents(&cd); evt++) {
            my_evt = poly_itr_next(&cd);
            printf("### Event %d at time %d\n", my_evt->pos, n);
            printf("\t");
            for(p = 0; p < my_evt->nvals; p++) {
                printf("%g ", my_evt->p[p]);
            }
            printf("\n");
        }
    }

    poly_destroy(&cd);
}

void poly_demo_2()
{
    int n, v1, v2, v3, v4;
    poly_voice *voice;
    poly_cluster clust;

    poly_cluster_init(&clust, 3);
    poly_cluster_add(&clust, &v1);
    poly_cluster_add(&clust, &v2);
    poly_cluster_add(&clust, &v3);
    poly_cluster_add(&clust, &v4);

    poly_cluster_reset(&clust);

    for(n = 0; n < poly_cluster_nvoices(&clust); n++) {
        voice = poly_next_voice(&clust);
        printf("voice id of %d\n", voice->val);
    }

    poly_cluster_remove(&clust, v2);

    poly_cluster_reset(&clust);
    for(n = 0; n < poly_cluster_nvoices(&clust); n++) {
        voice = poly_next_voice(&clust);
        printf("voice id of %d\n", voice->val);
    }

    poly_cluster_add(&clust, &v4);

    poly_cluster_reset(&clust);
    for(n = 0; n < poly_cluster_nvoices(&clust); n++) {
        voice = poly_next_voice(&clust);
        printf("voice id of %d\n", voice->val);
    }

    poly_cluster_destroy(&clust);
}

static void write_binary() 
{
    poly_data cd;
    float vals[2];
    poly_init(&cd);
    poly_binary_open(&cd, "test.bin");
    
    vals[0] = 62;
    vals[1] = 0.2;
    poly_binary_write(&cd, 0, 2, vals);
    
    //vals[0] = 67;
    //vals[1] = 0.2;
    //poly_binary_write(&cd, 0.5, 2, vals);
    //
    //vals[0] = 69;
    //vals[1] = 0.2;
    //poly_binary_write(&cd, 1.5, 2, vals);

    poly_binary_close(&cd);
    poly_destroy(&cd);
}

int main() {

    //poly_demo_1();
    //poly_demo_2();
    write_binary();

    return 0;
}
