#!/bin/bash

make

RED='\033[0;31m'
GREEN='\033[0;32m'
ORANGE='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

TESTSRUN=0
TESTSPASSED=0

STATS_ORIGINALSIZE=()
STATS_COMPRESSEDSIZE=()
STATS_EFFICIENCY=()
STATS_COMPRESSTIME=()

TIMEFORMAT=%R

echo "------------------------------------------"
echo -e "${BLUE}Running tests for huff_codec${NC}"
echo "------------------------------------------"

POSSIBLEFLAGS=("")
ALLFILES=()

for file in data/*.raw
do
    # add the file to the list of all files if it is not already there
    if [[ ! " ${ALLFILES[@]} " =~ " ${file} " ]]
    then
        ALLFILES+=("$file")
    fi

    for flag in "${POSSIBLEFLAGS[@]}"
    do
        rm -f compressed.tmp decompressed.tmp
        if [ -z "$flag" ]
        then
            echo "Running test for $file with no flags"
        else
            echo "Running test for $file with flags $flag"
        fi
        ORIGINALSIZE=$(stat -c%s "$file")

        # time ./huff_codec -c -i $file -o compressed.tmp -w 512 $flag
        # time the compression and save the time in COMPRESSTIME
        COMPRESSTIME=$( { time ./huff_codec -c -i $file -o compressed.tmp -w 512 $flag; } 2>&1 )
        # take only the last line of the output
        COMPRESSTIME=$(echo "$COMPRESSTIME" | tail -n 1)
        if [ $? -ne 0 ]
        then
            echo -e "${RED}Test failed on ${ORANGE}execution of compression${NC}"
            TESTSRUN=$((TESTSRUN+1))
            echo "------------------------------------------"
            continue
        fi
        COMPRESSEDSIZE=$(stat -c%s "compressed.tmp")
        # set the efficiency as the number of bits required to store one pixel (in BITS)
        EFFICIENCY=$(echo "scale=2; $COMPRESSEDSIZE*8/$ORIGINALSIZE" | bc)
        # write the stats into the array but tab it out so it looks nice
        STATS_ORIGINALSIZE+=("$ORIGINALSIZE")
        STATS_COMPRESSEDSIZE+=("$COMPRESSEDSIZE")
        STATS_EFFICIENCY+=("$EFFICIENCY")
        STATS_COMPRESSTIME+=("$COMPRESSTIME")

        ./huff_codec -d -i compressed.tmp -o decompressed.tmp $flag
        if [ $? -ne 0 ]
        then
            echo -e "${RED}Test failed on ${ORANGE}execution of decompression${NC}"
            TESTSRUN=$((TESTSRUN+1))
            echo "------------------------------------------"
            continue
        fi
        diff $file decompressed.tmp > /dev/null
        if [ $? -eq 0 ]
        then
            TESTSPASSED=$((TESTSPASSED+1))
            echo -e "${GREEN}Test passed${NC}"
        else
            echo -e "${RED}Test failed${NC}"
        fi
        TESTSRUN=$((TESTSRUN+1))
        echo "------------------------------------------"
    done
done

echo "Tests run: $TESTSRUN"
echo "Tests passed: $TESTSPASSED"
echo "------------------------------------------"
echo "Compression stats:"
echo "------------------------------------------"
# print the stats in a nice table
printf "%-30s %-10s %-10s %-10s %-10s %-10s\n" "File" "Flags" "Orig." "Comp." "Efficiency" "Time"
for (( i=0; i<${#ALLFILES[@]}; i++ ))
do
    for (( j=0; j<${#POSSIBLEFLAGS[@]}; j++ ))
    do
        # if the flag is empty, print "No flags"
        if [ -z "${POSSIBLEFLAGS[$j]}" ]
        then
            printf "%-30s %-10s %-10s %-10s %-10s %-10s\n" "${ALLFILES[$i]##*/}" "No flags" "${STATS_ORIGINALSIZE[$((i*${#POSSIBLEFLAGS[@]}+j))]}" "${STATS_COMPRESSEDSIZE[$((i*${#POSSIBLEFLAGS[@]}+j))]}" "${STATS_EFFICIENCY[$((i*${#POSSIBLEFLAGS[@]}+j))]}" "${STATS_COMPRESSTIME[$((i*${#POSSIBLEFLAGS[@]}+j))]}"
        else
            printf "%-30s %-10s %-10s %-10s %-10s %-10s\n" "${ALLFILES[$i]##*/}" "${POSSIBLEFLAGS[$j]}" "${STATS_ORIGINALSIZE[$((i*${#POSSIBLEFLAGS[@]}+j))]}" "${STATS_COMPRESSEDSIZE[$((i*${#POSSIBLEFLAGS[@]}+j))]}" "${STATS_EFFICIENCY[$((i*${#POSSIBLEFLAGS[@]}+j))]}" "${STATS_COMPRESSTIME[$((i*${#POSSIBLEFLAGS[@]}+j))]}"
        fi
    done
done

# if the script was called with the --latex flag, print the stats in a latex table. Otherwise, just print the stats in a nice table
# the file names are printed with the path, so we need to remove the path
# print two tables, one for the efficiency and one for the compression time (in seconds).
# each table has one column for filename and then columns each for the different flags

if [ "$1" == "--latex" ]
then
    # print the start of a latex table
    echo "\begin{table}[H]"
    echo "\centering"
    # print as many columns as there are flag options plus one for the filename
    echo "\begin{tabular}{|l|*{${#POSSIBLEFLAGS[@]}}{c|}}"
    echo "\hline"
    # print the filename column
    echo -n "File & "
    # print the flag options as the column headers
    for flag in "${POSSIBLEFLAGS[@]}"
    do
        if [ -z "$flag" ]
        then
            echo -n "No flags & "
        else
            #if last flag, don't print the &
            if [ "$flag" == "${POSSIBLEFLAGS[-1]}" ]
            then
                echo -n "$flag \\\\"
            else
                echo -n "$flag & "
            fi
        fi
    done
    echo "\hline"

    for (( i=0; i<${#ALLFILES[@]}; i++ ))
    do
        # print the filename
        echo -n "${ALLFILES[$i]##*/} & "
        for (( j=0; j<${#POSSIBLEFLAGS[@]}; j++ ))
        do
            # if last flag (use the j variable), don't print the &
            if [ "$j" -eq "$((${#POSSIBLEFLAGS[@]}-1))" ]
            then
                echo -n "${STATS_EFFICIENCY[$((i*${#POSSIBLEFLAGS[@]}+j))]} \\\\"
            else
                echo -n "${STATS_EFFICIENCY[$((i*${#POSSIBLEFLAGS[@]}+j))]} & "
            fi
        done
        echo "\hline"
    done
       # print double thick line at the end of the table
    echo "\hline"
    # print the average compression time
    echo -n "Average & "
    for (( j=0; j<${#POSSIBLEFLAGS[@]}; j++ ))
    do
        # its a floating number
        AVGPERFLAG=0
        for (( i=0; i<${#ALLFILES[@]}; i++ ))
        do
            AVGPERFLAG=$(echo "$AVGPERFLAG + ${STATS_EFFICIENCY[$((i*${#POSSIBLEFLAGS[@]}+j))]}" | bc -l)
        done
        AVGPERFLAG=$(echo "$AVGPERFLAG / ${#ALLFILES[@]}" | bc -l)
        # round to 2 decimal places
        AVGPERFLAG=$(printf "%.2f" $AVGPERFLAG)
        # if last flag (use the j variable), don't print the &
        if [ "$j" -eq "$((${#POSSIBLEFLAGS[@]}-1))" ]
        then
            echo -n "$AVGPERFLAG \\\\"
        else
            echo -n "$AVGPERFLAG & "
        fi
    done
    echo "\hline"
    # print the end of the table
    echo "\end{tabular}"
    echo "\caption{Efficiency of compression for different files and flags}"
    echo "\label{tab:efficiency}"
    echo "\end{table}"

    # print the start of a latex table
    echo "\begin{table}[H]"
    echo "\centering"
    # print as many columns as there are flag options plus one for the filename
    echo "\begin{tabular}{|l|*{${#POSSIBLEFLAGS[@]}}{c|}}"
    echo "\hline"
    # print the filename column
    echo -n "File & "
    # print the flag options as the column headers
    for flag in "${POSSIBLEFLAGS[@]}"
    do
        if [ -z "$flag" ]
        then
            echo -n "No flags & "
        else
            #if last flag, don't print the &
            if [ "$flag" == "${POSSIBLEFLAGS[-1]}" ]
            then
                echo -n "$flag \\\\"
            else
                echo -n "$flag & "
            fi
        fi
    done
    echo "\hline"

    for (( i=0; i<${#ALLFILES[@]}; i++ ))
    do
        # print the filename
        echo -n "${ALLFILES[$i]##*/} & "
        for (( j=0; j<${#POSSIBLEFLAGS[@]}; j++ ))
        do
            # if last flag (use the j variable), don't print the &
            if [ "$j" -eq "$((${#POSSIBLEFLAGS[@]}-1))" ]
            then
                echo -n "${STATS_COMPRESSTIME[$((i*${#POSSIBLEFLAGS[@]}+j))]} s \\\\"
            else
                echo -n "${STATS_COMPRESSTIME[$((i*${#POSSIBLEFLAGS[@]}+j))]} s & "
            fi
        done
        echo "\hline"
    done
    # print double thick line at the end of the table
    echo "\hline"
    # print the average compression time
    echo -n "Average & "
    for (( j=0; j<${#POSSIBLEFLAGS[@]}; j++ ))
    do
        # its a floating number
        AVGPERFLAG=0
        for (( i=0; i<${#ALLFILES[@]}; i++ ))
        do
            AVGPERFLAG=$(echo "$AVGPERFLAG + ${STATS_COMPRESSTIME[$((i*${#POSSIBLEFLAGS[@]}+j))]}" | bc -l)
        done
        AVGPERFLAG=$(echo "$AVGPERFLAG / ${#ALLFILES[@]}" | bc -l)
        # round to 2 decimal places
        AVGPERFLAG=$(printf "%.2f" $AVGPERFLAG)
        # if last flag (use the j variable), don't print the &
        if [ "$j" -eq "$((${#POSSIBLEFLAGS[@]}-1))" ]
        then
            echo -n "$AVGPERFLAG s\\\\"
        else
            echo -n "$AVGPERFLAG s& "
        fi
    done
    echo "\hline"

    # print the end of the table
    echo "\end{tabular}"
    echo "\caption{Compression time for different files and flags}"
    echo "\label{tab:compressiontime}"
    echo "\end{table}"
fi

rm -f compressed.tmp decompressed.tmp