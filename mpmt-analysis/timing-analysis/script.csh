make
declare -i slice
echo "Divide data into how many slices?"
read slice
declare -i runNo
echo "Run Number?"
read runNo
for (( i = 0; i < slice; i++ ))
do
    echo -e "$i\n$slice\n$runNo" | ./mpmt_timing_analysis.exe mpmt_Analysis_run0"${runNo}".root
done
echo -e "$runNo" | root ../rms.C
