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

// Estrutura de Produtos
typedef struct {
    int idproduto;
    char nomeproduto[50];
    float valorproduto;
    char tipovenda[20];
} Produto;

// Estrutura de Carrinho
typedef struct {
    char produtocarrinho[50];
    float valortotalcarrinho;
} Carrinho;

// Função para imprimir a lista de produtos
void imprimir_produtos(Produto produtos[], int quantidade) {
    for (int i = 0; i < quantidade; i++) {
        printf("%d Nome: %s Valor: R$ %.2f por %s\n", produtos[i].idproduto, produtos[i].nomeproduto, produtos[i].valorproduto, produtos[i].tipovenda);
    }
}

// Função para adicionar um novo produto
Produto* adicionar_produto(Produto* produtos, int* qtd_produtos) {
    // Aloca espaço para um novo produto
    produtos = (Produto*) realloc(produtos, (*qtd_produtos + 1) * sizeof(Produto));
    if (produtos == NULL) {
        puts("Erro de memória!");
        return NULL;
    }

    // Preenche as informações do novo produto
    Produto* novo_produto = &produtos[*qtd_produtos];
    novo_produto->idproduto = *qtd_produtos + 1; // ID automático

    // Nome do produto
    printf("Digite o nome do produto: ");
    scanf(" %49[^\n]", novo_produto->nomeproduto);

    // Valor do produto
    printf("Digite o valor do produto: ");
    scanf("%f", &novo_produto->valorproduto);

    // Tipo de venda
    char opcao_tipo;
    do {
        puts("Selecione o tipo de venda:");
        puts(" A. kg");
        puts(" B. unidade");
        printf("Opcao: ");
        scanf(" %c", &opcao_tipo);

        if (opcao_tipo == 'A') {
            strcpy(novo_produto->tipovenda, "kg");
        } else if (opcao_tipo == 'B') {
            strcpy(novo_produto->tipovenda, "unidade");
        } else {
            printf("Opcao invalida! Tente novamente.\n");
        }
    } while (opcao_tipo != 'A' && opcao_tipo != 'B');

    // Incrementa a quantidade de produtos
    (*qtd_produtos)++;
    printf("Produto adicionado com sucesso!\n");

    return produtos;
}

