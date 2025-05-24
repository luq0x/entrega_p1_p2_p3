# Compilador .LPN para Assembly Neander

**Aluno:** Lucas Gonçalo de Morais

## Descrição Geral

Este projeto implementa um pipeline completo para compilação de programas escritos na linguagem `.lpn`, transformando-os em código assembly compatível com o Neander, montando-os em binários executáveis e executando-os em uma máquina virtual simulada.

### Caso de Uso

1. Um arquivo `programa.lpn` será submetido ao compilador.
2. O compilador irá gerar o arquivo `programa.asm` (código assembly).
3. O arquivo `programa.asm` será enviado ao assembler, que irá gerar `programa.bin` (código binário).
4. O arquivo `programa.bin` será executado na máquina virtual (executor) ou no simulador gráfico do Neander.
5. O mesmo processo será repetido com outros arquivos `.lpn` gerados pelo docente, respeitando a gramática BNF fornecida.

---

## Organização do Repositório

- `compiler.c` – Código-fonte do compilador.
- `assembler.c` – Código-fonte do montador (assembler).
- `executor.c` – Código-fonte da máquina virtual (executor).
- `neander.h` – Cabeçalhos e definições comuns.
- `Makefile` – Script de compilação e execução.
- `programa.lpn` – Arquivo de teste da linguagem de entrada.
- `gramatica.pdf` – Gramática BNF reconhecida pelo compilador.
- `assembler.pdf` – Descrição da gramática implementada do assembler.
- `README.md` – Este arquivo.

---

## Instruções de Compilação e Execução

Este projeto é compatível com **Linux 64bits**.

### Requisitos

- `gcc` (GNU Compiler Collection)
- Terminal Bash ou equivalente

### Compilar os programas

```bash
make all
```

### Executar o pipeline completo

```bash
make run
```

### Limpar arquivos gerados

```bash
make clean
```
