# This script will take NRT notation and create a Poly file
# You will need to compile and install the NRT parser, found here:
# http://www.github.com/PaulBatchelor/NRT.git

#notes="d2r4t2."
notes="(dms)(rfl)m8fs"
bpm=120

scaleTempo() {
    local myBpm=$1
    awk -v bpm=$myBpm 'BEGIN{scale = 60/bpm} {print $1 * scale ,$2 * scale,$3 + 60,"0.1"}'
}

toBin() {
    ./str2poly score.bin
}

echo $notes | nrt | scaleTempo 120 | toBin

