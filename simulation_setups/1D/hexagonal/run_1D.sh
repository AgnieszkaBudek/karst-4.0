#! /bin/sh

printf "Building the project...\n\n"

# building the project
#rm -r build      # if I want to build from scratches
#mkdir build
cd build || exit
cmake ..
make || exit


printf "Preparing simulation...\n\n"
cd /Users/agnieszka/Desktop/KARST/DATA/dla_karst-4.0/1D || exit
current_date_time=$(date +test_%Y_%m_%d_%H_%M)
printf "$current_date_time"
mkdir "$current_date_time"
if [ -d "$current_date_time" ]; then
  echo "Directory '$current_date_time' created successfully."
else
  echo "Failed to create directory."
  exit 1
fi

cd "$current_date_time" || exit
rm *
cp /Users/agnieszka/Desktop/KARST/karst_4.0/simulation_setups/1D/hexagonal/config.txt . || exit

printf "Running the simulation...\n\n"

/Users/agnieszka/Desktop/KARST/karst_4.0/build/karst_4_0 config.txt #>std.out 2>err.out &
#/Users/agnieszka/Desktop/KARST/karst_4.0/cmake-build-debug_aga/karst_4_0 config.txt  #>wyjscie.out 2>bledy.out &
