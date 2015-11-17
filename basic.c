#include <soundpipe.h>
#include "poly.h"

typedef struct {
    sp_ftbl *ft;
    sp_osc *osc;
    sp_tenv *env;
    SPFLOAT tick;
} basic;

typedef struct {
    basic synth[3];
    poly_data cd;
    poly_cluster clust;
} UserData;

void basic_create(sp_data *sp, basic *b) 
{
    sp_ftbl_create(sp, &b->ft, 4096);
    sp_gen_sine(sp, b->ft);
    sp_osc_create(&b->osc);
    sp_osc_init(sp, b->osc, b->ft, 0);
    sp_tenv_create(&b->env);
    sp_tenv_init(sp, b->env);
    b->env->atk = 1;
    b->env->hold = 3;
    b->env->rel = 1;
    b->tick = 0;
}

void basic_compute(sp_data *sp, basic *b, SPFLOAT *out) 
{
    SPFLOAT env = 0;
    sp_osc_compute(sp, b->osc, NULL, out);
    sp_tenv_compute(sp, b->env, &b->tick, &env);
    *out *= env;
}

void basic_destroy(basic *b) 
{
    sp_ftbl_destroy(&b->ft);
    sp_osc_destroy(&b->osc);
    sp_tenv_destroy(&b->env);
}

static void process(sp_data *sp, void *udata) 
{
    UserData *ud = udata;
    int n;
    int id;
    SPFLOAT mix = 0, tmp = 0;
    poly_event *evt;
    poly_voice *voice;

    poly_compute(&ud->cd);
    poly_itr_reset(&ud->cd);
    for(n = 0; n < poly_nevents(&ud->cd); n++) {
        evt = poly_itr_next(&ud->cd);
        if(!poly_cluster_add(&ud->clust, &id)) {
            ud->synth[id].osc->freq = sp_midi2cps(evt->p[0]);
            ud->synth[id].osc->amp= evt->p[1];
            ud->synth[id].tick = 1.0;
        }
    }

    poly_cluster_reset(&ud->clust);
    for(n = 0; n < poly_cluster_nvoices(&ud->clust); n++) {
        voice = poly_next_voice(&ud->clust);
        basic_compute(sp, &ud->synth[voice->val], &tmp);
        ud->synth[voice->val].tick = 0;
        mix += tmp;
    }

    sp->out[0] = mix;
}

int main() 
{
    sp_data *sp;
    UserData ud;
    poly_data *cd = &ud.cd;
    poly_cluster *clust = &ud.clust;
    int n;
    sp_create(&sp);
    sp->sr = 44100;
    sp->len = sp->sr * 7;
    
    for(n = 0; n < 3; n++) {
        basic_create(sp, &ud.synth[n]);
    }
    
    poly_init(cd);

    //poly_add(cd, 0, 2);
    //    poly_pset(cd, 0, 62);
    //    poly_pset(cd, 1, 0.2);
    //
    //poly_add(cd, 0.5 * sp->sr, 2);
    //    poly_pset(cd, 0, 67);
    //    poly_pset(cd, 1, 0.2);
    //    
    //poly_add(cd, 1.5 * sp->sr, 2);
    //    poly_pset(cd, 0, 69);
    //    poly_pset(cd, 1, 0.2);

    if(poly_binary_parse(cd, "test.bin", 44100) != 0) {
        fprintf(stderr, "Could not find file!\n");    
    } else {
        poly_end(cd);
        
        int tmp; 
        poly_cluster_init(clust, 3);

        sp_process(sp, &ud, process);

        for(n = 0; n < 3; n++) {
            basic_destroy(&ud.synth[n]);
        }

        poly_cluster_destroy(clust);
    }

    poly_destroy(cd);
    sp_destroy(&sp);
    return 0;
}
