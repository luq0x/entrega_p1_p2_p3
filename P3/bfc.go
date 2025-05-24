package main

import (
	"fmt"
	"io"
	"os"
	"strings"
)

func repeat(char rune, times int) string {
	result := ""
	for i := 0; i < times; i++ {
		result += string(char)
	}
	return result
}

func encodeByte(b byte) string {
	loop := b / 10
	rest := b % 10

	if loop == 0 {
		return ">" + repeat('+', int(rest)) + ".[-]<"
	}
	bf := ""
	bf += repeat('+', 10)
	bf += "[" + ">" + repeat('+', int(loop)) + "<" + "-]"
	bf += ">" + repeat('+', int(rest)) + ".[-]<"
	return bf
}

func encodeString(s string) string {
	out := ""
	for _, b := range []byte(s) {
		out += encodeByte(b)
	}
	return out
}

func main() {
	data, err := io.ReadAll(os.Stdin)
	if err != nil {
		fmt.Println("Erro ao ler entrada:", err)
		return
	}
	entrada := strings.TrimSpace(string(data))

	eq := strings.Index(entrada, "=")
	if eq == -1 {
		fmt.Println("Entrada inválida")
		return
	}

	prefix := entrada[:eq+1] 
	expr := entrada[eq+1:]  

	fmt.Print(encodeString(prefix))

	fmt.Print(encodeString(expr))
}
