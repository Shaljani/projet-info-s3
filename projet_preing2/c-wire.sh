#!/bin/bash

show_help() {
    echo "Usage: $0 <data_file> <station_type> <consumer_type> [power_plant_id]"
    echo ""
    echo "Parameters:"
    echo "  data_file       Path to the input DATA_CWIRE.csv file"
    echo "  station_type    Type of station to process (hvb, hva, lv)"
    echo "  consumer_type   Type of consumer to process (comp, indiv, all)"
    echo "  power_plant_id  (Optional) Identifier of a specific power plant"
    echo ""
    echo "Examples:"
    echo "  $0 input/DATA_CWIRE.csv hvb comp"
    echo "  $0 input/DATA_CWIRE.csv lv all 2"
    echo ""
    exit 1
}

# Check if hep option is requested
if [[ "$1" == "-h" ]]; then
    show_help
fi

# Validate mandatory parameters
if [ $# -lt 3 ]; then
    echo "Error: Missing mandatory parameters."
    show_help
fi

DATA_FILE="$1"
STATION_TYPE="$2"
CONSUMER_TYPE="$3"
POWER_PLANT_ID="$4"

# Validate station_type
if [[ "$STATION_TYPE" != "hvb" && "$STATION_TYPE" != "hva" && "$STATION_TYPE" != "lv" ]]; then
    echo "Error: Invalid station_type. Must be one of hvb, hva, lv."
    show_help
fi

# Validate consumer_type
if [[ "$CONSUMER_TYPE" != "comp" && "$CONSUMER_TYPE" != "indiv" && "$CONSUMER_TYPE" != "all" ]]; then
    echo "Error: Invalid consumer_type. Must be one of comp, indiv, all."
    show_help
fi

# Check for forbidden combinations
if [[ ( "$STATION_TYPE" == "hvb" || "$STATION_TYPE" == "hva" ) && "$CONSUMER_TYPE" == "all" ]]; then
    echo "Error: Forbidden combination of station_type '$STATION_TYPE' with consumer_type 'all'."
    exit 1
fi

if [[ ( "$STATION_TYPE" == "hvb" || "$STATION_TYPE" == "hva" ) && "$CONSUMER_TYPE" == "indiv" ]]; then
    echo "Error: Forbidden combination of station_type '$STATION_TYPE' with consumer_type 'indiv'."
    exit 1
fi

# Check if data file exists
if [ ! -f "$DATA_FILE" ]; then
    echo "Error: Data file '$DATA_FILE' does not exist."
    exit 1
fi

# Ensure tests directory exists
mkdir -p tests

# Ensure codeC directory and executable
EXECUTABLE="./codeC/cwire"
if [ ! -f "$EXECUTABLE" ]; then
    echo "C executable not found. Compiling using make..."
    cd codeC || { echo "Error: Failed to enter codeC directory."; exit 2; }
    make
    if [ $? -ne 0 ]; then
        echo "Error: Compilation failed."
        exit 2
    fi
    cd .. || { echo "Error: Failed to return to parent directory."; exit 2; }
fi

# Ensure tmp directory exists and clean it
mkdir -p tmp
rm -f tmp/*

# Start timing
start_time=$(date +%s)

FILTERED_FILE="tmp/filtered_data.csv"

# Filtering logic based on station_type and consumer_type
case "$STATION_TYPE:$CONSUMER_TYPE" in
    hvb:comp)
        # HV-B stations and their company consumers
        awk -F';' 'NR > 1 && ( ($2 != "-" && $3 == "-" && $4 == "-" && $7 != "-" && $8 == "-") || ($2 != "-" && $3 == "-" && $4 == "-" && $5 != "-" && $7 == "-" && $8 != "-") )' "$DATA_FILE" > "$FILTERED_FILE"
        ;;
    hva:comp)
        # HV-A stations and their company consumers
        awk -F';' 'NR > 1 && ( ($3 != "-" && $4 == "-" && $7 != "-" && $8 == "-") || ($3 != "-" && $4 == "-" && $5 != "-" && $7 == "-" && $8 != "-") )' "$DATA_FILE" > "$FILTERED_FILE"
        ;;
    lv:comp)
        # LV stations and their company consumers
        awk -F';' 'NR > 1 && ( ($4 != "-" && $7 != "-" && $8 == "-") || ($4 != "-" && $5 != "-" && $7 == "-" && $8 != "-") )' "$DATA_FILE" > "$FILTERED_FILE"
        ;;
    lv:indiv)
        # LV stations and their individual consumers
        awk -F';' 'NR > 1 && ( ($4 != "-" && $7 != "-" && $8 == "-") || ($4 != "-" && $6 != "-" && $7 == "-" && $8 != "-") )' "$DATA_FILE" > "$FILTERED_FILE"
        ;;
    lv:all)
        # LV stations and both company & individual consumers
        awk -F';' 'NR > 1 && ( ($4 != "-" && $7 != "-") || ($4 != "-" && $7 == "-" && $8 != "-" && ($5 != "-" || $6 != "-")) )' "$DATA_FILE" > "$FILTERED_FILE"
        ;;
    *)
        echo "Error: Unsupported station_type and consumer_type combination."
        exit 1
        ;;
esac

# If power plant ID is given, filter further by power_plant
if [ -n "$POWER_PLANT_ID" ]; then
    # Apply power plant ID filter to the already filtered file
    awk -F';' -v pp_id="$POWER_PLANT_ID" '$1 == pp_id' "$FILTERED_FILE" > "tmp/filtered_pp_$POWER_PLANT_ID.csv"
    mv "tmp/filtered_pp_$POWER_PLANT_ID.csv" "$FILTERED_FILE"
fi

line_count=$(wc -l < "$FILTERED_FILE")
echo "Filtered lines: $line_count"

# Count station and consumer lines
station_count=$(awk -F';' '$8 == "-"' "$FILTERED_FILE" | wc -l)
consumer_count=$(awk -F';' '$8 != "-"' "$FILTERED_FILE" | wc -l)
echo "Station lines: $station_count"
echo "Consumer lines: $consumer_count"

# Determine output file name
OUTPUT_FILE="${STATION_TYPE}_${CONSUMER_TYPE}"
if [ -n "$POWER_PLANT_ID" ]; then
    OUTPUT_FILE="${OUTPUT_FILE}_${POWER_PLANT_ID}.csv"
else
    OUTPUT_FILE="${OUTPUT_FILE}.csv"
fi
OUTPUT_PATH="tests/$OUTPUT_FILE"

# Run the C program
./codeC/cwire "$FILTERED_FILE" "$OUTPUT_PATH"
if [ $? -ne 0 ]; then
    echo "Error: C program failed."
    exit 4
fi

# End timing
end_time=$(date +%s)
duration=$((end_time - start_time))
echo "Processing completed in $duration seconds."

exit 0

