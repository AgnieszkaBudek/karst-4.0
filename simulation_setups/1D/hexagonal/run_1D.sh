#! /bin/sh

printf "Preparing the simulation...\n\n"

#bash ./build.sh
#if ! bash ./build.sh; then
#    echo "Problem with compilation."
#    exit 1
#fi

cd /Users/agnieszka/Desktop/KARST/DATA/dla_karst-4.0/1D || exit

# Creating proper directory
current_date_time=$(date +test_%Y_%m_%d_%H_%M)
printf $current_date_time
mkdir "$current_date_time"
if [ -d "$current_date_time" ]; then
  echo "Directory '$current_date_time' created successfully."
else
  echo "Failed to create directory."
  exit 1
fi

cd "$current_date_time" || exit

pwd
cp /Users/agnieszka/Desktop/KARST/karst_4.0/simulation_setups/1D/hexagonal/config.txt . || exit


printf "Running the simulation...\n\n"


  /Users/agnieszka/Desktop/KARST/karst_4.0/build/karst config.txt >wyjscie.out 2>bledy.out &
