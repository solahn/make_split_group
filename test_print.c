#include <stdio.h>
#include "saved_groups.c"

int main() {
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