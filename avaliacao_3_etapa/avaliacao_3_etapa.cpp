#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARQUIVO "contas.dat"
#define SENHA_ADMIN "iangol123"

struct Conta {
    int numero;
    char titular[50];
    float saldo;
};

void criarConta();
void extrato();
void deposito();
void saque();
void transferencia();
void listarContas();
int buscarConta(int numero, struct Conta *conta);
void atualizarConta(struct Conta contaAtualizada);

int gerarNumeroConta();

int main() {
    int opcao;

    do {
        printf("\n=========== SISTEMA BANCARIO ===========\n");
        printf("1. Criar Conta\n");
        printf("2. Extrato\n");
        printf("3. Deposito\n");
        printf("4. Saque\n");
        printf("5. Transferencia\n");
        printf("6. Listar Todas as Contas (Admin)\n");
        printf("7. Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1: criarConta(); break;
            case 2: extrato(); break;
            case 3: deposito(); break;
            case 4: saque(); break;
            case 5: transferencia(); break;
            case 6: listarContas(); break;
            case 7: printf("Encerrando...\n"); break;
            default: printf("Opcao invalida!\n");
        }

    } while (opcao != 7);

    return 0;
}

void criarConta() {
    FILE *f = fopen(ARQUIVO, "ab");
    if (!f) { printf("Erro ao abrir arquivo.\n"); return; }

    struct Conta nova;

    printf("Nome do titular: ");
    getchar(); 
    fgets(nova.titular, 50, stdin);
    nova.titular[strcspn(nova.titular, "\n")] = 0;

    nova.numero = gerarNumeroConta();
    nova.saldo = 0.0;

    fwrite(&nova, sizeof(struct Conta), 1, f);
    fclose(f);

    printf("Conta criada com sucesso!\n");
    printf("Numero da conta: %d\n", nova.numero);
}

void extrato() {
    int numero;
    struct Conta c;

    printf("Numero da conta: ");
    scanf("%d", &numero);

    if (buscarConta(numero, &c)) {
        printf("\n--- EXTRATO ---\n");
        printf("Titular: %s\n", c.titular);
        printf("Numero: %d\n", c.numero);
        printf("Saldo: R$ %.2f\n", c.saldo);
    } else {
        printf("Conta nao encontrada.\n");
    }
}

void deposito() {
    int numero;
    float valor;
    struct Conta c;

    printf("Numero da conta: ");
    scanf("%d", &numero);

    if (buscarConta(numero, &c)) {
        printf("Valor a depositar: R$ ");
        scanf("%f", &valor);

        c.saldo += valor;
        atualizarConta(c);

        printf("Deposito realizado.\n");
    } else {
        printf("Conta nao encontrada.\n");
    }
}

void saque() {
    int numero;
    float valor;
    struct Conta c;

    printf("Numero da conta: ");
    scanf("%d", &numero);

    if (buscarConta(numero, &c)) {
        printf("Valor a sacar: R$ ");
        scanf("%f", &valor);

        if (valor > c.saldo) {
            printf("Saldo insuficiente.\n");
            return;
        }

        c.saldo -= valor;
        atualizarConta(c);

        printf("Saque realizado.\n");
    } else {
        printf("Conta nao encontrada.\n");
    }
}

void transferencia() {
    int origem, destino;
    float valor;
    struct Conta co, cd;

    printf("Conta de origem: ");
    scanf("%d", &origem);
    printf("Conta de destino: ");
    scanf("%d", &destino);
    printf("Valor da transferencia: R$ ");
    scanf("%f", &valor);

    if (!buscarConta(origem, &co)) {
        printf("Conta de origem não encontrada.\n");
        return;
    }

    if (!buscarConta(destino, &cd)) {
        printf("Conta de destino não encontrada.\n");
        return;
    }

    if (valor > co.saldo) {
        printf("Saldo insuficiente.\n");
        return;
    }

    co.saldo -= valor;
    cd.saldo += valor;

    atualizarConta(co);
    atualizarConta(cd);

    printf("Transferencia realizada com sucesso.\n");
}

void listarContas() {
    char senha[20];
    FILE *f = fopen(ARQUIVO, "rb");
    struct Conta c;

    printf("Senha de administrador: ");
    scanf("%s", senha);

    if (strcmp(senha, SENHA_ADMIN) != 0) {
        printf("Senha incorreta.\n");
        return;
    }

    if (!f) {
        printf("Nenhuma conta cadastrada.\n");
        return;
    }

    printf("\n--- LISTA DE CONTAS ---\n");
    while (fread(&c, sizeof(struct Conta), 1, f)) {
        printf("Conta: %d | Titular: %s | Saldo: R$ %.2f\n",
               c.numero, c.titular, c.saldo);
    }

    fclose(f);
}

int gerarNumeroConta() {
    FILE *f = fopen(ARQUIVO, "rb");
    struct Conta c;
    int ultimo = 0;

    if (!f) return 1;

    while (fread(&c, sizeof(struct Conta), 1, f)) {
        ultimo = c.numero;
    }

    fclose(f);
    return ultimo + 1;
}

int buscarConta(int numero, struct Conta *conta) {
    FILE *f = fopen(ARQUIVO, "rb");
    if (!f) return 0;

    while (fread(conta, sizeof(struct Conta), 1, f)) {
        if (conta->numero == numero) {
            fclose(f);
            return 1;
        }
    }

    fclose(f);
    return 0;
}

void atualizarConta(struct Conta contaAtualizada) {
    FILE *f = fopen(ARQUIVO, "rb+");
    struct Conta c;

    if (!f) return;

    while (fread(&c, sizeof(struct Conta), 1, f)) {
        if (c.numero == contaAtualizada.numero) {
            fseek(f, -sizeof(struct Conta), SEEK_CUR);
            fwrite(&contaAtualizada, sizeof(struct Conta), 1, f);
            fclose(f);
            return;
        }
    }

    fclose(f);
}

