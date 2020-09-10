#!/bin/bash

numOfArgs=$#

if [ "$numOfArgs" -ne "5" ]; then
    echo "Wrong number of arguments fam"
    exit
fi

diseasesFile=$1
countriesFile=$2
input_dir=$3
numFilesPerDirectory=$4
numRecordsPerFile=$5


rm -r $input_dir 2> /dev/null


#Checking if files Exist
if [  ! -f diseasesFile ]
then
    echo "DISEASES FILE DOES NOT EXIST ~ EXITING"
    exit
fi

if [  ! -f countriesFile ]
then
    echo "COUNTRIES FILE DOES NOT EXIST ~ EXITING"
    exit
fi

#checking if files are empty
if [  ! -s diseasesFile ]
then
    echo "EMPTY DISEASES FILE ~ EXITING"
    exit
fi

if [  ! -s countriesFile ]
then
    echo "EMPTY COUNTRIES FILE ~ EXITING"
    exit
fi

#checking if numbers are >0
if [ "$numFilesPerDirectory" -le 0 ]
then
    echo "INVALID NUMFILESPERDIRECTORY NUMBER ~ EXITING"
    exit
fi

if [ "$numRecordsPerFile" -le 0 ]
then
    echo "INVALID NUMRECORDSPERFILE NUMBER ~ EXITING"
    exit
fi

#read all diseases into a bash array
readarray -t diseases < "./$diseasesFile"

#read all countries into a bash array
readarray -t countries < "./$countriesFile"

#creating input directory
mkdir -p "$input_dir"

if [ $? -ne 0  ]
then
    echo "FAILED TO CREATE INPUT DIR ~ ΕΧΙΤΙΝG"
    exit
fi
#going into input directory
cd $input_dir

#creating a record id for every country, incrementing by 1 every iteration
    recID=0
    
#for every country create a new country directory
for country in "${countries[@]}" 
do
:
    mkdir -p "$country"
    if [ $? -ne 0  ]
        then
         echo "FAILED TO CREATE COUNTRY DIR ~ ΕΧΙΤΙΝG"
         exit
    fi

    cd $country
    
    for ((i=1;i<=$numFilesPerDirectory;i++)); do    #for every country directory create date files and write fixed number of random records

        #CREATING DATES IN THE DD-MM-YYYY FORMAT
        day=$(( ( RANDOM % 30 )  + 1 ))
        if [ $day -lt 10 ]
        then
            day=0$day
        fi
        month=$(( ( RANDOM % 12 )  + 1 ))
        if [ $month -lt 10 ]
        then
            month=0$month
        fi
        year=$(( ( RANDOM % 20 )  + 2000 ))

        name="$day"-"$month"-"$year"
    ################################################
        while [ -e $name ]  #checking if there is the same date file in the country dir, create a new random date till a new one is found
        do
            echo "FOUND SAME DATE... trying different date...."
            #CREATING DATES IN THE DD-MM-YYYY FORMAT
            day=$(( ( RANDOM % 30 )  + 1 ))
            if [ $day -lt 10 ]
            then
                day=0$day
            fi
                month=$(( ( RANDOM % 12 )  + 1 ))
            if [ $month -lt 10 ]
            then
                month=0$month
            fi
            year=$(( ( RANDOM % 20 )  + 2000 ))

            name="$day"-"$month"-"$year"
        done
        ################################################

        #creating a file with "date" name and writing stuff in it
        #echo >> $name
        for((j=1;j<=numRecordsPerFile;j++));do
            #adding recID
            rec=$recID
            #creating random state 0->Exit / 1->Entry
            state=$((RANDOM % 2))
            if [ $state -eq 0 ]
            then
                rec="$rec EXIT"
            else
                rec="$rec ENTER"
            fi
            ###################################################################
            #creating First Name
            lengthFirstName=$(( (RANDOM % 11) + 3))
            firstName=""

            chars=abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ
            for((k=1;k<=lengthFirstName;k++)); do
                firstName="$firstName${chars:RANDOM%${#chars}:1}"
            done
            #adding it to the rec
            rec="$rec $firstName"
            ###################################################################
            #creating Last Name
            lengthLastName=$(( (RANDOM % 11) + 3))
            lastName=""

            chars=abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ
            for((k=1;k<=lengthLastName;k++)); do
                lastName="$lastName${chars:RANDOM%${#chars}:1}"
            done
            #adding it to the rec
            rec="$rec $lastName"
            ###################################################################
            #selecting a random disease from my array
            selectedDisease=${diseases[$RANDOM % ${#diseases[@]} ]}
            rec="$rec $selectedDisease"

            #creating a random age
            age=$(( (RANDOM % 120) + 1))
            rec="$rec $age"

            #WRITING TO THE FILE THE COMPLETE RECORD
            echo $rec >> $name

            recID=$((recID+1))
        done

    done

    cd ..

done
