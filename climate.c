/**
 * climate.c
 *
 * Performs analysis on climate data provided by the
 * National Oceanic and Atmospheric Administration (NOAA).
 *
 * Input:    Tab-delimited file(s) to analyze.
 * Output:   Summary information about the data.
 *
 * Compile:  run make
 *
 * Example Run:      ./climate data_tn.tdv data_wa.tdv
 *
 *
 * Opening file: data_tn.tdv
 * Opening file: data_wa.tdv
 * States found: TN WA
 * -- State: TN --
 * Number of Records: 17097
 * Average Humidity: 49.4%
 * Average Temperature: 58.3F
 * Max Temperature: 110.4F 
 * Max Temperatuer on: Mon Aug  3 11:00:00 2015
 * Min Temperature: -11.1F
 * Min Temperature on: Fri Feb 20 04:00:00 2015
 * Lightning Strikes: 781
 * Records with Snow Cover: 107
 * Average Cloud Cover: 53.0%
 * -- State: WA --
 * Number of Records: 48357
 * Average Humidity: 61.3%
 * Average Temperature: 52.9F
 * Max Temperature: 125.7F
 * Max Temperature on: Sun Jun 28 17:00:00 2015
 * Min Temperature: -18.7F 
 * Min Temperature on: Wed Dec 30 04:00:00 2015
 * Lightning Strikes: 1190
 * Records with Snow Cover: 1383
 * Average Cloud Cover: 54.5%
 *
 * TDV format:
 *
 * CA» 1428300000000»  9prcjqk3yc80»   93.0»   0.0»100.0»  0.0»95644.0»277.58716
 * CA» 1430308800000»  9prc9sgwvw80»   4.0»0.0»100.0»  0.0»99226.0»282.63037
 * CA» 1428559200000»  9prrremmdqxb»   61.0»   0.0»0.0»0.0»102112.0»   285.07513
 * CA» 1428192000000»  9prkzkcdypgz»   57.0»   0.0»100.0»  0.0»101765.0» 285.21332
 * CA» 1428170400000»  9prdd41tbzeb»   73.0»   0.0»22.0»   0.0»102074.0» 285.10425
 * CA» 1429768800000»  9pr60tz83r2p»   38.0»   0.0»0.0»0.0»101679.0»   283.9342
 * CA» 1428127200000»  9prj93myxe80»   98.0»   0.0»100.0»  0.0»102343.0» 285.75
 * CA» 1428408000000»  9pr49b49zs7z»   93.0»   0.0»100.0»  0.0»100645.0» 285.82413
 *
 * Each field is separated by a tab character \t and ends with a newline \n.
 *
 * Fields:
 *      state code (e.g., CA, TX, etc),
 *      timestamp (time of observation as a UNIX timestamp),
 *      geolocation (geohash string),
 *      humidity (0 - 100%),
 *      snow (1 = snow present, 0 = no snow),
 *      cloud cover (0 - 100%),
 *      lightning strikes (1 = lightning strike, 0 = no lightning),
 *      pressure (Pa),
 *      surface temperature (Kelvin)
 */

#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUM_STATES 50

/* Creating the contents of a struct */
struct climate_info {
    char code[3];                   
    unsigned long num_records;      
    double max_temp;
    long max_temp_time;
    double min_temp;
    long min_temp_time;
    unsigned long num_lightning_strikes;    
    unsigned long num_snow;         
    long double sum_of_temperature;
    long double sum_of_humidity;
    double sum_of_cloud_cover;
};

void analyze_file(FILE *file, struct climate_info *states[], int num_states);
void print_report(struct climate_info *states[], int num_states);

int main(int argc, char *argv[]) {

    /* Checking if commands are less than 1 file */
    if (argc < 2) {
        printf("Usage: %s tdv_file1 tdv_file2 ... tdv_fileN \n", argv[0]);
        return EXIT_FAILURE;
    }

    /* Let's create an array to store our state data in. As we know, there are
     * 50 US states. */
    struct climate_info *states[NUM_STATES] = { NULL };

    int i;
    for (i = 1; i < argc; ++i) {
        /* Opening file */
        FILE *file;
        file = fopen(argv[i], "r");   

        /* Error if file does not exist */
        if (file == NULL) {
            printf("File does not exist. Moving on to next file...");
            return EXIT_FAILURE;
        }

        /* Analyze the file */
        analyze_file(file, states, NUM_STATES);
        
        fclose(file);
    }
    
    /* Now that we have recorded data for each file, we'll summarize them: */
    print_report(states, NUM_STATES);

    return 0;
}

