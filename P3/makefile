BFC = bfc
BFE = bfe
SRC_BFC = bfc.go
SRC_BFE = bfe.go

.PHONY: all clean run test

all: build

build:
	@echo "Compilando $(BFC) e $(BFE)..."
	@go build -o $(BFC) $(SRC_BFC)
	@go build -o $(BFE) $(SRC_BFE)
	@echo "Build completo"

clean:
	@echo "Limpando arquivos binários..."
	@rm -f $(BFC) $(BFE)
	@echo "Limpeza concluída"