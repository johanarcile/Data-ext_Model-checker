#include <stdio.h>
#include <stdlib.h>
#include "load.h"
#include "structure_DBM.h"
#include "structure_state_space_ta_reader.h"
#include "structure_state_space_ta_verifier.h"

int export_to_json(const char *input_filename)
{
    FILE *f = fopen(input_filename, "rb");
    if (!f)
    {
        printf("Erreur ouverture %s\n", input_filename);
        return -1;
    }

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    void *buf = malloc(size);
    fread(buf, 1, size, f);
    fclose(f);

    // vérification FlatBuffers
    if (State_space_ta_State_space_ta_verify_as_root(buf, size) != flatcc_verify_ok)
    {
        printf("Fichier invalide\n");
        free(buf);
        return -1;
    }

    char json_filename[256];
    snprintf(json_filename, sizeof(json_filename), "%s.json", input_filename);

    FILE *out = fopen(json_filename, "w");
    if (!out)
    {
        printf("Erreur: impossible de créer %s\n", json_filename);
        free(buf);
        return -1;
    }

    State_space_ta_State_space_ta_table_t root =
        State_space_ta_State_space_ta_as_root(buf);

    State_space_ta_State_vec_t etats =
        State_space_ta_State_space_ta_etats(root);

    State_space_ta_State_transitions_vec_t trans_vec =
        State_space_ta_State_space_ta_state_transitions(root);

    int nb_etats = State_space_ta_State_vec_len(etats);

    int *succ_count = calloc((size_t)nb_etats, sizeof(int));
    int *pred_count = calloc((size_t)nb_etats, sizeof(int));
    int **succ_lists = calloc((size_t)nb_etats, sizeof(int *));
    int **pred_lists = calloc((size_t)nb_etats, sizeof(int *));
    int *succ_fill = calloc((size_t)nb_etats, sizeof(int));
    int *pred_fill = calloc((size_t)nb_etats, sizeof(int));

    if (!succ_count || !pred_count || !succ_lists || !pred_lists || !succ_fill || !pred_fill)
    {
        printf("Erreur: allocation memoire export JSON\n");
        free(succ_count);
        free(pred_count);
        free(succ_lists);
        free(pred_lists);
        free(succ_fill);
        free(pred_fill);
        fclose(out);
        free(buf);
        return -1;
    }

    for (int i = 0; i < nb_etats; i++)
    {
        State_space_ta_State_transitions_vec_t tlist =
            State_space_ta_State_transitions_items(
                State_space_ta_State_transitions_vec_at(trans_vec, i));

        int nb_trans =
            State_space_ta_State_transition_vec_len(tlist);

        for (int j = 0; j < nb_trans; j++)
        {
            State_space_ta_State_transition_table_t t =
                State_space_ta_State_transition_vec_at(tlist, j);

            int target = State_space_ta_State_transition_cible(t);
            if (target < 0 || target >= nb_etats)
            {
                continue;
            }

            succ_count[i]++;
            pred_count[target]++;
        }
    }

    for (int i = 0; i < nb_etats; i++)
    {
        if (succ_count[i] > 0)
        {
            succ_lists[i] = malloc((size_t)succ_count[i] * sizeof(int));
            if (!succ_lists[i])
            {
                printf("Erreur: allocation memoire successeurs\n");
                for (int k = 0; k < nb_etats; k++)
                {
                    free(succ_lists[k]);
                    free(pred_lists[k]);
                }
                free(succ_count);
                free(pred_count);
                free(succ_lists);
                free(pred_lists);
                free(succ_fill);
                free(pred_fill);
                fclose(out);
                free(buf);
                return -1;
            }
        }

        if (pred_count[i] > 0)
        {
            pred_lists[i] = malloc((size_t)pred_count[i] * sizeof(int));
            if (!pred_lists[i])
            {
                printf("Erreur: allocation memoire predecesseurs\n");
                for (int k = 0; k < nb_etats; k++)
                {
                    free(succ_lists[k]);
                    free(pred_lists[k]);
                }
                free(succ_count);
                free(pred_count);
                free(succ_lists);
                free(pred_lists);
                free(succ_fill);
                free(pred_fill);
                fclose(out);
                free(buf);
                return -1;
            }
        }
    }

    for (int i = 0; i < nb_etats; i++)
    {
        State_space_ta_State_transitions_vec_t tlist =
            State_space_ta_State_transitions_items(
                State_space_ta_State_transitions_vec_at(trans_vec, i));

        int nb_trans =
            State_space_ta_State_transition_vec_len(tlist);

        for (int j = 0; j < nb_trans; j++)
        {
            State_space_ta_State_transition_table_t t =
                State_space_ta_State_transition_vec_at(tlist, j);

            int target = State_space_ta_State_transition_cible(t);
            if (target < 0 || target >= nb_etats)
            {
                continue;
            }

            succ_lists[i][succ_fill[i]++] = target;
            pred_lists[target][pred_fill[target]++] = i;
        }
    }

    fprintf(out, "{\"nodes\": [\n");

    for (int i = 0; i < nb_etats; i++)
    {
        State_space_ta_State_table_t s =
            State_space_ta_State_vec_at(etats, i);

        int state_id = i;
        int location = State_space_ta_State_location(s);
        int var_value = State_space_ta_Variable_v(State_space_ta_State_var(s));

        flatbuffers_int32_vec_t dbm_vals =
            State_space_ta_DBM_matrice_applatie(State_space_ta_State_clock_zone(s));

        fprintf(out, "{\"data\": {\"id\": \"%d\", \"location\": %d, \"variable_v\": %d, \"clock_zone\": [",
                state_id, location, var_value);

        for (int r = 0; r < DBM_DIM; r++)
        {
            fprintf(out, "[");
            for (int c = 0; c < DBM_DIM; c++)
            {
                int32_t val = flatbuffers_int32_vec_at(dbm_vals, r * DBM_DIM + c);
                fprintf(out, "%d", val);
                if (c < DBM_DIM - 1)
                    fprintf(out, ", ");
            }
            fprintf(out, "]");
            if (r < DBM_DIM - 1)
                fprintf(out, ", ");
        }

        fprintf(out, "], \"predecesseurs\": [");
        for (int p = 0; p < pred_count[i]; p++)
        {
            fprintf(out, "%d", pred_lists[i][p]);
            if (p < pred_count[i] - 1)
                fprintf(out, ", ");
        }

        fprintf(out, "], \"successeurs\": [");
        for (int sidx = 0; sidx < succ_count[i]; sidx++)
        {
            fprintf(out, "%d", succ_lists[i][sidx]);
            if (sidx < succ_count[i] - 1)
                fprintf(out, ", ");
        }

        fprintf(out, "]}}");

        if (i < nb_etats - 1)
            fprintf(out, ",\n");
        else
            fprintf(out, "\n");
    }

    fprintf(out, "], \"edges\": [\n");

    int first_edge = 1;

    for (int i = 0; i < nb_etats; i++)
    {
        State_space_ta_State_table_t s =
            State_space_ta_State_vec_at(etats, i);

        int source_id = i;

        State_space_ta_State_transitions_vec_t tlist =
            State_space_ta_State_transitions_items(
                State_space_ta_State_transitions_vec_at(trans_vec, i));

        int nb_trans =
            State_space_ta_State_transition_vec_len(tlist);

        for (int j = 0; j < nb_trans; j++)
        {
            State_space_ta_State_transition_table_t t =
                State_space_ta_State_transition_vec_at(tlist, j);

            int target = State_space_ta_State_transition_cible(t);
            int action = State_space_ta_State_transition_action_id(t);

            if (!first_edge)
                fprintf(out, ",\n");
            first_edge = 0;

            fprintf(out, "{\"data\": {\"source\": \"%d\", \"target\": \"%d\", \"label\": \"%d\"}}",
                    source_id, target, action);
        }
    }

    fprintf(out, "\n]}");

    fclose(out);
    for (int i = 0; i < nb_etats; i++)
    {
        free(succ_lists[i]);
        free(pred_lists[i]);
    }
    free(succ_count);
    free(pred_count);
    free(succ_lists);
    free(pred_lists);
    free(succ_fill);
    free(pred_fill);
    free(buf);
    return 0;
}

void free_state_space(State_space_TA *ss)
{
    if (!ss)
    {
        return;
    }

    free(ss->etats);
    free(ss->nb_trans_by_state);

    if (ss->state_transitions)
    {
        for (int i = 0; i < ss->nb_etats; i++)
        {
            free(ss->state_transitions[i]);
        }
        free(ss->state_transitions);
    }

    ss->etats = NULL;
    ss->nb_trans_by_state = NULL;
    ss->state_transitions = NULL;
    ss->nb_etats = 0;
    ss->capacite_etats = 0;
}