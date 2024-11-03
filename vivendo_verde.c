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

// Função para limpar o buffer de entrada
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

typedef struct {
    char produtocarrinho[50];
    float valortotalcarrinho;
} Carrinho;

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
                } 
                else if (tipoescolha == 2) {
                    strcpy(tiponovo, "unidade");
                } 
                else {
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
    char usuario[16], senha[16], opcao;
    char caixaU[16] = "Compra", caixaS[16] = "senha123";
    char admU[16] = "Josney", admS[16] = "Jos72";
    int loginCorreto = 0;

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

    // Array Carrinho 
    Carrinho *carrinho = NULL;
    int num_itens = 0;

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
                printf(" A. Adicionar Produto ao Carrinho\n");
                printf(" B. Consultar Lista de Produtos\n");
                printf(" C. Visualizar Carrinho\n");
                printf(" D. Finalizar Compra\n");
                printf(" E. Cancelar Compra\n");
                printf(" F. Sair\n");
                printf("Opcao: ");
                scanf(" %c", &opcao);
            
                int continuar = 1;

                switch (opcao) {
                case 'A':
                    imprimir_produtos(produtos, qtd_linhas);

                    while (continuar == 1) {
                        float quantidade;
                        int id = 0, j, encontrado = 0;

                        // Solicita o ID do produto desejado
                        printf("Digite o ID do produto que deseja comprar: ");
                        scanf("%d", &id);

                        // Busca o produto pelo ID
                        for (j = 0; j < qtd_linhas; j++) {
                            if (produtos[j].idproduto == id) {
                                encontrado = 1;

                                // Exibe informações do produto encontrado
                                printf("Produto encontrado: %s (%.2f por %s)\n", produtos[j].nomeproduto, produtos[j].valorproduto, produtos[j].tipovenda);
                                printf("Digite a quantidade em %s: ", produtos[j].tipovenda);
                                scanf("%f", &quantidade);

                                // Aloca espaço para o novo item no carrinho
                                carrinho = (Carrinho *)realloc(carrinho, (num_itens + 1) * sizeof(Carrinho));
                                if (carrinho == NULL) {
                                    printf("Erro de memoria!\n");
                                    return 1;
                                }

                                // Armazena o produto e o valor total no carrinho
                                strcpy(carrinho[num_itens].produtocarrinho, produtos[j].nomeproduto);
                                carrinho[num_itens].valortotalcarrinho = produtos[j].valorproduto * quantidade;
                                num_itens++;

                                printf("Produto adicionado ao carrinho!\n");
                                break;
                            }
                        }

                        if (!encontrado) {
                            printf("Produto nao encontrado!\n");
                        }

                        // Pergunta se o usuário deseja adicionar mais produtos
                        printf("Deseja adicionar outro produto? (1 - Sim, 2 - Nao): ");
                        scanf("%d", &continuar);
                    }
                    break;

                    case 'B':
                        imprimir_produtos(produtos, qtd_linhas);
                        break;
                    case 'C':
                        if (num_itens == 0) {
                            printf("O carrinho esta vazio!\n");
                        } 
                        else {
                            for (int i = 0; i < num_itens; i++) {
                                printf("Produto: %s | Valor Total: R$%.2f\n", carrinho[i].produtocarrinho, carrinho[i].valortotalcarrinho);
                            }
                        }
                        break;
                    case 'D':
                        if (num_itens == 0) {
                            printf("O carrinho esta vazio!\n");
                            delay(1);
                            break;
                        }

                        // Exibe os itens no carrinho e calcula o total
                        printf("\n. . . . . . . . Itens no Carrinho . . . . . . . .\n");
                        float valor_total = 0.0;
                        for (int i = 0; i < num_itens; i++) {
                            printf("Produto: %s | Valor Total: R$%.2f\n", carrinho[i].produtocarrinho, carrinho[i].valortotalcarrinho);
                            valor_total += carrinho[i].valortotalcarrinho;
                        }
                        printf("Valor total da compra: R$%.2f\n", valor_total);
                        printf("_________________________________________________\n");

                        // Pergunta se o usuário deseja finalizar a compra
                        int finalizar;
                        printf("Deseja finalizar a compra? (1 - Sim, 2 - Nao): ");
                        scanf("%d", &finalizar);

                        if (finalizar == 1) {
                            printf("Compra finalizada! Valor total: R$%.2f\n", valor_total);
                            // Limpa o carrinho
                            free(carrinho);
                            carrinho = NULL;
                            num_itens = 0;
                        } 
                        else {
                            printf("Compra nao finalizada!\n");
                        }
                        break;
                    case 'E':
                        if (num_itens == 0) {
                            printf("O carrinho esta vazio!\n");
                            delay(1);
                            break;
                        }

                        int cancelar;
                        printf("Tem certeza de que deseja cancelar a compra? (1 - Sim, 2 - Nao): ");
                        scanf("%d", &cancelar);

                        if (cancelar == 1) {
                            printf("Compra cancelada. Todos os itens foram removidos do carrinho\n");
                            // Libera a memória alocada para o carrinho e redefine as variáveis
                            free(carrinho);
                            carrinho = NULL;
                            num_itens = 0;
                        }
                        else {
                            printf("Cancelamento de compra abortado\n");
                        }
                        break;
                    case 'F':
                        printf("Saindo do sistema...\n");
                        delay(2);
                        break;
                    default:
                        printf("Opcao invalida!\n");
                        limpar_entrada();
                        delay(2);
                }
            } while (opcao != 'F');
        }
        // Tela da Administração
        else if (strcmp(usuario, admU) == 0 && strcmp(senha, admS) == 0) {
            loginCorreto = 1;
            printf("\nBem-vindo ao sistema de Administracao, %s!\n", usuario);

            do {
                printf("\nSelecione uma Opcao:\n");
                printf(" A. Adicionar Produto\n");
                printf(" B. Consultar Lista de Produtos\n");
                printf(" C. Sair\n");
                printf("Opcao: ");
                scanf(" %c", &opcao);

                switch (opcao) {
                    case 'A':
                       adicionar_produto(&produtos, &qtd_linhas);
                        break;
                    case 'B':
                        imprimir_produtos(produtos, qtd_linhas);
                        break;
                    case 'C':
                        printf("Saindo do sistema...\n");
                        delay(2);
                        break;
                    default:
                        printf("Opcao invalida!\n");
                        limpar_entrada();
                        delay(2);
                }
            } while (opcao != 'C');
        } 
        else {
            printf("Usuario ou senha incorretos! Tente novamente...\n");
            delay(2);
        }
    }

    free(produtos);
    return 0;
}