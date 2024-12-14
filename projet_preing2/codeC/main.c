#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"

#define TOP_COUNT 10

// trim newline and carriage return characters
void trim_newline(char* str) {
    char* pos;
    if ((pos = strchr(str, '\n')) != NULL)
        *pos = '\0';
    if ((pos = strchr(str, '\r')) != NULL)
        *pos = '\0';
}

typedef struct {
    int id;
    long long capacity;
    long long consumption;
} StationEntry;

typedef struct {
    StationEntry entries[TOP_COUNT];
    int size;
} TopStations;

// initialize TopStations
void initTopStations(TopStations* top) {
    top->size = 0;
}

// insert into TopStations for high consumption
void insertTopHigh(TopStations* top, StationEntry station) {
    long long difference = station.capacity - station.consumption;

    if (top->size < TOP_COUNT) {
        top->entries[top->size++] = station;
        // Bubble up to maintain ascending order of difference
        for (int i = top->size - 1; i > 0; i--) {
            long long current_diff = top->entries[i].capacity - top->entries[i].consumption;
            long long previous_diff = top->entries[i - 1].capacity - top->entries[i - 1].consumption;

            if (current_diff < previous_diff) {
                StationEntry temp = top->entries[i];
                top->entries[i] = top->entries[i - 1];
                top->entries[i - 1] = temp;
            } else {
                break;
            }
        }
    } else {
        // check if current station has a more negative difference than the largest in top_high
        long long largest_diff = top->entries[TOP_COUNT - 1].capacity - top->entries[TOP_COUNT - 1].consumption;

        if (difference < largest_diff) {
            top->entries[TOP_COUNT - 1] = station;
            // Bubble up to maintain ascending order of difference
            for (int i = TOP_COUNT - 1; i > 0; i--) {
                long long current_diff = top->entries[i].capacity - top->entries[i].consumption;
                long long previous_diff = top->entries[i - 1].capacity - top->entries[i - 1].consumption;

                if (current_diff < previous_diff) {
                    StationEntry temp = top->entries[i];
                    top->entries[i] = top->entries[i - 1];
                    top->entries[i - 1] = temp;
                } else {
                    break;
                }
            }
        }
    }
}

// insert into TopStations for low consumption
void insertTopLow(TopStations* top, StationEntry station) {
    long long difference = station.capacity - station.consumption;

    if (top->size < TOP_COUNT) {
        top->entries[top->size++] = station;
        // Bubble up to maintain descending order of difference (most positive first)
        for (int i = top->size - 1; i > 0; i--) {
            long long current_diff = top->entries[i].capacity - top->entries[i].consumption;
            long long previous_diff = top->entries[i - 1].capacity - top->entries[i - 1].consumption;

            if (current_diff > previous_diff) {
                StationEntry temp = top->entries[i];
                top->entries[i] = top->entries[i - 1];
                top->entries[i - 1] = temp;
            } else {
                break;
            }
        }
    } else {
        // check if current station has a more positive difference than the smallest in top_low
        long long smallest_diff = top->entries[TOP_COUNT - 1].capacity - top->entries[TOP_COUNT - 1].consumption;

        if (difference > smallest_diff) {
            top->entries[TOP_COUNT - 1] = station;
            // Bubble up to maintain descending order of difference
            for (int i = TOP_COUNT - 1; i > 0; i--) {
                long long current_diff = top->entries[i].capacity - top->entries[i].consumption;
                long long previous_diff = top->entries[i - 1].capacity - top->entries[i - 1].consumption;

                if (current_diff > previous_diff) {
                    StationEntry temp = top->entries[i];
                    top->entries[i] = top->entries[i - 1];
                    top->entries[i - 1] = temp;
                } else {
                    break;
                }
            }
        }
    }
}

