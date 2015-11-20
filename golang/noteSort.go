package main

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"os"
	"sort"
)

type Note struct {
	Start float32
	Vals  []float32
}

func (p Note) String() string {
	return fmt.Sprintf("%g: %g", p.Start, p.Vals)
}

type ByAge []Note

func (a ByAge) Len() int           { return len(a) }
func (a ByAge) Swap(i, j int)      { a[i], a[j] = a[j], a[i] }
func (a ByAge) Less(i, j int) bool { return a[i].Start < a[j].Start }

func DeltaTimes(notes []Note) {
	var ptime float32 = 0
	var oldtime float32 = 0
	sort.Sort(ByAge(notes))
	for i := 0; i < len(notes); i++ {
		oldtime = notes[i].Start
		notes[i].Start -= ptime
		ptime = oldtime
	}
}

func WriteLine(file *os.File, delta float32, vals []float32) {
	d1 := new(bytes.Buffer)
	binary.Write(d1, binary.LittleEndian, delta)
	binary.Write(d1, binary.LittleEndian, uint32(len(vals)))
	for _, elem := range vals {
		binary.Write(d1, binary.LittleEndian, elem)
	}
	file.Write(d1.Bytes())
	d1.Reset()
}

func WriteNotes(file *os.File, notes *[]Note) {
	DeltaTimes(*notes)
	for _, k := range *notes {
		fmt.Println(k)
		WriteLine(file, k.Start, k.Vals)
	}
}

func main() {
	notes := []Note{
		{0, []float32{5.0, 60, 0.1}},
		{1, []float32{4.0, 62, 0.1}},
		{2, []float32{3.0, 71, 0.1}},
		{3, []float32{2.0, 64, 0.1}},
	}
	fmt.Println(notes)
	file, _ := os.Create("test.bin")
	defer file.Close()
	WriteNotes(file, &notes)
}
