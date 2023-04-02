#include <stdio.h>
#include "/avees_ssd/darknet/saved_split_info.c"

#define number_G 500
#define number_L 500
#define MAX_memory 9437184 * 2 // 9 MB

typedef struct {
    int group_id;
    int group_members;
    int group_memory;
    int split_layer_ids[number_L];
    int split_layer_nums[number_L];
    int split_layer_memory[number_L];
    int split_layer_w[number_L];
    int split_layer_b[number_L];
} Group;

void trim_array(int split_data[][5], int n){
    for (int i =0; i <n ; i++){
        // printf("%d %d", i, split_data[i][0]);
        split_data[i][0] += 1;
        // printf("%d\n", split_data[i][0]);
    }
}

void save_groups_to_file(int maximum_mem, Group *groups, int group_count, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        return;
    }
    fprintf(file, "#define MAX_memory %d\n", maximum_mem);
    fprintf(file, "#define number_G %d\n", group_count);
    fprintf(file, "#define number_L %d\n", number_L);
    fprintf(file, "\n");
    fprintf(file, "typedef struct {\n");
    fprintf(file, "    int group_id;\n");
    fprintf(file, "    int group_members;\n");
    fprintf(file, "    int group_memory;\n");
    fprintf(file, "    int split_layer_ids[number_L];\n");
    fprintf(file, "    int split_layer_nums[number_L];\n");
    fprintf(file, "    int split_layer_memory[number_L];\n");
    fprintf(file, "    int split_layer_w[number_L];\n");
    fprintf(file, "    int split_layer_b[number_L];\n");
    fprintf(file, "} Group;\n");
    fprintf(file, "\n");
    fprintf(file, "Group groups[number_G] = {\n");

    for (int i = 0; i < group_count; i++) {
        fprintf(file, "    {%d, %d, %d,\n", groups[i].group_id, groups[i].group_members, groups[i].group_memory);
        fprintf(file, "    {");
        for (int j = 0; j < number_L; j++) {
            if (j != 0) {
                fprintf(file, ", ");
            }
            fprintf(file, "%d", groups[i].split_layer_ids[j]);
        }
        fprintf(file, "},\n");
        fprintf(file, "    {");
        for (int j = 0; j < number_L; j++) {
            if (j != 0) {
                fprintf(file, ", ");
            }
            fprintf(file, "%d", groups[i].split_layer_nums[j]);
        }
        fprintf(file, "},\n");
        fprintf(file, "    {");
        for (int j = 0; j < number_L; j++) {
            if (j != 0) {
                fprintf(file, ", ");
            }
            fprintf(file, "%d", groups[i].split_layer_memory[j]);
        }
        fprintf(file, "},\n");
        fprintf(file, "    {");
        for (int j = 0; j < number_L; j++) {
            if (j != 0) {
                fprintf(file, ", ");
            }
            fprintf(file, "%d", groups[i].split_layer_w[j]);
        }
        fprintf(file, "},\n");
        fprintf(file, "    {");
        for (int j = 0; j < number_L; j++) {
            if (j != 0) {
                fprintf(file, ", ");
            }
            fprintf(file, "%d", groups[i].split_layer_b[j]);
        }
        fprintf(file, "}}%s\n", i + 1 < group_count ? "," : "");
    }
    fprintf(file, "};\n");
    fprintf(file, "int group_count = %d;\n", group_count);

    fclose(file);
    printf("Group struct array saved to file: %s\n", filename);
}

// ... create_groups function ...
void create_groups(int split_data[][5], int n, Group *groups, int *group_count, int max_memory) {
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
            int split_w = split_data[i][3];
            int split_b = split_data[i][4];
            int layer_count =  0;

            if ( split_data[i][1] != 0 ) layer_count = remaining_memory / split_memory;
            else layer_count = remaining_memory / 1;

            if (layer_count > split_num) {
                layer_count = split_num;
            }

            groups[current_group_id].split_layer_ids[num_layers] = layer_id;
            groups[current_group_id].split_layer_nums[num_layers] = layer_count;
            groups[current_group_id].split_layer_memory[num_layers] = layer_count*split_memory;
            groups[current_group_id].split_layer_w[num_layers] = layer_count*split_w;
            groups[current_group_id].split_layer_b[num_layers] = layer_count*split_b;
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

void check_each_num_of_group_member (Group *groups, int group_count) {
    
    for (int j = 0; j < group_count; ++j) {
        int num_of_group_member = 0;
        for (int i = 0; i < number_L && groups[j].split_layer_ids[i] != 0; ++i) {
            // printf("groups[j].split_layer_ids[i]: %d\n", groups[j].split_layer_ids[i]);
            num_of_group_member ++;
        }
        groups[j].group_members = num_of_group_member;
        // printf("num_of_group_member: %d\n", groups[j].group_members);
    }
}

int main() {

    int n = sizeof(split_data) / sizeof(split_data[0]);

    Group groups[number_G] = {0,};

    int group_count = 0;

    //trim_array(split_data, n);
    create_groups(split_data, n, groups, &group_count, MAX_memory);
    check_each_num_of_group_member(groups, group_count);

    // Save groups to file
    save_groups_to_file(MAX_memory, groups, group_count, "saved_groups.c");
    
    for (int i = 0; i < group_count; i++) {
        printf("Group %d, members: %d, memory %d:\n", groups[i].group_id, groups[i].group_members, groups[i].group_memory);
        for (int j = 0; j < number_G && groups[i].split_layer_ids[j] != 0; j++) {
            printf("Layer ID: %d, count: %d, memory: %d, w: %d, b: %d\n",
                   groups[i].split_layer_ids[j],
                   groups[i].split_layer_nums[j],
                   groups[i].split_layer_memory[j],
                   groups[i].split_layer_w[j],
                   groups[i].split_layer_b[j]);
        }
        printf("\n");
    }


    return 0;
}

