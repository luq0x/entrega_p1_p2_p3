#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define MEMORY_SIZE 516
#define LINE_SIZE 16
#define HEADER_SIZE 4
#define DATA_OFFSET 0x100

#define OPCODE_NOP 0x00 // Sem operação
#define OPCODE_STA 0x10 // Armazena acumulador em memória
#define OPCODE_LDA 0x20 // Carrega acumulador da memória
#define OPCODE_ADD 0x30 // Soma memória ao acumulador
#define OPCODE_SUB 0x31 // Subtrai memória do acumulador
#define OPCODE_OR 0x40  // OR lógico
#define OPCODE_AND 0x50 // AND lógico
#define OPCODE_NOT 0x60 // NOT lógico
#define OPCODE_JMP 0x80 // Salto incondicional
#define OPCODE_JMN 0x90 // Salto se acumulador negativo
#define OPCODE_JMZ 0xA0 // Salto se acumulador zero
#define OPCODE_HLT 0xF0 // Parada

#define DEFAULT_RESULT_OFFSET (DATA_OFFSET + 4)

/**
 * Symbol – representa um símbolo na tabela de execução
 */
typedef struct
{
    char identifier[32];
    int memAddr;
    int assignedValue;
    bool isInitialized;
} Symbol;

#define MAX_SYMBOLS 256
Symbol symbolTable[MAX_SYMBOLS];
int totalSymbols = 0;

/**
 * registerSymbol – registra um símbolo na tabela
 * @name: nome do símbolo
 * @address: endereço de memória
 * @value: valor inicial do símbolo
 * @initialized: se o símbolo foi inicializado
 *
 * @return: void
 */
void registerSymbol(const char *name, int address, int value, bool initialized)
{
    if (totalSymbols >= MAX_SYMBOLS)
    {
        fprintf(stderr, "Erro: tabela de simbolos cheia\n");
        return;
    }
    strncpy(symbolTable[totalSymbols].identifier, name, 31);
    symbolTable[totalSymbols].memAddr = address;
    symbolTable[totalSymbols].assignedValue = value;
    symbolTable[totalSymbols].isInitialized = initialized;
    totalSymbols++;
    printf("Simbolo registrado: %s at %d (value: %d)\n", name, address, value);
}

/**
 * getSymbolAddress – obtém endereço de um símbolo
 * @symbolName: nome do símbolo
 *
 * @return: endereço se encontrado, -1 caso contrário
 */
int getSymbolAddress(const char *name)
{
    for (int i = 0; i < totalSymbols; i++)
    {
        if (strcmp(symbolTable[i].identifier, name) == 0)
            return symbolTable[i].memAddr;
    }
    return -1;
}

/**
 * isSymbolKnown – verifica se símbolo está na tabela
 * @symbolName: nome do símbolo
 *
 * @return: true se existir, false caso contrário
 */
bool isSymbolKnown(const char *name)
{
    for (int i = 0; i < totalSymbols; i++)
    {
        if (strcmp(symbolTable[i].identifier, name) == 0)
            return true;
    }
    return false;
}

/**
 * convertToNumber – converte string decimal ou hexadecimal em inteiro
 * @text: string representando número (ex: "42" ou "0x2A")
 *
 * @return: valor inteiro convertido
 */
int convertToNumber(const char *input)
{
    return (input[0] == '0' && (input[1] == 'x' || input[1] == 'X')) ? strtol(input, NULL, 16) : atoi(input);
}

/**
 * removeCommentsAndTrim – remove comentários começando em ';' e trima espaços finais
 * @line: buffer da linha (modificado in-place)
 *
 * @return: void
 */
void removeCommentsAndTrim(char *line)
{
    char *comment = strchr(line, ';');
    if (comment)
        *comment = '\0';
    int len = strlen(line);
    while (len > 0 && isspace((unsigned char)line[len - 1]))
        line[--len] = '\0';
}