/* This function dynamically allocates the climate data */
void analyze_file(FILE *file, struct climate_info **states, int num_states) {
    const int line_sz = 100;                       
    char line[line_sz]; 
    
    while (fgets(line, line_sz, file) != NULL) {           
        
        /* tokenize line and split it at every tab */
        char* token = strtok(line, "\t");

        /* create array to store line data */
        char* data[9];                                                                                        
        int index = 0;
        while (token != NULL) {
            data[index] = token;
            token = strtok(NULL, "\t");
            index++;
        }

        int val = 0;
        while (val < num_states && states[val] != NULL ){
            if(strcmp(data[0],states[val]->code) == 0){
                break;
            }
            val++;
        }

        /* Initialize struct in memory */
        if (states[val] == NULL) {
            states[val] = (struct climate_info*) malloc (sizeof(struct climate_info));
            strcpy((states[val])->code, data[0]);                      
            (states[val])->num_records = 1;
            (states[val])->max_temp = (atof(data[8]) * 1.8 - 459.67);                  
            (states[val])->max_temp_time = atol(data[1]) / 1000;
            (states[val])->min_temp = (atof(data[8]) * 1.8 - 459.67);
            (states[val])->min_temp_time = atol(data[1]) / 1000;
            (states[val])->num_lightning_strikes = atol(data[6]);     
            (states[val])->num_snow = atol(data[4]);                  
            (states[val])->sum_of_temperature = (atof(data[8]) * 1.8 - 459.67);        
            (states[val])->sum_of_humidity = atof(data[3]);           
            (states[val])->sum_of_cloud_cover = atof(data[5]);
        } else {                                                            //else we update the struct
            (states[val])->num_records +=1;                                       
            if ((states[val])->max_temp < (atof(data[8]) * 1.8 - 459.67)){             
                (states[val])->max_temp = (atof(data[8]) * 1.8 - 459.67);
                (states[val])->max_temp_time = atol(data[1]) / 1000;        
            }                                                       
            if ((states[val])->min_temp > (atof(data[8]) * 1.8 - 459.67)) {
                (states[val])->min_temp = (atof(data[8]) * 1.8 - 459.67);
                (states[val])->min_temp_time = atol(data[1]) / 1000;
            }
            (states[val])->num_lightning_strikes += atol(data[6]);         
            (states[val])->num_snow += atol(data[4]);                      
            (states[val])->sum_of_temperature += (atof(data[8]) * 1.8 - 459.67);            
            (states[val])->sum_of_humidity += atof(data[3]);               
            (states[val])->sum_of_cloud_cover += atof(data[5]);
        }
    }
}    

/* This function prints out the climate data */     
void print_report(struct climate_info *states[], int num_states) {
    printf("Welcome. This program erforms analysis on climate data provided by the National Oceanic and Atmospheric Administration (NOAA).\n");
    
    printf("States found: ");
    int i;
    for (i = 0; i < num_states; ++i) {
        if (states[i] != NULL) {
            struct climate_info *info = states[i];
            printf("%s ", info->code);
        }
    }
    printf("\n");

    /* Print out the summary for each state */
    for (int i = 0; i < num_states; i++) {
        struct climate_info *info = states[i];
        if(info!=NULL){
            printf("-- State: %s --\n", (info->code));
            printf("Number of Records: %ld\n", (info->num_records));
            printf("Average humidity: %.1Lf%%\n", (info)->sum_of_humidity / info->num_records);
            printf("Average temperature: %.1LFF\n", (info)->sum_of_temperature / info->num_records);  
            printf("Max temperature: %.1fF\n", (info)->max_temp);
            printf("Max temperature on: %s", ctime(&(info)->max_temp_time));         
            printf("Min temperature: %.1fF\n", (info)->min_temp);
            printf("Min Temperature on: %s", ctime(&(info)->min_temp_time));
            printf("Lightning Strikes: %ld\n", (info)->num_lightning_strikes);     //this # / 50 is correct
            printf("Records with Snow Cover: %ld\n", (info)->num_snow);             //this # / 50 is correct
            printf("Average Cloud Cover: %.1f%%\n", (info)->sum_of_cloud_cover / (info)->num_records);
        }

    }
}