// Main
int main() {
    int produtos_iniciais = 6;
    char usuario[16], senha[16], opcao;
    int verificar_login = 0, qtd_produtos = 6;

    char caixaU[16] = "caixa1", caixaS[16] = "caixa";
    char admU[16] = "adm1", admS[16] = "adm";
    
    // Array dinâmico de Produtos
    Produto* produtos = (Produto*)malloc(produtos_iniciais * sizeof(Produto));
    if (produtos == NULL) {
        puts("Erro de memória!");
        return 1;
    }

    // Produtos iniciais
    produtos[0] = (Produto){1, "Banana", 3.99, "kg"};
    produtos[1] = (Produto){2, "Maca", 5.49, "kg"};
    produtos[2] = (Produto){3, "Laranja", 4.29, "kg"};
    produtos[3] = (Produto){4, "Alface", 2.99, "unidade"};
    produtos[4] = (Produto){5, "Tomate", 6.79, "kg"};
    produtos[5] = (Produto){6, "Manga", 2.99, "kg"};

    while (!verificar_login) {
        // Usuário
        printf("\nDigite seu Usuario: ");
        scanf("%15s", usuario);
        // Senha 
        printf("\nDigite sua Senha: ");
        scanf("%15s", senha);

        // Tela Caixa
        if (strcmp(usuario, caixaU) == 0 && strcmp(senha, caixaS) == 0) {
            verificar_login = 1;
            printf("Bem-vindo, %s!\n", usuario);

            // Array Carrinho 
            Carrinho *carrinho = NULL;
            int num_itens = 0;
            char continuar = 'S';

            do {
                puts("\nSelecione uma Opcao:");
                puts(" A. Consultar Lista de Produtos");
                puts(" B. Adicionar Produto ao Carrinho");
                puts(" C. Visualizar Carrinho");
                puts(" D. Finalizar Compra");
                puts(" E. Cancelar Compra");
                puts(" F. Sair");
                printf("Opcao: ");
                scanf(" %c", &opcao);

                switch (opcao) {
                    case 'A':
                        // Lista de Produtos
                        imprimir_produtos(produtos, qtd_produtos);
                        break;
                    case 'B':
                        // Adicionar ao Carrinho
                        continuar = 'S'; 
                        while (continuar == 'S') {
                            int idcarrinho = 0, j, encontrado = 0;
                            float quantidade;

                            printf("Digite o ID do produto que deseja comprar: ");
                            scanf("%d", &idcarrinho);

                            // Busca o produto pelo ID
                            for (j = 0; j < qtd_produtos; j++) {
                                if (produtos[j].idproduto == idcarrinho) {
                                    encontrado = 1;

                                    // Exibe informações do produto encontrado
                                    printf("Produto encontrado: %s (%.2f por %s)\n", produtos[j].nomeproduto, produtos[j].valorproduto, produtos[j].tipovenda);

                                    // Solicita a quantidade com base no tipo de venda
                                    printf("Digite a quantidade em %s: ", produtos[j].tipovenda);
                                    scanf("%f", &quantidade);

                                    // Aloca espaço para o novo item no carrinho
                                    carrinho = (Carrinho *)realloc(carrinho, (num_itens + 1) * sizeof(Carrinho));
                                    if (carrinho == NULL) {
                                        puts("Erro de memoria!\n");
                                        return 1;
                                    }

                                    // Armazena o produto e o valor total no carrinho
                                    strcpy(carrinho[num_itens].produtocarrinho, produtos[j].nomeproduto);
                                    carrinho[num_itens].valortotalcarrinho = produtos[j].valorproduto * quantidade;
                                    num_itens++;
                                    puts("Produto adicionado ao carrinho!");
                                    break;
                                }
                            }

                            if (!encontrado) {
                                puts("Produto nao encontrado!");
                            }

                            // Pergunta se o usuário deseja adicionar mais produtos
                            do {
                                printf("Deseja adicionar outro produto? (S - Sim, N - Nao): ");
                                scanf(" %c", &continuar);
                                if (continuar != 'S' && continuar != 'N') {
                                    puts("Opcao invalida! Tente novamente");
                                }
                            } while (continuar != 'S' && continuar != 'N');
                        }
                        break;

                    case 'C':
                        // Visualizar o Carrinho
                        // Se o carrinho estiver vazio
                        if (num_itens == 0) {
                            puts("O carrinho esta vazio!");
                        }
                        // Se o carrinho ter itens
                        else {
                            for (int i = 0; i < num_itens; i++) {
                                printf("Produto: %s | Valor Total: R$%.2f\n", carrinho[i].produtocarrinho, carrinho[i].valortotalcarrinho);
                            }
                        }
                        break;

                    case 'D':
                        // Finalizar Compra
                        // Se o carrinho estiver vazio
                        if (num_itens == 0) {
                            puts("O carrinho esta vazio!");
                            break;
                        }

                        // Exibe os itens no carrinho e calcula o total
                        puts(". . . . . . . . Itens no Carrinho . . . . . . . .");
                        float valor_total = 0.0;
                        for (int i = 0; i < num_itens; i++) {
                            printf("Produto: %s | Valor Total: R$%.2f\n", carrinho[i].produtocarrinho, carrinho[i].valortotalcarrinho);
                            valor_total += carrinho[i].valortotalcarrinho;
                        }
                        printf("Valor total da compra: R$%.2f\n", valor_total);
                        puts("_________________________________________________");

                        // Pergunta se o usuário deseja finalizar a compra
                        int finalizar;
                        printf("Deseja finalizar a compra? (S - Sim, N - Nao): ");
                        do {
                            scanf(" %c", &finalizar);
                            if (finalizar != 'S' && finalizar != 'N') {
                                puts("Opcao invalida! Tente novamente");
                            } 
                        }while(finalizar != 'S' && finalizar != 'N');
                        
                        if (finalizar == 'S') {
                            printf("Compra finalizada! Valor total: R$%.2f\n", valor_total);
                            
                            // Limpa o carrinho para a próxima compra
                            free(carrinho);
                            carrinho = NULL;
                            num_itens = 0;
                        } 
                        else {
                            puts("Compra nao finalizada!");
                        }
                        break;
                     
                    case 'E':
                        // Cancelar Compra
                        // Se o carrinho estiver vazio
                        if (num_itens == 0) {
                            printf("O carrinho esta vazio!\n");
                            delay(1);
                            break;
                        }
                        // Se o carrinho ter itens
                        char cancelar;
                        printf("Tem certeza de que deseja cancelar a compra? (S - Sim, N - Nao): ");
                        do {
                            scanf(" %c", &cancelar);
                            if (cancelar != 'S' && cancelar != 'N') {
                                puts("Opcao invalida! Tente novamente");
                            }
                        }while (cancelar != 'S' && cancelar != 'N');

                        if (cancelar == 'S') {
                            puts("Compra cancelada. Todos os itens foram removidos do carrinho");

                            // Libera a memória alocada para o carrinho e redefine as variáveis
                            free(carrinho);
                            carrinho = NULL;
                            num_itens = 0;
                        }
                        else {
                            puts("Cancelamento de compra abortado");
                        }
                        break;

                    case 'F':
                        // Sair
                        puts("Saindo do sistema...");
                        delay(2);
                        break;

                    default:
                        // Opção invalida
                        puts("Opcao invalida!");
                        delay(2);
                }
            } while(opcao != 'F');
        }

        // Tela Adm
        else if (strcmp(usuario, admU) == 0 && strcmp(senha, admS) == 0) {
            verificar_login = 1;
            printf("Bem-vindo, %s!\n", usuario);
            do {
                puts("\nSelecione uma Opcao:");
                puts(" A. Adicionar Produto");
                puts(" B. Consultar Lista de Produtos");
                puts(" C. Sair");
                printf("Opcao: ");
                scanf(" %c", &opcao);

                switch (opcao) {
                    case 'A':
                        // Adicionar Produto
                        produtos = adicionar_produto(produtos, &qtd_produtos);
                        if (produtos == NULL) {
                            return 1;
                        }
                        break;

                    case 'B':
                        // Imprimir Lista de Produtos
                        imprimir_produtos(produtos, qtd_produtos);
                        break;
                    case 'C':
                        // Sair
                        puts("Saindo do sistema...");
                        delay(2);
                        break;

                    default:
                        puts("Opcao invalida!");
                        delay(2);
                        // Opção invalida
                }
            } while(opcao != 'C');
        }

        else {
            puts("Usuario ou senha incorretos! Tente novamente...\n");
            delay(2);
        }
    } 

    return 0;
}