#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define HEADER_SIZE 4
#define MEMORY_SIZE 512
#define LINE_SIZE 256
#define DATA_OFFSET 0x100

#define INS_NOP 0x00
#define INS_STA 0x10
#define INS_LDA 0x20
#define INS_ADD 0x30
#define INS_SUB 0x31
#define INS_OR 0x40
#define INS_AND 0x50
#define INS_NOT 0x60
#define INS_JMP 0x80
#define INS_JMN 0x90
#define INS_JMZ 0xA0
#define INS_HLT 0xF0

#define RESULT_ADDR_OFFSET (DATA_OFFSET + 4)

/**
 * Symbol – representa um rótulo/símbolo na tabela de símbolos
 */
typedef struct
{
    char labelName[32];
    int memoryAddr;
    int initialValue;
    bool isDefined;
} Symbol;

#define MAX_LABEL_COUNT 256
Symbol labelTable[MAX_LABEL_COUNT];
int labelTotal = 0;

/**
 * registerSymbol – registra um símbolo na tabela
 * @name: nome do símbolo
 * @addr: endereço de memória
 * @value: valor inicial (se houver)
 * @defined: true se o símbolo estiver definido
 *
 * @return: void
 */
void registerSymbol(const char *name, int addr, int value, bool defined)
{
    if (labelTotal < MAX_LABEL_COUNT)
    {
        strncpy(labelTable[labelTotal].labelName, name, sizeof(labelTable[labelTotal].labelName) - 1);
        labelTable[labelTotal].labelName[sizeof(labelTable[labelTotal].labelName) - 1] = '\0';
        labelTable[labelTotal].memoryAddr = addr;
        labelTable[labelTotal].initialValue = value;
        labelTable[labelTotal].isDefined = defined;
        labelTotal++;
        printf("Simbolo registrado: %s (address: %d, value: %d)\n", name, addr, value);
    }
    else
    {
        fprintf(stderr, "Error: Lista de simbolo cheia\n");
    }
}

/**
 * lookupSymbolAddress – obtém endereço de um símbolo pelo nome
 * @name: nome do símbolo
 *
 * @return: endereço se encontrado, -1 caso contrário
 */
int lookupSymbolAddress(const char *name)
{
    for (int i = 0; i < labelTotal; i++)
    {
        if (strcmp(labelTable[i].labelName, name) == 0)
            return labelTable[i].memoryAddr;
    }
    return -1;
}

/**
 * isSymbolDefined – verifica se um símbolo já está na tabela
 * @name: nome do símbolo
 *
 * @return: true se existir, false caso contrário
 */
bool isSymbolDefined(const char *name)
{
    for (int i = 0; i < labelTotal; i++)
    {
        if (strcmp(labelTable[i].labelName, name) == 0)
            return true;
    }
    return false;
}

/**
 * parseNumberOrHex – converte string decimal ou hexadecimal em inteiro
 * @text: string contendo o número (ex: "123" ou "0x7B")
 *
 * @return: valor inteiro convertido
 */
int parseNumberOrHex(const char *txt)
{
    if (txt[0] == '0' && (txt[1] == 'x' || txt[1] == 'X'))
        return (int)strtol(txt, NULL, 16);
    return atoi(txt);
}

/**
 * removeCommentsAndTrim – remove comentários a partir de ';' e espaços finais
 * @line: string da linha que sofrerá alteração in place
 *
 * @return: void
 */
void removeCommentsAndTrim(char *line)
{
    char *commentPos = strchr(line, ';');
    if (commentPos)
        *commentPos = '\0';
    int len = strlen(line);
    while (len > 0 && isspace((unsigned char)line[len - 1]))
        line[--len] = '\0';
}

/**
 * assembleSource – processa o arquivo ASM e gera arquivo binário
 * @sourceFile: nome do arquivo .asm de entrada
 * @binOutputFile: nome do arquivo .bin de saída
 *
 * @return: true se sucesso, false caso erro
 */
