#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h> // Função "Sleep" no Windows
#else
#include <unistd.h>  // Função "sleep" no Linux
#endif

// Função de delay
void delay(int seconds) {
    // Windows
    #ifdef _WIN32
        Sleep(seconds * 1000);
    // Linux
    #else
        sleep(seconds);
    #endif
}

// Função para Exibir a Logo
int exibir_logo() {
    // Localizar logo.txt
    FILE *arquivo = fopen("logo.txt", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir a Logo\n");
        return 1;
    }
    // Imprimir o Arquivo
    char linha[256];
    while (fgets(linha, sizeof(linha), arquivo)) {
        printf("%s", linha);
        delay(1);
    }
    // Fechar o Arquivo
    fclose(arquivo);
    return 0;
}

// Função para limpar o buffer
void limpar_entrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Estrutura para armazenar os dados do produto
typedef struct {
    int idproduto;
    char nomeproduto[50];
    float valorproduto;
    char tipovenda[20];
} Produto;

// Função para contar quantidade de produtos no arquivo
int contar_linhas(FILE *arquivo) {
    int contador = 0;
    char linha[100];

    rewind(arquivo); 

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        if (strlen(linha) > 1) {
            contador++;
        }
    }
    return contador;
}

// Função para imprimir todos os produtos
void imprimir_produtos(Produto *produtos, int qtd) {
    printf("\n. . . . . . . . Lista de Produtos . . . . . . . .\n");
    for (int i = 0; i < qtd; i++) {
        printf("%d - %s Preco: %.2f por %s\n", produtos[i].idproduto, produtos[i].nomeproduto, produtos[i].valorproduto, produtos[i].tipovenda);
    }
    printf("_________________________________________________\n");
}

// Função para ler produtos do arquivo e preencher o array de produtos
void ler_produtos(FILE *arquivo, Produto **produtos, int qtd) {
    rewind(arquivo);
    char linha[100];
    int i = 0;

    // Aloca memória para os produtos
    *produtos = (Produto *)malloc(qtd * sizeof(Produto));
    if (*produtos == NULL) {
        printf("Erro de alocacao!\n");
        exit(1); 
    }

    while (fgets(linha, sizeof(linha), arquivo) != NULL && i < qtd) {
        sscanf(linha, "%49s %f %19s", (*produtos)[i].nomeproduto, &(*produtos)[i].valorproduto, (*produtos)[i].tipovenda);
        (*produtos)[i].idproduto = i + 1;
        i++;
    }
}

// Função para verificar se o produto já foi registrado
int produto_existe(Produto *produtos, int qtd, const char *nome) {
    for (int i = 0; i < qtd; i++) {
        if (strcmp(produtos[i].nomeproduto, nome) == 0) {
            return 1;  // = Produto encontrado
        }
    }
    return 0;  // = Produto não encontrado
}

// Função para adicionar novos produtos
void adicionar_produto(Produto **produtos, int *qtd) {
    FILE *arquivo = fopen("produtos.txt", "a");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        return;
    }

    char prodnovo[50], tiponovo[10];
    float valornovo;
    int tipoescolha, escolhaoutro;

    do {
        printf("Digite o nome do produto: ");
        scanf(" %49[^\n]", prodnovo);

        if (produto_existe(*produtos, *qtd, prodnovo)) {
            printf("Produto ja cadastrado!\n");
        } 
        else {
            printf("Digite o valor do produto: ");
            scanf("%f", &valornovo);
            limpar_entrada();

            // Escolha do tipo de venda com opção
            do {
                printf("Escolha o tipo de venda (1 - kg, 2 - unidade): ");
                scanf("%d", &tipoescolha);
                limpar_entrada();
                if (tipoescolha == 1) {
                    strcpy(tiponovo, "kg");
                } else if (tipoescolha == 2) {
                    strcpy(tiponovo, "unidade");
                } else {
                    printf("Digite uma entrada valida!\n");
                    limpar_entrada();
                    delay(2);
                }
            } while (tipoescolha != 1 && tipoescolha != 2);

            // Adicionando o novo produto ao arquivo com uma nova linha
            fprintf(arquivo, "%s %.2f %s\n", prodnovo, valornovo, tiponovo);
            fflush(arquivo);  // Força a gravação imediata no arquivo
            printf("Produto adicionado com sucesso!\n");

            // Reallocando memória do array de produtos
            Produto *temp = realloc(*produtos, (*qtd + 1) * sizeof(Produto));
            if (temp == NULL) {
                printf("Erro de alocacao de memoria!\n");
                fclose(arquivo);
                return;
            }
            *produtos = temp;

            // Atualizando o array de produtos
            (*produtos)[*qtd].idproduto = *qtd + 1;
            strcpy((*produtos)[*qtd].nomeproduto, prodnovo);
            (*produtos)[*qtd].valorproduto = valornovo;
            strcpy((*produtos)[*qtd].tipovenda, tiponovo);
            (*qtd)++;
        }

        // Pergunta se o usuário deseja adicionar mais produtos
        do {
            printf("Deseja adicionar outro produto? (1 - Sim, 2 - Nao): ");
            scanf("%d", &escolhaoutro);
            if (escolhaoutro != 1 && escolhaoutro != 2) {
                printf("Digite uma entrada valida!\n");
                limpar_entrada();
                delay(2);
            }
        } while (escolhaoutro != 1 && escolhaoutro != 2);

    } while (escolhaoutro == 1);

    fclose(arquivo);
}

