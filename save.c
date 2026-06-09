#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "save.h"
#include "structure_DBM.h"
#include "flatcc/flatcc_builder.h"
#include "structure_state_space_ta_builder.h"

typedef struct
{
    float x;
    float y;
} Pos2D;

static Pos2D *compute_bfs_layout(State_space_TA *ss, float x_spacing, float y_spacing)
{
    int n = ss->nb_etats;
    Pos2D *pos = calloc(n, sizeof(Pos2D));
    int *depth = malloc(n * sizeof(int));
    int *queue = malloc(n * sizeof(int));

    for (int i = 0; i < n; i++)
        depth[i] = -1;

    int head = 0, tail = 0;
    depth[0] = 0;
    queue[tail++] = 0;

    while (head < tail)
    {
        int cur = queue[head++];
        for (int j = 0; j < ss->nb_trans_by_state[cur]; j++)
        {
            int nxt = ss->state_transitions[cur][j].cible;
            if (depth[nxt] == -1)
            {
                depth[nxt] = depth[cur] + 1;
                queue[tail++] = nxt;
            }
        }
    }

    for (int i = 0; i < n; i++)
        if (depth[i] == -1)
            depth[i] = 9999999;

    int max_depth = 0;
    for (int i = 0; i < n; i++)
        if (depth[i] > max_depth && depth[i] != 9999999)
            max_depth = depth[i];

    int *layer_count = calloc(max_depth + 2, sizeof(int));
    for (int i = 0; i < n; i++)
    {
        int d = depth[i] < max_depth + 1 ? depth[i] : max_depth + 1;
        layer_count[d]++;
    }

    int *layer_cur = calloc(max_depth + 2, sizeof(int));

    for (int i = 0; i < tail; i++)
    {
        int id = queue[i];
        int d = depth[id] < max_depth + 1 ? depth[id] : max_depth + 1;
        int slot = layer_cur[d]++;
        int total = layer_count[d];
        pos[id].x = d * x_spacing;
        pos[id].y = (slot - (total - 1) / 2.0f) * y_spacing;
    }

    {
        int d = max_depth + 1;
        int slot = layer_cur[d];
        for (int i = 0; i < n; i++)
        {
            if (depth[i] == 9999999)
            {
                int total = layer_count[d];
                pos[i].x = d * x_spacing;
                pos[i].y = (slot - (total - 1) / 2.0f) * y_spacing;
                slot++;
            }
        }
    }

    free(depth);
    free(queue);
    free(layer_count);
    free(layer_cur);
    return pos;
}

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
    State_space_ta_State_transitions_vec_ref_t succ_ref =
        State_space_ta_State_transitions_vec_end(&B);

    flatbuffers_int32_vec_ref_t nb_succ_ref =
        flatbuffers_int32_vec_create(&B, (int32_t *)ss->nb_trans_by_state, ss->nb_etats);

    State_space_ta_State_transitions_vec_start(&B);
    for (int i = 0; i < ss->nb_etats; i++)
    {
        State_space_ta_State_transition_vec_start(&B);
        for (int j = 0; j < pred_count[i]; j++)
        {
            int pred_id = pred_lists[i][j];
            int action_id = 0;
            for (int k = 0; k < ss->nb_trans_by_state[pred_id]; k++)
            {
                if (ss->state_transitions[pred_id][k].cible == i)
                {
                    action_id = ss->state_transitions[pred_id][k].action_id;
                    break;
                }
            }
            State_space_ta_State_transition_vec_push(&B,
                                                     State_space_ta_State_transition_create(&B, pred_id, action_id));
        }
        State_space_ta_State_transitions_vec_push(&B,
                                                  State_space_ta_State_transitions_create(&B,
                                                                                          State_space_ta_State_transition_vec_end(&B)));
    }
    State_space_ta_State_transitions_vec_ref_t pred_ref =
        State_space_ta_State_transitions_vec_end(&B);

    flatbuffers_int32_vec_ref_t nb_pred_ref =
        flatbuffers_int32_vec_create(&B, (int32_t *)pred_count, ss->nb_etats);

    const float X_SPACING = 160.0f;
    const float Y_SPACING = 80.0f;
    Pos2D *layout = compute_bfs_layout(ss, X_SPACING, Y_SPACING);

    State_space_ta_Position_vec_start(&B);
    for (int i = 0; i < ss->nb_etats; i++)
        State_space_ta_Position_vec_push(&B,
                                         State_space_ta_Position_create(&B, layout[i].x, layout[i].y));
    State_space_ta_Position_vec_ref_t positions_ref =
        State_space_ta_Position_vec_end(&B);

    free(layout);

    State_space_ta_State_space_ta_create_as_root(&B, etats_ref, succ_ref, pred_ref, nb_succ_ref, nb_pred_ref, positions_ref);

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