bool assembleSource(const char *sourceFile, const char *binOutputFile)
{
    FILE *source = fopen(sourceFile, "r");
    if (!source)
    {
        perror("Falha ao abrir o arquivo");
        return false;
    }

    uint8_t memory[MEMORY_SIZE] = {0};
    uint8_t header[HEADER_SIZE] = {0x03, 0x4E, 0x44, 0x52};
    memcpy(memory, header, HEADER_SIZE);

    int dataPos = DATA_OFFSET;
    int originOffset = 0;
    int codeStart = HEADER_SIZE + originOffset * 2;
    int codePos = codeStart;

    enum
    {
        NONE,
        DATA,
        CODE
    } currentSection = NONE;
    registerSymbol("RES", RESULT_ADDR_OFFSET, 0, false);

    char line[LINE_SIZE];
    /* primeira passagem: coleta de rótulos e dados */
    int tempCodePos = codeStart;
    while (fgets(line, sizeof(line), source))
    {
        removeCommentsAndTrim(line);
        char *ptr = line;
        while (isspace((unsigned char)*ptr))
            ptr++;
        if (*ptr == '\0')
            continue;

        char tag[32] = {0};
        if (strchr(ptr, ':'))
        {
            sscanf(ptr, "%31[^:]:", tag);
            if (strlen(tag) > 0 && currentSection == CODE)
            {
                if (!isSymbolDefined(tag))
                {
                    registerSymbol(tag, tempCodePos, 0, true);
                    printf("Simbolo encontrado: %s at %d\n", tag, tempCodePos);
                }
                continue;
            }
        }
        if (strncasecmp(ptr, ".DATA", 5) == 0)
        {
            currentSection = DATA;
            continue;
        }
        if (strncasecmp(ptr, ".CODE", 5) == 0)
        {
            currentSection = CODE;
            continue;
        }
        if (currentSection == DATA)
        {
            char keyword[16], valueStr[32];
            int items = sscanf(ptr, "%31s %15s %31s", tag, keyword, valueStr);
            if (items >= 2 && strcasecmp(keyword, "DB") == 0)
            {
                int val = 0;
                bool def = true;
                if (items < 3 || strcmp(valueStr, "?") == 0)
                {
                    def = false;
                    val = 0;
                }
                else
                {
                    val = parseNumberOrHex(valueStr);
                }
                if (dataPos % 2 != 0)
                    dataPos++;
                if (!isSymbolDefined(tag))
                {
                    registerSymbol(tag, dataPos, val, def);
                }
                memory[dataPos] = (uint8_t)val;
                memory[dataPos + 1] = 0;
                dataPos += 2;
            }
        }
        else if (currentSection == CODE)
        {
            if (strncasecmp(ptr, ".ORG", 4) == 0)
            {
                int newOrg;
                if (sscanf(ptr, ".ORG %d", &newOrg) == 1)
                {
                    originOffset = newOrg;
                    codeStart = HEADER_SIZE + originOffset * 2;
                    tempCodePos = codeStart;
                }
                continue;
            }
            /* cálculo de posições de instruções */
            char instruction[16], operand[32];
            if (sscanf(ptr, "%15s %31s", instruction, operand) >= 1)
            {
                tempCodePos += 4;
            }
        }
    }
    rewind(source);

    /* segunda passagem: geração de binário */
    currentSection = NONE;
    codePos = codeStart;
    while (fgets(line, sizeof(line), source))
    {
        removeCommentsAndTrim(line);
        char *ptr = line;
        while (isspace((unsigned char)*ptr))
            ptr++;
        if (*ptr == '\0')
            continue;
        if (strchr(ptr, ':'))
            continue;
        if (strncasecmp(ptr, ".DATA", 5) == 0)
        {
            currentSection = DATA;
            continue;
        }
        if (strncasecmp(ptr, ".CODE", 5) == 0)
        {
            currentSection = CODE;
            continue;
        }
        if (currentSection == CODE)
        {
            if (strncasecmp(ptr, ".ORG", 4) == 0)
            {
                int newOrg;
                if (sscanf(ptr, ".ORG %d", &newOrg) == 1)
                {
                    originOffset = newOrg;
                    codeStart = HEADER_SIZE + originOffset * 2;
                    codePos = codeStart;
                }
                continue;
            }
            char instruction[16], operand[32];
            int count = sscanf(ptr, "%15s %31s", instruction, operand);
            if (count < 1)
                continue;
            uint8_t opcode = 0;
            uint8_t opByte = 0;

            /* mapeamento de instruções */
            if (strcasecmp(instruction, "LDA") == 0)
                opcode = INS_LDA;
            else if (strcasecmp(instruction, "ADD") == 0)
                opcode = INS_ADD;
            else if (strcasecmp(instruction, "SUB") == 0)
                opcode = INS_SUB;
            else if (strcasecmp(instruction, "STA") == 0)
                opcode = INS_STA;
            else if (strcasecmp(instruction, "HLT") == 0)
                opcode = INS_HLT;
            else if (strcasecmp(instruction, "NOP") == 0)
                opcode = INS_NOP;
            else if (strcasecmp(instruction, "NOT") == 0)
                opcode = INS_NOT;
            else if (strncasecmp(instruction, "JMP", 3) == 0)
                opcode = INS_JMP;
            else if (strncasecmp(instruction, "JMN", 3) == 0)
                opcode = INS_JMN;
            else if (strncasecmp(instruction, "JMZ", 3) == 0)
                opcode = INS_JMZ;
            else if (strcasecmp(instruction, "OR") == 0)
                opcode = INS_OR;
            else if (strcasecmp(instruction, "AND") == 0)
                opcode = INS_AND;
            else
            {
                fprintf(stderr, "Instrucao desconhecida: %s\n", instruction);
                continue;
            }

            /* calcula operando se aplicável */
            if (opcode != INS_HLT && opcode != INS_NOP && opcode != INS_NOT && count == 2)
            {
                int addr = lookupSymbolAddress(operand);
                if (addr < 0)
                {
                    if (dataPos % 2 != 0)
                        dataPos++;
                    registerSymbol(operand, dataPos, 0, false);
                    addr = dataPos;
                    dataPos += 2;
                }
                opByte = (uint8_t)((addr - HEADER_SIZE) / 2);
            }

            /* grava instrução na memória */
            memory[codePos] = opcode;
            memory[codePos + 1] = 0;
            memory[codePos + 2] = opByte;
            memory[codePos + 3] = 0;

            printf("Instruções: %s %s - Opcode: 0x%02X Operand: 0x%02X (Addr: %d)\n",
                   instruction, count > 1 ? operand : "", opcode, opByte, codePos);

            codePos += 4;
        }
    }
    fclose(source);

    /* avisos de símbolos não definidos */
    for (int i = 0; i < labelTotal; i++)
    {
        if (!labelTable[i].isDefined)
        {
            printf("Simbolo '%s' usado não definido\n", labelTable[i].labelName);
        }
    }

    FILE *out = fopen(binOutputFile, "wb");
    if (!out)
    {
        perror("Falha ao criar o arquivo binario");
        return false;
    }
    fwrite(memory, 1, MEMORY_SIZE, out);
    fclose(out);

    printf("\nAssembly criado: %s\n", binOutputFile);
    return true;
}

int main(int argc, char *argv[])
{
    char asmFile[256] = "programa.asm";
    char binFile[256] = "programa.bin";

    if (argc > 1)
        strncpy(asmFile, argv[1], sizeof(asmFile) - 1);
    if (argc > 2)
        strncpy(binFile, argv[2], sizeof(binFile) - 1);

    printf("Assembling: %s -> %s\n\n", asmFile, binFile);
    if (!assembleSource(asmFile, binFile))
    {
        fprintf(stderr, "Assembly falhou.\n");
        return 1;
    }
    return 0;
}