// Main
int main() {
    char usuario[16], senha[16];
    char caixaU[16] = "Compra", caixaS[16] = "senha123";
    char admU[16] = "Josney", admS[16] = "Jos72";
    int loginCorreto = 0, opcao = 0;

    FILE *arquivo = fopen("produtos.txt", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        return 1;
    }

    // Contando as linhas no arquivo e alocando memória
    int qtd_linhas = contar_linhas(arquivo);
    rewind(arquivo);

    // Realiza o Array
    Produto *produtos = NULL;
    ler_produtos(arquivo, &produtos, qtd_linhas);
    fclose(arquivo);

    // Logo
    exibir_logo();

    // Loop Login
    while (!loginCorreto) {
        printf("\nDigite seu Usuario: ");
        scanf("%15s", usuario);

        printf("\nDigite sua Senha: ");
        scanf("%15s", senha);

        // Tela do Caixa
        if (strcmp(usuario, caixaU) == 0 && strcmp(senha, caixaS) == 0) {
            loginCorreto = 1;
            printf("\nBem-vindo, %s!\n", usuario);

            do {
                printf("\nSelecione uma Opcao:\n");
                printf(" 1. Adicionar Produto ao Carrinho\n");
                printf(" 2. Consultar Lista de Produtos\n");
                printf(" 3. Visualizar Carrinho\n");
                printf(" 4. Finalizar Compra\n");
                printf(" 5. Cancelar Compra\n");
                printf(" 6. Sair\n");
                printf("Opcao: ");
                scanf("%d", &opcao);

                switch (opcao) {
                    case 1:
                        // Mostrar Lista de Produtos
                        // Adicionar Produto ao carrinho (ID)
                        // Perguntar Se quer adicionar outro produto
                        break;
                    case 2:
                        imprimir_produtos(produtos, qtd_linhas);
                        break;
                    case 4:
                        // Mostrar o Carrinho
                        // Perguntar se Deseja Finalizar Compra
                        // Limpa o Carrinho
                        break;
                    case 5:
                        // Pergunta se Deseja Cancelar Compra
                        // Limpa o Carrinho
                        break;
                    case 6:
                        printf("Saindo do sistema...\n");
                        limpar_entrada();
                        delay(2);
                        break;
                    default:
                        printf("Opcao invalida!\n");
                        delay(2);
                }
            } while (opcao != 6);
        } 
        // Tela da Administração
        else if (strcmp(usuario, admU) == 0 && strcmp(senha, admS) == 0) {
            loginCorreto = 1;
            printf("\nBem-vindo ao sistema de Administracao, %s!\n", usuario);

            do {
                printf("\nSelecione uma Opcao:\n");
                printf(" 1. Adicionar Produto\n");
                printf(" 2. Consultar Lista de Produtos\n");
                printf(" 3. Sair\n");
                printf("Opcao: ");
                scanf("%d", &opcao);

                switch (opcao) {
                    case 1:
                       adicionar_produto(&produtos, &qtd_linhas);
                        break;
                    case 2:
                        imprimir_produtos(produtos, qtd_linhas);
                        break;
                    case 3:
                        printf("Saindo do sistema...\n");
                        delay(2);
                        break;
                    default:
                        printf("Opcao invalida!\n");
                        limpar_entrada();
                        delay(2);
                }
            } while (opcao != 3);
        } 
        else {
            printf("Usuario ou senha incorretos! Tente novamente.\n");
            delay(2);
        }
    }

    free(produtos);
    return 0;
}