/**
 * printMemoryDump – exibe dump de memória em linhas de bytes
 * @memory: ponteiro para buffer de memória
 * @length: tamanho a exibir
 *
 * @return: void
 */
void printMemoryDump(uint8_t *memory, size_t length)
{
    for (size_t i = 0; i < length; i += LINE_SIZE)
    {
        printf("%08lx:", i);
        for (int j = 0; j < LINE_SIZE && i + j < length; j++)
        {
            printf(" %02x", memory[i + j]);
        }
        printf("\n");
    }
    printf("\n");
}

/**
 * executeBinaryFile – carrega e executa binário em memória simulada
 * @filename: nome do arquivo .bin
 *
 * @return: true se sucesso, false se erro
 */
bool executeBinaryFile(const char *filename)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        perror("Nao e possível abrir o arquivo binario");
        return false;
    }

    uint8_t memory[MEMORY_SIZE] = {0};
    uint8_t header[HEADER_SIZE];
    fread(header, 1, HEADER_SIZE, fp);

    const uint8_t expectedHeader[] = {0x03, 0x4E, 0x44, 0x52};
    if (memcmp(header, expectedHeader, HEADER_SIZE) != 0)
    {
        printf("Header binario invalido\n");
        fclose(fp);
        return false;
    }

    fread(memory + HEADER_SIZE, 1, MEMORY_SIZE - HEADER_SIZE, fp);
    fclose(fp);

    printMemoryDump(memory, MEMORY_SIZE);

    uint8_t accumulator = 0;
    uint8_t programCounter = 0;
    bool zeroFlag = false, negativeFlag = false;

    while (memory[programCounter] != OPCODE_HLT)
    {
        zeroFlag = (accumulator == 0);
        negativeFlag = (accumulator & 0x80);

        uint16_t operandAddr = memory[programCounter + 2] * 2 + HEADER_SIZE;

        switch (memory[programCounter])
        {
        case OPCODE_NOP:
            break;
        case OPCODE_STA:
            memory[operandAddr] = accumulator;
            break;
        case OPCODE_LDA:
            accumulator = memory[operandAddr];
            break;
        case OPCODE_ADD:
            accumulator += memory[operandAddr];
            break;
        case OPCODE_SUB:
            accumulator -= memory[operandAddr];
            break;
        case OPCODE_OR:
            accumulator |= memory[operandAddr];
            break;
        case OPCODE_AND:
            accumulator &= memory[operandAddr];
            break;
        case OPCODE_NOT:
            accumulator = ~accumulator;
            programCounter += 2;
            continue;
        case OPCODE_JMP:
            programCounter = operandAddr;
            continue;
        case OPCODE_JMN:
            if (negativeFlag)
            {
                programCounter = operandAddr;
                continue;
            }
            break;
        case OPCODE_JMZ:
            if (zeroFlag)
            {
                programCounter = operandAddr;
                continue;
            }
            break;
        case OPCODE_HLT:
            break;
        }

        programCounter += 4;
    }

    printMemoryDump(memory, MEMORY_SIZE);

    printf("AC: 0x%02X\n", accumulator);
    printf("PC: 0x%02X\n", programCounter);

    int found = 0;
    for (int i = HEADER_SIZE; i < MEMORY_SIZE; i += 2)
    {
        if (memory[i] == accumulator)
        {
            printf("Resultado: 0x%02X = %d\n", memory[i], (int8_t)memory[i]);
            found = 1;
            break;
        }
    }

    if (!found)
        printf("Resultado não encontrando na memoria\n");
    return true;
}

int main(int argc, char *argv[])
{
    const char *defaultInput = "programa.bin";
    const char *inputFile = (argc > 1) ? argv[1] : defaultInput;

    printf("Executando arquivo binario: %s\n\n", inputFile);
    if (!executeBinaryFile(inputFile))
    {
        fprintf(stderr, "Falha na execucao\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}