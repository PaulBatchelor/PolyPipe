#include <soundpipe.h>
#include "poly.h"

#define MAX_VOICES 4

typedef struct {
    sp_ftbl *ft;
    sp_osc *osc;
    sp_tenv *env;
    SPFLOAT tick;
    uint32_t dur, id;
} basic;

typedef struct {
    basic synth[MAX_VOICES];
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
    b->env->atk = 0.1;
    b->env->hold = 3;
    b->env->rel = 0.1;
    b->tick = 0;
    b->id = 0;
}

void basic_compute(sp_data *sp, basic *b, SPFLOAT *out, poly_cluster *clust) 
{
    SPFLOAT env = 0;
    b->dur--;
    sp_osc_compute(sp, b->osc, NULL, out);
    sp_tenv_compute(sp, b->env, &b->tick, &env);
    b->tick = 0;
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
    float dur;
    for(n = 0; n < poly_nevents(&ud->cd); n++) {
        evt = poly_itr_next(&ud->cd);
        if(!poly_cluster_add(&ud->clust, &id)) {
            ud->synth[id].osc->freq = sp_midi2cps(evt->p[1]);
            ud->synth[id].osc->amp= evt->p[2];
            ud->synth[id].dur = (SPFLOAT) evt->p[0] * sp->sr;
            printf("duration of %d\n", ud->synth[id].dur);
            dur = evt->p[0] - (ud->synth[id].env->atk + ud->synth[id].env->rel);

            if(dur >= 0) {
                ud->synth[id].env->hold = dur;
            } else {
                ud->synth[id].env->hold = 0;
            }

            ud->synth[id].tick = 1.0;
            ud->synth[id].id = id;
            printf(">>> id is %d\n", id);
        }
    }

    poly_cluster_reset(&ud->clust);
    for(n = 0; n < poly_cluster_nvoices(&ud->clust); n++) {
        voice = poly_next_voice(&ud->clust);
        basic_compute(sp, &ud->synth[voice->val], &tmp, &ud->clust);
        mix += tmp;
    }
    
    poly_cluster_reset(&ud->clust);
    uint32_t nvoices = poly_cluster_nvoices(&ud->clust);
    for(n = 0; n < nvoices; n++) {
        voice = poly_next_voice(&ud->clust);
        if(ud->synth[voice->val].dur <= 0) {
            printf("turning off %d now...\n", voice->val);
            poly_cluster_remove(&ud->clust, ud->synth[voice->val].id);
        }
    }

    sp->out[0] = mix;
}

int main(int argc, char *argv[]) 
{
    sp_data *sp;
    UserData ud;
    poly_data *cd = &ud.cd;
    poly_cluster *clust = &ud.clust;
    int n;
    sp_create(&sp);
    sp->sr = 44100;
    sp->len = sp->sr * 7;
    
    for(n = 0; n < MAX_VOICES; n++) {
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


    if(argc != 2) {
    
        fprintf(stderr, "Please supply a filename.\n");
        goto destroy;
    }

    if(poly_binary_parse(cd, argv[1], 44100) != 0) {
        fprintf(stderr, "Could not find file!\n");    
    } else {
        poly_end(cd);
        
        int tmp; 
        poly_cluster_init(clust, MAX_VOICES);

        sp_process(sp, &ud, process);

        for(n = 0; n < MAX_VOICES; n++) {
            basic_destroy(&ud.synth[n]);
        }

        poly_cluster_destroy(clust);
    }

    poly_destroy(cd);
    destroy:
    sp_destroy(&sp);
    return 0;
}