void traverseAndProcess(AVLNode* node, FILE* fp_out, TopStations* top_high, TopStations* top_low, const char* station_type, const char* consumer_type);

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    char* input_path = argv[1];
    char* output_path = argv[2];

    FILE* fp_in = fopen(input_path, "r");
    if (!fp_in) {
        perror("Error opening input file");
        return 2;
    }

    FILE* fp_out = fopen(output_path, "w");
    if (!fp_out) {
        perror("Error opening output file");
        fclose(fp_in);
        return 3;
    }

    char line[4096];
    AVLNode* root = NULL;

    // Extract station_type and consumer_type from output file name
    char* filename = strrchr(output_path, '/');
    if (!filename) {
        filename = output_path;
    } else {
        filename++;
    }

    char station_type[10] = {0};
    char consumer_type[10] = {0};

    sscanf(filename, "%[^_]_%[^_.]", station_type, consumer_type);

    // Determine header based on station_type and consumer_type
    char header[256] = {0};
    if (strcmp(station_type, "hvb") == 0) {
        if (strcmp(consumer_type, "comp") == 0) {
            strcpy(header, "Station HV-B:Capacité:Consommation (entreprises)");
        }
    } else if (strcmp(station_type, "hva") == 0) {
        if (strcmp(consumer_type, "comp") == 0) {
            strcpy(header, "Station HV-A:Capacité:Consommation (entreprises)");
        }
    } else if (strcmp(station_type, "lv") == 0) {
        if (strcmp(consumer_type, "comp") == 0) {
            strcpy(header, "Station LV:Capacité:Consommation (entreprises)");
        } else if (strcmp(consumer_type, "indiv") == 0) {
            strcpy(header, "Station LV:Capacité:Consommation (individuels)");
        } else if (strcmp(consumer_type, "all") == 0) {
            strcpy(header, "Station LV:Capacité:Consommation (tous)");
        }
    } else {
        strcpy(header, "Station:Capacité:Consommation");
    }

    // write header
    fprintf(fp_out, "%s\n", header);

    // init TopStations
    TopStations top_high;
    TopStations top_low;
    initTopStations(&top_high);
    initTopStations(&top_low);

    // Read data and insert into AVL tree
    while (fgets(line, sizeof(line), fp_in)) {
        trim_newline(line);

        if (strlen(line) == 0)
            continue;

        // Format: Power plant;HV-B Station;HV-A Station;LV Station;Company;Individual;Capacity;Load
        char* token = strtok(line, ";");
        if (token == NULL) continue;
        int power_plant = (strcmp(token, "-") == 0) ? -1 : atoi(token);

        token = strtok(NULL, ";");
        int hvb_station = (strcmp(token, "-") == 0) ? -1 : atoi(token);

        token = strtok(NULL, ";");
        int hva_station = (strcmp(token, "-") == 0) ? -1 : atoi(token);

        token = strtok(NULL, ";");
        int lv_post = (strcmp(token, "-") == 0) ? -1 : atoi(token);

        token = strtok(NULL, ";");
        int company = (strcmp(token, "-") == 0) ? -1 : atoi(token);
		(void) company;
		
        token = strtok(NULL, ";");
        int individual = (strcmp(token, "-") == 0) ? -1 : atoi(token);
		(void) individual;
		
        token = strtok(NULL, ";");
        long long capacity = (strcmp(token, "-") == 0) ? 0 : atoll(token);

        token = strtok(NULL, ";");
        long long load = (strcmp(token, "-") == 0) ? 0 : atoll(token);

        int station_id = -1;

        // Priority: LV > HV-A > HV-B > power_plant
        if (lv_post != -1) {
            station_id = lv_post;
        } else if (hva_station != -1) {
            station_id = hva_station;
        } else if (hvb_station != -1) {
            station_id = hvb_station;
        } else if (power_plant != -1) {
            station_id = power_plant;
        } else {
            continue;
        }

        if (station_id <= 0) continue;

        Station station;
        station.id = station_id;
        // Determine line type
        if (capacity > 0 && load == 0) {
            // Station line
            station.capacity = capacity;
            station.consumption = 0;
        } else if (load > 0 && capacity == 0) {
            // Consumer line
            station.capacity = 0;
            station.consumption = load;
        } else {
            continue;
        }

        // Insert/update in AVL
        root = insertAVL(root, station);
    }

    traverseAndProcess(root, fp_out, &top_high, &top_low, station_type, consumer_type);

    fclose(fp_out);
    fclose(fp_in);

    // If lv_all, produce lv_all_minmax.csv
    if (strcmp(station_type, "lv") == 0 && strcmp(consumer_type, "all") == 0) {
        // Create minmax file path
        char minmax_path[1024];
        strcpy(minmax_path, output_path);
        char* pos = strstr(minmax_path, "_all.csv");
        if (pos) {
            strcpy(pos, "_all_minmax.csv"); // replace _all.csv with _all_minmax.csv
        } else {
            // fallback if not found
            strcpy(minmax_path, "lv_all_minmax.csv");
        }

        FILE* fp_mm = fopen(minmax_path, "w");
        if (!fp_mm) {
            perror("Error creating lv_all_minmax.csv");
            freeAVL(root);
            return 4;
        }

        fprintf(fp_mm, "Min and Max 'capacity-load' extreme nodes\n");
        fprintf(fp_mm, "Station LV:Capacité:Consommation (tous)\n");

        // Write top_high first
        for(int i = 0; i < top_high.size; i++) {
            fprintf(fp_mm, "%d:%lld:%lld\n", top_high.entries[i].id, top_high.entries[i].capacity, top_high.entries[i].consumption);
        }

        // Write top_low second
        for(int i = top_low.size - 1; i >= 0; i--) {
            fprintf(fp_mm, "%d:%lld:%lld\n", top_low.entries[i].id, top_low.entries[i].capacity, top_low.entries[i].consumption);
        }

        fclose(fp_mm);
    }

    freeAVL(root);

    return 0;
}

// traverseAndProcess
void traverseAndProcess(AVLNode* node, FILE* fp_out, TopStations* top_high, TopStations* top_low, const char* station_type, const char* consumer_type) {
    if (!node) return;
    traverseAndProcess(node->left, fp_out, top_high, top_low, station_type, consumer_type);

    // write to main output
    fprintf(fp_out, "%d:%lld:%lld\n", node->station.id, node->station.capacity, node->station.consumption);

    // maintain top 10 high and low consumption
    if (strcmp(station_type, "lv") == 0 && strcmp(consumer_type, "all") == 0) {
        insertTopHigh(top_high, (StationEntry){node->station.id, node->station.capacity, node->station.consumption});
        insertTopLow(top_low, (StationEntry){node->station.id, node->station.capacity, node->station.consumption});
    }

    traverseAndProcess(node->right, fp_out, top_high, top_low, station_type, consumer_type);
}

