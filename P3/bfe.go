package main

import (
	"fmt"
	"io"
	"os"
	"strconv"
	"strings"
)

func eval(expr string) int {
	expr = strings.ReplaceAll(expr, " ", "")
	tokens := []string{}
	num := ""

	for _, c := range expr {
		if c >= '0' && c <= '9' {
			num += string(c)
		} else if strings.ContainsRune("+-*/", c) {
			if num != "" {
				tokens = append(tokens, num)
				num = ""
			}
			tokens = append(tokens, string(c))
		}
	}
	if num != "" {
		tokens = append(tokens, num)
	}

	stack := []string{tokens[0]}
	i := 1
	for i < len(tokens) {
		op := tokens[i]
		num := tokens[i+1]
		if op == "*" || op == "/" {
			left, _ := strconv.Atoi(stack[len(stack)-1])
			right, _ := strconv.Atoi(num)
			res := 0
			if op == "*" {
				res = left * right
			} else {
				if right == 0 {
					res = 0
				} else {
					res = left / right
				}
			}
			stack[len(stack)-1] = strconv.Itoa(res)
			i += 2
		} else {
			stack = append(stack, op, num)
			i += 2
		}
	}

	result, _ := strconv.Atoi(stack[0])
	i = 1
	for i < len(stack) {
		op := stack[i]
		num, _ := strconv.Atoi(stack[i+1])
		if op == "+" {
			result += num
		} else {
			result -= num
		}
		i += 2
	}
	return result
}

func main() {
	const memorySize = 30000
	mem := make([]byte, memorySize)
	ptr := 0

	code, err := io.ReadAll(os.Stdin)
	if err != nil {
		fmt.Println("Erro ao ler entrada:", err)
		return
	}

	var output strings.Builder
	loopStack := []int{}
	for pc := 0; pc < len(code); pc++ {
		switch code[pc] {
		case '>':
			ptr++
		case '<':
			ptr--
		case '+':
			mem[ptr]++
		case '-':
			mem[ptr]--
		case '.':
			output.WriteByte(mem[ptr])
		case ',':
			b := make([]byte, 1)
			os.Stdin.Read(b)
			mem[ptr] = b[0]
		case '[':
			if mem[ptr] == 0 {
				loop := 1
				for loop > 0 {
					pc++
					if code[pc] == '[' {
						loop++
					} else if code[pc] == ']' {
						loop--
					}
				}
			} else {
				loopStack = append(loopStack, pc)
			}
		case ']':
			if mem[ptr] != 0 {
				pc = loopStack[len(loopStack)-1]
			} else {
				loopStack = loopStack[:len(loopStack)-1]
			}
		}
	}

	text := output.String()
	if eq := strings.Index(text, "="); eq != -1 && eq+1 < len(text) {
		prefix := text[:eq+1]
		expr := text[eq+1:]
		res := eval(expr)
		fmt.Printf("%s%d\n", prefix, res)
	} else {
		fmt.Println(text)
	}
}
