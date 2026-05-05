#include <stdio.h>
#include <stdlib.h>
#include "save.h"
#include "structure_DBM.h"
#include "flatcc/flatcc_builder.h"
#include "structure_state_space_ta_builder.h"

int save_to_fbs(State_space_TA *ss, const char *filename)
{
    flatbuffers_builder_t B;
    flatcc_builder_init(&B);

    int *pred_count = calloc(ss->nb_etats, sizeof(int));
    int **pred_lists = calloc(ss->nb_etats, sizeof(int *));
    int *pred_fill = calloc(ss->nb_etats, sizeof(int));
    for (int i = 0; i < ss->nb_etats; i++)
        for (int j = 0; j < ss->nb_trans_by_state[i]; j++)
            pred_count[ss->state_transitions[i][j].cible]++;
    for (int i = 0; i < ss->nb_etats; i++)
        if (pred_count[i] > 0)
            pred_lists[i] = malloc(pred_count[i] * sizeof(int));
    for (int i = 0; i < ss->nb_etats; i++)
        for (int j = 0; j < ss->nb_trans_by_state[i]; j++)
        {
            int cible = ss->state_transitions[i][j].cible;
            pred_lists[cible][pred_fill[cible]++] = i;
        }

    State_space_ta_State_vec_start(&B);
    for (int i = 0; i < ss->nb_etats; i++)
    {
        State *s = &ss->etats[i];

        int32_t dbm_flat[DBM_DIM * DBM_DIM];
        for (int r = 0; r < DBM_DIM; r++)
            for (int c = 0; c < DBM_DIM; c++)
                dbm_flat[r * DBM_DIM + c] = s->clock_zone[r][c];

        State_space_ta_DBM_ref_t dbm_ref = State_space_ta_DBM_create(&B,
                                                                     DBM_DIM, flatbuffers_int32_vec_create(&B, dbm_flat, DBM_DIM * DBM_DIM));

        State_space_ta_State_vec_push(&B, State_space_ta_State_create(&B,
                                                                      s->location,
                                                                      dbm_ref,
                                                                      State_space_ta_Variable_create(&B, s->var.v)));
    }
    State_space_ta_State_vec_ref_t etats_ref = State_space_ta_State_vec_end(&B);

    State_space_ta_State_transitions_vec_start(&B);
    for (int i = 0; i < ss->nb_etats; i++)
    {
        State_space_ta_State_transition_vec_start(&B);
        for (int j = 0; j < ss->nb_trans_by_state[i]; j++)
        {
            State_transition *t = &ss->state_transitions[i][j];
            State_space_ta_State_transition_vec_push(&B,
                                                     State_space_ta_State_transition_create(&B, t->cible, t->action_id));
        }
        State_space_ta_State_transitions_vec_push(&B,
                                                  State_space_ta_State_transitions_create(&B,
                                                                                          State_space_ta_State_transition_vec_end(&B)));
    }
    State_space_ta_State_transitions_vec_ref_t trans_ref =
        State_space_ta_State_transitions_vec_end(&B);

    flatbuffers_int32_vec_ref_t nb_trans_ref =
        flatbuffers_int32_vec_create(&B, (int32_t *)ss->nb_trans_by_state, ss->nb_etats);

    State_space_ta_State_transitions_vec_start(&B);
    for (int i = 0; i < ss->nb_etats; i++)
    {
        State_space_ta_State_transition_vec_start(&B);
        for (int j = 0; j < ss->nb_trans_by_state[i]; j++)
        {
            State_transition *t = &ss->state_transitions[i][j];
            State_space_ta_State_transition_vec_push(&B,
                                                     State_space_ta_State_transition_create(&B, t->cible, t->action_id));
        }
        State_space_ta_State_transitions_vec_push(&B,
                                                  State_space_ta_State_transitions_create(&B,
                                                                                          State_space_ta_State_transition_vec_end(&B)));
    }

    State_space_ta_State_transitions_vec_start(&B);
    for (int i = 0; i < ss->nb_etats; i++)
    {
        State_space_ta_State_transition_vec_start(&B);
        for (int j = 0; j < pred_count[i]; j++)
        {
            int pred_id = pred_lists[i][j];
            State_space_ta_State_transition_vec_push(&B,
                                                     State_space_ta_State_transition_create(&B, pred_id, 0));
        }
        State_space_ta_State_transitions_vec_push(&B,
                                                  State_space_ta_State_transitions_create(&B,
                                                                                          State_space_ta_State_transition_vec_end(&B)));
    }
    State_space_ta_State_transitions_vec_ref_t pred_ref =
        State_space_ta_State_transitions_vec_end(&B);

    State_space_ta_State_space_ta_create_as_root(&B, etats_ref, trans_ref, pred_ref, nb_trans_ref);

    size_t size;
    void *buf = flatcc_builder_finalize_buffer(&B, &size);
    flatcc_builder_clear(&B);

    for (int i = 0; i < ss->nb_etats; i++)
        free(pred_lists[i]);
    free(pred_count);
    free(pred_lists);
    free(pred_fill);

    if (!buf)
        return -1;

    FILE *f = fopen(filename, "wb");
    if (!f)
    {
        free(buf);
        return -1;
    }
    fwrite(buf, 1, size, f);
    fclose(f);
    free(buf);

    printf("Sauvegarde : %s (%zu octets, %d etats)\n", filename, size, ss->nb_etats);
    return 0;
}