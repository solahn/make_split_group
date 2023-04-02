#include <stdio.h>
#include "/avees_ssd/darknet/saved_split_info.c"

#define number_G 500
#define number_L 500
 
typedef struct {
    int group_id;
    int group_memory;
    int split_layer_ids[number_L];
    int split_layer_nums[number_L];
} Group;

void create_groups(int split_data[][3], int n, Group *groups, int *group_count, int max_memory) {
    int current_group_id = 0;
    int i = 0;
    while (i < n) {
        int remaining_memory = max_memory;
        groups[current_group_id].group_id = current_group_id;
        groups[current_group_id].group_memory = 0;

        int num_layers = 0;
        while (i < n && remaining_memory >= split_data[i][1]) {
            int layer_id = split_data[i][0];
            int split_memory = split_data[i][1];
            int split_num = split_data[i][2];
            int layer_count =  0;

            if ( split_data[i][1] != 0 ) layer_count = remaining_memory / split_memory;
            else layer_count = remaining_memory / 1;

            if (layer_count > split_num) {
                layer_count = split_num;
            }

            groups[current_group_id].split_layer_ids[num_layers] = layer_id;
            groups[current_group_id].split_layer_nums[num_layers] = layer_count;
            num_layers++;

            remaining_memory -= layer_count * split_memory;
            split_data[i][2] -= layer_count;

            if (split_data[i][2] == 0) {
                i++;
            }
        }

        groups[current_group_id].group_memory = max_memory - remaining_memory;
        current_group_id++;
    }
    *group_count = current_group_id;
}

int main() {

    int n = sizeof(split_data) / sizeof(split_data[0]);

    Group groups[number_G] = {0,};

    int group_count = 0;
    int max_memory = 5242880;

    create_groups(split_data, n, groups, &group_count, max_memory);

    for (int i = 0; i < group_count; i++) {
        printf("Group %d, memory %d:\n", groups[i].group_id, groups[i].group_memory);
        for (int j = 0; j < number_G && groups[i].split_layer_ids[j] != 0; j++) {
            printf("Layer ID: %d, count: %d\n",
                   groups[i].split_layer_ids[j],
                   groups[i].split_layer_nums[j]);
        }
        printf("\n");
    }

    return 0;
}