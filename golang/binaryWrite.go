package main

import "os"

//import "fmt"
import "bytes"
import "encoding/binary"

func check(err error) {
	if err != nil {
		panic(err)
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

func main() {
	file, err := os.Create("../out.bin")
	check(err)
	defer file.Close()
	//vals := []float32{62, 0.5}
	WriteLine(file, 0, []float32{62, 0.1})
	WriteLine(file, 0.5, []float32{64, 0.1})
	WriteLine(file, 0.5, []float32{73, 0.1})
}
