# Brainfuck Compiler

Este projeto é um compilador em Go que transforma expressões do tipo `AÇÃO=1+2*3` em **código Brainfuck executável**, com suporte total a:

- Strings UTF-8 (como "AÇÃO")
- Expressões aritméticas com `+`, `-`, `*`, `/`
- Impressão correta de múltiplos dígitos
- Interpretação e execução com máquina virtual própria

---

## Estrutura

- `bfc.go`: compilador — transforma texto + expressão em código Brainfuck
- `bfe.go`: interpretador — executa o código Brainfuck gerado
- `Makefile`: comandos automáticos pra build, run e teste

---

## Como usar

### Compilar:

```bash
make
```

```bash
echo "AÇÃO=1+2*3" | ./bfc | ./bfe
```

### Limpar binários:

```bash
make
```

